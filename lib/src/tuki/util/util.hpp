#pragma once

#include <string>
#include <array>

#include "handle.hpp"

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