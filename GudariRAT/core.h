#pragma once
#include "stdafx.h"

#define DEFAULT_BUFLEN 512

class CORE
{
private:
	std::string _ip;
	int _puerto;
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;
	int _operation;
	std::string ultimoMensaje;
public:
	CORE(std::string ip, int puerto);
	int conectar();
	int mandar(std::string data);
	std::string recibir();
	SOCKET getSocket();
	std::string getLastMSG();
	void clearLastMSG();
	struct addrinfo* getResult();
};
