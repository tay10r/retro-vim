#ifndef VIM_ENVIRONMENT_HPP
#define VIM_ENVIRONMENT_HPP

#include <string>
#include <string_view>

namespace vim {

template <typename Derived>
class Environment
{
public:
	std::string
	get_value(const std::string_view &key) const;
};

class SystemEnv final : public Environment<SystemEnv>
{
public:
	static Environment<SystemEnv> &
	get_singleton() noexcept;

	std::string
	get_value(const std::string_view &key) const;
};

template <typename DerivedEnv>
std::string
expand_env(const std::string_view &input, const Environment<DerivedEnv> &env);

} // namespace vim

#include "environment.inl"

#endif // VIM_ENVIRONMENT_HPP
