#pragma once

#include <string>
#include <array>

template<int start, int end>
std::array<int, end - start> getNumberSequenceArray()
{
	std::array<int, end - start> res;
	for (int i = start; i < end; i++)
	{
		res[i - start] = i;
	}
	return res;
}

std::string loadStringFromFile(const char *fileName);



// sort multiple vectors using the criteria of the first vector
template<typename Compare, typename T, typename S>
void sortMultipleVectors(std::vector<T>& t, std::vector<T> ...)
{

}
