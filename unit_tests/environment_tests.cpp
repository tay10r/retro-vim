#include <gtest/gtest.h>

#include <vim/environment.hpp>

namespace {

using namespace std::literals;

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
	auto Result = vim::expand_env("$FOO/$BAR"sv, FakeEnv{});
	EXPECT_EQ(Result, "foo/bar"sv);
}

TEST(environment, expand_env_no_refs)
{
	auto Result = vim::expand_env("$MISSING$FOO"sv, FakeEnv{});
	EXPECT_EQ(Result, "foo"sv);
}

TEST(environment, expand_env_single_dollar_sign)
{
	auto Result = vim::expand_env("$ $FOO"sv, FakeEnv{});
	EXPECT_EQ(Result, "$ foo"sv);
}

TEST(environment, expand_env_wchar_t)
{
	auto FakeEnv = [](const std::wstring_view &Key) -> std::wstring_view {
		if (Key == L"var")
			return L"value";
		else
			return L"";
	};

	auto Result = vim::expand_env(std::wstring_view(L" $var "), FakeEnv);

	EXPECT_EQ(Result, L" value ");
}
