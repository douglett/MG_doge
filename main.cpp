#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include "libsrc/xcengine.h"
#include "globals.h"
// #include "mapgen/tmap.h"
// #include "mapgen/lazymap.h"
#include "mapgen/testmap.h"

using namespace std;


mob  create_mob(map<string, int>& mm);
int  get_action();
void revealfog();


const vector<string> mob_names = {
	"???",
	"scorp",
	"cakey"
};
const int FOG_ENABLED = 1;


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

	// sprite images
	display::sprites = texture::get("images")->tex;

	// set up main player
	display::camera.w = ceil(game::width/12.0);
	display::camera.h = ceil(game::height/12.0);
	playermob.hp = playermob.maxhp = 20;
	playermob.xp = 0;
	// playermob.x = 4;
	// playermob.y = 3;
	playermob.name = "player";

	reset_level(true);

	gamestate::movecount = 0;

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


void reset_level(int reset_player) {
	// build maps
	testmap::buildmap(6000 + dungeon_floor);
	gmap = testmap::gmap;
	auto& mobcache = testmap::gmobs;

	// see if the map creator sent us some start coordinates
	if (mobcache.size() > 0 && mobcache[0]["type"] == -1) {
		if (reset_player) {
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
			string(gmap[0].size(), FOG_ENABLED) );

	// reset player
	srand(time(NULL));
	playermob.hp = playermob.maxhp;
	menu::reset_cards();
	revealfog();
	display::centercam();
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


int get_action_inner() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		 case SDL_QUIT:
			return action::ACT_KILL;
		 case SDL_WINDOWEVENT:
			// (int)event.window.event
			break;
		 case SDL_KEYDOWN:
			// handle key press
			switch (event.key.keysym.sym) {
			 case SDLK_ESCAPE:
				return action::ACT_KILL;
			 case SDLK_LEFT:
				return action::ACT_WEST;
			 case SDLK_RIGHT:
				return action::ACT_EAST;
			 case SDLK_UP:
				return action::ACT_NORTH;
			 case SDLK_DOWN:
				return action::ACT_SOUTH;
			 case SDLK_SPACE:
			 case SDLK_z:
				return action::ACT_ACTION;
			 case SDLK_x:
				return action::ACT_CANCEL;
			 case SDLK_s:
				return action::ACT_MENU;
			 case SDLK_a:
				return action::ACT_SELECT;
			}
			break;
		}  // end switch
	}  // end while

	return action::ACT_NONE;
}

int get_action() {
	int event = get_action_inner();
	if (game::clearevents())
		return action::ACT_KILL;
	return event;
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
		combatlog("you leveled up!");
		return 1;
	}
	return 0;
}

int chest_item() {
	combatlog("you got a bean");
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
	for (int y = -3; y <= 3; y++) {
		yy = playermob.y + y;
		for (int x = -3; x <= 3; x++) {
			xx = playermob.x + x;
			if (xx < 0 || xx >= fogofwar[0].size() || yy < 0 || yy >= fogofwar.size())
				continue;
			if (abs(x) + abs(y) < 4)
				fogofwar[playermob.y+y][playermob.x+x] = 0;
		}
	}
}
