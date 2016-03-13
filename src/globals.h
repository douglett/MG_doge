#pragma once

#include <string>
#include <vector>
#include <sstream>
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
	int x = 0;
	int y = 0;
	int type = 0;
	std::string s;
};

// game state
namespace gamestate {
	enum mode {
		MODE_NONE,
		MODE_GAME,
		MODE_GAMEMENU
	};
	extern int gamemode;
	extern int movecount;
}

// attack actions
namespace action {
	enum Action {
		ACT_NONE = 0,
		ACT_KILL,
		ACT_WEST,
		ACT_EAST,
		ACT_NORTH,
		ACT_SOUTH,
		ACT_ACTION,
		ACT_CANCEL,
		ACT_MENU,
		ACT_SELECT
	};
	int  playeraction(int action);
	void allenemyactions();
	int  dospell(int cardtype);
}

// menu actions 
namespace menu {
	enum cardtypes {
		CARD_SPADE,
		CARD_HEART,
		CARD_CLUB,
		CARD_DIAMOND
	};
	extern std::vector<int> cards;
	extern int deck_size;
	extern int handpos;
	int playeraction(int action);
	int reset_cards();
	int givecard();
}

// display
namespace display {
	extern SDL_Texture *sprites, *crownsprite;
	extern int animstate;
	extern SDL_Rect camera;
	void draw();
	void centercam();
}

// globals
std::stringstream& ss(int reset = 0);
int   stringtoseed(std::string seedstr);
void  reset_level(int reset_pos = 0);
void  player_rest();
gtext create_gtext(int x, int y, std::string s, int type = 0);
void  cleardead();
void  combatlog(const std::string& s);
int   level_up();
int   chest_item();
extern int dungeon_floor;
extern std::vector<std::string> gmap, fogofwar, combat_log;
extern std::vector<mob> gmobs, effects;
extern std::vector<gtext> gtexts;
extern mob playermob;
