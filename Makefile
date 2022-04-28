# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019

CC= gcc
CFLAGS= -g -Wall
LIBS = pollLib.c safeUtil.c list.c


all:   myClient myServer

myClient: client.c networks.o gethostbyname.o
	$(CC) $(CFLAGS) -o client client.c pdu.c networks.o gethostbyname.o $(LIBS)

myServer: server.c networks.o gethostbyname.o
	$(CC) $(CFLAGS) -o server server.c pdu.c networks.o gethostbyname.o $(LIBS)

myPDU: pdu.c networks.o gethostbyname.o
	$(CC) $(CFLAGS) -o pdu server.c networks.o gethostbyname.o $(LIBS)


#.c.o:
#	gcc -c $(CFLAGS) $< -o $@ $(LIBS)

cleano:
	rm -f *.o

clean:
	rm -f server client *.o




