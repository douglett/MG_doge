#include <cassert>
#include "globals.h"
#include "libsrc/xcengine.h"

using namespace std;



namespace scene {

	vector<int> scenestack;

	static void show() {
		cout << "scenes: ";
		for (auto& s : scenestack)
			cout << s << " "; 
		cout << endl;
	}

	int current() {
		if (scenestack.size() == 0)
			return NOSCENE;
		return scenestack[scenestack.size()-1];
	}

	int add(SCENETYPE mode) {
		scenestack.push_back(mode);
		show();
		return 0;
	}

	int clear(SCENETYPE mode) {
		for (int i = scenestack.size()-1; i >= 0; i--)
			if (scenestack[i] == mode) {
				scenestack.erase(scenestack.begin()+i);
				show();
				return 1;
			}
		return 0;
	}

} // end scene



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



namespace titlemenu {

	static int mstate = 0;

	void reset() {
		mstate = 0;
		scene::add(scene::TITLEMENU);
		fadeblack::reset(fadeblack::FADEIN);
	}

	int step(const string& k) {
		if (mstate == 1) {
			scene::clear(scene::TITLEMENU);
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