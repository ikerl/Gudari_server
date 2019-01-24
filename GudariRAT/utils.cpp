#include "stdafx.h"

void debug(std::string mensaje)
{
	std::cout << mensaje;
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

void rc4(unsigned char * ByteInput, unsigned char * pwd,
	unsigned char * &ByteOutput) {
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
