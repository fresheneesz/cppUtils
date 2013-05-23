#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
	/* 
	Must use:
	
-lSDLmain 
-lSDL 
-lsdl_net
	*/
	
	#include <SDL/SDL.h>
	#include <SDL/SDL_net.h>
	
	#include <windows.h>
	#define mainH int WINAPI WinMain(	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) 

#else	// linux and unix

	#include <sys/socket.h>
	#include <netdb.h>
	#include <net/if.h>
	#include <string.h>
	
	#define mainH main(int argc, char** argv)
#endif

	
// initialize - returns 0 on success
// returns 1 on SDL_Init failure
// returns 2 on SDLNet_Init failure
int netInit()
{	
	#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
		if(!SDL_Init(0))
		{	if(SDLNet_Init()<0)
			{	return 2;
			}
		}else 
		{	return 1; 
		}
		return 0;
	#else	// nothing for linux
		return 0;
	#endif
}

// quits SDL and SDL_net
void netQuit()
{	
	#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
		SDLNet_Quit(); 
		SDL_Quit();
	#else	// nothing for linux
	#endif
}

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
	// already defined in SDL_net
#else
	typedef struct
	{	int host;            // 32-bit IPv4 host address 
		int port;            // 16-bit protocol port 
	} IPaddress;
#endif

// puts the first IP address associated with hostname into IP
void Resolve(IPaddress* IP, char* hostname)
{	
	#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
		SDLNet_ResolveHost(IP,hostname,0);
	#else
		// get IPaddress of hostname
		struct hostent* host = gethostbyname(hostname);		
		// set IPaddress in structure
		IP->host = ((struct in_addr*)(host->h_addr_list)[0])->s_addr;	
	#endif
}




