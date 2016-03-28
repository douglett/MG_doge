#include <string>
#include <vector>
#include "src/globals.h"

using namespace std;


namespace spellmenu {

	static int  use_card();
	static void move_cursor(int direction);

	// globals
	int deck_size = 1;
	int draw_count = 0;
	int cursorpos = 0;
	vector<int> hand;

	// member vars
	static const int HAND_SIZE_MAX = 4;
	static vector<int> deck;
	
	
	int action(const string& k) {
		if (k == "^l") {
			move_cursor(-1);
		} else if (k == "^r") {
			move_cursor(1);
		} else if (k == "z") {
			if ( use_card() ) {
		 		scene::clear(scene::SPELLMENU);
		 		cleardead();
		 		return 1;
		 	}
		} else if (k == "s" || k == "x") {
			scene::clear(scene::SPELLMENU);
		}
		return 0;
	}
	

	static void move_cursor(int direction) {
		cursorpos += direction;
		if (cursorpos < 0)
			cursorpos = 0;
		else if (cursorpos >= HAND_SIZE_MAX)
			cursorpos = HAND_SIZE_MAX - 1;
	}


	static int use_card() {
		if (cursorpos < 0 || cursorpos >= hand.size())
			return 0;
		int card = hand[cursorpos];
		int action_performed = 0;
		// try to use card
		switch (card) {
		 case CARD_SPADE:
		 case CARD_HEART:
		 case CARD_CLUB:
		 case CARD_DIAMOND:
		 	action_performed = action::dospell(card);
		 	break;
		}
		// check if card was used correctly
		if (action_performed) {
			hand.erase(hand.begin()+cursorpos);
			return 1;
		} else {
			return 0;
		}
	}


	int reset_cards() {
		hand.erase(hand.begin(), hand.end());
		hand.push_back(CARD_HEART);
		draw_count = 1;
		return 0;
	}


	int givecard() {
		if (hand.size() >= HAND_SIZE_MAX || draw_count >= deck_size)
			return 0;
		hand.push_back( rand() % 4 );
		draw_count++;
		return 1;
	}

	void givecard(int card) {
		deck.push_back(card);
	}

} // end spellmenu

