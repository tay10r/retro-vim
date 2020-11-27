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

template <typename char_type>
class token final
{
	token_kind Kind = token_kind::normal;
	std::basic_string_view<char_type> Text;
public:
	constexpr token() = default;

	constexpr token(token_kind K, const std::basic_string_view<char_type> &T) noexcept : Kind(K), Text(T) {}

	constexpr auto get_text() const noexcept { return Text; }

	constexpr auto operator==(token_kind K) const noexcept { return Kind == K; }
};

template <typename char_type>
class lexer final {
	std::basic_string_view<char_type> Text;
	std::size_t Index = 0;
  public:
	constexpr lexer(const std::basic_string_view<char_type> &T) noexcept : Text(T) {}

	constexpr auto is_done() const noexcept { return Index >= Text.size(); }

	constexpr auto lex() noexcept -> token<char_type>
	{
		auto FirstChar = this->peek(0);
		if (!FirstChar)
			return token<char_type> {};

		if (are_equal(*FirstChar, '$') && is_non_digit(this->peek(1)))
			return lex_var_ref(1);
		else
			return lex_normal(1);
	}
  protected:

	static constexpr auto are_equal(char_type A, char_type B) noexcept { return A == B; }

	static constexpr auto in_range(std::optional<char_type> C, char_type Lo, char_type Hi) noexcept
	{
		if (!C)
			return false;
		else
			return in_range(*C, Lo, Hi);
	}

	static constexpr auto in_range(char_type C, char_type Lo, char_type Hi) noexcept -> bool
	{
		return (C >= Lo) && (C <= Hi);
	}

	static constexpr auto is_digit(std::optional<char_type> C) noexcept -> bool
	{
		return in_range(C, '0', '9');
	}

	static constexpr auto is_non_digit(std::optional<char_type> C) noexcept -> bool
	{
		return (C == '_') || in_range(C, 'a', 'z') || in_range(C, 'A', 'Z');
	}

	constexpr auto out_of_bounds(std::size_t Offset) const noexcept -> bool
	{
		return (Index + Offset) >= Text.size();
	}

	constexpr auto peek(std::size_t Offset) const noexcept -> std::optional<char_type>
	{
		if (out_of_bounds(Offset))
			return std::nullopt;
		else
			return Text[Index + Offset];
	}

	constexpr auto produce(token_kind Kind, std::size_t Length) noexcept -> token<char_type>
	{
		token Token(Kind, std::basic_string_view(Text.data() + Index, Length));
		Index += Length;
		return Token;
	}

	constexpr auto lex_normal(std::size_t StartingOffset) noexcept -> token<char_type>
	{
		auto Length = StartingOffset;

		while (!out_of_bounds(Length))
		{
			auto C = peek(Length);
			if (!C || are_equal(*C, '$'))
				break;
			Length++;
		}

		return produce(token_kind::normal, Length);
	}

	constexpr auto lex_var_ref(std::size_t StartingOffset) noexcept -> token<char_type>
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

template <typename char_type>
auto remove_var_prefix(const std::basic_string_view<char_type> &VarRef) noexcept -> std::basic_string_view<char_type>
{
	auto Copy = VarRef;
	if (Copy.size() < 1)
		return Copy;
	Copy.remove_prefix(1);
	return Copy;
}

} // namespace environment_detail

template <typename char_type, typename environment>
auto expand_env(const std::basic_string_view<char_type> &Input, const environment &Env) -> std::basic_string<char_type>
{
	std::basic_ostringstream<char_type> OutputStream;

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
