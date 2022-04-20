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

#define MAXBUF 1024
#define DEBUG_FLAG 1

int recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);
int serverControl();

int main(int argc, char *argv[])
{
    int serverSocket = 0;   //socket descriptor for the server socket
    int clientSocket = 0;   //socket descriptor for the client socket
    int portNumber = 0;

    portNumber = checkArgs(argc, argv);


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
        printf("Client on socket %d has disconnected\n", clientSocket);
        removeFromPollSet(clientSocket);
        return 0;
    }
    if(!strcmp(buf, "exit")){
        printf("Client on socket %d has disconnected\n", clientSocket);
        removeFromPollSet(clientSocket);
        return -1;
    }

    printf("Message received, length: %d Data: %s\n", messageLen, buf);
    if (sendPDU(clientSocket, (uint8_t*)buf, MAXBUF) < 0){
        perror("send issue");
        exit(-1);
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

