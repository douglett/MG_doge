#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include "libsrc/xcengine.h"
#include "globals.h"
// #include "mapgen/testmap.h"
#include "mapgen/boxmap.h"

using namespace std;


static int loopt();


const int 
	FOG_ENABLED = 1,
	FOG_SPOTLIGHT_SIZE = 4;


// main.cpp globals
int seed = 6000;
int movecount = 0;
int dungeon_floor = 1;
vector<string> gmap, fogofwar;
vector<pair<int, string> > combat_log;
vector<mob> gmobs, effects;
mob playermob;
vector<gtext> gtexts;



int main() {
	cout << "hello world" << endl;
	if (game::init())
		return 1;

	// game init work
	display::sprites = texture::get("images")->tex;
	display::crownsprite = texture::get("crown")->tex;
	display::camera.w = ceil(game::width/12.0);
	display::camera.h = ceil(game::height/12.0);

	// start state
	titlemenu::reset();
	loopt();

	game::quit();
}


static int loopt() {
	using namespace scene;
	
	while (true) {
		// cls
		SDL_SetRenderDrawColor(game::ren, 0, 0, 0, 255);
		SDL_RenderClear(game::ren);
		// draw
		for (int mscene : scene::scenestack)
			switch (mscene) {
			 case TITLEMENU:
				display::draw_mainmenuscene();
				break;
			 case FADEBLACK:
			 	fadeblack::draw();
				break;
			 case GAME:
			 	display::draw_gamescene();
			 	display::draw_menu();
			 	break;
			}
		// display
		display::flip();

		// handle input
		int rval = 0;
		string k = keys::getkey();
		if (k == "^q")
			break;
		switch (scene::current()) {
		 case TITLEMENU:
		 	rval = titlemenu::step(k);
			break;
		 case FADEBLACK:
		 	rval = fadeblack::step(k);
			break;
		 case GAME:
		 	rval = action::taketurn(k);
		}
		// handle return values
		// ...
	}

	return 0;
}


void start_game() {
	// reset game;
	playermob.hp = playermob.maxhp = 20;
	movecount = 0;
	dungeon_floor = 1;
	// seed = stringtoseed(playermob.name)
	reset_level(true);
	player_rest();

	scene::add(scene::GAME);
	fadeblack::reset(fadeblack::FADEIN);
}


int loop_fadewhite() {
	SDL_Rect dst = { 0, 0, game::width, game::height };

	// main game loop
	for (int i = 0; i < 255; i += 4) {
		SDL_SetRenderDrawColor(game::ren, 0, 0, 0, 255);
		SDL_RenderClear(game::ren);  // cls
		
		display::draw_gamescene();
		display::draw_menu();

		int a = ( i < 128 ? i : 128-(i-128) );  // get alpha
		SDL_SetRenderDrawColor(game::ren, 255, 255, 255, a);
		SDL_RenderFillRect(game::ren, &dst);
		display::flip();

		// clear player action
		// if (get_action() == action::ACT_KILL)
		if (keys::getkey() == "^q")
			return 1;
	}

	return 0;
}


// change an 8 character word to an integer seed
int stringtoseed(string seedstr) {
	// sanitize seed a-z only
	string seed;
	for (auto c : seedstr) {
		c = tolower(c);
		if (seed.length() >= 8)
			;
		else if (c >= 'a' && c <= 'z')
			seed += c;
		else if (c >= '0' && c <= '9')
			seed += c;
	}
	// make prefix and suffix
	int s1 = 0;
	int s2 = 0;
	for (int i = 0; i < seed.length(); i++) {
		char c = seed[i];
		if (i < 3)
			s1 += c - 'a' + 1;
		else
			s2 += c - 'a' + 1;
	}
	int s = s1*100 + s2;  // make seed number
	printf("seed: %s  %d\n", seed.c_str(), s);  // debug
	return s;
}


void reset_level(int fullreset) {
	// if not resetting, check for used action blocks
	vector<pair<int, int> > usedblocks;
	if (!fullreset) {
		for (int y = 0; y < gmap.size(); y++)
			for (int x = 0; x < gmap[y].size(); x++)
				if (gmap[y][x] == 'c' || gmap[y][x] == 'j')
					usedblocks.push_back({ x, y });
	}

	// (re)build maps
	boxmap::buildmap(seed, dungeon_floor);
	gmap = boxmap::gmap;
	auto& mobcache = boxmap::gmobs;

	// replace used action blocks
	for (auto& p : usedblocks) {
		if (gmap[p.second][p.first] == 'C')
		 	gmap[p.second][p.first] = 'c';
		else if (gmap[p.second][p.first] == 'i')
			gmap[p.second][p.first] = 'j';
	}

	// see if the map creator sent us some start coordinates
	if (mobcache.size() > 0 && mobcache[0]["type"] == -1) {
		if (fullreset) {
			playermob.x = mobcache[0]["x"];
			playermob.y = mobcache[0]["y"];
		}
		mobcache.erase(mobcache.begin());
	}

	// make mobs
	gmobs.erase(gmobs.begin(), gmobs.end());
	for (auto& mm : mobcache)
		gmobs.push_back(mobmaker::make(mm));

	// clear fog of war
	if (fullreset) {
		fogofwar = vector<string>(
			gmap.size(), string(gmap[0].size(), FOG_ENABLED*2) );
	}

	// reset cam
	revealfog();
	display::centercam();
}


void player_rest() {
	// reset player
	srand(time(NULL));
	playermob.hp = playermob.maxhp;
	menu::reset_cards();
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


// helpers - create floating text inline
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


void cleardead() {
	for (int i = 0; i < gmobs.size(); i++)
		if (gmobs[i].hp <= 0) {
			// die message
			ss(1) << gmobs[i].name << " died. +" << gmobs[i].xp << "xp";
			combatlog(ss().str());
			// add xp
			if (gmobs[i].lvl >= playermob.lvl - 2)
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
	static int 
		last_movecount = 0,
		last_col = 0;
	// calculate alternating color
	if (movecount > last_movecount) {
		last_col = !last_col;
		last_movecount = movecount;
	}
	// add log
	combat_log.push_back({ last_col, s });
	// calculate overflow
	int overflow = combat_log.size() - 50;
	if (overflow > 0)
		combat_log.erase( combat_log.begin(), combat_log.begin()+overflow );
}
