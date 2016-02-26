#include <iomanip>
#include "libsrc/xcengine.h"
#include "globals.h"

using namespace std;


namespace display {

	// consts
	const SDL_Rect
		parchment = { 0, 0, 100, 28 },
		cardback = { 0, 29, 16, 18 },
		spade = { 17, 30, 14, 16 },
		heart = { 31, 30, 14, 16 },
		club = { 47, 30, 15, 16 },
		diamond = { 62, 30, 14, 16 },
		man = { 0, 48, 12, 12 },
		scorpion = { 24, 48, 12, 12 },
		cake = { 48, 48, 12, 12 },
		flame = { 0, 63, 12, 12 },
		stairs = { 24, 63, 12, 12 },
		chest = { 48, 63, 12, 12 },
		chest_open = { 72, 63, 12, 12 };

	// external vars
	int animstate = 0;
	SDL_Texture* sprites = NULL;
	SDL_Rect camera = { 0, 0, 10, 10 };
	// internal vars
	const int COMBAT_LOG_LENGTH = 6;
	int showmenu = 1;



	// all draw actions
	void draw() {
		static const int
			offsety = 0,
			offsetx = 0;

		// vars
		SDL_Rect src, dst;

		// cls
		SDL_SetRenderDrawColor(game::ren, 0, 0, 0, 255);
		SDL_RenderClear(game::ren);

		// draw map
		dst = { 0, 0, 12, 12 };
		for (int y = 0; y < camera.h; y++) {
			if (camera.y+y < 0 || camera.y+y >= gmap.size())
				continue;
			dst.y = y * 12 + offsety;

			for (int x = 0; x < camera.w; x++) {
				if (camera.x+x < 0 || camera.x+x >= gmap[0].size())
					continue;

				dst.x = x * 12 + offsetx;
				// draw block
				if ( fogofwar[camera.y+y][camera.x+x] == 0 ) {
					// background color
					switch ( gmap[camera.y+y][camera.x+x] ) {
						case ' ':
							continue;  // use background
						case '#':
							SDL_SetRenderDrawColor(game::ren, 100, 100, 100, 255);
							break;
						case '/':
							SDL_SetRenderDrawColor(game::ren, 160, 100, 100, 255);
							break;
						case '.':
						case 'i':
						case '%':
						case 'C':
						case 'c':
							SDL_SetRenderDrawColor(game::ren, 0, 200, 0, 255);
							break;
						default:
							SDL_SetRenderDrawColor(game::ren, 255, 0, 255, 255);  // unknown - hot pink
					}
					// foreground tile
					const SDL_Rect* tile = NULL;
					switch ( gmap[camera.y+y][camera.x+x] ) {
						case 'i':
							tile = &flame;
							break;
						case '%':
							tile = &stairs;
							break;
						case 'C':
							tile = &chest;
							break;
						case 'c':
							tile = &chest_open;
							break;
					}
					// draw em
					SDL_RenderFillRect(game::ren, &dst); // fill
					if (tile) {
						SDL_Rect s = *tile;
						s.x += 12 * animstate;
						SDL_RenderCopy(game::ren, sprites, &s, &dst); // draw top sprite
					}
				}
				// draw lines
				// SDL_SetRenderDrawColor(game::ren, 255, 150, 150, 255);
				// SDL_RenderDrawRect(game::ren, &dst);
			}
		}

		// main character
		src = man;
		src.x += 12 * animstate;
		dst = man;
		dst.x = 12 * (playermob.x - camera.x) + offsetx;
		dst.y = 12 * (playermob.y - camera.y) + offsety;
		SDL_RenderCopy(game::ren, sprites, &src, &dst);

		// mobs
		for (auto m : gmobs) {
			if (m.x < camera.x || m.x >= camera.x+camera.w || m.y < camera.y || m.y >= camera.y+camera.h)
				continue;
			if ( fogofwar[m.y][m.x] == 1 )
				continue;
			if (m.type == 1)
				src = scorpion;
			else if (m.type == 2)
				src = cake;
			src.x += 12 * animstate;
			dst = src;
			dst.x = 12 * (m.x - camera.x) + offsetx;
			dst.y = 12 * (m.y - camera.y) + offsety;
			SDL_RenderCopy(game::ren, sprites, &src, &dst);
		}

		// draw special effects
		for (auto e : effects) {
			dst.x = 12 * (e.x - camera.x) + offsetx;
			dst.y = 12 * (e.y - camera.y) + offsety;
			if (e.type == 1) {
				src = flame;
				src.x += 12 * animstate;
				SDL_RenderCopy(game::ren, sprites, &src, &dst);
			} else {
				SDL_SetRenderDrawColor(game::ren, 255, 0, 0, 100);
				SDL_RenderFillRect(game::ren, &dst);
			}
		}
		
		// attack text
		for (auto g : gtexts) {
			dst.x = 12 * (g.x - camera.x) + offsetx + 1;
			dst.y = 12 * (g.y - camera.y) + offsety - 2;
			game::qbcolor(0, 0, 0);
			game::qbprint(dst.x+1, dst.y+1, g.s);
			if (g.type == 1)
				game::qbcolor(0, 240, 0);
			else
				game::qbcolor(200, 0, 0);
			game::qbprint(dst.x, dst.y, g.s);
		}
		
		// draw game menus
		draw_menu();
	}


