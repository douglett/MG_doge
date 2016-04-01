#include "globals.h"
#include <map>

using namespace std;


// initial stats for various mobs.
// TODO: could be cleaner, in own file, etc
namespace mobmaker {

	mob make(map<string, int>& mm) {
		mob m;
		m.x = mm["x"];
		m.y = mm["y"];
		m.type = mm["type"];

		switch (m.type) {
		 case CAKEY:
		 	m.name = "cakey";
		 	m.lvl = 1;
		 	m.xp = 3;
		 	m.maxhp = 4;
		 	m.atk = 1;
		 	break;
		 case SCORP:
		 	m.name = "scorp";
		 	m.lvl = 2;
		 	m.xp = 6;
		 	m.maxhp = 2;
		 	m.atk = 2;
		 	m.def = 2;
		 	break;
		 case GHOST:
		 	m.name = "ghost";
		 	m.lvl = 3;
		 	m.xp = 10;
		 	m.maxhp = 5;
		 	m.atk = 2;
		 	m.def = 5;
		 	break;
		 case FDISK:
		 	m.name = "fdisk";
		 	m.lvl = 4;
		 	m.xp = 15;
		 	m.maxhp = 15;
		 	m.def = 2;
		 	m.atk = 4;
		 	break;
		 default:
		 	m.name = "???";
		}

		m.hp = m.maxhp;
		return m;
	}

} // end mobmaker