#pragma once

#include <string>
#include <vector>
#include <map>

namespace testmap {
	extern std::vector<std::string> gmap;
	extern std::vector<std::map<std::string, int> > gmobs;
	int buildmap(int seed, int level);
}
