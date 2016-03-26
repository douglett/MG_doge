/**
 * Deterministic Random Number Generator
 * author: doug
 * date: march 2016
 *
 * Uses George Marsaglia's MWC generator (multiply with carry)
 * (thanks to John D. Cook for tutorial)
*/
#pragma once

#include <stdint.h>

namespace rng {
	void seed(uint32_t m);
	int32_t rand();
}
