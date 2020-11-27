#include <gtest/gtest.h>

#include <vim/environment.hpp>

namespace {

class FakeEnv final
{
public:
	auto operator () (const std::string_view &Key) const -> std::string {
		if (Key == "FOO")
			return "foo";
		else if (Key == "BAR")
			return "bar";
		else
			return "";
	};
};

} // namespace

TEST(environment, expand_env)
{
	auto result = vim::expand_env("$FOO/$BAR", FakeEnv{});
	EXPECT_EQ(result, "foo/bar");
}

TEST(environment, expand_env_no_refs)
{
	auto result = vim::expand_env("$MISSING$FOO", FakeEnv{});
	EXPECT_EQ(result, "foo");
}

TEST(environment, expand_env_single_dollar_sign)
{
	auto result = vim::expand_env("$ $FOO", FakeEnv{});
	EXPECT_EQ(result, "$ foo");
}
