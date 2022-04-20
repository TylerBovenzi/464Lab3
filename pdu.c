
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include "pdu.h"
#include <arpa/inet.h>

    int recvPDU(int clientSocket, uint8_t * dataBuffer, int bufferLen){
        int recieveLength = 0;
        uint16_t pduLength = 0;

        if((recieveLength = recv(clientSocket, dataBuffer, 2, MSG_WAITALL)) <= 0){

            return 0;
        }
        memcpy(&pduLength, dataBuffer, 2);
        pduLength = ntohs(pduLength);

        if(pduLength > bufferLen){
            printf("PDU Length > Buffer Length");
            exit(-1);
        }

        if((recieveLength = recv(clientSocket, dataBuffer, pduLength-2, MSG_WAITALL)) < 0){

            perror("recieve error2");
            exit(-1);
        }
        if(recieveLength == 0){
            return 0;
        }

        return recieveLength;
    }

int sendPDU(int socketNumber, uint8_t * dataBuffer, int lengthOfData){
    uint16_t pduLength = lengthOfData + 2;
    uint16_t npdulength = htons(pduLength);
    uint8_t *data = malloc(sizeof(uint8_t)*pduLength);
    memcpy(data, &npdulength, 2);
    memcpy(data+2, dataBuffer, lengthOfData);

    if(send(socketNumber, data, pduLength, MSG_WAITALL) <0) {
        perror("Send Error");
        exit(-1);
    }

    return lengthOfData;
}
