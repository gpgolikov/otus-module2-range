#include <catch2/catch.hpp>

#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <array>

#include "ip_filter.h"

using namespace std;
using namespace Catch::Matchers;

template<typename T>
class equals : public Catch::MatcherBase<T> {
    T value_;

public:
    explicit equals(T value) : value_(move(value)) {}

    virtual bool match(const T& to_match) const override {
        return value_ == to_match;
    }

    virtual std::string describe() const override {
        ostringstream os;
        os << "equals \"" << value_ << '"';
        return os.str();
    }
};

template <typename T>
inline auto Equals(T value) {
    return equals<T>(move(value));
}

inline auto Equals(const char* str) {
    return equals<string_view>(str);
}

TEST_CASE("split") {

    SECTION("normal") {
        string s = "79.180.73.190\t2\t1";
        vector<string_view> splitted_s;

        split(s, '\t', back_inserter(splitted_s));
        REQUIRE_THAT(splitted_s.size(), Equals(3));
        REQUIRE_THAT(splitted_s[0], Equals("79.180.73.190"));
        REQUIRE_THAT(splitted_s[1], Equals("2"));
        REQUIRE_THAT(splitted_s[2], Equals("1"));

        vector<string_view> splitted_ip;
        split(splitted_s[0], '.', back_inserter(splitted_ip));
        REQUIRE_THAT(splitted_ip.size(), Equals(4));
        REQUIRE_THAT(splitted_ip[0], Equals("79"));
        REQUIRE_THAT(splitted_ip[1], Equals("180"));
        REQUIRE_THAT(splitted_ip[2], Equals("73"));
        REQUIRE_THAT(splitted_ip[3], Equals("190"));
    }

    SECTION("bad cases") {
        {
            INFO("empty stirng");
            string s = "";
            vector<string_view> splitted_s;

            split(s, '\t', back_inserter(splitted_s));
            REQUIRE_THAT(splitted_s.size(), Equals(1));
            REQUIRE_THAT(splitted_s[0], Equals(""));
        }

        {
            INFO("only delimiter present");
            string s = "\t";
            vector<string_view> splitted_s;

            split(s, '\t', back_inserter(splitted_s));
            REQUIRE_THAT(splitted_s.size(), Equals(2));
            REQUIRE_THAT(splitted_s[0], Equals(""));
            REQUIRE_THAT(splitted_s[1], Equals(""));
        }

        {
            INFO("more than one delimiter present");
            string s = "...";
            vector<string_view> splitted_s;

            split(s, '.', back_inserter(splitted_s));
            REQUIRE_THAT(splitted_s.size(), Equals(4));
            REQUIRE_THAT(splitted_s[0], Equals(""));
            REQUIRE_THAT(splitted_s[1], Equals(""));
            REQUIRE_THAT(splitted_s[2], Equals(""));
            REQUIRE_THAT(splitted_s[3], Equals(""));
        }

        {
            INFO("delimiter doubled");
            string s = "1..2.3";
            vector<string_view> splitted_s;

            split(s, '.', back_inserter(splitted_s));
            REQUIRE_THAT(splitted_s.size(), Equals(4));
            REQUIRE_THAT(splitted_s[0], Equals("1"));
            REQUIRE_THAT(splitted_s[1], Equals(""));
            REQUIRE_THAT(splitted_s[2], Equals("2"));
            REQUIRE_THAT(splitted_s[3], Equals("3"));
        }
    }
}

TEST_CASE("ip_read") {

    SECTION("normal") {
        {
            INFO("ordinary ip address")
            string ip_string = "79.180.73.190";

            auto ip = ip_read(ip_string);
            REQUIRE_THAT(make_ip(79, 180, 73, 190), Equals(ip));
        }

        {
            INFO("broadcast ip")
            string ip_string = "255.255.255.0";

            auto ip = ip_read(ip_string);
            REQUIRE_THAT(make_ip(255, 255, 255, 0), Equals(ip));
        }
    }

    SECTION("bad cases") {
        {
            INFO("extra parts");
            string ip_string = "1.1.1.1.1";

            decay_t<decltype(ip_read(""))> ip;
            REQUIRE_NOTHROW(ip = ip_read(ip_string));
            REQUIRE_THAT(make_ip(1, 1, 1, 1), Equals(ip));
        }

        {
            INFO("missed part of ip address");
            string ip_string = "1..1.1";

            REQUIRE_THROWS(ip_read(ip_string));
        }

        {
            INFO("empty string");
            string ip_string = "";

            REQUIRE_THROWS(ip_read(ip_string));
        }
    }
}

TEST_CASE("ip_match") {

    SECTION("normal") {
        {
            INFO("by first component");
            auto ip = make_ip(1, 2, 3, 4);

            REQUIRE(ip_match(ip, 1));
            REQUIRE_FALSE(ip_match(ip, 2));
        }

        {
            INFO("by first and second components");
            auto ip = make_ip(1, 2, 3, 4);

            REQUIRE(ip_match(ip, 1, 2));
            REQUIRE_FALSE(ip_match(ip, 2, 2));
            REQUIRE_FALSE(ip_match(ip, 1, 3));
        }
    }
}

TEST_CASE("ip_match_any") {

    SECTION("normal") {
        {
            INFO("by one value");
            auto ip = make_ip(1, 2, 3, 4);

            REQUIRE(ip_match_any(ip, 1));
            REQUIRE(ip_match_any(ip, 2));
            REQUIRE_FALSE(ip_match_any(ip, 5));
        }

        {
            INFO("by two values");
            auto ip = make_ip(1, 2, 3, 4);

            REQUIRE(ip_match_any(ip, 1, 2));
            REQUIRE(ip_match_any(ip, 2, 5));
            REQUIRE(ip_match_any(ip, 5, 2));
            REQUIRE_FALSE(ip_match(ip, 5, 6));
        }
    }
}

