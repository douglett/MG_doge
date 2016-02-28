#include <iostream>
#include <vector>
#include "tmap.h"

using namespace std;

// Map generation!
// 
// Author: simon

const int MAPHEIGHT = 64;
const int MAPWIDTH = 64;

enum tiletype {
	tile_empty,
	tile_floor, 
	tile_wall, 
	tile_exit_north, 
	tile_exit_east, 
	tile_exit_south, 
	tile_exit_west,
	tile_outofbounds
};

enum direction {
	dir_north,
	dir_east,
	dir_south,
	dir_west
};

struct coord {
	int x, y;
};

struct space {
	int n, e, s, w;
};

// Sector
class Sector {
private:
	std::vector<Sector> children;
};

// std::vector<Sector> findSectors() {

// }

// Sector SectorSquare() {
	
// }

struct Exit {
	int x, y;
	direction dir;
	bool open;
};

// LevelGrid
class LevelGrid {
public:
	void setTileType(int x, int y, tiletype tile);
	tiletype getTileType(int x, int y) const;
	void setWall(int x, int y, int wall);
	int getWall(int x, int y);
	space getAvailableSize(int startx, int starty);
	void addExit(int x, int y, direction dir);
	void showexits();
	void lockExit(int index);
	void closeExit(int index);
	int getOpenExitCount();
	std::vector<Exit> exits;
private:
	tiletype tileplane[MAPWIDTH][MAPHEIGHT];
	int wallplane[MAPWIDTH][MAPHEIGHT];
};

void LevelGrid::setTileType(int x, int y, tiletype tile) {
	if (x < 0 || x > MAPWIDTH || y < 0 || y > MAPHEIGHT) {
		std::cerr << x << ", " << y << " out of bounds in setTileType; skipping \n";
		return;
	}
	tileplane[x][y] = tile;
}

tiletype LevelGrid::getTileType(int x, int y) const {
	if (x < 0 || x > MAPWIDTH || y < 0 || y > MAPHEIGHT) {
		return tile_outofbounds;
	}
	return tileplane[x][y];
}

void LevelGrid::setWall(int x, int y, int wall) {
	wallplane[x][y] = wall;
}

space LevelGrid::getAvailableSize(int startx, int starty) {
	int x = startx;
	int y = starty;
	// int x_start = 0;
	// int y_start = 0; 
	space s = {0,0,0,0};
	// Check north
	while (getTileType(x, y-1) == tile_empty) {
		y--;
		s.n++;
	}	
	// Check east
	while (getTileType(x+1, y) == tile_empty) {
		x++;
		s.e++;
	}		
	// Check south
	while (getTileType(x, y+1) == tile_empty) {
		y++;
		s.s++;
	}
	// Check west
	while (getTileType(x-1, y) == tile_empty) {
		x--;
		s.w++;
	}
	return s;
}

void LevelGrid::addExit(int x, int y, direction dir) {
	tiletype ext;
	switch (dir) {
		case dir_north: ext = tile_exit_north; break;
		case dir_east: ext = tile_exit_east; break;
		case dir_south: ext = tile_exit_south; break;
		case dir_west: ext = tile_exit_west; break;
	}
	setTileType(x, y, ext);
	exits.push_back({x, y, dir, true});
}

void LevelGrid::showexits() {
	for (int i=0; i<exits.size(); ++i) {
		std::cout << i << ") " << exits[i].x << ", " << exits[i].y << "(" << exits[i].dir << ") - " << exits[i].open << "\n";
	}
}

void LevelGrid::lockExit(int index) {
	exits[index].open = false;
}

void LevelGrid::closeExit(int index) {
	exits[index].open = false;
	tileplane[exits[index].x][exits[index].y] = tile_wall;
}

int LevelGrid::getOpenExitCount() {
	int count = 0;
	for (int i=0; i<exits.size(); ++i) {
		if (exits[i].open) {
			++count;
			//std::cout << exits[i].open << "\n";
		}
	}
	return count;
}

// Helper functions or rather placeholders for more sophisticated systems

