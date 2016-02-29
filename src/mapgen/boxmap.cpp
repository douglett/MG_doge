#include "boxmap.h"
#include <iostream>
#include <algorithm>

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
		srand(mseed);

		int width = 40;
		int height = 40;
		vector<box> boxlist;

		// make random boxes
		while (boxlist.size() < 6) {
			int w = rand()%7+5;
			int h = rand()%7+5;
			box b = { rand()%(width-w), rand()%(height-h), w, h };
			int boxintersect = 0;
			for (const auto &bb : boxlist)
				if (intersect(b, bb)) {
					boxintersect = true;
					break;
				}
			if (boxintersect)
				continue;

			boxlist.push_back(b);
		}

		// build map
		string s = string(width, ' ');
		gmap = vector<string> ( height, s );

		// printf("%i %i\n", s.size(), gmap.size());

		for (const auto& b : boxlist) {
			for (int y = 0; y < b.h; y++)
				for (int x = 0; x < b.w; x++)
					gmap[b.y+y][b.x+x] = (
						x == 0 || y == 0 || x == b.w-1 || y == b.h-1 ?
						'#' : '.'
					);
		}
					
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