TEST_CASE("ip_filter") {

    SECTION("normal") {
        {
            INFO("by first component");
            array<ip_address_t, 5> ips = {
                make_ip(1, 2, 3, 4),
                make_ip(2, 2, 3, 4),
                make_ip(1, 3, 3, 4),
                make_ip(3, 2, 3, 4),
                make_ip(1, 2, 3, 4),
            };

            vector<ip_address_t> ip_filtered;
            ip_filter(begin(ips), end(ips), back_inserter(ip_filtered), 1);
            REQUIRE_THAT(ip_filtered.size(), Equals(3));
            REQUIRE_THAT(ip_filtered[0], Equals(ips[0]));
            REQUIRE_THAT(ip_filtered[1], Equals(ips[2]));
            REQUIRE_THAT(ip_filtered[2], Equals(ips[4]));

            ip_filtered.clear();
            ip_filter(begin(ips), end(ips), back_inserter(ip_filtered), 2);
            REQUIRE_THAT(ip_filtered.size(), Equals(1));
            REQUIRE_THAT(ip_filtered[0], Equals(ips[1]));
        }

        {
            INFO("by first and second components");
            array<ip_address_t, 5> ips = {
                make_ip(1, 2, 3, 4),
                make_ip(2, 2, 3, 4),
                make_ip(1, 3, 3, 4),
                make_ip(3, 2, 3, 4),
                make_ip(1, 2, 3, 4),
            };

            vector<ip_address_t> ip_filtered;
            ip_filter(begin(ips), end(ips), back_inserter(ip_filtered), 1, 2);
            REQUIRE_THAT(ip_filtered.size(), Equals(2));
            REQUIRE_THAT(ip_filtered[0], Equals(ips[0]));
            REQUIRE_THAT(ip_filtered[1], Equals(ips[4]));

            ip_filtered.clear();
            ip_filter(begin(ips), end(ips), back_inserter(ip_filtered), 3, 2);
            REQUIRE_THAT(ip_filtered.size(), Equals(1));
            REQUIRE_THAT(ip_filtered[0], Equals(ips[3]));
        }
    }
}

TEST_CASE("ip_filter_any") {

    SECTION("normal") {
        {
            INFO("by one value");
            array<ip_address_t, 5> ips = {
                make_ip(1, 2, 3, 4),
                make_ip(2, 2, 6, 4),
                make_ip(3, 3, 1, 4),
                make_ip(3, 2, 0, 4),
                make_ip(1, 2, 0, 4),
            };

            vector<ip_address_t> ip_filtered;
            ip_filter_any(begin(ips), end(ips), back_inserter(ip_filtered), 1);
            REQUIRE_THAT(ip_filtered.size(), Equals(3));
            REQUIRE_THAT(ip_filtered[0], Equals(ips[0]));
            REQUIRE_THAT(ip_filtered[1], Equals(ips[2]));
            REQUIRE_THAT(ip_filtered[2], Equals(ips[4]));

            ip_filtered.clear();
            ip_filter_any(begin(ips), end(ips), back_inserter(ip_filtered), 3);
            REQUIRE_THAT(ip_filtered.size(), Equals(3));
            REQUIRE_THAT(ip_filtered[0], Equals(ips[0]));
            REQUIRE_THAT(ip_filtered[1], Equals(ips[2]));
            REQUIRE_THAT(ip_filtered[2], Equals(ips[3]));

            ip_filtered.clear();
            ip_filter_any(begin(ips), end(ips), back_inserter(ip_filtered), 6);
            REQUIRE_THAT(ip_filtered.size(), Equals(1));
            REQUIRE_THAT(ip_filtered[0], Equals(ips[1]));

            ip_filtered.clear();
            ip_filter_any(begin(ips), end(ips), back_inserter(ip_filtered), 5);
            REQUIRE(ip_filtered.empty());
        }

        {
            INFO("by two values");
            array<ip_address_t, 5> ips = {
                make_ip(1, 2, 3, 4),
                make_ip(2, 2, 6, 4),
                make_ip(3, 3, 1, 4),
                make_ip(3, 2, 0, 4),
                make_ip(1, 2, 0, 4),
            };

            vector<ip_address_t> ip_filtered;
            ip_filter_any(begin(ips), end(ips), back_inserter(ip_filtered), 1, 2);
            REQUIRE_THAT(ip_filtered.size(), Equals(5));
            REQUIRE_THAT(ip_filtered[0], Equals(ips[0]));
            REQUIRE_THAT(ip_filtered[1], Equals(ips[1]));
            REQUIRE_THAT(ip_filtered[2], Equals(ips[2]));
            REQUIRE_THAT(ip_filtered[3], Equals(ips[3]));
            REQUIRE_THAT(ip_filtered[4], Equals(ips[4]));

            ip_filtered.clear();
            ip_filter_any(begin(ips), end(ips), back_inserter(ip_filtered), 5, 6);
            REQUIRE_THAT(ip_filtered.size(), Equals(1));
            REQUIRE_THAT(ip_filtered[0], Equals(ips[1]));

            ip_filtered.clear();
            ip_filter_any(begin(ips), end(ips), back_inserter(ip_filtered), 5, 7);
            REQUIRE(ip_filtered.empty());
        }
    }
}