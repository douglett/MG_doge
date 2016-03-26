#include <string>
#include <vector>
#include "src/globals.h"

using namespace std;


namespace spellmenu {

	int  use_card();
	void move_hand(int direction);

	const int HAND_SIZE_MAX = 4;

	vector<int> cards;
	int deck_size = 1;
	int draw_count = 0;
	int handpos = 0;
	
	
	int action(const string& k) {
		if (k == "^l") {
			move_hand(-1);
		} else if (k == "^r") {
			move_hand(1);
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
	

	void move_hand(int direction) {
		handpos += direction;
		if (handpos < 0)
			handpos = 0;
		else if (handpos >= HAND_SIZE_MAX)
			handpos = HAND_SIZE_MAX - 1;
	}


	int use_card() {
		if (handpos < 0 || handpos >= cards.size())
			return 0;
		int card = cards[handpos];
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
			cards.erase(cards.begin()+handpos);
			return 1;
		} else {
			return 0;
		}
	}


	int reset_cards() {
		cards.erase(cards.begin(), cards.end());
		cards.push_back(CARD_HEART);
		draw_count = 1;
		return 0;
	}


	int givecard() {
		if (cards.size() >= HAND_SIZE_MAX || draw_count >= deck_size)
			return 0;
		cards.push_back( rand() % 4 );
		draw_count++;
		return 1;
	}

} // end spellmenu

