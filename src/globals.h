#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <SDL.h>
#include "scenes/scenes.h"

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
		FDISK,
		GKING
	};
	mob make(std::map<std::string, int>& mm);
}

// attack actions
namespace action {
	int  taketurn();
	int  playeraction(const std::string& k);
	int  dospell(int cardtype);
}

// display
namespace display {
	extern SDL_Texture *sprites, *crownsprite;
	extern int animstate;
	extern SDL_Rect camera;
	extern const SDL_Rect 
		crown,
		cardback,
		spade,
		heart,
		club,
		diamond;
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
void  reflowfog();
int   level_up();
int   chest_item();
