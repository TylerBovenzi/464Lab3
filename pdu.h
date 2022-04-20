#include <unistring/stdint.h>
#include <unistring/stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

int recvPDU(int clientSocket, uint8_t * dataBuffer, int bufferLen);
int sendPDU(int socketNumber, uint8_t * dataBuffer, int lengthOfData);

