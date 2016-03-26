#include "src/globals.h"

using namespace std;


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
} // end titlemenu