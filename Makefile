# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019

CC= gcc
CFLAGS= -g -Wall
LIBS = 


all:   myClient myServer

myClient: myClient.c networks.o gethostbyname.o
	$(CC) $(CFLAGS) -o myClient myClient.c pdu.c networks.o gethostbyname.o $(LIBS)

myServer: newServer.c networks.o gethostbyname.o
	$(CC) $(CFLAGS) -o newServer myServer.c pdu.c networks.o gethostbyname.o $(LIBS)

myPDU: pdu.c networks.o gethostbyname.o
	$(CC) $(CFLAGS) -o pdu myServer.c networks.o gethostbyname.o $(LIBS)
.c.o:
	gcc -c $(CFLAGS) $< -o $@ $(LIBS)

cleano:
	rm -f *.o

clean:
	rm -f myServer myClient *.o




