// stdafx.h: archivo de inclusión de los archivos de inclusión estándar del sistema
// o archivos de inclusión específicos de un proyecto utilizados frecuentemente,
// pero rara vez modificados
//
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <array>
#include <stdexcept>
#include <memory>
#include <cstdio>
#include <WinBase.h>
#include <signal.h>

#include "core.h"
#include "modulos.h"
#include "utils.h"
#include "gudari.h"
#include "proxy.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma comment(lib,"ws2_32")

// TODO: mencionar aquí los encabezados adicionales que el programa necesita
