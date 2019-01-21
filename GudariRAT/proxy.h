#pragma once
#include "stdafx.h"

unsigned int transfer(int from, int to);
void BindHandle(int client, const char *remote_host, const char *remote_port);
void ReverseHandle(int client, const char *remote_host, const char *remote_port);
int bindForward(const char *local_host, const char *local_port, const char *remote_host, const char *remote_port, bool reverse);

