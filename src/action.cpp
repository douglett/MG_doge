#include <string>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "globals.h"

using namespace std;


const string 
	walkable = ".,/",
	actionable = "%ijC";


namespace action {

	static int  playeraction(const std::string& k);
	static int  collision(int x, int y);
	static mob* findmob(int x, int y);
	static void doattack(mob* attacker, mob* defender);
	static void doactionblock(int x, int y);
	static void allenemyactions();
	static int  enemyaction(mob& m);
	static void dofireball(int x, int y);
	static void doheal(mob* m);
	static void callback_gototitle();


	// called from main loop - take a turn
	int taketurn(const string& k) {
		int action_performed = playeraction(k);

		// do turn actions
		if (action_performed) {
			allenemyactions();
			cleardead();
			spellmenu::givecard(); // add random card to hand if space available
			movecount++; // increment moves
			revealfog();
			display::centercam();
		}

		if (playermob.hp <= 0) {
			playermob.hp = 0;
			fadeblack::reset(1, callback_gototitle);
		}

		return 0;
	}

	static void callback_gototitle() {
		scene::clear(scene::GAME);
		titlemenu::reset();
	}


	// called from spellmenu - casts a spell
	int dospell(int cardtype) {
		int x = playermob.x;
		int y = playermob.y;
		switch(cardtype) {
		 case spellmenu::CARD_HEART:
		 	combatlog("spell: heart");
			doheal(&playermob);
		 	return 1;
		 case spellmenu::CARD_CLUB:
			dofireball(x-1, y);
		 	dofireball(x+1, y);
		 	dofireball(x, y-1);
		 	dofireball(x, y+1);
		 	ss(1) << "spell: club";
		 	combatlog(ss().str());
		 	return 1;
		 case spellmenu::CARD_DIAMOND:
		 	dofireball(x-1, y-1);
		 	dofireball(x+1, y-1);
		 	dofireball(x-1, y+1);
		 	dofireball(x+1, y+1);
		 	ss(1) << "spell: diamond";
		 	combatlog(ss().str());
		 	return 1;
		 case spellmenu::CARD_SPADE:
		 	combatlog("spell: spade (fail)");
		 	return 1;
		}
		return 0;
	}

	
	int playeraction(const string& k) {
		int x = 0, y = 0;
		int collide = -1;  // default, no movement

		if (k == "^l") {
			x = -1;
			collide = collision(playermob.x + x, playermob.y + y);
		} else if (k == "^r") {
			x = +1;
			collide = collision(playermob.x + x, playermob.y + y);
		} else if (k == "^d") {
			y = +1;
			collide = collision(playermob.x + x, playermob.y + y);
		} else if (k == "^u") {
			y = -1;
			collide = collision(playermob.x + x, playermob.y + y);
		} else if (k == "z") {
			collide = 0; // no-op
		} else if (k == "s") {
		 	// gamestate::gamemode = gamestate::MODE_GAMEMENU;
		 	scene::add(scene::SPELLMENU);
		}

		// do movement actions
		if (collide == 0 || collide == 2 || collide == 4) {
			gtexts.erase(gtexts.begin(), gtexts.end());
			effects.erase(effects.begin(), effects.end());
			// player can move
			if (collide == 0) {
				playermob.x += x;
				playermob.y += y;
			} 
			// player hits a mob - do attack
			else if (collide == 2) {
				doattack(&playermob, findmob(playermob.x + x, playermob.y + y));
			} 
			// player hits an action block
			else if (collide == 4) {
				doactionblock(playermob.x+x, playermob.y+y);
			}
			return 1;
		}

		return 0;
	}


	int collision(int x, int y) {
		if (y < 0 || y >= gmap.size() || x < 0 || x >= gmap[0].size())
			return 1; // out of bounds
		if ( count(actionable.begin(), actionable.end(), gmap[y][x]) > 0 )
			return 4; // hit an action square
		if ( count(walkable.begin(), walkable.end(), gmap[y][x]) == 0 )
			return 1; // hit a wall
		for (auto &m : gmobs)
			if (m.x == x && m.y == y)
				return 2; // hit an enemy
		if (playermob.x == x && playermob.y == y)
			return 3; // hit the player (mob use)
		return 0; // no collision
	}


