#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <SDL.h>
#include "rng.h"

// mob types
class mob {
public:
	int x = 0;
	int y = 0;
	int type = 0;
	int hp = 3;
	int maxhp = 3;
	int atk = 1;
	int def = 0;
	int lvl = 1;
	int xp = 3;
	std::string name;
};

// visible text
class gtext {
public:
	int x;
	int y;
	int type;
	std::string s;
};

// mobmaker
namespace mobmaker {
	enum MOBTYPE {
		UNKNOWN,
		CAKEY,
		SCORP,
		GHOST,
		FDISK
	};
	mob make(std::map<std::string, int>& mm);
}

// game states
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
namespace fadeblack {
	enum FADEDIR {
		FADEIN = -1,
		FADEOUT = 1
	};
	void reset(int dir);
	int  step(const std::string& k);
	void draw();
}
namespace fadewhite {
	void reset();
	int  step(const std::string& k);
	void draw();	
}
namespace titlemenu {
	void reset();
	int  step(const std::string& k);
}

// attack actions
namespace action {
	int  taketurn(const std::string& k);
	void allenemyactions();
	int  dospell(int cardtype);
}

// menu actions 
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

// display
namespace display {
	extern SDL_Texture *sprites, *crownsprite;
	extern int animstate;
	extern SDL_Rect camera;
	extern const SDL_Rect crown;
	void flip();
	void advance_anim();
	void centercam();
	void draw_titlemenu();
	void draw_gamescene();
	void draw_spellmenu();
}

// main.cpp globals
std::stringstream& ss(int reset = 0);
int   stringtoseed(std::string seedstr);
extern std::vector<std::string> gmap, fogofwar;
extern std::vector<std::pair<int, std::string> > combat_log;
extern std::vector<mob> gmobs, effects;
extern std::vector<gtext> gtexts;

// gamestate.cpp globals
extern int 
		seed, 
		dungeon_floor, 
		movecount;
extern mob playermob;
void  combatlog(const std::string& s);
void  start_game();
void  reset_level(int reset = 0);
void  player_rest();
void  cleardead();
void  revealfog();
int   level_up();
int   chest_item();
