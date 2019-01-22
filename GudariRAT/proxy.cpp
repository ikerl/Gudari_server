/*
*  A simple TCP proxy
*  by Martin Broadhurst (www.martinbroadhurst.com)
*  Usage: tcpproxy local_host local_port remote_host remote_port
*/

#include "stdafx.h"

#define BACKLOG  10      /* Passed to listen() */
#define BUF_SIZE 4096    /* Buffer for  transfers */

unsigned int transfer(int from, int to)
{
	char buf[BUF_SIZE];
	unsigned int disconnected = 0;
	size_t bytes_read, bytes_written;
	bytes_read = recv(from, buf, BUF_SIZE,0);
	if (bytes_read == 0) {
		disconnected = 1;
	}
	else {
		bytes_written = send(to, buf, bytes_read,0);
		if (bytes_written == -1) {
			disconnected = 1;
		}
	}
	return disconnected;
}

void BindHandle(int master, const char *local_host, const char *local_port)
{
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	int server = -1;
	unsigned int disconnected = 0;
	fd_set set;
	unsigned int max_sock;

	/* Get the address info */
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int iResult = getaddrinfo(local_host, local_port, &hints, &result);
	if (iResult != 0) {
		debug("getaddrinfo failed: "+ iResult);
		WSACleanup();
		return;
	}

	SOCKET ListenSocket = INVALID_SOCKET;

	// Create a SOCKET for the server to listen for client connections

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	/* Connect to the host */
	if (ListenSocket == INVALID_SOCKET) {
		debug("Error at socket(): "+ WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		debug("bind failed with error: "+ WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	freeaddrinfo(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		debug("Listen failed with error: "+ WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	SOCKET ClientSocket;


	/* Main transfer loop */
	while(true)
	{
		disconnected = 0;
		ClientSocket = INVALID_SOCKET;

		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			debug("accept failed: "+ WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return;
		}
		debug("[+] New connection accepted\n");

		if (master > ClientSocket) {
			max_sock = master;
		}
		else {
			max_sock = ClientSocket;
		}

		while (!disconnected) 
		{
			FD_ZERO(&set);
			FD_SET(master, &set);
			FD_SET(ClientSocket, &set);
			if (select(max_sock + 1, &set, NULL, NULL, NULL) == -1) {
				//perror("select");
				break;
			}
			if (FD_ISSET(master, &set)) {
				disconnected = transfer(master, ClientSocket);
			}
			if (FD_ISSET(ClientSocket, &set)) {
				disconnected = transfer(ClientSocket, master);
			}
		}
		debug("[-] Connection closed\n");
		closesocket(ClientSocket);
	}
	closesocket(master);
}

void ReverseHandle(int master, const char *local_host, const char *local_port)
{
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	int server = -1;
	unsigned int disconnected = 0;
	fd_set set;
	unsigned int max_sock;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int iResult = getaddrinfo(local_host, local_port, &hints, &result);
	if (iResult != 0) {
		debug("getaddrinfo failed: "+ iResult);
		WSACleanup();
		return;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		debug("Error at socket(): "+ WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// Connect to server.
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		debug("Unable to connect to server!");
		WSACleanup();
		return;
	}


	/* Main transfer loop */
	if (master > ConnectSocket) {
		max_sock = master;
	}
	else {
		max_sock = ConnectSocket;
	}

	while (!disconnected)
	{
		FD_ZERO(&set);
		FD_SET(master, &set);
		FD_SET(ConnectSocket, &set);
		if (select(max_sock + 1, &set, NULL, NULL, NULL) == -1) {
			//perror("select");
			break;
		}
		if (FD_ISSET(master, &set)) {
			disconnected = transfer(master, ConnectSocket);
		}
		if (FD_ISSET(ConnectSocket, &set)) {
			disconnected = transfer(ConnectSocket, master);
		}
	}
	closesocket(ConnectSocket);
	closesocket(master);
}

int bindForward(const char *local_host, const char *local_port, const char *remote_host, const char *remote_port, bool reverse)
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		debug("WSAStartup failed: "+ iResult);
		return 1;
	}
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	int reuseaddr = 1; /* True */

	/* Get the address info */
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(remote_host, remote_port, &hints, &result);
	if (iResult != 0) {
		debug("Error en el forward: "+ iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	
	if (ConnectSocket == INVALID_SOCKET) {
		debug("Error at socket(): "+ WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}


	/* Enable the socket to reuse the address */
	if (setsockopt(ConnectSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseaddr, sizeof(int)) < 0) {
		//perror("setsockopt");
		freeaddrinfo(result);
		return 1;
	}
	
	// Connect to server.
	iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) 
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) 
	{
		debug("Unable to connect to server!");
		WSACleanup();
		return 1;
	}

	/* Ignore broken pipe signal */
	signal(EPIPE, SIG_IGN);

	/* Main loop */
	while (1) 
	{
		if (!reverse)
		{
			printf("[+] (BIND) Mapeando %s:%s -> %s:%s\n", remote_host, remote_port, local_host, local_port);
			BindHandle(ConnectSocket, local_host, local_port);
			return 2;
		}
		else
		{
			printf("[+] (REVERSE) Mapeando %s:%s -> %s:%s\n", remote_host, remote_port, local_host, local_port);
			ReverseHandle(ConnectSocket, local_host, local_port);
			return 2;
		}
			
	}

	closesocket(ConnectSocket);

	return 0;
}