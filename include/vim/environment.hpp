#ifndef VIM_ENVIRONMENT_HPP
#define VIM_ENVIRONMENT_HPP

#include <string>
#include <string_view>

namespace vim {

template <typename Environment>
auto expand_env(const std::string_view &input, const Environment &) -> std::string;

} // namespace vim

#include <vim/detail/environment.hpp>

#endif // VIM_ENVIRONMENT_HPP
