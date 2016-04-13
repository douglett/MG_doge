#include "globals.h"
// #include "mapgen/testmap.h"
#include "mapgen/boxmap.h"

using namespace std;


// consts
static const int 
	FOG_ENABLED = 1,
	FOG_SPOTLIGHT_SIZE = 4;

// globals
int seed = 6000;
int movecount = 0;
int dungeon_floor = 1;
mob playermob;



// add items to the combat log, alternating colors per turn
void combatlog(const string& s) {
	static int 
		last_movecount = 0,
		last_col = 0;
	// calculate alternating color
	if (movecount != last_movecount) {
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


void start_game() {
	// reset game;
	playermob.hp = playermob.maxhp = 20;
	playermob.lvl = 1;
	playermob.xp = 0;
	playermob.atk = 1;
	playermob.def = 0;
	movecount = 0;
	dungeon_floor = 1;
	// seed = stringtoseed(playermob.name)
	combat_log.erase(combat_log.begin(), combat_log.end());
	spellmenu::clear_deck();
	reset_level(true);
	player_rest();

	scene::add(scene::GAME);
	// fadeblack::reset(fadeblack::FADEIN, NULL);
	castmenu::reset();
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
	// clear gtexts
	gtexts.erase(gtexts.begin(), gtexts.end());

	// reset cam
	revealfog();
	display::centercam();
}


void player_rest() {
	// reset player
	srand(time(NULL));
	playermob.hp = playermob.maxhp;
	spellmenu::reset_cards();
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


void cleardead() {
	for (int i = 0; i < gmobs.size(); i++)
		if (gmobs[i].hp <= 0) {
			// die message
			ss(1) << gmobs[i].name << " died.";
			// add xp
			if (gmobs[i].lvl >= playermob.lvl - 2) {
				playermob.xp += gmobs[i].xp;
				ss() << " +" << gmobs[i].xp << "xp";
			}
			combatlog(ss().str());
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
