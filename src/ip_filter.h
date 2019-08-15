#pragma once

#include <iostream>
#include <string_view>
#include <tuple>

template<typename OutIt>
OutIt split(std::string_view s, char d, OutIt out) {
    using namespace std;

    do {
        auto pos = s.find_first_of(d);
        *out = s.substr(0, pos);
        ++out;
        if (pos == string_view::npos)
            break;
        ++pos;
        s.remove_prefix(pos);
    } while(true);

    return out;
}

using ip_address_t = std::tuple<int, int, int, int>;
inline ip_address_t make_ip(int ip_part1, int ip_part2, int ip_part3, int ip_part4) {
    return std::make_tuple(ip_part1, ip_part2, ip_part3, ip_part4);
}

ip_address_t ip_read(std::string_view ip); 

template <size_t Idx>
bool __ip_match(const ip_address_t& ip, int ip_part) {
    return std::get<Idx>(ip) == ip_part;
}

template <size_t Idx, typename... Int>
bool __ip_match(const ip_address_t& ip, int ip_part, Int... ip_parts) {
    return __ip_match<Idx>(ip, ip_part) && __ip_match<Idx + 1>(ip, ip_parts...);
}

template <typename... Int>
bool ip_match(const ip_address_t& ip, int ip_part1, Int... ip_parts) {
    return __ip_match<0>(ip, ip_part1, ip_parts...);
}

inline bool ip_match_any(const ip_address_t& ip, int ip_part) {
    return __ip_match<0>(ip, ip_part) ||
           __ip_match<1>(ip, ip_part) ||
           __ip_match<2>(ip, ip_part) ||
           __ip_match<3>(ip, ip_part);
}

template <typename... Int>
bool ip_match_any(const ip_address_t& ip, int ip_part, Int... ip_parts) {
    return ip_match_any(ip, ip_part) || ip_match_any(ip, ip_parts...);
}

template <typename InIt, typename OutIt, typename... Int>
OutIt ip_filter(InIt f, InIt l, OutIt out, int ip_part1, Int... ip_parts) {
    for (; f != l; ++f) {
        if (!ip_match(*f, ip_part1, ip_parts...))
            continue;
        *out = *f;
        ++out;
    }
    return out;
}

template <typename InIt, typename OutIt, typename... Int>
OutIt ip_filter_any(InIt f, InIt l, OutIt out, int ip_part, Int... ip_parts) {
    for (; f != l; ++f) {
        if (!ip_match_any(*f, ip_part, ip_parts...))
            continue;
        *out = *f;
        ++out;
    }
    return out;
}

namespace std {

template<typename Ch>
basic_ostream<Ch, char_traits<Ch>>& operator<<(basic_ostream<Ch, char_traits<Ch>>& os, const ip_address_t& t) {
    return os << get<0>(t) << '.'
              << get<1>(t) << '.'
              << get<2>(t) << '.'
              << get<3>(t);
}

}
