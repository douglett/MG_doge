#include <string>
#include <vector>
#include <algorithm>
#include "src/globals.h"

using namespace std;


namespace spellmenu {

	static int  use_card();
	static void move_cursor(int direction);
	static void remake_hand();

	// globals
	vector<int> hand;

	// member vars
	static const int HAND_SIZE_MAX = 3;
	static const string CARD_NAMES[] = {
		"SPADE",
		"HEART",
		"CLUB",
		"DIAMOND" };
	static vector<int> deck, playdeck;
	static int cursorpos = 0;
	
	
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


	// external info & status effects
	void give_card(int card) {
		deck.push_back(card);
		playdeck.push_back(card);
		remake_hand();
		ss(1) << "got a new card: " << CARD_NAMES[card];
		combatlog(ss().str());
	}
	void reset_cards() {
		playdeck.erase(playdeck.begin(), playdeck.end());
		for (auto i : deck)
			playdeck.push_back(i);
		remake_hand();
	}
	void clear_deck() {
		deck.erase(deck.begin(), deck.end());
		deck.push_back(CARD_HEART);
		reset_cards();
	}
	int cursor_pos() {
		return cursorpos;
	}
	int deck_remaining() {
		return max(int(playdeck.size()) - 3, 0);
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
			playdeck.erase(playdeck.begin()+cursorpos);
			remake_hand();
			return 1;
		} else {
			return 0;
		}
	}


	// recalculate current hand from deck-in-play
	static void remake_hand() {
		hand.erase(hand.begin(), hand.end());
		for (int i = 0; i < min(int(playdeck.size()), HAND_SIZE_MAX); i++)
			hand.push_back( playdeck[i] );
	}

} // end spellmenu

