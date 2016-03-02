#pragma once

#include <stdint.h>

namespace rng {
	void seed(uint32_t m);
	int32_t rand();
}