typedef struct 
{	
	#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
		TCPsocket sock;
	#else
		int sock;
	#endif
} TCPsoc;

	void TCPsocConstruct(TCPsoc* a)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			// nothing for SDL_net
		#else
			a->sock = socket(PF_INET, SOCK_STREAM, 0);
		#endif
	}
	
	// gets the address of the peer this socket is connected to
	// returns true on failure
	bool getPeerAdd(TCPsoc* a, IPaddress* ip)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			IPaddress* temp;
			temp = SDLNet_TCP_GetPeerAddress(a->sock);	// not sure what kind of memory this returns
			if(temp==NULL)
				return true;
			ip->host = temp->host;
			ip->port = temp->port;
			return false;
		#else
			struct sockaddr_in addr;
			
			int len = sizeof(addr);
			if(getpeername(a->sock, (struct sockaddr*)&addr, &len))
				return true;
			
			ip->host = addr.sin_addr.s_addr;
			ip->port = addr.sin_port;
			return false;
		#endif
	}
	
	
	// Turns socket into a server and listens on the port
	// returns true on fail
	bool Listen(TCPsoc* a, int port)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			IPaddress portHolder;
			TCPsocket portHolderSocket;
			
			if(SDLNet_ResolveHost(&portHolder,NULL,port))		// Resolve the string host, and fill in the IPaddress 
			{	a->sock=0;
				return true;	// fail.
			}
			portHolderSocket = SDLNet_TCP_Open(&portHolder); 		// open the tcp server socket 
			if(!portHolderSocket)
			{	a->sock=0;
				return true;	// fail.
			} 
			a->sock = portHolderSocket;
			return false;	// no fail
		#else
			struct sockaddr_in my_addr;

			my_addr.sin_family = AF_INET;         // host byte order
			my_addr.sin_port = htons(port);     // short, network byte order
			my_addr.sin_addr.s_addr = htons(INADDR_ANY);
			memset(&(my_addr.sin_zero), '\0', sizeof(my_addr.sin_zero));
	
			if(-1 == bind(a->sock, (struct sockaddr *)&my_addr, sizeof(my_addr)))
				return true;
			//else
				return false;
			
			if(-1 == listen(a->sock, 128))		// set "backlog" to 128
			{	return true;	// fail
			}
			return false;
		#endif
	}
	
	// Allows a server to accept a connection
	// even if connection returns non-NULL it might still not be connected
	// if that happens try testing if you can getPeerAdd
	// returns *true* on accept
	bool Accept(TCPsoc* a, TCPsoc* contacter)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			contacter->sock = SDLNet_TCP_Accept(a->sock);		//try to accept a connection
			if(contacter->sock == NULL)
				return false;
			else
				return true;
		#else
			struct sockaddr_in cliaddr;
			socklen_t *addrlen;
			
			int len = sizeof(cliaddr);
			if(-1==(contacter->sock = accept(a->sock, (struct sockaddr *)&cliaddr, &len)))
			{	//printf("listener->sock: %d\n",listener->sock);
				return false;	// no accept
			}
			
			if (contacter->sock)
			{	IPaddress iptemp;
				
				if(getPeerAdd(contacter, &iptemp))		// get the clients IP and port number        
				{	return true;	// accept
				}
				else
				{	return false;	// no accept
				}
			}
		#endif
	}
	
	
	// Allows a server to accept a connection
	// even if connection returns non-NULL it might still not be connected
	// if that happens try testing if you can getPeerAdd
	// returns *true* on accept
	/*bool AcceptRigorous(TCPsoc* listener, TCPsoc* b)
	{	struct sockaddr_in cliaddr;
		socklen_t *addrlen;
		
		int len = sizeof(cliaddr);
		if(-1==(b->sock = accept(listener->sock, (struct sockaddr *)&cliaddr, &len)))
		{	//printf("listener->sock: %d\n",listener->sock);
			return false;	// no accept
		}
		
		if (b->sock != -1)
		{	IPaddress iptemp;
			
			if(getPeerAdd(b,&iptemp))		// get the clients IP and port number        
				return true;	// accept
			else
				return false;	// no accept
		}
		else
			return false;	// no accept
	}*/
	
	// connects to a server
	// returns true on error
	bool Connect(TCPsoc* a, char* hostname, int port)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			IPaddress ip;
			if(SDLNet_ResolveHost(&ip,hostname,port)<0)	// Resolve the argument into an IPaddress type 
			{	return true;
			}
			a->sock=SDLNet_TCP_Open(&ip);  // open socket 
			if(a->sock==NULL)
				return true;
			//else
			return false;
		#else
			struct sockaddr_in serverAdd;   // will hold the destination addr

			serverAdd.sin_family = AF_INET;          // host byte order
		    serverAdd.sin_port = htons(port);   // short, network byte order
		    struct hostent* host = gethostbyname(hostname);		// get IPaddress of hostname
			serverAdd.sin_addr.s_addr = ((struct in_addr*)(host->h_addr_list)[0])->s_addr;	// set IPaddress in structure
		    memset(&(serverAdd.sin_zero), 0, sizeof(serverAdd.sin_zero));	// set last part of struct to 0 (might be optional?)
	
			if(connect(a->sock, (struct sockaddr *)&serverAdd, sizeof(serverAdd)))
				return true; // for error
			//else
			return false;
		#endif
	}
	
	// receives len bytes of data into container
	// blocks until len bytes is received
	// returns number of bytes received
	// if 0 is returned, the socket was closed from the opposite side
	// if 0 or len isn't returned there was an error
	inline int Recv(TCPsoc* a, void* container, int len)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			return SDLNet_TCP_Recv(a->sock, container, len);
		#else
			return recv(a->sock, container, len, 0); 
		#endif
	}	
	
	// attempts to send len bytes of data
	// returns number of bytes sent
	// if len isn't returned, there was an error or disconnect
	inline int Send(TCPsoc* a, void* data, int len)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			return SDLNet_TCP_Send(a->sock, data, len);
		#else
			return send(a->sock, data, len, 0); 
		#endif
	}
	
	inline void Close(TCPsoc* a)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			SDLNet_TCP_Close(a->sock);
		#else
			close(a->sock);
		#endif
	}
	
	


