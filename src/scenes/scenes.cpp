#include <iostream>
#include "src/globals.h"

using namespace std;


namespace scene {

	vector<int> scenestack;

	// handy debug function
	void show() {
		cout << "scenes: ";
		for (auto& s : scenestack)
			cout << s << " "; 
		cout << endl;
	}

	int current() {
		if (scenestack.size() == 0)
			return NOSCENE;
		return scenestack[scenestack.size()-1];
	}

	int add(SCENETYPE mode) {
		scenestack.push_back(mode);
		// show();
		return 0;
	}

	int clear(SCENETYPE mode) {
		for (int i = scenestack.size()-1; i >= 0; i--)
			if (scenestack[i] == mode) {
				scenestack.erase(scenestack.begin()+i);
				// show();
				return 1;
			}
		return 0;
	}

} // end scene
