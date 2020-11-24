#include "environment.hpp"

#include <optional>

#include <cstdlib>

namespace vim {

namespace environment_detail {

enum class TokenKind
{
	variable_ref,
	normal
};

class Token final
{
	TokenKind kind = TokenKind::normal;
	std::string_view text;
public:
	constexpr Token(TokenKind kind_, const std::string_view &text_) noexcept
	  : kind(kind_), text(text_) {}

	constexpr std::string_view get_text() const noexcept { return text; }

	constexpr bool operator == (TokenKind other_k) const noexcept { return kind == other_k; }
};

class Lexer final {
	std::string_view text;
	std::size_t pos = 0;
  public:
	constexpr Lexer(const std::string_view &t) noexcept : text(t) {}

	constexpr bool is_done() const noexcept { return pos >= text.size(); }

	constexpr Token
	lex() noexcept
	{
		if (this->peek(0) == '$')
			return lex_var_ref(1);
		else
			return lex_normal(1);
	}
  protected:

	static constexpr bool
	in_range(std::optional<char> c, char lo, char hi) noexcept
	{
		if (!c)
			return false;
		else
			return in_range(*c, lo, hi);
	}

	static constexpr bool
	in_range(char c, char lo, char hi) noexcept
	{
		return (c >= lo) && (c <= hi);
	}

	static constexpr bool
	is_digit(std::optional<char> c) noexcept
	{
		return in_range(c, '0', '9');
	}

	static constexpr bool
	is_non_digit(std::optional<char> c) noexcept
	{
		return (c == '_') || in_range(c, 'a', 'z') || in_range(c, 'A', 'Z');
	}

	constexpr bool
	out_of_bounds(std::size_t offset) const noexcept
	{
		return (pos + offset) >= text.size();
	}

	constexpr std::optional<char>
	peek(std::size_t offset) const noexcept
	{
		if (out_of_bounds(offset))
			return std::nullopt;
		else
			return text[pos + offset];
	}

	constexpr Token
	produce(TokenKind kind, std::size_t length) noexcept
	{
		Token token(kind, std::string_view(text.data() + pos, length));
		pos += length;
		return token;
	}

	constexpr Token
	lex_normal(std::size_t starting_offset) noexcept
	{
		std::size_t length = starting_offset;

		while (!out_of_bounds(length))
		{
			if (peek(length) == '$')
			    break;
			length++;
		}

		return produce(TokenKind::normal, length);
	}

	constexpr Token
	lex_var_ref(std::size_t starting_offset) noexcept
	{
		std::size_t length = starting_offset;

		while (!out_of_bounds(length))
		{
			auto c = peek(length);
			if (!is_digit(c) && !is_non_digit(c))
			    break;
			length++;
		}

		return produce(TokenKind::variable_ref, length);
	}
};

constexpr auto
remove_var_prefix(const std::string_view &var_ref) noexcept
{
	auto copy = var_ref;
	if (copy.size() > 0)
	{
		copy.remove_prefix(1);
		return copy;
	}
	return copy;
}

} // namespace environment_detail

template <typename DerivedEnv>
std::string
Environment<DerivedEnv>::get_value(const std::string_view &name) const
{
	return static_cast<const DerivedEnv*>(this)->get_value(name);
}

std::string
SystemEnv::get_value(const std::string_view &key) const
{
	std::string key_copy(key);
	return std::getenv(key_copy.c_str());
}

Environment<SystemEnv> &
SystemEnv::get_singleton() noexcept
{
	static SystemEnv env;
	return env;
}

template <typename DerivedEnv>
std::string
expand_env(const std::string_view &input, const Environment<DerivedEnv> &env)
{
	std::string result;

	environment_detail::Lexer lxr(input);

	while (!lxr.is_done())
	{
		auto tok = lxr.lex();
		if (tok == environment_detail::TokenKind::variable_ref)
		{
			auto key = environment_detail::remove_var_prefix(tok.get_text());
			auto val = env.get_value(key);
			for (auto c : val)
				result += c;
		}
		else
		{
			result += tok.get_text();
		}
	}

	return result;
}

} // namespace vim
