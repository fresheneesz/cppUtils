/* This file was created by Billy Tetrud. Use of this file is free as long as it is credited.
*/

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
	/* 
	Must use:
	
-lSDLmain 
-lSDL 
-lsdl_net
	*/

	#include <SDL/SDL_net.h>
	
	#include <windows.h>
	#define mainH int WINAPI WinMain(	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) 

#else	// linux and unix

	#include <sys/socket.h>
	#include <netdb.h>
	#include <net/if.h>
	#include <string.h>
	#include <unistd.h>
	
	#define mainH main(int argc, char** argv)
#endif

#include "definebool.h"
	
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
	int getPort(IPaddress* addr)
	{	return ((unsigned char*)&addr->port)[0]*256 + ((unsigned char*)&addr->port)[1];
	}

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

void getOwnIP(IPaddress* IP)
{	
	#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
		#warning SDL can't do getOwnIP god damnit
		exit(99);	//	heed the warning ^
	#else
		char name[256];
		gethostname(name, 256);
		struct hostent* host = gethostbyname(name);		
		IP->host = ((struct in_addr*)(host->h_addr_list)[0])->s_addr;
	#endif
}	

// some weird byte-order thing might be going on with this one...
/*void getOwnIPstring(char* IPadd)
{	char name[256];
	int temp1, temp2, temp3;
	IPaddress me;
	gethostname(name, 256);
	struct hostent* host = gethostbyname(name);		
	me.host = ((struct in_addr*)(host->h_addr_list)[0])->s_addr;
	
	temp1 = me.host%(256*256*256);
	temp2 = temp1%(256*256);
	temp3 = temp2%(256);
	
	Scopy2(IPadd, itS(temp3, name), ".");
	Scat2(IPadd, itS(temp2/(256), name), ".");
	Scat2(IPadd, itS(temp1/(256*256), name), ".");
	Scat(IPadd, itS(me.host/(256*256*256), name));
}	
*/

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
			
			// set timeout to 3 seconds
			struct timeval theTimeout;
			theTimeout.tv_sec = 3/1000000;
			theTimeout.tv_usec = 0; 
			
			setsockopt(a->sock, SOL_SOCKET, SO_RCVTIMEO, &theTimeout, sizeof(theTimeout));
			setsockopt(a->sock, SOL_SOCKET, SO_SNDTIMEO, &theTimeout, sizeof(theTimeout));
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
			
			if(-1 == listen(a->sock, 128))		// set "backlog" to 128
			{	return true;	// fail
			}
			return false;
		#endif
	}
	
	// Allows a server to accept a connection - non-blocking
	// even if connection returns non-NULL it might still not be connected
	// if that happens try testing if you can getPeerAdd
	// returns *true* on accept
	bool AcceptNB(TCPsoc* a, TCPsoc* contacter)
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
			{	return false;	// no accept
			}
			return true;	// AHHH MAYBE I SHOUDLNT DO THIS!! or maybe i should that just made things work....
		#endif
	}
	
	// Allows a server to accept a connection - blocking
	// even if connection returns non-NULL it might still not be connected
	// if that happens try testing if you can getPeerAdd
	// returns *true* on accept
	bool Accept(TCPsoc* a, TCPsoc* contacter)
	{	while(!AcceptNB(a, contacter))
		{	
			#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
				SDL_Delay(25);
			#else
				usleep(25);
			#endif
		}
		return true;
	}
	
	// Allows a server to accept a connection - blocking with time out
	// even if connection returns non-NULL it might still not be connected
	// if that happens try testing if you can getPeerAdd
	// returns *true* on accept, false on timeout
	bool AcceptTO(TCPsoc* a, TCPsoc* contacter, int timeoutInMicroseconds)
	{	while(!AcceptNB(a, contacter))
		{	if(timeoutInMicroseconds==0)
			{	return false;
			}
			#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
				SDL_Delay(25);
			#else
				usleep(25);
			#endif
			timeoutInMicroseconds--;
		}
		return true;
	}
	
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
			if(host==0)
			{	return true;	// error
			}
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
	
	
	// gets num characters from socket and puts them in 'a'
	// returns number actually received 
	int RecvNum(TCPsoc* a, char* container, int num)
	{	int n, test;
		for(n=0; n<num; n++)
		{	test = Recv(a, &(container[n]), 1);
			if(test==-1 || test==0)
			{	container[n]=0;
				return test;
			}
		}
		container[n]=0;
		return n;
	}
	
	// gets chacters until a certain one
	// returns 0 if successful before connection closed, -1 for error, and positive integer if successful and connection still open
	int RecvEnd(TCPsoc* a, char* container, char end, int maxN)
	{	int n, test;
		for(n=0; n<maxN; n++)
		{	test = Recv(a, &(container[n]), 1);
			if(test==-1 || test==0)
			{	container[n]=0;
				return test;
			}
			if(container[n]==end)
			{	container[n+1] = 0;
				return n+1;
			}
		}
		container[n]=0;
		return n;
	}
	
	// not meant to be used publically
	// this function was jacked from BillysStrings....h
	bool isIn_forNetworkHandling(char a, char* ins)
	{	int m;
		for(m=0; ins[m]!=0; m++)
		{	if(a==ins[m])
			{	return true;
			}
		}
		return false;
	}
	
	// gets chacters until reaching one of a number of characters
	// returns the number of characters gotten
	int RecvEnds(TCPsoc* a, char* container, char* ends)
	{	int n, test;
		for(n=0; 1; n++)
		{	test = Recv(a, &(container[n]), 1);
			if(test==-1 || isIn_forNetworkHandling(container[n], ends))
				break;
		}
		container[n]=0;
		return n;
	}
	
	
	


