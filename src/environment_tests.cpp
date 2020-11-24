#include <gtest/gtest.h>

#include "environment.hpp"

namespace {

class FakeEnv final : public vim::Environment<FakeEnv>
{
public:
	std::string
	get_value(const std::string_view &key) const;
};

std::string
FakeEnv::get_value(const std::string_view &key) const
{
	if (key == "FOO")
		return "foo";
	else if (key == "BAR")
		return "bar";
	return "";
}

} // namespace

TEST(environment, expand_env)
{
	FakeEnv env;
	auto result = vim::expand_env("$FOO/$BAR", env);
	EXPECT_EQ(result, "foo/bar");
}

TEST(environment, expand_env_no_refs)
{
	FakeEnv env;
	auto result = vim::expand_env("$MISSING$FOO", env);
	EXPECT_EQ(result, "foo");
}

TEST(environment, expand_env_single_dollar_sign)
{
	FakeEnv env;
	auto result = vim::expand_env("$ $FOO", env);
	EXPECT_EQ(result, "$ foo");
}