	// helpers
	void centercam() {
		camera.x = playermob.x - floor((camera.w-0.5)/2);
		camera.y = playermob.y - floor((camera.h-0.5)/2);
	}
	static void drawcard(int type, int x, int y) {
		static SDL_Rect cards[] = { spade, heart, club, diamond };

		SDL_Rect dst = cardback;
		dst.x = x;
		dst.y = y;
		SDL_RenderCopy(game::ren, sprites, &cardback, &dst);

		dst = cards[type];
		dst.x = x + 1;
		dst.y = y + 1;
		SDL_RenderCopy(game::ren, sprites, &cards[type], &dst);
	}
	static void menu_text(int line, string s, const int* col) {
		int x = game::width - (s.length()*8) - 1;
		int y = line * 8 + 1;
		game::qbcolor(0, 0, 0);
		game::qbprint(x+1, y+1, s);
		game::qbcolor(col[0], col[1], col[2]);
		game::qbprint(x, y, s);
	}


	void draw_menu() {
		// draw large info
		if (showmenu) {
			// draw parchment background
			auto parchment_pos = parchment;
			// parchment_pos.x = (game::width - parchment_pos.w) - 1;
			// parchment_pos.y = game::height - parchment_pos.h - 1;
			parchment_pos.x = 1;
			parchment_pos.y = 1;
			SDL_RenderCopy(game::ren, sprites, &parchment, &parchment_pos);

			// draw cards (staggered)
			// drawcard(0, 22, 74);
			// drawcard(1, 39, 78);
			// drawcard(2, 56, 81);
			// drawcard(3, 73, 80);

			// draw cards (inline)
			for (int i = 0; i < menu::cards.size(); i++)
				drawcard(menu::cards[i], parchment_pos.x+22+(i*17), parchment_pos.y+6);
			
			// menu markers
			if (gamestate::gamemode == gamestate::MODE_GAMEMENU) {
				int x = parchment_pos.x + 22 + 5 + (menu::handpos*17);
				int y = parchment_pos.y + 26;
				string s = string()+char(24);
				game::qbcolor(70, 70, 70);
				game::qbprint(x+1, y+1, s);
				game::qbcolor(255, 0, 0);
				game::qbprint(x, y, s);
			}

			// background
			// SDL_Rect textbox = { 0, 1, 41, 28 };
			// textbox.x = game::width - textbox.w - 1;
			// SDL_SetRenderDrawColor(game::ren, 0, 0, 0, 150);
			// SDL_RenderFillRect(game::ren, &textbox);

			static const int 
				yellow[] = { 230, 230, 0 },
				green[] = { 0, 200, 0 },
				red[] = { 200, 80, 80 };

			// HP text
			ss(1) << setfill('0') << setw(2) << playermob.hp << "/"
				<< setfill('0') << setw(2) << playermob.maxhp;
			menu_text(0, ss().str(), green);
			// ATK text
			ss(1) << playermob.atk << " atk";
			menu_text(1, ss().str(), yellow);
			// DEF text
			ss(1) <<  playermob.def << " def";
			menu_text(2, ss().str(), yellow);
			// LVL text
			ss(1) << playermob.lvl << " lvl";
			menu_text(3, ss().str(), yellow);
			// XP text
			ss(1) << playermob.xp << " xp";
			menu_text(4, ss().str(), yellow);
			// move count
			ss(1) << gamestate::movecount << "m";
			menu_text(5, ss().str(), red);

			// combat log
			for (int i = 0; i < COMBAT_LOG_LENGTH; i++) {
				if (i >= combat_log.size())
					break;
				int x = 1;
				int y = game::height - 10 - 8*i;
				auto s = *(combat_log.end() - (1+i));
				game::qbcolor(0, 0, 0);
				game::qbprint(x+1, y+1, s);
				// game::qbcolor(255, 255, 255);
				game::qbcolor(230, 230, 0);
				game::qbprint(x, y, s);
			}
		}
		// draw small info
		else {
			// HP text
			ss(1) << playermob.hp << "/" << playermob.maxhp;
			string s = ss().str();

			// background
			SDL_Rect textbox = { 0, 1, int(s.length())*8 + 1, 10 };
			textbox.x = game::width - textbox.w - 1;
			SDL_SetRenderDrawColor(game::ren, 0, 0, 0, 150);
			SDL_RenderFillRect(game::ren, &textbox);

			game::qbcolor(0, 0, 0);
			game::qbprint(textbox.x+2, textbox.y+2, s);
			game::qbcolor(0, 200, 0);
			game::qbprint(textbox.x+1, textbox.y+1, s);
		}
	}

} // end display