#include "networkHandling for C v080215.h"
#include "KBhandling for C v080203.h"
#include "theStrings for C v080215.h"

mainH
{	int port = 9999;
	
	netInit();
	
	printf("Options:\n\tU. UDP\n\tT.TCP\n");
	if(gechn() == 'u')
	{	printf("UDP Options:\n\tL. listen\n\tC. connect\n");
		if(gechn() == 'l')	// listen
		{	UDPsoc serverMoo;
			UDPsoc* server = &serverMoo;
			UDPsocConstruct(server);
		
			if(OpenUDP(server, port))
				printf("couldn't open port\n");
			printf("Listening....\n");
			
			char message[100];
	
			IPaddress client;
			RecvUDP(server, message, 100, &client);
			printf("\tData:    %s\n", message);
			BindRaw(server, &client);
			Scopy(message, "Boo");
			SendUDP(server, message, 4);
		 	CloseUDP(server);
		}
		else	// connect
		{	UDPsoc sockMoo;
			UDPsoc* sock;
			UDPsocConstruct(sock);
		
			if(OpenUDP(sock, 0))
				printf("couldn't open any port!? wtf!?\n");
				
			char hostname[50];
			printf("Type host name: ");
			gecharsEnd(hostname, '\n');
			Bind(sock, hostname, port);
			
			char message[100];
			Scopy(message, "HEyo");
			printf("Sending... on port %d\n", port);
			SendUDP(sock, message, 5);
			if(0==RecvUDP(sock, message, 100, 0))
				printf("SHIT\n");
			printf("Got: %s\n", message);
			CloseUDP(sock);
		}
	}
	else
	{	printf("TCP Options:\n\tL. listen\n\tC. connect\n");
		if(gechn() == 'l')	// listen
		{	TCPsoc serverMoo;
			TCPsoc* server = &serverMoo;
			TCPsocConstruct(server);
		
			if(Listen(server, port))
				printf("couldn't open port\n");
			printf("Listening....\n");
			
			char message[100];
	
			TCPsoc connection;
			Accept(server, &connection);
			printf("Connected!\n");
			
			Recv(&connection, message, 100);
			printf("\tData:    %s\n", message);
			Scopy(message, "Boo");
			Send(&connection, message, 4);
		 	
			 Close(server);
			 Close(&connection);
		}
		else	// connect
		{	TCPsoc sockMoo;
			TCPsoc* sock = &sockMoo;
			TCPsocConstruct(sock);
		
			if(Connect(sock, "localhost", port))
				printf("couldn't connect!? wtf!?\n");
			printf("Connected\n");
			
			char message[100];
			Scopy(message, "HEyo");
			printf("Sending... on port %d\n", port);
			Send(sock, message, 5);
			if(0==Recv(sock, message, 100))
				printf("SHIT\n");
			printf("Got: %s\n", message);

			Close(sock);
		}
	}
		
	getchar();
}
