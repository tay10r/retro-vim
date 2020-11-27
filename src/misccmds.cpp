#include <vim/environment.hpp>

#include <algorithm>

#include <cstring>

/*
 * Expand environment variable with path name.
 * If anything fails no expansion is done and dst equals src.
 */
extern "C" void expand_env(const char *src, char *dst, size_t dstlen) {

	if (dstlen < 1)
		return;

	auto SystemEnv = [](const std::string_view &Key) -> std::string {
		std::string KeyCopy(Key);
		return std::getenv(KeyCopy.c_str());
	};

	auto result = vim::expand_env(src, SystemEnv);

	auto min_size = std::min(result.size(), dstlen - 1);

	std::memcpy(dst, result.c_str(), min_size);

	dst[min_size] = 0;
}