int GetPointBetween(int start, int end) {
	int val = 0;
	if (start==end) {
		val = start;
	} else {
		val = rand() % (end-start) + start;	
	}
	if (val < start || val > end) 
		std::cerr<<"GetPointBetween impossible value: "<<val<<" is not between "<<start<<" and "<<end<<"\n";
	return val;
}

// Main generation algorithm. This will be refactored, but logically we need the following:
// - set an origin point based on the passed startx, starty and dir
// - build room from origin
// - add 'opposite' exit adjacent to start pos
// - add all other exits
void BuildSectorTiles(int startx, int starty, LevelGrid& lg, space s, direction dir) {
	int max_x = rand() % 8 + 3;
	int max_y = rand() % 8 + 3;
	int origin_x = startx;
	int origin_y = starty;
	// Set an origin point; we always build from the top left so need to create offsets
	switch(dir) {
		// north: up shift full height, left shift by %
		case dir_north: 
			// set space
			if (max_x > s.e) max_x = s.e;
			if (max_y > s.n) max_y = s.n;
			// set origins
			origin_y -= max_y-1; 
			origin_x -= GetPointBetween(1, max_x-2);
			// add exits
			lg.addExit(origin_x, origin_y + GetPointBetween(1, max_y-2), dir_west);
			lg.addExit(startx, starty, dir_south);
			// Lock the exit to prevent it being used as the starting point for a new room; we know it isn't!
			lg.lockExit(lg.exits.size()-1);
			lg.addExit(max_x+origin_x-1, origin_y + GetPointBetween(1, max_y-2), dir_east);
			lg.addExit(origin_x + GetPointBetween(1, max_x-2), origin_y, dir_north);
			break;
		// east: up shift by %
		case dir_east:
			// set space
			if (max_x > s.e) max_x = s.e;
			if (max_y > s.s) max_y = s.s;
			// set origins
			origin_y -= GetPointBetween(1, max_y-2);
			// Add exits
			lg.addExit(startx, starty, dir_west);
			lg.lockExit(lg.exits.size()-1);
			lg.addExit(origin_x + (rand() % (max_x-2) + 1), max_y+origin_y-1, dir_south);
			lg.addExit(max_x+origin_x-1, origin_y + (rand() % (max_y-2) + 1), dir_east);
			lg.addExit(origin_x + (rand() % (max_x-2) + 1), origin_y, dir_north);
			break;
		// south: left shift by %
		case dir_south: 
			// set space
			//if (max_x > s.e) max_x = s.e;
			if (max_y > s.s) max_y = s.s;
			// set origins
			origin_x -= GetPointBetween(1, max_x-2);
			// Add exits
			lg.addExit(origin_x, origin_y + GetPointBetween(1, max_y-2), dir_west);
			lg.addExit(origin_x + (rand() % (max_x-2) + 1), max_y+origin_y-1, dir_south);
			lg.addExit(max_x+origin_x-1, origin_y + (rand() % (max_y-2) + 1), dir_east);
			lg.addExit(startx, starty, dir_north);
			lg.lockExit(lg.exits.size()-1);
			break;
		// west: up shift by %, left shift full width
		case dir_west: origin_y -= (rand() % max_y-2) + 1; origin_x -= max_x; break;
	}

	for (int x=0; x<max_x; ++x) {
		for (int y=0; y<max_y; ++y) {
			if (lg.getTileType(x+origin_x, y+origin_y) == tile_empty) {
				if (x > 0 && y > 0  && x < max_x-1 && y < max_y-1)
					lg.setTileType(x+origin_x, y+origin_y, tile_floor);
				else
					lg.setTileType(x+origin_x, y+origin_y, tile_wall);
			}
		}
	}
}



// transform to map string
int make_map_string(const LevelGrid& lg) {
	auto& gmap = tmap::gmap;
	gmap.erase(gmap.begin(), gmap.end());

	for (int y=0; y<MAPHEIGHT; ++y) {
		string s;
		for (int x=0; x<MAPWIDTH; ++x) {
			switch (lg.getTileType(x, y)) {
				case tile_empty: 
					s += ' '; 
					break;
				case tile_floor: 
					s += '.'; 
					break;
				case tile_wall: 
					s += '#'; 
					break;
				case tile_exit_north:
				case tile_exit_east:
				case tile_exit_south:
				case tile_exit_west:
					s += '/'; 
					break;
				default:
					s += '?';
					break;
			}
		}
		gmap.push_back(s);
	}

	return 0;
}



