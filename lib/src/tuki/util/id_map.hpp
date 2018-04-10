#pragma once

#include <unordered_map>
#include <vector>
#include <string>

class IdMap
{
	std::unordered_map<std::string, int> ids;
	std::vector<std::string> names;
public:
	IdMap() {}
	IdMap(const std::vector<std::string>& names);
	void add(const std::string& name);
	int id(const std::string& name)const;
	const std::string& name(int id)const;
};