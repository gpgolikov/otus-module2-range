#include <iostream>
#include <deque>
#include <vector>
#include <iterator>
#include <algorithm>
#include <string_view>

#include <range/v3/all.hpp>

#include "ip_filter.h"

int main()
{
    using namespace std;

    try
    {
        vector<ip_address_t> ips;

        // read ips
        for(std::string line; std::getline(std::cin, line);)
            ips.push_back(ip_read(line));
        
        // reverse sort of ips
        ranges::sort(ips, greater<ip_address_t>());
        ranges::for_each(ips, [] (const ip_address_t& ip) {
            cout << ip << std::endl;
        });
        // filter by first byte (equals 1)
        ip_filter(begin(ips), end(ips), ostream_iterator<ip_address_t>(cout, "\n"), 1);
        // filter by first and second bytes (equal 46 anf 70)
        ip_filter(begin(ips), end(ips), ostream_iterator<ip_address_t>(cout, "\n"), 46, 70);
        // filter by any byte (46)
        ip_filter_any(begin(ips), end(ips), ostream_iterator<ip_address_t>(cout, "\n"), 46);
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}