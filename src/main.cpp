#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include "libsrc/xcengine.h"
#include "globals.h"
// #include "mapgen/testmap.h"
#include "mapgen/boxmap.h"

using namespace std;


mob  create_mob(map<string, int>& mm);
int  get_action();
void revealfog();


const vector<string> mob_names = {
	"???",
	"scorp",
	"cakey"
};
const int 
	FOG_ENABLED = 1,
	FOG_SPOTLIGHT_SIZE = 4;


// main.cpp globals
int animtt = 0;
int dungeon_floor = 1;
vector<string> gmap, fogofwar, combat_log;
vector<mob> gmobs, effects;
mob playermob;
vector<gtext> gtexts;

namespace gamestate {
	int gamemode = MODE_GAME;
	int movecount = 0;
}



int main() {
	cout << "hello world" << endl;
	if (game::init())
		return 1;

	// game init work
	display::sprites = texture::get("images")->tex;
	display::camera.w = ceil(game::width/12.0);
	display::camera.h = ceil(game::height/12.0);

	// reset game
	gamestate::movecount = 0;
	playermob.name = "player";
	playermob.hp = playermob.maxhp = 20;
	dungeon_floor = 1;
	reset_level(true);
	player_rest();

	// main game loop
	while (true) {
		animtt++;
		if (animtt % 30 == 0) {
			animtt = 0;
			display::animstate = !display::animstate;
		}

		display::draw();
		SDL_RenderPresent(game::ren);
		game::waitscreen();
		
		// take player action
		int action = get_action();
		int action_performed = 0;
		if (action == action::ACT_KILL) {
			break;
		} else if (gamestate::gamemode == gamestate::MODE_GAME) {
			action_performed = action::playeraction(action);
		} else if (gamestate::gamemode == gamestate::MODE_GAMEMENU) {
			action_performed = menu::playeraction(action);
		}

		// do turn actions
		if (action_performed) {
			cleardead();
			action::allenemyactions();
			menu::givecard(); // add random card to hand if space available
			gamestate::movecount++; // increment moves
			revealfog();
			display::centercam();
		}
		
		// kill player
		if (playermob.hp <= 0) {
			cout << "you died" << endl;
			break;
		}
	}

	game::quit();
}


void reset_level(int reset_pos) {
	// build maps
	boxmap::buildmap(6000, dungeon_floor);
	gmap = boxmap::gmap;
	auto& mobcache = boxmap::gmobs;

	// see if the map creator sent us some start coordinates
	if (mobcache.size() > 0 && mobcache[0]["type"] == -1) {
		if (reset_pos) {
			playermob.x = mobcache[0]["x"];
			playermob.y = mobcache[0]["y"];
		}
		mobcache.erase(mobcache.begin());
	}

	// make mobs
	gmobs.erase(gmobs.begin(), gmobs.end());
	for (auto& mm : mobcache)
		gmobs.push_back(create_mob(mm));

	// make fog of war
	fogofwar = vector<string>(
			gmap.size(),
			string(gmap[0].size(), FOG_ENABLED*2) );
	// reset cam
	revealfog();
	display::centercam();
}

void player_rest() {
	// reset player
	srand(time(NULL));
	if (playermob.hp < playermob.maxhp)
		playermob.hp = playermob.maxhp;
	menu::reset_cards();
}

mob create_mob(map<string, int>& mm) {
	mob m;
	m.x = mm["x"];
	m.y = mm["y"];
	m.type = mm["type"];
	m.name = mob_names[m.type];
	return m;
}
gtext create_gtext(int x, int y, string s, int type) {
	gtext g;
	g.x = x;
	g.y = y;
	g.s = s;
	g.type = type;
	return g;
}


stringstream& ss(int reset) {
	static stringstream strm;
	if (reset)
		strm.str(""), strm.clear();
	return strm;
}


int get_action() {
	action::Action act = action::ACT_NONE;
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		 case SDL_QUIT:
			return action::ACT_KILL;  // override, force quit
		 case SDL_WINDOWEVENT:
			// (int)event.window.event
			break;
		 case SDL_KEYDOWN:
			// handle key press
			switch (event.key.keysym.sym) {
			 case SDLK_ESCAPE:
				return action::ACT_KILL;  // override
			 case SDLK_LEFT:
			 	if (!act) act = action::ACT_WEST;
				break;
			 case SDLK_RIGHT:
			 	if (!act) act = action::ACT_EAST;
				break;
			 case SDLK_UP:
			 	if (!act) act = action::ACT_NORTH;
				break;
			 case SDLK_DOWN:
			 	if (!act) act = action::ACT_SOUTH;
				break;
			 case SDLK_SPACE:
			 case SDLK_z:
			 	if (!act) act = action::ACT_ACTION;
				break;
			 case SDLK_x:
			 	if (!act) act = action::ACT_CANCEL;
				break;
			 case SDLK_s:
			 	if (!act) act = action::ACT_MENU;
				break;
			 case SDLK_a:
			 	if (!act) act = action::ACT_SELECT;
				break;
			}
			break;
		}  // end switch
	}  // end while

	return act;
}

void cleardead() {
	for (int i = 0; i < gmobs.size(); i++)
		if (gmobs[i].hp <= 0) {
			// die message
			ss(1) << gmobs[i].name << " died. +" << gmobs[i].xp << "xp";
			combatlog(ss().str());
			// add xp
			playermob.xp += gmobs[i].xp;
			level_up();
			// erase
			gmobs.erase(gmobs.begin()+i);
			i--;
		}
}

int level_up() {
	int nextlevel = 20 * pow(2, playermob.lvl-1);
	if (playermob.xp >= nextlevel) {
		playermob.lvl += 1;
		playermob.xp %= nextlevel; // add level num
		playermob.maxhp += 4;
		playermob.hp += 4; // add health
		combatlog("LEVEL UP!");
		return 1;
	}
	return 0;
}

int chest_item() {
	// 1 in 3 chance to get an upgrade
	switch (rand()%6) {
	 case 1:
	 	if (playermob.atk >= playermob.lvl) 
	 		break;
	 	playermob.atk++;
		ss(1) << "you got sword+" << playermob.atk;
		combatlog(ss().str());
		return 1;
	 case 2:
	 	if (playermob.def >= playermob.lvl) 
	 		break;
		playermob.def++;
		ss(1) << "you got mail+" << playermob.def;
		combatlog(ss().str());
		return 1;
	}
	
	// bad luck, or nothing to get this level - have a bean!
	playermob.hp += 4;
	combatlog("you got a bean! +4 hp");
	return 0;
}


void combatlog(const string& s) {
	combat_log.push_back(s);
	int overflow = combat_log.size() - 50;
	if (overflow > 0)
		combat_log.erase( combat_log.begin(), combat_log.begin()+overflow );
}

void revealfog() {
	int xx, yy;
	for (int y = -FOG_SPOTLIGHT_SIZE; y <= FOG_SPOTLIGHT_SIZE; y++) {
		yy = playermob.y + y;
		for (int x = -FOG_SPOTLIGHT_SIZE; x <= FOG_SPOTLIGHT_SIZE; x++) {
			xx = playermob.x + x;
			if (xx < 0 || xx >= fogofwar[0].size() || yy < 0 || yy >= fogofwar.size())
				continue;
			else if (abs(x) + abs(y) == FOG_SPOTLIGHT_SIZE)
				fogofwar[playermob.y+y][playermob.x+x] = 1;
			else if (abs(x) + abs(y) < FOG_SPOTLIGHT_SIZE)
				fogofwar[playermob.y+y][playermob.x+x] = 0;
		}
	}
}
