#include <string>
#include "globals.h"

using namespace std;


namespace keys {

	string keystr;

	string& getkey() {
		SDL_Event event;
		keystr = "";
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
			 		keystr = "^e";
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
			 		 	keystr += 'b';
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

} // end keys