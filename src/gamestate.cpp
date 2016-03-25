#include <cassert>
#include "globals.h"
#include "libsrc/xcengine.h"

using namespace std;



namespace gamestate {

	int gamemode = MODE_GAME;
	int movecount = 0;
	vector<int> gstack;

	void showmodes() {
		cout << "s ";
		for (auto& m : gstack)
			cout << m << " "; 
		cout << endl;
	}

	int current() {
		if (gstack.size() == 0)
			return MODE_NONE;
		return gstack[gstack.size()-1];
	}

	int addmode(int mode) {
		gstack.push_back(mode);
		showmodes();
		return 0;
	}

	int clearmode(int mode) {
		for (int i = gstack.size()-1; i >= 0; i--)
			if (gstack[i] == mode) {
				gstack.erase(gstack.begin()+i);
				showmodes();
				return 1;
			}
		return 0;
	}

} // end gamestate



namespace fadeblack {

	using namespace gamestate;

	static int 
		mdir = 1,
		fadeval = 0;

	void reset(int dir) {
		assert(dir == -1 || dir == 1);
		mdir = dir;
		addmode(MODE_FADEBLACK);
		fadeval = (dir == 1 ? 0 : 255);
	}

	int step(const string& k) {
		fadeval += mdir * 4;
		fadeval = min(max(0, fadeval), 255);
		// cout << "f " << fadeval << endl;
		if (fadeval == 0 || fadeval == 255)
			clearmode(MODE_FADEBLACK);
		return 0;
	}

	void draw() {
		SDL_Rect dst = { 0, 0, game::width, game::height };
		SDL_SetRenderDrawColor(game::ren, 0, 0, 0, fadeval);
		SDL_RenderFillRect(game::ren, &dst);
	}

} // end fadeblack



namespace titlemenu {

	using namespace gamestate;

	static int mstate = 0;

	void reset() {
		using namespace gamestate;
		mstate = 0;
		addmode(MODE_TITLEMENU);
		fadeblack::reset(fadeblack::FADEIN);
	}

	int step(const string& k) {
		if (mstate == 1) {
			clearmode(MODE_TITLEMENU);
			start_game();
			return 0;
		}

		// handle input
		if (k == "^b" && playermob.name.length()) // backspace
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

		return 0;
	}

} // end mainloop