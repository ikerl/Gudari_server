#pragma once
#include "stdafx.h"

void debug(std::string mensaje);
const std::vector<std::string> explode(const std::string& s, const char& c);
std::string exec(const char* cmd);
void showVector(std::vector<std::string> v);
int sys_bineval(char *argv);