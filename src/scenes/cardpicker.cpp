#include <cassert>
#include "libsrc/xcengine.h"
#include "src/globals.h"

using namespace std;


namespace cardpicker {
	static const int
		COLOR_MAX = 50;
	static int 
		card_1 = 0,
		card_2 = 2,
		pos = 0,
		dir = 1,
		colormod = 0;

	void reset() {
		if (spellmenu::deck_full())
			return;
		card_1 = rand()%4;
		card_2 = rand()%4;
		while (card_1 == card_2)
			card_2 = rand()%4;
		pos = 0;
		scene::add(scene::CARDPICKER);
	}

	int step(const string& k) {
		if (k == "z") {
			spellmenu::give_card(pos == 0 ? card_1 : card_2);
			scene::clear(scene::CARDPICKER);
		} 
		else if (k == "^r")
			pos = 1;
		else if (k == "^l")
			pos = 0;

		// flash card background
		colormod += dir*4;
		colormod = max(min(colormod, COLOR_MAX), 0);
		if (colormod == 0 || colormod == COLOR_MAX)
			dir *= -1;
		return 0;
	}

	void draw() {
		using namespace display;
		// setup
		const SDL_Rect* cards[] = { &spade, &heart, &club, &diamond };
		const int SCALE = 2;
		SDL_Rect back = { 0, 0, cardback.w*SCALE, cardback.h*SCALE };
		SDL_Rect front = { 0, 0, heart.w*SCALE, heart.h*SCALE };
		back.y = game::height/2 - cardback.h;
		front.y = back.y + 1*SCALE;

		// draw flashing selection box
		SDL_Rect dst = { 0, back.y-2*SCALE, back.w+4*SCALE, back.h+4*SCALE };
		if (pos == 0) 
			dst.x = game::width/2 - back.w - 8*SCALE;
		else
			dst.x = game::width/2 + 4*SCALE;
		SDL_SetRenderDrawColor(game::ren, 150+colormod, 0, 0, 180);
		SDL_RenderFillRect(game::ren, &dst);

		// card 1
		back.x = game::width/2 - back.w - 6*SCALE;
		front.x = back.x + 1*SCALE;
		SDL_RenderCopy(game::ren, sprites, &cardback, &back);
		SDL_RenderCopy(game::ren, sprites, cards[card_1], &front);
		// card 2
		back.x = game::width/2 + 6*SCALE;
		front.x = back.x + 1*SCALE;
		SDL_RenderCopy(game::ren, sprites, &cardback, &back);
		SDL_RenderCopy(game::ren, sprites, cards[card_2], &front);
	}
} // end cardpicker