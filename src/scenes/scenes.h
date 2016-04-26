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
		CASTMENU,
		FADEBLACK,
		FADEWHITE
	};
	extern std::vector<int> scenestack;
	void show();
	int  current();
	int  add(SCENETYPE mode);
	int  clear(SCENETYPE mode);
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

// spell menu actions 
namespace spellmenu {
	enum CARDTYPE {
		CARD_SPADE,
		CARD_HEART,
		CARD_CLUB,
		CARD_DIAMOND
	};
	extern std::vector<int> hand;
	int  action(const std::string& k);
	void give_card(int card);
	void reset_cards();
	void clear_deck();
	int  cursor_pos();
	int  deck_remaining();
	int  deck_full();
}

// select new card
namespace cardpicker {
	void reset();
	int  step(const std::string& k);
	void draw();
}

// aim casted spells
namespace castmenu {
	extern int dir;
	void reset(void (*func)(int));
	int  step(const std::string& k);
}