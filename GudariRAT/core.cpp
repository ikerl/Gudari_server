#include "stdafx.h"

CORE::CORE(std::string ip, int puerto)
{
	_ip = ip;
	_puerto = puerto;
}

int CORE::conectar()
{
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("[-] WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo((PCSTR)(char*)_ip.c_str(), std::to_string(_puerto).c_str(), &hints, &result);
	if (iResult != 0)
	{
		printf("[-] No se ha podido procesar la direccion: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("[-] Error de socket: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		//printf("[-] No se ha podido establecer la conexion!\n");
		WSACleanup();
		return 1;
	}
	return 0;
}

int CORE::mandar(std::string sendbuf)
{
	// Send an initial buffer
	iResult = send(ConnectSocket, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
	if (iResult == SOCKET_ERROR) {
		debug("[-] Error en transmision: " + WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	//printf("[+] Bytes Sent: %ld\n", iResult);
	return 0;
}

std::string CORE::recibir()
{
	SecureZeroMemory(recvbuf, recvbuflen);
	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0)
	{
		//recvbuf[iResult] = '\0';
		//printf("[+] Orden recibida: %s\n",recvbuf);

		std::string operation(recvbuf);
		ultimoMensaje = operation;
		return operation;
	}
	else if (iResult == 0)
		debug("[-] Conexion cerrada\n");
	else
		debug("[-] Error de recepcion: " + WSAGetLastError());

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return "EXIT";
}

SOCKET CORE::getSocket()
{
	return ConnectSocket;
}

std::string CORE::getLastMSG()
{
	return ultimoMensaje;
}

void CORE::clearLastMSG()
{
	ultimoMensaje = "";
}

addrinfo* CORE::getResult()
{
	return result;
}
