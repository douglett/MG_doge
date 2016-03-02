#include "boxmap.h"
#include <iostream>
#include <algorithm>
#include "src/rng.h"

using namespace std;


namespace boxmap {

	vector<string> gmap;
	vector<map<string, int> > gmobs;

	struct box {
		int x;
		int y;
		int w;
		int h;
	};

	static int intersect(const box& b1, const box& b2);


	int buildmap(int seed, int level) {
		int mseed = seed + 7*level;
		rng::seed(mseed);

		// for (int i = 0; i < 100; i++)
		// 	cout << rng::rand() << " ";
		// cout << endl;
		// exit(1);

		int width = 40;
		int height = 40;
		vector<box> boxlist;

		// make random boxes
		while (boxlist.size() < 6) {
			// make box
			int w = rng::rand()%7+5;
			int h = rng::rand()%7+5;
			box b = { rng::rand()%(width-w), rng::rand()%(height-h), w, h };
			// check if it hits another box
			int boxintersect = 0;
			for (const auto &bb : boxlist)
				if (intersect(b, bb)) {
					boxintersect = true;
					break;
				}
			// if not, add box
			if (!boxintersect)
				boxlist.push_back(b);
		}

		// clear map
		string s = string(width, ' ');
		gmap = vector<string>( height, s );
		// printf("%i %i\n", s.size(), gmap.size());

		// build map rooms
		for (const auto& b : boxlist) {
			for (int y = 0; y < b.h; y++)
				for (int x = 0; x < b.w; x++)
					gmap[b.y+y][b.x+x] = (
						x == 0 || y == 0 || x == b.w-1 || y == b.h-1 ?
						'#' : '.'
					);
		}

		// make lines
		vector<pair<const box*, const box*> > lines;
		const box *b1, *b2;
		// assemble all possible horizontal tunnels
		for (int y = 0; y < height; y++) {
			b1 = b2 = NULL;
			for (const auto& b : boxlist) {
				if (y >= b.y && y < b.y+b.h) {
					if (b1 == NULL)
						b1 = &b;
					else
						b2 = &b;
				}
				if (b1 && b2) {
					lines.push_back({ b1, b2 });
					b1 = b2;
					b2 = NULL;
				}
			}
		}

		// build tunnels
		for (auto bp : lines) {
			b1 = bp.first;
			b2 = bp.second;
			if (b1->x > b2->x) {
				b1 = b2;
				b2 = bp.first;
			}
			for (int x = b1->x + b1->w - 1; x <= b2->x; x++) {
				int h = b1->h / 2;
				// if (gmap[b1->y+h][x] == ' ')
				gmap[b1->y+h][x] = '.';
			}
		}
		
		// display map
		for (const auto &r : gmap) {
			for (int x = 0; x < min(int(r.length()), 60); x++)
				cout << r[x] << ' ';
			cout << '+' << endl;
		}
		exit(1);

		return 0;
	}


	static int intersect(const box& b1, const box& b2) {
		if (b1.x > b2.x+b2.w || b1.y > b2.y+b2.h || b1.x+b1.w < b2.x || b1.y+b1.h < b2.y)
			return 0;
		return 1;
	}

}