	mob* findmob(int x, int y) {
		for (auto& m : gmobs)
			if (m.x == x && m.y == y)
				return &m;
		if (playermob.x == x && playermob.y == y)
			return &playermob;
		return NULL;
	}


	void doattack(mob* attacker, mob* defender) {
		assert(attacker != NULL && defender != NULL);

		// do attack
		int atk = attacker->atk - defender->def;
		if (atk < 1) atk = 1;
		defender->hp -= atk;

		// display attack
		ss(1) << atk;
		gtexts.push_back({ defender->x, defender->y, 0, ss().str() });

		// add player log
		ss(1) << attacker->name << " hits " << defender->name << ": -" << atk;
		combatlog(ss().str());
	}


	void doactionblock(int x, int y) {
		char block = gmap[y][x];
		switch (block) {
		 case 'i':  // brazier
		 	spellmenu::deck_size++;
		 case 'j':  // brazier lit
		 	gmap[y][x] = 'j';
		 	combatlog("brazier: reset level");
		 	reset_level();
		 	player_rest();
		 	fadewhite::reset();
		 	break;
		 case '%':  // ladder
		 	combatlog("descended the ladder");
		 	dungeon_floor++;
		 	// loop_fadeblack(1, 2);
		 	reset_level(true);
		 	// loop_fadeblack(0, 2);
		 	break;
		 case 'C':
		 	combatlog("opened a chest");
		 	chest_item();
		 	gmap[y][x] = 'c';
		 	break;
		 default:
			cout << "unknown action block: " << block << endl;
			break;
		}
	}


	void doheal(mob* target) {
		assert(target != NULL);

		// do heal
		int heal = 4 + round( target->lvl/2 );
		if (target->hp >= target->maxhp)
			heal = 0;
		else if (heal > target->maxhp - target->hp)
			heal = target->maxhp - target->hp;
		target->hp += heal;

		// display attack
		ss(1) << heal;
		gtexts.push_back({ target->x, target->y, 1, ss().str() });

		// add player log
		ss(1) << "-> " << target->name << " (+" << heal << ")";
		combatlog(ss().str());
	}


	void dofireball(int x, int y) {
		mob* m = findmob(x, y);
		if (m) {
			int dmg = 4 + round( playermob.lvl/2 );
			m->hp -= dmg;
			// display attack
			ss(1) << dmg;
			gtexts.push_back({ x, y, 0, ss().str() });
		}
		// make fireball
		mob e;
		e.x = x;
		e.y = y;
		e.type = 1;
		e.name = "fireball";
		effects.push_back(e);
	}


	void allenemyactions() {
		for (auto &m : gmobs) {
			int dist = sqrt(pow(playermob.x - m.x, 2) + pow(playermob.y - m.y, 2));
			// printf("%d %d %d\n", (playermob.x - m.x), (playermob.y - m.y), dist);
			if (dist <= 3)
				enemyaction(m);
		}
	}


	int enemyaction(mob& m) {
		int diffx = playermob.x - m.x;
		int diffy = playermob.y - m.y;
		// cout << diffx << " " << diffy << endl;
		if (diffy <= -1)
			switch ( collision(m.x, m.y-1) ) {
			 case 0:
			 	m.y -= 1;
			 	return 1;
			 case 3:
			 	doattack(&m, &playermob);
			 	return 1;
			}	
		else if (diffy >= 1)
			switch ( collision(m.x, m.y+1) ) {
			 case 0:
				m.y += 1;
				return 1;
			 case 3:
				doattack(&m, &playermob);
				return 1;
			}
		if (diffx <= -1) 
			switch ( collision(m.x-1, m.y) ) {
			 case 0:
				m.x -= 1;
				return 1;
			 case 3:
				doattack(&m, &playermob);
				return 1;
			}
		else if (diffx >= 1)
			switch ( collision(m.x+1, m.y) ) {
			 case 0:
				m.x += 1;
				return 1;
			 case 3:
				doattack(&m, &playermob);
				return 1;
			}
		return 0;
	}

} // end actions
