#include <cassert>
#include "libsrc/xcengine.h"
#include "src/globals.h"

using namespace std;


namespace fadeblack {
	static int 
		speed = 8,
		mdir = 1,
		fadeval = 0;

	void reset(int dir) {
		assert(dir == -1 || dir == 1);
		mdir = dir;
		scene::add(scene::FADEBLACK);
		fadeval = (dir == 1 ? 0 : 255);
	}

	int step(const string& k) {
		fadeval += mdir * speed;
		fadeval = min(max(0, fadeval), 255);
		// cout << "f " << fadeval << endl;
		if (fadeval == 0 || fadeval == 255)
			scene::clear(scene::FADEBLACK);
		return 0;
	}

	void draw() {
		SDL_Rect dst = { 0, 0, game::width, game::height };
		SDL_SetRenderDrawColor(game::ren, 0, 0, 0, fadeval);
		SDL_RenderFillRect(game::ren, &dst);
	}
} // end fadeblack


namespace fadewhite {
	static int 
		speed = 4,
		mdir = 1,
		fadeval = 0;

	void reset() {
		scene::add(scene::FADEWHITE);
		mdir = 1;
		fadeval = 0;
	}

	int step(const string& k) {
		fadeval += mdir * speed;
		if (fadeval >= 160) {
			fadeval = 160;
			mdir = -1;
		} else if (fadeval <= 0) {
			fadeval = 0;
			scene::clear(scene::FADEWHITE);
		}
		return 0;
	}

	void draw() {
		SDL_Rect dst = { 0, 0, game::width, game::height };
		SDL_SetRenderDrawColor(game::ren, 255, 255, 255, fadeval);
		SDL_RenderFillRect(game::ren, &dst);
	}
} // end fadeblack
