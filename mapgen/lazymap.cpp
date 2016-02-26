#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include "lazymap.h"

using namespace std;


const int 
	MAP_WIDTH = 20, 
	MAP_HEIGHT = 20;


namespace lazymap {

	vector<string> gmap;
	vector<map<string, int> > gmobs;


	int buildmap(int seed) {
		// clear old
		gmap.erase(gmap.begin(), gmap.end());
		gmobs.erase(gmobs.begin(), gmobs.end());

		// int seed = time(NULL);
		// seed = 6000;

		// create map
		srand(seed);
		for (int y = 0; y < MAP_HEIGHT; y++) {
			string s;
			for (int x = 0; x < MAP_WIDTH; x++) {
				if (x == 0 || y == 0 || x == MAP_WIDTH-1 || y == MAP_HEIGHT-1)
					s += '#';
				else if (rand()%15 == 0)
					s += '#';
				else {
					// map[y].push_back( rand()%2 );
					rand();
					s += '.';
				}
			}
			gmap.push_back(s);
		}
		
		// make font(s) and exit steps
		gmap[4][4] = 'i';
		gmap[4][6] = '%';
		gmap[16][16] = '%';

		// make mobs
		srand(seed);
		int mobcount = 15 + rand()%15;
		for (int i = 0; i < mobcount; i++) {
			map<string, int> mob;
			mob["x"] = rand()%17 + 1;
			mob["y"] = rand()%17 + 1;
			mob["type"] = rand()%2 + 1;
			gmobs.push_back(mob);
		}

		return 0;
	}

} // end lazymap

