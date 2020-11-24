#include "environment.hpp"

#include <algorithm>

#include <cstring>

/*
 * Expand environment variable with path name.
 * If anything fails no expansion is done and dst equals src.
 */
extern "C" void expand_env(const char *src, char *dst, size_t dstlen) {

	if (dstlen < 1)
		return;

	auto result = vim::expand_env(src, vim::SystemEnv::get_singleton());

	auto min_size = std::min(result.size(), dstlen - 1);

	std::memcpy(dst, result.c_str(), min_size);

	dst[min_size] = 0;
}
