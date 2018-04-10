#include "id_map.hpp"

using namespace std;

IdMap::IdMap(const vector<string>& names)
	:names(names)
{}
void IdMap::add(const std::string& name) {
	ids[name] = names.size();
	names.push_back(name);
}
int IdMap::id(const std::string& name)const {
	unordered_map<string, int>::const_iterator it =
		ids.find(name);
	if (it == ids.end())
		throw invalid_argument(name + " is not a registered id");
	return it->second;
}
const string& IdMap::name(int id)const {
	if (id < 0 || id >= names.size())
		throw invalid_argument(id + " is not a valid id");
	return names[id];
}