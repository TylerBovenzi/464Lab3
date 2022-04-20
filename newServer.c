/******************************************************************************
* tcp_server.c
*
* CPE 464 - Program 1
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "networks.h"
#include "pdu.h"
#define MAXBUF 1024
#define DEBUG_FLAG 1

int recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	int serverSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   //socket descriptor for the client socket
	int portNumber = 0;
	
	portNumber = checkArgs(argc, argv);
	
	//create the server socket
	serverSocket = tcpServerSetup(portNumber);

	// wait for client to connect
	clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);

	while( recvFromClient(clientSocket) > 0){}
	
	/* close the sockets */
	close(clientSocket);
	close(serverSocket);

	
	return 0;
}

int recvFromClient(int clientSocket)
{
	char buf[MAXBUF];
	int messageLen = 0;

    //do {
        //now get the data from the client_socket
        memset(buf, 0, MAXBUF);
        if ((messageLen = recvPDU(clientSocket, buf, MAXBUF)) < 0) {
            perror("recv call");
            exit(-1);
        }
       if(messageLen == 0){
            return 0;
        }
        if(!strcmp(buf, "exit")){
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

