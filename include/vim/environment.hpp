#ifndef VIM_ENVIRONMENT_HPP
#define VIM_ENVIRONMENT_HPP

#include <string>
#include <string_view>

namespace vim {

template <typename char_type, typename environment>
auto expand_env(const std::basic_string_view<char_type> &, const environment &) -> std::basic_string<char_type>;

} // namespace vim

#include <vim/detail/environment.hpp>

#endif // VIM_ENVIRONMENT_HPP
