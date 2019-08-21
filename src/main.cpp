#include <iostream>
#include <vector>
#include <cctype>
#include <string_view>

#include <range/v3/all.hpp>

#include "ip_address.h"

/// @brief Converts Range of Range of chars to Range of ints
auto atoi_range() {
    using namespace ranges;
    return view::transform([] (auto str) { 
        return stoi(str | to_<std::string>());
    });
}

/// @brief Makes immediately IpAddress from Range of ints
auto make_ip_address() {
    using namespace ranges;
    return view::transform([] (auto parts) {
        if (distance_compare(parts, 4))
            throw std::invalid_argument("make_ip_address: ip contains exactly 4 parts");
        
        int ip[4];
        for_each(parts, [&ip, i=0] (int p) mutable {
            ip[i] = p;
            ++i;
        });
        return griha::IpAddress { std::make_tuple(ip[0], ip[1], ip[2], ip[3]) };
    });
}

int main()
{
    using namespace std;
    using namespace std::placeholders;
    using namespace griha;
    using namespace ranges;

    // read IpAddress objects from standart input
    auto ips = ( getlines(cin)
    // convert lines to Ranges of ints
               | view::transform([] (string_view line) {
                   return line 
                        | view::split('.')
                        | atoi_range();
               })
    // make IpAddress objects from Ranges of ints
               | make_ip_address()
    // convert to vector of IpAddress objects for further sort
               | to_<vector>()
    // sort inplace
               | action::sort(greater<>()));

    ranges::copy(ips,
                 make_ostream_joiner(cout, '\n'));
    endl(cout);
    ranges::copy(ips | view::filter([] (auto& ip) { return ip_match(ip, 1); } ),
                 make_ostream_joiner(cout, '\n'));
    endl(cout);
    ranges::copy(ips | view::filter([] (auto& ip) { return ip_match(ip, 46, 70); } ),
                 make_ostream_joiner(cout, '\n'));
    endl(cout);
    ranges::copy(ips | view::filter([] (auto& ip) { return ip_match_any(ip, 46); } ),
                 make_ostream_joiner(cout, '\n'));
    endl(cout);

    return 0;
}