namespace tmap {

	vector<string> gmap;
	vector<map<string, int> > gmobs;


	// main
	int buildmap(int seed) {
		
		srand(seed);

		LevelGrid lg = LevelGrid();
		int startx = rand() % (MAPWIDTH/2);
		int starty = rand() % (MAPHEIGHT/2)+20;
		space s;
		Exit e;

		map<string, int> player;
		player["x"] = startx;
		player["y"] = starty;
		player["type"] = -1;
		gmobs.push_back(player);

		s = lg.getAvailableSize(startx, starty);
		BuildSectorTiles(startx, starty, lg, s, dir_east);

		//Loop through the exits and add more
		int i = 0;
		while (lg.getOpenExitCount() > 0) {
			//std::cout<<"Iteration "<<i<<"\n";
			e = lg.exits[i];
			if (e.open) {
				switch (e.dir) {
					case dir_north:
						s = lg.getAvailableSize(e.x, e.y-1);
						if (s.n > 2 && s.e > 2) {	
							BuildSectorTiles(e.x, e.y-1, lg, s, e.dir); 
							//std::cout << "Going north\n";
							lg.lockExit(i);
						} else {
							//std::cout << "Not enough room; closing exit\n";
							lg.closeExit(i);
						}
						break;
					case dir_east:
						s = lg.getAvailableSize(e.x+1, e.y);
						if (s.s > 2 && s.e > 2) {
							BuildSectorTiles(e.x+1, e.y, lg, s, e.dir); 
							//std::cout << "Going east\n";
							lg.lockExit(i);
						} else {
							//std::cout << "Not enough room; closing exit\n";
							lg.closeExit(i);
						}
						break;
					case dir_south:
						s = lg.getAvailableSize(e.x, e.y+1);
						if (s.s > 2 && s.e > 2) {
							BuildSectorTiles(e.x, e.y+1, lg, s, e.dir);
							//std::cout << "Going south\n";
							lg.lockExit(i);
						} else {
							//std::cout << "Not enough room; closing exit\n";
							lg.closeExit(i);
						}
						break;
					case dir_west:
						lg.closeExit(i);
						//std::cout << "West not implemented; closing\n";
						break;
					default:
						//std::cout << "This is an error!\n";
						break;
				}
			} else {
				//std::cout << "Skipping; closed " << i << ": " << lg.exits[i].open << "\n";
			}
			++i;
			//lg.showexits();
			//if (i == lg.getOpenExitCount()) i = 0;
			//std::cout << lg.getOpenExitCount() << "\n";
			//std::cout << "Closed exit " << i << "\n";
			// std::cin.get();

			// for (int y=0; y<MAPHEIGHT; ++y) {
			// 	for (int x=0; x<MAPWIDTH; ++x) {
			// 		switch (lg.getTileType(x, y)) {
			// 			case tile_empty: std::cout << " "; break;
			// 			case tile_floor: std::cout << "."; break;
			// 			case tile_wall: std::cout << "#"; break;
			// 			default: std::cout << "/"; break;
			// 		}
			// 		//std::cout << lg.getTileType(x, y);
			// 	}
			// 	std::cout << "\n";
			// }

		}
		// Clear exit list
		lg.exits.clear();


		// show map
		// if (false) {
		// 	for (int y=0; y<MAPHEIGHT; ++y) {
		// 		for (int x=0; x<MAPWIDTH; ++x) {
		// 			switch (lg.getTileType(x, y)) {
		// 				case tile_empty: std::cout << " "; break;
		// 				case tile_floor: std::cout << "."; break;
		// 				case tile_wall: std::cout << "#"; break;
		// 				default: std::cout << "/"; break;
		// 			}
		// 			//std::cout << lg.getTileType(x, y);
		// 		}
		// 		std::cout << "\n";
		// 	}
		// }

		make_map_string(lg);

		return 0;
	}

} // end tmap
