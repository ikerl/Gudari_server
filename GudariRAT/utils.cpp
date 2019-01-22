#include "stdafx.h"

void debug(std::string mensaje)
{
	//std::cout << mensaje;
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

