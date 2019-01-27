#pragma once
#include "stdafx.h"

void debug(std::string mensaje);
const std::vector<std::string> explode(const std::string& s, const char& c);
std::string exec(const char* cmd);
void rc4(unsigned char * ByteInput, unsigned char * pwd, unsigned char * &ByteOutput);
void rc4crypt(unsigned char * ByteInput, unsigned char * pwd, unsigned char * &ByteOutput);
void rc4decrypt(byte * ByteInput, unsigned char * pwd, byte * &ByteOutput, int buflen);
//void showVector(std::vector<std::string> v);