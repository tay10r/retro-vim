#ifndef VIM_DETAIL_ENVIRONMENT_HPP
#define VIM_DETAIL_ENVIRONMENT_HPP

#include <optional>
#include <sstream>

#include <cstdlib>

namespace vim {

namespace environment_detail {

enum class token_kind
{
	variable_ref,
	normal
};

class token final
{
	token_kind Kind = token_kind::normal;
	std::string_view Text;
public:
	constexpr token(token_kind K, const std::string_view &T) noexcept : Kind(K), Text(T) {}

	constexpr auto get_text() const noexcept -> std::string_view { return Text; }

	constexpr auto operator==(token_kind K) const noexcept { return Kind == K; }
};

class lexer final {
	std::string_view Text;
	std::size_t Index = 0;
  public:
	constexpr lexer(const std::string_view &T) noexcept : Text(T) {}

	constexpr auto is_done() const noexcept { return Index >= Text.size(); }

	constexpr auto lex() noexcept -> token
	{
		if ((this->peek(0) == '$') && is_non_digit(this->peek(1)))
			return lex_var_ref(1);
		else
			return lex_normal(1);
	}
  protected:

	static constexpr auto in_range(std::optional<char> C, char Lo, char Hi) noexcept
	{
		if (!C)
			return false;
		else
			return in_range(*C, Lo, Hi);
	}

	static constexpr auto in_range(char C, char Lo, char Hi) noexcept -> bool
	{
		return (C >= Lo) && (C <= Hi);
	}

	static constexpr auto is_digit(std::optional<char> C) noexcept -> bool
	{
		return in_range(C, '0', '9');
	}

	static constexpr auto is_non_digit(std::optional<char> C) noexcept -> bool
	{
		return (C == '_') || in_range(C, 'a', 'z') || in_range(C, 'A', 'Z');
	}

	constexpr auto out_of_bounds(std::size_t Offset) const noexcept -> bool
	{
		return (Index + Offset) >= Text.size();
	}

	constexpr auto peek(std::size_t Offset) const noexcept -> std::optional<char>
	{
		if (out_of_bounds(Offset))
			return std::nullopt;
		else
			return Text[Index + Offset];
	}

	constexpr auto produce(token_kind Kind, std::size_t Length) noexcept -> token
	{
		token Token(Kind, std::string_view(Text.data() + Index, Length));
		Index += Length;
		return Token;
	}

	constexpr auto lex_normal(std::size_t StartingOffset) noexcept -> token
	{
		auto Length = StartingOffset;

		while (!out_of_bounds(Length))
		{
			if (peek(Length) == '$')
			    break;
			Length++;
		}

		return produce(token_kind::normal, Length);
	}

	constexpr auto lex_var_ref(std::size_t StartingOffset) noexcept -> token
	{
		std::size_t Length = StartingOffset;

		while (!out_of_bounds(Length))
		{
			auto C = peek(Length);
			if (!is_digit(C) && !is_non_digit(C))
			    break;
			Length++;
		}

		return produce(token_kind::variable_ref, Length);
	}
};

auto remove_var_prefix(const std::string_view &VarRef) noexcept -> std::string_view
{
	auto Copy = VarRef;
	if (Copy.size() < 1)
		return Copy;
	Copy.remove_prefix(1);
	return Copy;
}

} // namespace environment_detail

template <typename environment_type>
auto expand_env(const std::string_view &Input, const environment_type &Env) -> std::string
{
	std::ostringstream OutputStream;

	environment_detail::lexer Lexer(Input);

	while (!Lexer.is_done())
	{
		auto Token = Lexer.lex();
		if (Token == environment_detail::token_kind::variable_ref)
		{
			auto Key = environment_detail::remove_var_prefix(Token.get_text());
			auto Value = Env(Key);
			OutputStream << Value;
		}
		else
		{
			OutputStream << Token.get_text();
		}
	}

	return OutputStream.str();
}

} // namespace vim

#endif // VIM_DETAIL_ENVIRONMENT_HPP
