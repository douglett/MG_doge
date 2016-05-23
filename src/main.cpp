#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include "libsrc/xcengine.h"
#include "globals.h"

using namespace std;


// local functions
static int loopt();
static string getkey();

// main.cpp globals
vector<string> gmap, fogofwar, cheats;
vector<pair<int, string> > combat_log;
vector<mob> gmobs, effects;
vector<gtext> gtexts;



int main(int argc, char** argv) {
	cout << "doge start" << endl;
	for (int i = 0; i < argc; i++)
		cheats.push_back(argv[i]);

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
	return 0;
}


// helper - manage global stringstream
stringstream& ss(int reset) {
	static stringstream strm;
	if (reset)
		strm.str(""), strm.clear();
	return strm;
}


// main loop
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
				display::draw_titlemenu();
				break;
			 case FADEBLACK:
			 	fadeblack::draw();
				break;
			 case FADEWHITE:
			 	fadewhite::draw();
			 	break;
			 case GAME:
			 	display::draw_gamescene();
			 	display::draw_spellmenu();
			 	break;
			 case CARDPICKER:
			 	cardpicker::draw();
			 	break;
			 case WINSCENE:
			 	display::draw_winscene();
			 	break;
			}
		// display
		display::flip();

		// handle input
		int rval = 0;
		string k = getkey();
		if (k == "^q")
			break;
		switch (scene::current()) {
		 case TITLEMENU:
		 	rval = titlemenu::step(k);
			break;
		 case FADEBLACK:
		 	rval = fadeblack::step(k);
			break;
		 case FADEWHITE:
		 	rval = fadewhite::step(k);
		 	break;
		 case GAME:
		 	rval = action::playeraction(k);
		 	break;
		 case SPELLMENU:
			rval = spellmenu::action(k);
			break;
		 case CARDPICKER:
		 	rval = cardpicker::step(k);
		 	break;
		 case CASTMENU:
		 	rval = castmenu::step(k);
		 	break;
		 case WINSCENE:
		 	rval = winscene::step(k);
		}
		// handle return values
		// ...
	}

	return 0;
}


// get key input as useful string
static string getkey() {
	SDL_Event event;
	string keystr = "";
	int key;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		 case SDL_QUIT:
			keystr = "^q";  // override, force quit
			break;
		 case SDL_KEYDOWN:
		 	if (keystr.length())
		 		break;
		 	key = event.key.keysym.sym;
		 	if (key == SDLK_ESCAPE)
		 		keystr = "^q";
		 	else if (key == SDLK_RETURN)
		 		keystr = "^ret";
		 	else if (key >= SDLK_a && key <= SDLK_z)
		 		keystr += 'a' + (key - SDLK_a);
		 	else if (key >= SDLK_0 && key <= SDLK_9)
		 		keystr += '0' + (key - SDLK_0);
		 	else {
		 		keystr = '^';
		 		switch (key) {
		 		 case SDLK_UP:
		 		 	keystr += 'u';
		 		 	break;
		 		 case SDLK_DOWN:
		 		 	keystr += 'd';
		 		 	break;
		 		 case SDLK_LEFT:
		 		 	keystr += 'l';
		 		 	break;
		 		 case SDLK_RIGHT:
		 			keystr += 'r';
		 			break;
		 		 case SDLK_BACKSPACE:
		 		 case SDLK_DELETE:
		 		 	keystr += "bs";
		 		 	break;
		 		 default:
		 		 	keystr = "";
		 			printf("k %d\n", key);
		 		}
			}
		}
	}

	return keystr;
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
