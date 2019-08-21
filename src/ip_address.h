#pragma once

#include <iostream>
#include <string_view>
#include <tuple>

#include <range/v3/all.hpp>

namespace griha {

struct IpAddress {
    using type = std::tuple<int, int, int, int>;
    type value;
};

bool operator> (const IpAddress& lhs, const IpAddress& rhs) {
    return lhs.value > rhs.value;
}

/// @name Stream operations
/// @{
template<typename Ch>
std::basic_ostream<Ch, std::char_traits<Ch>>&
operator<<(std::basic_ostream<Ch, std::char_traits<Ch>>& os, const IpAddress& t) {
    using namespace ranges;
    auto osj = make_ostream_joiner(os, '.');
    tuple_for_each(t.value, [&osj] (int v) { *osj = v; });
    return os;
}
/// @}

/// @name IP address matching functions
/// @{

/// @brief Checks if part of IP address completely match. 
template <typename... Int>
bool ip_match(const IpAddress& ip, Int... ip_parts) {
    using namespace ranges;
    static_assert(sizeof...(Int) <= 4, "IP actually has only 4 parts");
    auto ret = tuple_transform(std::array<int, sizeof...(Int)>{ ip_parts... }, ip.value,
                               [] (auto lhs, auto rhs) { return lhs == rhs; });
    return tuple_foldl(ret, true, [] (bool lhs, bool rhs) { return lhs && rhs; });
}

/// @brief Checks if any part of IP matches any value in @c ip_parts
template <typename... Int>
bool ip_match_any(const IpAddress& ip, Int... ip_parts) {
    using namespace ranges;
    std::array<int, sizeof...(Int)> parts = { ip_parts... };
    return any_of(
              ( parts
              | view::transform([&ip] (int part) {
                    return tuple_foldl(ip.value, false,
                                       [part] (bool v, int p) { return v || p == part; });
                })),
                [] (bool v) { return v; });
}

/// @}

} // namespace griha