typedef struct
{	
	#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
		UDPsocket sock;
		UDPpacket packIn, packOut;
	#else
		int sock;
		struct sockaddr_in dest;
	#endif
}UDPsoc;
	
	void UDPsocConstruct(UDPsoc* a)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			// nothing needs to happen
		#else
			a->sock = socket(PF_INET, SOCK_DGRAM, 0);
			a->dest.sin_family = AF_INET;				// host byte order
			memset(&(a->dest.sin_zero), 0, sizeof(a->dest.sin_zero));	// set last part of struct to 0 (might be optional?)
		#endif
	}
	
	// gets the address of the peer this socket is connected to
	// returns true on failure
	bool getPeerAddUDP(UDPsoc* a, IPaddress* ip)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			IPaddress* temp;
			temp = SDLNet_UDP_GetPeerAddress(a->sock, -1);	// not sure what kind of memory this returns
			if(temp==NULL)
				return true;
			ip->host = temp->host;
			ip->port = temp->port;
			return false;
		#else
			struct sockaddr_in addr;
			
			int len = sizeof(addr);
			if(getpeername(a->sock, (struct sockaddr*)&addr, &len))
				return true;
			
			ip->host = addr.sin_addr.s_addr;
			ip->port = addr.sin_port;
			return false;
		#endif
	}

	bool OpenUDP(UDPsoc* a, int port)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			a->sock = SDLNet_UDP_Open(port);
			if(a->sock==NULL)
				return true;
		#else
			struct sockaddr_in my_addr;
	
			my_addr.sin_family = AF_INET;         // host byte order
			my_addr.sin_port = htons(port);     // short, network byte order
			my_addr.sin_addr.s_addr = htons(INADDR_ANY);
			memset(&(my_addr.sin_zero), '\0', sizeof(my_addr.sin_zero));
	
			if(-1 == bind(a->sock, (struct sockaddr *)&my_addr, sizeof(my_addr)))
				return true;
		#endif
		
		return false;
	}

	// binds a UDPsoc to its destination (for sending)
	// void because this can't fail
	void Bind(UDPsoc* a, char* hostname, int port)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			if (SDLNet_ResolveHost(&(a->packOut.address), hostname, port))	// Resolve server name  
			{	//return true		// fail
			}	
		#else
			a->dest.sin_port = htons(port);   // short, network byte order
			struct hostent* host = gethostbyname(hostname);		// get IPaddress of hostname
			a->dest.sin_addr.s_addr = ((struct in_addr*)(host->h_addr_list)[0])->s_addr;	// set IPaddress in structure
		#endif

	}
	
	// binds a UDPsoc to its destination
	// void because this can't fail
	void BindRaw(UDPsoc* a, IPaddress* addr)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			a->packOut.address.host = addr->host;
			a->packOut.address.port = addr->port;
		#else
			a->dest.sin_port = addr->port;
			a->dest.sin_addr.s_addr = addr->host;
		#endif

	}
	
	// receives len bytes of data into container
	// blocks until len bytes is received
	// returns number of bytes received
	// if 0 is returned, the socket was closed from the opposite side
	// if 0 or len isn't returned there was an error
	inline int RecvUDP(UDPsoc* a, void* container, int len, IPaddress* into)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			a->packIn.data = (Uint8*)container;
			int ready=0;
			while(ready==0)
			{	ready=SDLNet_UDP_Recv(a->sock, &(a->packIn));
				SDL_Delay(10);
			}

			if(into!=0)
			{	into->host = a->packIn.address.host;
				into->port = a->packIn.address.port;
			}
			
			return a->packIn.len;
		#else
			struct sockaddr_in addr;
			
			int fromLen = sizeof(addr);
			int bytes;
			bytes = recvfrom(a->sock, container, len, 0, (struct sockaddr*)&addr, &fromLen); 
			
			if(into!=0)
			{	into->host = addr.sin_addr.s_addr;
				into->port = addr.sin_port; 
			}
			
			return bytes;
		#endif
	}	
	
	// attempts to send len bytes of data
	// returns number of bytes sent
	// if len isn't returned, there was an error or disconnect
	inline int SendUDP(UDPsoc* a, void* data, int len)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			a->packOut.data = (Uint8*)data;
			a->packOut.len = len;
			a->packOut.maxlen = len;
			
			if(SDLNet_UDP_Send(a->sock, -1, &(a->packOut)) == 0)
			{	return true;
			}
		#else
			if(sendto(a->sock, data, len, 0, (struct sockaddr*)&(a->dest), sizeof(a->dest)) == -1)
				return true;
		#endif
		return false; // no error
	}
	
	inline void CloseUDP(UDPsoc* a)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			SDLNet_UDP_Close(a->sock);
		#else
			close(a->sock);
		#endif
	}
