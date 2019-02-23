#include "stdafx.h"

void debug(std::string mensaje)
{
	std::cout << mensaje << std::endl;
}

const std::vector<std::string> explode(const std::string& s, const char& c)
{
	std::string buff{ "" };
	std::vector<std::string> v;

	for (auto n : s)
	{
		if (n != c) buff += n; else
			if (n == c && buff != "") { v.push_back(buff); buff = ""; }
	}
	if (buff != "") v.push_back(buff);

	return v;
}

std::string exec(const char* cmd)
{
	std::array<char, 128> buffer;
	std::string result;
	std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) throw std::runtime_error("popen() failed!");
	while (!feof(pipe.get()))
	{
		if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
			result += buffer.data();
	}
	return result;
}
/*
void showVector(std::vector<std::string> v)
{
	debug("[+]El vector contiene:\n => ");
	for (unsigned i = 0; i<v.size(); i++)
		std::cout << ' ' << v[i];
	std::cout << '\n';
}
*/

void rc4(unsigned char * ByteInput, unsigned char * pwd, unsigned char * &ByteOutput) 
{
	unsigned char * temp;
	int i, j = 0, t, tmp, tmp2, s[256], k[256];
	for (tmp = 0; tmp<256; tmp++) {
		s[tmp] = tmp;
		k[tmp] = pwd[(tmp % strlen((char *)pwd))];
	}
	for (i = 0; i<256; i++) {
		j = (j + s[i] + k[i]) % 256;
		tmp = s[i];
		s[i] = s[j];
		s[j] = tmp;
	}
	temp = new unsigned char[(int)strlen((char *)ByteInput) + 1];
	i = j = 0;
	for (tmp = 0; tmp<(int)strlen((char *)ByteInput); tmp++) {
		i = (i + 1) % 256;
		j = (j + s[i]) % 256;
		tmp2 = s[i];
		s[i] = s[j];
		s[j] = tmp2;
		t = (s[i] + s[j]) % 256;
		if (s[t] == ByteInput[tmp])
			temp[tmp] = ByteInput[tmp];
		else
			temp[tmp] = s[t] ^ ByteInput[tmp];
	}
	temp[tmp] = '\0';
	ByteOutput = temp;
}

void rc4crypt(unsigned char * ByteInput, unsigned char * pwd, unsigned char *& ByteOutput)
{
	int stageSize = 1024; // Size of encryptation block
	int TotalSize = (int)strlen((char *)ByteInput);
	unsigned char* temp;
	int numBloques = (int)(TotalSize / 1024);
	int tamanoFinal = TotalSize + numBloques * 2 + 1;
	unsigned char* tempFinal = new unsigned char[(int)strlen((char *)ByteInput) + numBloques*2 + 1];
	int totalCifrado = 0;
	int bloquesRecorridos = 1;
	
	while (totalCifrado < TotalSize)
	{
		if (TotalSize - totalCifrado < 1024)
		{
			std::cout << "Cifrando.. " << TotalSize - totalCifrado << std::endl;
			temp = new unsigned char[TotalSize - totalCifrado + 1];

			int i, j = 0, t, tmp, tmp2, s[256], k[256];
			for (tmp = 0; tmp < 256; tmp++) {
				s[tmp] = tmp;
				k[tmp] = pwd[(tmp % strlen((char *)pwd))];
			}
			for (i = 0; i < 256; i++) {
				j = (j + s[i] + k[i]) % 256;
				tmp = s[i];
				s[i] = s[j];
				s[j] = tmp;
			}

			i = j = 0;
			for (tmp = 0; tmp < (TotalSize - totalCifrado); tmp++) {
				i = (i + 1) % 256;
				j = (j + s[i]) % 256;
				tmp2 = s[i];
				s[i] = s[j];
				s[j] = tmp2;
				t = (s[i] + s[j]) % 256;
				if (s[t] == ByteInput[tmp + totalCifrado])
					temp[tmp] = ByteInput[tmp + totalCifrado];
				else
					temp[tmp] = s[t] ^ ByteInput[tmp + totalCifrado];
			}
			temp[tmp] = '\0';
			memcpy(tempFinal + totalCifrado + bloquesRecorridos*2, temp, TotalSize-totalCifrado);

			break;
		}
		else
		{
			std::cout << "Cifrando.. " << stageSize << std::endl;
			temp = new unsigned char[1024 + 1];

			int i, j = 0, t, tmp, tmp2, s[256], k[256];
			for (tmp = 0; tmp < 256; tmp++) {
				s[tmp] = tmp;
				k[tmp] = pwd[(tmp % strlen((char *)pwd))];
			}
			for (i = 0; i < 256; i++) {
				j = (j + s[i] + k[i]) % 256;
				tmp = s[i];
				s[i] = s[j];
				s[j] = tmp;
			}

			i = j = 0;
			for (tmp = 0; tmp < (1024); tmp++) {
				i = (i + 1) % 256;
				j = (j + s[i]) % 256;
				tmp2 = s[i];
				s[i] = s[j];
				s[j] = tmp2;
				t = (s[i] + s[j]) % 256;
				if (s[t] == ByteInput[tmp+totalCifrado])
					temp[tmp] = ByteInput[tmp+totalCifrado];
				else
					temp[tmp] = s[t] ^ ByteInput[tmp+totalCifrado];
			}
			//temp[tmp] = '\0';

			memcpy(tempFinal + totalCifrado + bloquesRecorridos*2, temp, 1024);
			totalCifrado += stageSize;
			bloquesRecorridos++;
		}
	}
	
	memcpy(tempFinal, &tamanoFinal, 2);
	ByteOutput = tempFinal;

}

void rc4decrypt(byte * ByteInput, unsigned char * pwd, byte *& ByteOutput, int buflen)
{
	byte * temp;
	int i, j = 0, t, tmp, tmp2, s[256], k[256];
	for (tmp = 0; tmp<256; tmp++) {
		s[tmp] = tmp;
		k[tmp] = pwd[(tmp % strlen((char *)pwd))];
	}
	for (i = 0; i<256; i++) {
		j = (j + s[i] + k[i]) % 256;
		tmp = s[i];
		s[i] = s[j];
		s[j] = tmp;
	}
	temp = new byte[buflen + 1];
	i = j = 0;
	for (tmp = 0; tmp<buflen; tmp++) {
		i = (i + 1) % 256;
		j = (j + s[i]) % 256;
		tmp2 = s[i];
		s[i] = s[j];
		s[j] = tmp2;
		t = (s[i] + s[j]) % 256;
		if (s[t] == ByteInput[tmp])
			temp[tmp] = ByteInput[tmp];
		else
			temp[tmp] = s[t] ^ ByteInput[tmp];
		//std::cout << "DByte: " << temp[tmp] << std::endl;
	}
	temp[tmp] = '\0';
	ByteOutput = temp;
}
