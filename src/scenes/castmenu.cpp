#include "src/globals.h"

using namespace std;


namespace castmenu {
	int dir = 1;

	void reset() {
		scene::add(scene::CASTMENU);
		dir = 1;
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
		else if (k == "x")
			scene::clear(scene::CASTMENU);  // go back
		else if (k == "z") {
			// switch (dir) {
			//  case 0:	playermob.y -= 5;	break;
			//  case 1:	playermob.x += 5;	break;
			//  case 2:	playermob.y += 5;	break;
			//  case 3:	playermob.x -= 5;	break;
			// }
			action::dospell(spellmenu::CARD_SPADE);
			scene::clear(scene::CASTMENU);
		}
		return 0;
	}
} // end castmenu