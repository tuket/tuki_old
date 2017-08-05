#include "util.hpp"

#include <fstream>
#include <stdexcept>

using namespace std;

string loadStringFromFile(const char *fileName)
{
	// open the file
	std::ifstream file;
	file.open(fileName, std::ios::in);
	if (!file)
	{
		throw runtime_error("Error loading file: " + string(fileName));
	}

	// compute file length
	file.seekg(0, std::ios::end);
	int fileLen = (unsigned int)file.tellg();
	file.seekg(std::ios::beg);

	// reserve memory in the string
	string str(fileLen, ' ');

	// read file content
	file.read(&str[0], fileLen);

	return str;
}