typedef struct
{	
	#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
		UDPsocket sock;
		UDPpacket packOut, packIn;
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
			IPaddress* temp;	// there is a reason i do this - the reason is cause the below might return weird memory
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

	// Opens on a UDP port
	// Opening on port 0 will open a random unused port
	// returns true on failure
	bool OpenUDP(UDPsoc* a, int port)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			a->sock = SDLNet_UDP_Open(port);
			if(a->sock == 0)
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
	// returns true on failure
	bool Bind(UDPsoc* a, char* hostname, int port)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			if(SDLNet_ResolveHost(&((a->packOut).address), hostname, port) != 0)
				return true;
		#else
			a->dest.sin_port = htons(port);   // short, network byte order
			struct hostent* host = gethostbyname(hostname);		// get IPaddress of hostname
			if(host == 0)
				return true;
			a->dest.sin_addr.s_addr = ((struct in_addr*)(host->h_addr_list)[0])->s_addr;	// set IPaddress in structure
		#endif
	
		return false;
	}
	
	// binds a UDPsoc to its destination
	// void because this can't fail
	void BindRaw(UDPsoc* a, IPaddress* addr)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			(a->packOut).address.host = addr->host;
			(a->packOut).address.port = addr->port;
		#else
			a->dest.sin_port = addr->port;
			a->dest.sin_addr.s_addr = addr->host;
		#endif

	}
	
	// receives len bytes of data into container
	// blocks until len bytes is received
	// returns number of bytes received
	// if 0 or len isn't returned there was an error
	inline int RecvUDP(UDPsoc* a, void* container, int len, IPaddress* into)
	{	
		#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
			(a->packIn).data = (Uint8*)container;
			int ready;	
			while(1)
			{	ready = SDLNet_UDP_Recv(a->sock, &(a->packIn));
				if(ready!=0)
					break;
				SDL_Delay(10);
			}
			
			if(ready==-1)
				return 0;
			if(into!=0)
			{	into->host = (a->packIn).address.host;
				into->port = (a->packIn).address.port;
			}
			
			return (a->packIn).len;
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
			(a->packOut).data = (Uint8*)data;
			(a->packOut).len = len;
			(a->packOut).maxlen = len;
			
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
