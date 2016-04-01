/**
 * Simple map generator using room boxes
 * author: doug
 * date: feb 2016
 */
#include "boxmap.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <sys/ioctl.h>
#include "rng.h"

using namespace std;


namespace boxmap {

	struct box {
		int x;
		int y;
		int w;
		int h;
	};

	static int  intersect(const box& b1, const box& b2);
	static void make_rooms();
	static void make_corridors(char type);
	static void make_furniture();
	static void make_mobs(int level);

	vector<string> gmap;
	vector<map<string, int> > gmobs;
	static int 
		width = 40,
		height = 40;
	static vector<box> roomlist;


	int buildmap(int seed, int level) {
		// init seed
		int mseed = seed + 7*level;
		rng::seed(mseed);

		// init map
		string s = string(width, ' ');
		gmap = vector<string>( height, s );
		gmobs.erase(gmobs.begin(), gmobs.end());
		roomlist.erase(roomlist.begin(), roomlist.end());

		// make rooms & map parts
		make_rooms();
		make_corridors('h');
		make_corridors('v');
		make_furniture();
		make_mobs(level);

		display();

		return 0;
	}


	void display() {
		// add mobs to map
		auto map2 = gmap;
		for (auto& m : gmobs)
			map2[ m.at("y") ][ m.at("x") ] = '*';
		
		struct winsize w;
		ioctl(0, TIOCGWINSZ, &w);
		int col = w.ws_col;

		// display map
		for (const auto &r : map2) {
			for (int x = 0; x < min(int(r.length()), col); x++)
				cout << r[x] << ' ';
			cout << endl;
		}
	}



	static int intersect(const box& b1, const box& b2) {
		if (b1.x > b2.x+b2.w || b1.y > b2.y+b2.h || b1.x+b1.w < b2.x || b1.y+b1.h < b2.y)
			return 0;
		return 1;
	}



	static void make_rooms() {
		// make random boxes
		while (roomlist.size() < 6) {
			// make box
			int w = rng::rand()%7+5;
			int h = rng::rand()%7+5;
			box room = { rng::rand()%(width-w), rng::rand()%(height-h), w, h };
			// check if it hits another box
			int boxintersect = 0;
			for (const auto &r : roomlist)
				if (intersect(r, room)) {
					boxintersect = true;
					break;
				}
			// if not, add box
			if (!boxintersect)
				roomlist.push_back(room);
		}
		

		// build map rooms
		for (const auto& b : roomlist) {
			for (int y = 0; y < b.h; y++)
				for (int x = 0; x < b.w; x++)
					gmap[b.y+y][b.x+x] = (
						x == 0 || y == 0 || x == b.w-1 || y == b.h-1 ?
						'#' : '.'
					);
		}
	}



	static void make_corridors(char type) {
		// lots of code shared between h and v tunnels
		assert(type == 'h' || type == 'v');
		
		// make all lines
		vector<pair<const box*, const box*> > lines;

		// assemble all possible horizontal/vertical tunnels
		int size = (type == 'h' ? height : width);
		for (int i = 0; i < size; i++) {
			vector<const box*> l;
			for (const auto& b : roomlist)
				if (type == 'h' && i > b.y && i < b.y+b.h-1)
					l.push_back(&b);
				else if (type == 'v' && i > b.x && i < b.x+b.w-1)
					l.push_back(&b);
			// split into pairs
			while (l.size() >= 2) {
				lines.push_back({ l[l.size()-1], l[l.size()-2] });
				l.pop_back();
			}
		}

		// clear duplicate lines
		for (int i = 0; i < lines.size(); i++)
			for (int j = i+1; j < lines.size(); j++)
				if (lines[i].first == lines[j].first && lines[i].second == lines[j].second) {
					lines.erase(lines.begin() + j);
					j--;
				}

		// debug
		// cout << "horizontal: " << lines.size() << endl;
		// for (auto &l : lines)
		// 	printf("%d,%d  %d,%d\n", l.first->x, l.first->y, l.second->x, l.second->y );

		// draw lines
		const box *b1, *b2;
		for (auto &l : lines) {
			b1 = l.first;
			b2 = l.second;
			if ((type == 'h' && b1->x > b2->x) || (type == 'v' && b1->y > b2->y)) {
				b1 = b2;
				b2 = l.first;
			}
			if (type == 'h') {
				int posy = (b1->y*2 + b1->h + b2->y*2 + b2->h) / 4;  // find center point
				int tstart = b1->x + b1->w - 2;
				int tend = b2->x + 2;
				for (int x = tstart; x < tend; x++)  // draw line
					if (gmap[posy][x] != '.')
						gmap[posy][x] = ',';
			}
			else if (type == 'v') {
				int posx = (b1->x*2 + b1->w + b2->x*2 + b2->w) / 4;  // find center point between boxes
				int tstart = b1->y + b1->h - 2;
				int tend = b2->y + 2;
				for (int y = tstart; y < tend; y++)  // draw line
					if (gmap[y][posx] != '.')
						gmap[y][posx] = ',';
			}
		}
	}


