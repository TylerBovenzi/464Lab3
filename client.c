/******************************************************************************
* myClient.c
*
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "pollLib.h"
#include "networks.h"
#include "pdu.h"
#define MAXBUF 1024
#define DEBUG_FLAG 1


int sendToServer(int socketNum);
int readFromStdin(char * buffer);
int clientControl(int serverSocket);
int readHandle(char * buffer);
void recvUserInput(int socketNum);
int setUpConnection(char *handleInput, int socketNum);
void checkArgs(int argc, char * argv[]);

char *myHandle;
int myHandleLength;

int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor



	checkArgs(argc, argv);

	socketNum = tcpClientSetup(argv[2], argv[3], DEBUG_FLAG);

    if(!setUpConnection(argv[1],socketNum)) return 0;
    printf("$: ");
    fflush(stdout);
    clientControl(socketNum);

    //while(sendToServer(socketNum ) > 0){}

	close(socketNum);
	
	return 0;
}

int recvFromServer(int serverSocket)
{
    char buf[MAXBUF];
    int messageLen = 0;

    if ((messageLen = recvPDU(serverSocket, (uint8_t*)buf, MAXBUF)) < 0)
    {
        perror("recv call");
        exit(-1);
    }

    if(messageLen == 0){
        printf("\nServer Closed Unexpectedly\n");
        return 0;
    }

    uint8_t flag = buf[0];
    uint32_t numClients;
    char handleBuf[101];
    int senderLen;
    int index;
    int targets;
    int numTargets;
    switch(flag){
        case 4  :
            senderLen = buf[1];
            printf("\n");
            for(index = 0; index < senderLen; index++){
                printf("%c", buf[index+2]);
            }
            printf(": ");
            for(index = senderLen + 2; index < messageLen; index++){
                printf("%c", buf[index]);
            }
            printf("\n$: ");
            fflush(stdout);
            break;

        case 5  :
            senderLen = buf[1];
            printf("\n");
            index =0;
            while(index<senderLen){
                printf("%c", buf[index+2]);
                index++;
            }
            printf(": ");
            numTargets = buf[senderLen + 2];
            index = senderLen + 3;
            for(targets = 0; targets < numTargets; targets++){
                index += 1 + buf[index];
            }
            index++;
            while(index <messageLen){
                printf("%c", buf[index]);
                index++;
            }
            printf("\n$: ");
            fflush(stdout);
            break;

        case 7  :
            memcpy(handleBuf, &buf[2],buf[1]);
            handleBuf[buf[1]] = 0;
            printf("\nClient with handle %s does not exist", handleBuf);
            printf("\n$: ");
            fflush(stdout);
            break;

        case 9  :
            printf("\nServer Granted Termination. Exiting..\n");
            return 0;
        case 11  :
            memcpy(&numClients, &buf[1],4);
            numClients = ntohl(numClients);

            printf("Number of clients: %d\n", numClients);
            fflush(stdout);
            break;

        case 12  :
            memcpy(handleBuf, &buf[2],buf[1]);
            handleBuf[buf[1]] = 0;
            printf("\t%s\n", handleBuf);
            fflush(stdout);
            break;

        case 13  :
            printf("$: ");
            fflush(stdout);
            break;
    }

    return 1;
}


int clientControl(int serverSocket){
    int socket = 0;

    setupPollSet();
    addToPollSet(serverSocket);
    addToPollSet(STDIN_FILENO);
    while(1){
        socket = pollCall(POLL_WAIT_FOREVER);
        if(socket == serverSocket){
            if(!recvFromServer(serverSocket)) break;
        }
        else{
            recvUserInput(serverSocket);

            fflush(stdout);

        }
    }
    return 0;
}

int setUpConnection(char *handleInput, int socketNum){
    char sendBuf[2+MAXBUF];   //data buffer
    int sent = 0;            //actual amount of data sent/* get the data and send it   */

    sendBuf[0] = (char)1;

    unsigned long size = strlen(handleInput);
    myHandle = handleInput;
    myHandleLength = (int)size;
    if(size == 0 || size > 100){
        printf("Bad Handle Length. Exiting..\n");
        return 0;
    }
    sendBuf[1] =(char) size;
    memcpy(&sendBuf[2], handleInput, size);

    sent =  sendPDU(socketNum, (uint8_t *)sendBuf, size + 2);
    if (sent < 0)
    {
        perror("send call");
        exit(-1);
    }

    char response;
    int messageLen = 0;


    if ((messageLen = recvPDU(socketNum, (uint8_t*)&response, MAXBUF)) < 0)
    {
        perror("recv call");
        exit(-1);
    }
    if(response == 2) {
        printf("Successfully Connected\n");
        return 1;
    }
    else {
        printf("Handle in use. Exiting..\n");
        return 0;
    }
}
void userExit(int socketNum){
    uint8_t Flag = 8;
    sendPDU(socketNum, &Flag, 1);
    printf("Waiting for Server ACK to terminate..\n");
}

void userList(int socketNum){
    uint8_t Flag = 10;
    sendPDU(socketNum, &Flag, 1);
}

