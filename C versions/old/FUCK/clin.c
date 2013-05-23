// because this is c...
typedef int bool;
#define false 0
#define true 1

#include "NetworkHandlingTry.h"
//#include "BillysStrings for C v071022.h"
//#include "KBhandlingv071026.h"
#include <stdio.h>

mainH
{	
		// This block of code declares and initializes sockets
		// pointers are used to look pretier (no &sock junk)
	// delcares:
	// server, sock
	UDPsoc theAllocatedSocket;
	UDPsoc *sock;
	sock = &theAllocatedSocket;
	UDPsocConstruct(sock);

	// memory for use in sending and receiving messages
	char message[1000]="FISKALOOA";
	int len;
	int port=9999;
	char* hostname="localhost";

	Bind(sock, hostname, port);
	//	perror("Not connected");
	
	printf("ARG\n");
	getchar();
	
	//printf("Enter string: ");
	//gechars(message,'\n');	// gets a message that ends in '\n'

	SendUDP(sock, message,10);
	SendUDP(sock, "wtf liew",9);
	
	CloseUDP(sock);
	
}