	// get an available room square
	static void room_pos(int* x, int* y, box** room) {
		// static const string walkable = ".,";
		while (true) {
			*room = &roomlist[ rng::rand() % roomlist.size() ];
			*x = (*room)->x+1 + rng::rand()%((*room)->w-2);
			*y = (*room)->y+1 + rng::rand()%((*room)->h-2);
			if ( gmap[*y][*x] != '.' )
				continue;  // make sure position is empty
			if (gmap[*y-1][*x] == ',' || gmap[*y+1][*x] == ',' || gmap[*y][*x-1] == ',' || gmap[*y][*x+1] == ',')
				continue;  // make sure we are not next to a path, to avoid blocking exits
			break;
		}
	}


	static void make_furniture() {
		box* room = NULL;
		int x = 0, y = 0;

		// treasure chests
		int chestcount = 2 + rng::rand()%4;
		while (chestcount > 0) {
			room_pos(&x, &y, &room);
			gmap[y][x] = 'C';
			chestcount--;
		}

		// brazier
		int braziercount = 1;
		while (braziercount > 0) {
			room_pos(&x, &y, &room);
			gmap[y][x] = 'i';
			braziercount--;
		}

		// exit ladder
		int laddercount = 1;
		while (laddercount > 0) {
			room_pos(&x, &y, &room);
			// check for player room here
			gmap[y][x] = '%';
			laddercount--;
		}
	}


	static void mob_pos(int* x, int* y) {
		box* room = NULL;
		while (true) {
			// get room
			room_pos(x, y, &room);
			int hit = 0;
			// don't stack mobs on top of each other
			for (const auto& m : gmobs)
				if (m.at("x") == *x && m.at("y") == *y) {
					hit = 1;
					break;
				}
			if (!hit)
				return;
		}
	}

	// static double getdist(const map<string, int>& mob, int x, int y) {
	// 	int dx = mob.at("x") - x;
	// 	int dy = mob.at("y") - y;
	// 	return sqrt(dx*dx + dy*dy);  // trig woo
	// }

	// static double getdist(const map<string, int>& mob1, const map<string, int>& mob2) {
	// 	int dx = mob1.at("x") - mob2.at("x");
	// 	int dy = mob1.at("y") - mob2.at("y");
	// 	// trig woo
	// 	return sqrt(dx*dx + dy*dy);
	// }

	static double getdist(int x1, int y1, int x2, int y2) {
		int dx = x1 - x2;
		int dy = y1 - y2;
		return sqrt(dx*dx + dy*dy);  // trig woo
	}

	static int find_tile(int* pos, char tile) {
		for (int y = 0; y < gmap.size(); y++)
			for (int x = 0; x < gmap[y].size(); x++)
				if (gmap[y][x] == tile) {
					pos[0] = x;
					pos[1] = y;
					return 1;
				}
		return 0;
	}

	static void make_mobs(int level) {
		// find special tiles
		int bp[2],  // brazier
			lp[2];  // ladder
		find_tile(bp, 'i');
		find_tile(lp, '%');

		// player start pos
		int pp[2];
		int player = 1;
		while (player > 0) {
			mob_pos( &pp[0], &pp[1] );
			if (getdist(pp[0], pp[1], lp[0], lp[1]) < 10)  // don't spawn player near ladder
				continue;
			gmobs.push_back({ 
				{ "x", pp[0] },
				{ "y", pp[1] },
				{ "type", -1 }
			});
			player--;
		}

		// mob start pos
		int x = 0, y = 0;
		int mobcount = 7 + rng::rand()%7;
		while (mobcount > 0) {
			mob_pos(&x, &y);
			if (getdist(pp[0], pp[1], x, y) < 6)  // don't spawn near player spawn
				continue;
			if (getdist(bp[0], bp[1], x, y) < 6)  // don't spawn near brazier
				continue;
			int z = (rng::rand()%3 == 0 ? 0 : 1);
			if (level == 1)
				z = 1;
			else if (level <= 4)
				z += level - 1;
			else
				z += 3;
			gmobs.push_back({ 
				{ "x", x },
				{ "y", y },
				{ "type", z }
			});
			mobcount--;
		}
	}

} // end boxmap
