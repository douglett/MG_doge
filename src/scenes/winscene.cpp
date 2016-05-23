#include <iostream>
#include "scenes.h"

using namespace std;


namespace winscene {

	void reset() {
		scene::add(scene::WINSCENE);
		fadeblack::reset(fadeblack::FADEIN, NULL);
	}

	static void callback_title() {
		scene::clear(scene::WINSCENE);
		titlemenu::reset();
	}
	
	int step(const string& k) {
		if (k == "^ret") {
			fadeblack::reset(fadeblack::FADEOUT, callback_title);
		}
		return 0;
	}

} // end winscene