
#include "NetworkHandlingTry.h"
//#include "BillysStrings for C v071022.h"
#include <stdio.h>

mainH
{		// This block of code declares and initializes sockets
		// pointers are used to look pretier (no &sock junk)
	// delcares:
	// server, sock
	UDPsoc theAllocatedSocket;
	UDPsoc *sock;
	sock = &theAllocatedSocket;
	UDPsocConstruct(sock);
	
	// memory for use in sending and receiving messages
	char message[1000];	
	int len;
	int port=9999;

	printf("Listening on %d.\n",port);
	if(OpenUDP(sock, port))
	{	printf("Why me?\n");
	}

	//printf("CONNECTED\n");
	len = RecvUDP(sock, message,10, 0);
	printf("%s\n", message);
	len = RecvUDP(sock, message,9, 0);
	printf("%s\n", message);
	
	CloseUDP(sock);
	
}
