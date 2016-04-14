#include "src/globals.h"

using namespace std;


namespace castmenu {
	int dir = 1;
	static void (*callback)(int) = NULL;

	void reset(void (*func)(int)) {
		scene::add(scene::CASTMENU);
		dir = 1;
		callback = func;
	}

	int step(const string& k) {
		if (k == "^u")
			dir = 0;
		else if (k == "^r")
			dir = 1;
		else if (k == "^d")
			dir = 2;
		else if (k == "^l")
			dir = 3;
		else if (k == "x") {
			scene::clear(scene::CASTMENU);  // go back
			if (callback != NULL)
				callback(0);
		} else if (k == "z") {
			action::dospell(spellmenu::CARD_SPADE);
			scene::clear(scene::CASTMENU);
			if (callback != NULL)
				callback(1);
		}
		return 0;
	}
} // end castmenu