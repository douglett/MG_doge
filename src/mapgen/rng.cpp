#include "rng.h"
#include <cassert>


namespace rng {

	#define CONA 36969
	#define CONB 18000
	#define CONZ 65535

	static uint32_t 
		m_z = 100,
		m_w = 200;

	void seed(uint32_t m) {
		assert(m > 0);
		m_z = m;
		m_w = m+1;
	}

	// George Marsaglia's MWC generator (multiply with carry)
	// thanks to John D. Cook for tutorial
	int32_t rand() {
		m_z = CONA * (m_z & CONZ) + (m_z >> 16);
		m_w = CONB * (m_w & CONZ) + (m_w >> 16);
		return ((m_z << 16) + m_w) / 2;
	}

} // end rng
