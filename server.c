/******************************************************************************
* tcp_server.c
*
* CPE 464 - Program 1
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>

#include "networks.h"
#include "pdu.h"
#include "pollLib.h"
#include "list.h"


#define MAXBUF 1024
#define DEBUG_FLAG 1

int recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);
int serverControl();

linkedList* clientList;

int main(int argc, char *argv[])
{
    int serverSocket = 0;   //socket descriptor for the server socket
    int clientSocket = 0;   //socket descriptor for the client socket
    int portNumber = 0;

    portNumber = checkArgs(argc, argv);
    clientList  = malloc(sizeof(linkedList));
    initList(clientList);

    serverSocket = tcpServerSetup(portNumber);



    //begin server control
    serverControl(serverSocket);

    /* close the sockets */
    close(clientSocket);
    close(serverSocket);


    return 0;
}

void addNewClient(int serverSocket){
    int clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
    addToPollSet(clientSocket);
}

int  processClient(int clientSocket){
    return recvFromClient(clientSocket);
}

int serverControl(int serverSocket){
    int socket = 0;

    setupPollSet();
    addToPollSet(serverSocket);
    while(1){
        socket = pollCall(POLL_WAIT_FOREVER);
        if(socket == serverSocket){
            addNewClient(socket);
        }
        else{
            processClient(socket);
        }
    }
    return 0;
}

void handleDisconnection(int clientSocket){
    printf("Client on socket %d has disconnected\n", clientSocket);
    removeFromPollSet(clientSocket);
    int index = getSocket(clientList, clientSocket);
    if(index != -1) removeNode(clientList, index); //IF client in handle table, remove

}

void handleHandler(char *buf, int clientSocket){
    uint8_t handleLen = buf[1];
    char responseBuf[1];
    if(handleLen > 100 || handleLen == 0){
        printf("Client on Socket %d sent invalid handle\n", clientSocket);
        removeFromPollSet(clientSocket);
        responseBuf[0] = 3;
    } else if(getHandle(clientList, &buf[2], handleLen) != -1) {
        printf("Client on Socket %d sent used handle\n", clientSocket);
        responseBuf[0] = 3;
        removeFromPollSet(clientSocket);
    }  else {
        printf("Client on Socket %d sent valid handle\n", clientSocket);
        appendNode(clientList, clientSocket, &buf[2], handleLen);
        responseBuf[0] = 2;
    }
    if(sendPDU(clientSocket, (uint8_t*)responseBuf, 1) < 0){
        perror("send issue");
        exit(-1);
    }
}

int recvFromClient(int clientSocket)
{
    char buf[MAXBUF];
    int messageLen = 0;


    if ((messageLen = recvPDU(clientSocket, (uint8_t*)buf, MAXBUF)) < 0)
    {
        perror("recv call");
        exit(-1);
    }
    if(messageLen == 0){
        handleDisconnection(clientSocket);
        return 0;
    }

    uint8_t flag = buf[0];
    uint8_t responseflag;
    char sender[101];
    int client = 0;
    int targets;
    switch(flag){
        case 1  :
            handleHandler(buf, clientSocket);
            break;

        case 4  :
            memcpy(sender, &buf[2], buf[1]);
            sender[(int)buf[1]] = 0;
            for(client = 0; client < clientList->size; client++)
                if(strcmp(getHandleAtIndex(clientList,client),sender) != 0) {
                    sendPDU(getSocketAtIndex(clientList, client), (uint8_t *)buf, messageLen);
                }
            break;

        case 5  :
            targets = (int)buf[2+buf[1]];
            int index = 3+buf[1];
            for(client =0; client < targets; client++){
                int currentHandleLen = buf[index];
                index++;
                int targetIndex = getHandle(clientList, &buf[index], currentHandleLen);
                if(targetIndex == -1){
                    char badHandlePDU[1 + 1 + 100];
                    badHandlePDU[0] = 7;
                    badHandlePDU[1] = (char)currentHandleLen;

                    memcpy(&badHandlePDU[2], &buf[index], currentHandleLen);
                    sendPDU(clientSocket, (uint8_t *)badHandlePDU, 2+currentHandleLen);
                } else {
                    int targetSocket = getSocketAtIndex(clientList, targetIndex);

                    sendPDU(targetSocket, (uint8_t *)buf, messageLen);
                }
                index+=currentHandleLen;
            }

            break;

        case 8  :
            responseflag = 9;
            printf("Client on Socket %d requested termination\n", clientSocket);
            sendPDU(clientSocket, &responseflag, 1);
            handleDisconnection(clientSocket);
            break;

        case 10  :
            printf("Client on Socket %d requested client list\n", clientSocket);
            responseflag = 11;
            uint8_t responseBuf[5];
            responseBuf[0] = responseflag;
            uint32_t numClients = htonl(clientList->size);
            memcpy(&responseBuf[1], &numClients, 4);
            sendPDU(clientSocket, responseBuf, 5);
            uint8_t handlePacket[102];
            handlePacket[0] = 12;
            int i;
            for(i = 0; i < ntohl(numClients); i++){
                handlePacket[1] = (uint8_t)copyHandle(clientList, (char *)&handlePacket[2], i);
                sendPDU(clientSocket, handlePacket, handlePacket[1]+2);
            }

            responseflag = 13;
            sendPDU(clientSocket, &responseflag, 1);
            break;
    }

    return 1;
}

int checkArgs(int argc, char *argv[])
{
    // Checks args and returns port number
    int portNumber = 0;

    if (argc > 2)
    {
        fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
        exit(-1);
    }

    if (argc == 2)
    {
        portNumber = atoi(argv[1]);
    }

    return portNumber;
}

