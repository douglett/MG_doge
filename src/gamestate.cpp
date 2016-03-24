#include <cassert>
#include "globals.h"
#include "libsrc/xcengine.h"

using namespace std;



namespace gamestate {
	int gamemode = MODE_GAME;
	int movecount = 0;
	vector<int> gstack;
} // end gamestate



namespace fadeblack {

	static int 
		mdir = 1,
		fadeval = 0;

	void reset(int dir) {
		assert(dir == 1 || dir == -1);
		mdir = dir;
		fadeval = (dir == 1 ? 0 : 255);
	}

	int step() {
		fadeval += mdir * 8;
		fadeval = min(max(0, fadeval), 255);
		if (fadeval == 0 || fadeval == 255)
			return 1;
		return 0;
	}

	void draw() {
		SDL_Rect dst = { 0, 0, game::width, game::height };
		SDL_SetRenderDrawColor(game::ren, 0, 0, 0, fadeval);
		SDL_RenderFillRect(game::ren, &dst);
	}

} // end fadeblack



namespace mainloop {

	int titlemenu(string& k) {
		return 0;
	}

} // end mainloop