void userBroadcast(int socketNum, int inLen, char *inBuf){
    int index = 3;
    int outIndex;
    uint8_t outBuf[302];
    outBuf[0] = 4;
    outBuf[1] = (uint8_t)myHandleLength;
    memcpy(&outBuf[2], myHandle, myHandleLength);
    if(inLen==2 || inLen==3){
        sendPDU(socketNum, outBuf, myHandleLength+2);
        return;
    }
    outIndex = 2+myHandleLength;
    while(inBuf[index]) {
        if(outIndex == 2+myHandleLength+200){
            sendPDU(socketNum, outBuf, outIndex);
            outIndex = 2+myHandleLength;
        }
        outBuf[outIndex] = inBuf[index];
        index++;
        outIndex++;
    }


    sendPDU(socketNum, outBuf, outIndex);
}

void userMulticast(int socketNum, int inLen, char *inBuf){
    int outIndex;
    int index = 5;
    int handlelength = 0;
    uint8_t outBuf[1 + 1 + 100 + 9 + 900 + 200];    //Flag + send handle + 9 client handle + message
    outBuf[0] = 5;
    outBuf[1] = (uint8_t)myHandleLength;
    memcpy(&outBuf[2], myHandle, myHandleLength);
    outIndex = 2+myHandleLength;
    int targets = inBuf[3] - 48;
    int target;
    if((targets > 9) || (targets < 1)){
        printf("Number of targets must be 1-9\n");
        return;
    }
    if((inBuf[4] != ' ')){
        printf("Invalid Syntax in Target Declaration\n");
        return;
    }
    outBuf[outIndex] = (uint8_t) targets;
    outIndex++;

    while(inBuf[index] == ' '){
        index++;
    }


    for(target = 0; target<targets; target++){
        handlelength = 0;
        if((inBuf[index] == ' ') || (!inBuf[index])){
            printf("Incorrect Number of Arguments\n");
            return;
        }
        int currentStart = outIndex;
        while((inBuf[index]) && (inBuf[index] != ' ')){
            if(handlelength > 99){
                printf("Target %d is invalid\n", 1+target);
                return;
            }

            outIndex++;
            outBuf[outIndex] = inBuf[index];
            index++;
            handlelength++;

        }
        if(!inBuf[index]) {
            if(target == targets-1 ) {
                outBuf[currentStart] = handlelength;
                outBuf[outIndex + 1] = 0;
                outIndex++;
                break;
            }
            else {
                printf("Incorrect Number of Arguments\n");
                return;
            }
        }
        outBuf[currentStart] = handlelength;
        outIndex++;

        index++;
    }

    int pdudatastart = outIndex;
    outIndex++;

    while(inBuf[index]) {
        if(outIndex == pdudatastart+1+200){
            outBuf[pdudatastart] = outIndex - (pdudatastart);
            sendPDU(socketNum, outBuf, outIndex);
            outIndex = pdudatastart+1;
        }
        outBuf[pdudatastart] = outIndex - (pdudatastart);
        outBuf[outIndex] = inBuf[index];
        index++;
        outIndex++;
    }

    sendPDU(socketNum, outBuf, outIndex);
}

void sendInputError(){
    printf("Please use a valid command:\n\t %%M: Multicast\n\t %%B: Broadcast\n\t %%L: List Handles\n\t %%E: Exit\n$: ");
    fflush(stdout);
}


void recvUserInput(int socketNum){

	char inBuf[MAXBUF];
    int inLen = readFromStdin(inBuf);
	if(inLen == 0){
        return;
    }

    if(inBuf[0] != '%'){
        sendInputError();
        return;
    }

    if(inLen > 3){
        if(inBuf[2]!=' ') {
            sendInputError();
            return;
        }
    }

    switch(inBuf[1]) {
        case 'M'  :
            userMulticast(socketNum, inLen, inBuf);
            break;

        case 'm'  :
            userMulticast(socketNum, inLen, inBuf);
            break;

        case 'B'  :
            userBroadcast(socketNum, inLen, inBuf);
            break;

        case 'b'  :
            userBroadcast(socketNum, inLen, inBuf);
            break;

        case 'L'  :
            userList(socketNum);
            return;

        case 'l'  :
            userList(socketNum);
            return;

        case 'E'  :
            userExit(socketNum);
            return;

        case 'e'  :
            userExit(socketNum);
            return;

        default:
            sendInputError();
            return;
    }

    printf("$: ");

}

int readFromStdin(char * buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}
	
	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;
	
	return inputLen;
}

int readHandle(char * buffer)
{
    char aChar = 0;
    int inputLen = 0;

    // Important you don't input more characters than you have space
    buffer[0] = '\0';
    printf("Enter Handle: ");
    while (inputLen < (MAXBUF - 1) && aChar != '\n')
    {
        aChar = getchar();
        if (aChar != '\n')
        {
            buffer[inputLen] = aChar;
            inputLen++;
        }
    }

    // Null terminate the string
    buffer[inputLen] = '\0';
    inputLen++;

    return inputLen;
}

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 4)
	{
		printf("usage: %s handle host-name port-number \n", argv[0]);
		exit(1);
	}
}
