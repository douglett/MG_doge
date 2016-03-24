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
		using namespace gamestate;
		assert(dir == -1 || dir == 1);
		mdir = dir;
		gstack.push_back(MODE_FADEBLACK);
		fadeval = (dir == 1 ? 0 : 255);
	}

	int step(string& k) {
		fadeval += mdir * 8;
		fadeval = min(max(0, fadeval), 255);
		if (fadeval == 0 || fadeval == 255)
			gamestate::gstack.pop_back();
		return 1; // absorb keys
	}

	void draw() {
		SDL_Rect dst = { 0, 0, game::width, game::height };
		SDL_SetRenderDrawColor(game::ren, 0, 0, 0, fadeval);
		SDL_RenderFillRect(game::ren, &dst);
	}

} // end fadeblack



namespace titlemenu {

	static int mstate = 0;

	void reset() {
		using namespace gamestate;
		mstate = 0;
		gstack.push_back(MODE_TITLEMENU);
		fadeblack::reset(fadeblack::FADEIN);
	}

	int step(string& k) {
		if (mstate == 1) {
			gamestate::gstack.pop_back();
			return 1;
		}

		// handle input
		if (k == "^q")
			return 2;
		else if (k == "^b" && playermob.name.length()) // backspace
			playermob.name.pop_back();
		else if (k == "^e" && playermob.name.length() > 0) { // enter
			mstate = 1;
			fadeblack::reset(fadeblack::FADEOUT);
		}
		else if (playermob.name.length() >= 8)
			;
		else if (k[0] >= 'a' && k[0] <= 'z')
			playermob.name += k[0];
		else if (k[0] >= '0' && k[0] <= '9')
			playermob.name += k[0];

		return 1; // absorb keys
	}

} // end mainloop