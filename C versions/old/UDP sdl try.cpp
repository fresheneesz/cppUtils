#include <stdio.h>
#include <stdlib.h>
#include "KBhandling for C v080203.h"
#include "theStrings for C v080215.h"
#include "networkHandling for C v080215.h"
 
//#include <SDL/SDL_net.h>

//#include <windows.h>
//int WINAPI WinMain(	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) 
mainH
{	int port=9999;
	
	if(gechn() == 'l')	// listen
	{
		UDPsoc sd;       /* Socket descriptor */
			
		netInit();
	 
		if (OpenUDP(&sd, port))		/* Open a socket on a port */
		{	printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		}
		printf("Listening on port %d\n", port);
		char moo[512];
	 
	 	int quit=0;
		while (!quit)
		{	/* Wait a packet. UDP_Recv returns != 0 if a packet is coming */

			RecvUDP(&sd, moo, 500, 0);
			printf("UDP Packet incoming\n");
			printf("\tData:    %s\n", moo);
			
			
		}
	 
		/* Clean and exit */
		//SDLNet_FreePacket(p);
		SDLNet_Quit();
		getchar();
		return EXIT_SUCCESS;
	}
	else 
	{
		UDPsoc sd;
		
		char host[]="localhost";
	 
		netInit();
		
		if (OpenUDP(&sd, 0))	// Open a socket on random port
		{	printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		}
	 
		if (Bind(&sd, host, port))	// Resolve server name  
		{	printf("SDLNet_ResolveHost(%s %d): %s\n", host, port, SDLNet_GetError());
		}
		char moo[512];
	 
		int quit=0;
		while (!quit)
		{	printf("Fill the buffer\n>");
			gecharsEnds(moo, "\n");
			SendUDP(&sd, moo, strlen(moo)+1); /* This sets the p->channel */
				
		}
		
		SDLNet_Quit();
		return EXIT_SUCCESS;
	}
}
