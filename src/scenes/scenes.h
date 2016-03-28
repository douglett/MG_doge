#pragma once

#include <string>
#include <vector>

// handle all game scenes
namespace scene {
	enum SCENETYPE {
		NOSCENE,
		TITLEMENU,
		GAME,
		SPELLMENU,
		CARDPICKER,
		FADEBLACK,
		FADEWHITE
	};
	extern std::vector<int> scenestack;
	int current();
	int add(SCENETYPE mode);
	int clear(SCENETYPE mode);
}

// spell menu actions 
namespace spellmenu {
	enum cardtypes {
		CARD_SPADE,
		CARD_HEART,
		CARD_CLUB,
		CARD_DIAMOND
	};
	extern std::vector<int> cards;
	extern int deck_size;
	extern int handpos;
	int action(const std::string& k);
	int reset_cards();
	int givecard();
}

namespace titlemenu {
	void reset();
	int  step(const std::string& k);
}

namespace fadeblack {
	enum FADEDIR {
		FADEIN = -1,
		FADEOUT = 1
	};
	void reset(int dir, void (*func)());
	int  step(const std::string& k);
	void draw();
}

namespace fadewhite {
	void reset();
	int  step(const std::string& k);
	void draw();	
}
