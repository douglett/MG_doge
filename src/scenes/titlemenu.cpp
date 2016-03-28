#include "src/globals.h"

using namespace std;


namespace titlemenu {
	void reset() {
		scene::add(scene::TITLEMENU);
		fadeblack::reset(fadeblack::FADEIN, NULL);
	}

	static void callback_start() {
		scene::clear(scene::TITLEMENU);
		start_game();
	}

	int step(const string& k) {
		// handle input
		if (k == "^b" && playermob.name.length()) // backspace
			playermob.name.pop_back();
		else if (k == "^e" && playermob.name.length() > 0) // enter
			fadeblack::reset(fadeblack::FADEOUT, callback_start);
		else if (playermob.name.length() >= 8)
			;
		else if (k[0] >= 'a' && k[0] <= 'z')
			playermob.name += k[0];
		else if (k[0] >= '0' && k[0] <= '9')
			playermob.name += k[0];

		return 0;
	}
} // end titlemenu