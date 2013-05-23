/* This file was created by Billy Tetrud. Use of this file is free.

 This may only be used on linux and systems witih compatible socket.h and netdb.h files. 
 I wrote this for a class which didn't let me use c++.. so its not really meant to be portable

  I used Beej's guide at http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
  Since this guide doesn't contain code I used word for word, I'm not going to properly site it.

 These are the defined functions and structures:

	typedef struct IPADD IPaddress;
	typedef struct TCPSOCKET TCPsoc;
	
	void TCPsocConstruct(TCPsoc* a)
	
	bool getPeerAdd(TCPsoc* a, IPaddress* ip)
	
	bool Listen(TCPsoc* a, int port)
	bool Accept(TCPsoc* listener, TCPsoc* b)
	bool Connect(TCPsoc* a, char* hostname, int port)
	inline int Recv(TCPsoc* a, void* container, int len)	
	inline int Send(TCPsoc* a, void* data, int len)

	inline void Close(TCPsoc* a)

	typedef struct UDPSOCKET

	void UDPsocConstruct(UDPsoc* a)
	void Bind(UDPsoc* a, char* hostname, int port)
	void BindRaw(UDPsoc* a, IPaddress* addr)
	inline int RecvUDP(UDPsoc* a, void* container, int len, IPaddress* into)
	inline int SendUDP(UDPsoc* a, void* data, int len)

	inline void CloseUDP(UDPsoc* a)

*/


	#include <sys/socket.h>
	#include <netdb.h>
	#include <net/if.h>

	typedef struct IPADD
	{	int host;            // 32-bit IPv4 host address 
   		int port;            // 16-bit protocol port 
	} IPaddress;
	
	// initialize - returns 0 on success
	// returns 1 on SDL_Init failure
	// returns 2 on SDLNet_Init failure
	int netInit()
	{	return 0;	// do nothing, no need to initialize
	}
	
	void netQuit()
	{	// do nothing, no need to quit
	}

	typedef struct TCPSOCKET
 	{	int sock;
	} TCPsoc;
	
	void TCPsocConstruct(TCPsoc* a)
	{	a->sock = socket(PF_INET, SOCK_STREAM, 0);	// tcp socket
	}
	
	// gets the address of the peer this socket is connected to
	// returns true on failure
	bool getPeerAdd(TCPsoc* a, IPaddress* ip)
	{	struct sockaddr_in addr;
		
		int len = sizeof(addr);
		if(getpeername(a->sock, (struct sockaddr*)&addr, &len))
			return true;
		
		ip->host = addr.sin_addr.s_addr;
		ip->port = addr.sin_port;
		return false;
	}
	
	
	// Turns socket into a server and listens on the port
	// returns true on fail
	bool Listen(TCPsoc* a, int port)
	{	struct sockaddr_in my_addr;

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
	}
	
	// Allows a server to accept a connection
	// even if connection returns non-NULL it might still not be connected
	// if that happens try testing if you can getPeerAdd
	// returns *true* on accept
	bool Accept(TCPsoc* listener, TCPsoc* b)
	{	struct sockaddr_in cliaddr;
		socklen_t *addrlen;
		
		int len = sizeof(cliaddr);
		if(-1==(b->sock = accept(listener->sock, (struct sockaddr *)&cliaddr, &len)))
		{	//printf("listener->sock: %d\n",listener->sock);
			return false;	// no accept
		}
		
		if (b->sock)
		{	IPaddress iptemp;
			
			if(getPeerAdd(b,&iptemp))		// get the clients IP and port number        
			{	return true;	// accept
			}
			else
			{	return false;	// no accept
			}
		}
	}
	
	// connects to a server
	// returns true on error
	bool Connect(TCPsoc* a, char* hostname, int port)
	{	struct sockaddr_in serverAdd;   // will hold the destination addr

		serverAdd.sin_family = AF_INET;          // host byte order
	    serverAdd.sin_port = htons(port);   // short, network byte order
	    struct hostent* host = gethostbyname(hostname);		// get IPaddress of hostname
		serverAdd.sin_addr.s_addr = ((struct in_addr*)(host->h_addr_list)[0])->s_addr;	// set IPaddress in structure
	    memset(&(serverAdd.sin_zero), 0, sizeof(serverAdd.sin_zero));	// set last part of struct to 0 (might be optional?)

		if(connect(a->sock, (struct sockaddr *)&serverAdd, sizeof(serverAdd)))
			return true; // for error
		//else
		return false;
	}
	
	// receives len bytes of data into container
	// blocks until len bytes is received
	// returns number of bytes received
	// if 0 is returned, the socket was closed from the opposite side
	// if 0 or len isn't returned there was an error
	inline int Recv(TCPsoc* a, void* container, int len)
	{	return recv(a->sock, container, len, 0); 
	}	
	
	// attempts to send len bytes of data
	// returns number of bytes sent
	// if len isn't returned, there was an error or disconnect
	inline int Send(TCPsoc* a, void* data, int len)
	{	return send(a->sock, data, len, 0); 
	}
	
	inline void Close(TCPsoc* a)
	{	close(a->sock);
	}

	typedef struct UDPSOCKET
	{	int sock;
		struct sockaddr_in dest;
	}UDPsoc;
	
	void UDPsocConstruct(UDPsoc* a)
	{	a->sock = socket(PF_INET, SOCK_DGRAM, 0);	// udp socket
		a->dest.sin_family = AF_INET;				// host byte order
		memset(&(a->dest.sin_zero), 0, sizeof(a->dest.sin_zero));	// set last part of struct to 0 (might be optional?)
	}

	// binds a UDPsoc to its destination
	// void because this can't fail
	void Bind(UDPsoc* a, char* hostname, int port)
	{	a->dest.sin_port = htons(port);   // short, network byte order
	    struct hostent* host = gethostbyname(hostname);		// get IPaddress of hostname
		a->dest.sin_addr.s_addr = ((struct in_addr*)(host->h_addr_list)[0])->s_addr;	// set IPaddress in structure

	}
	
	// binds a UDPsoc to its destination
	// void because this can't fail
	void BindRaw(UDPsoc* a, IPaddress* addr)
	{	a->dest.sin_port = addr->port;
		a->dest.sin_addr.s_addr = addr->host;

	}
	
	// receives len bytes of data into container
	// blocks until len bytes is received
	// returns number of bytes received
	// if 0 is returned, the socket was closed from the opposite side
	// if 0 or len isn't returned there was an error
	inline int RecvUDP(UDPsoc* a, void* container, int len, IPaddress* into)
	{	struct sockaddr_in addr;
		
		int fromLen = sizeof(addr);
		int bytes;
		bytes = recvfrom(a->sock, container, len, 0, (struct sockaddr*)&addr, &fromLen); 
		
		into->host = addr.sin_addr.s_addr;
		into->port = addr.sin_port; 
		
		return bytes;
	}	
	
	// attempts to send len bytes of data
	// returns number of bytes sent
	// if len isn't returned, there was an error or disconnect
	inline int SendUDP(UDPsoc* a, void* data, int len)
	{	return sendto(a->sock, data, len, 0, (struct sockaddr*)&(a->dest), sizeof(a->dest));
	}
	
	inline void CloseUDP(UDPsoc* a)
	{	close(a->sock);
	}
