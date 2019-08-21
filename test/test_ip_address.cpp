#include <catch2/catch.hpp>

#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <array>

#include "ip_address.h"

using namespace std;
using namespace griha;
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

TEST_CASE("ip_match") {

    SECTION("normal") {
        {
            INFO("by first component");
            auto ip = IpAddress { make_tuple(1, 2, 3, 4) };

            REQUIRE(ip_match(ip, 1));
            REQUIRE_FALSE(ip_match(ip, 2));
        }

        {
            INFO("by first and second components");
            auto ip = IpAddress { make_tuple(1, 2, 3, 4) };

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
            auto ip = IpAddress { make_tuple(1, 2, 3, 4) };

            REQUIRE(ip_match_any(ip, 1));
            REQUIRE(ip_match_any(ip, 2));
            REQUIRE_FALSE(ip_match_any(ip, 5));
        }

        {
            INFO("by two values");
            auto ip = IpAddress { make_tuple(1, 2, 3, 4) };

            REQUIRE(ip_match_any(ip, 1, 2));
            REQUIRE(ip_match_any(ip, 2, 5));
            REQUIRE(ip_match_any(ip, 5, 2));
            REQUIRE_FALSE(ip_match_any(ip, 5, 6));
        }
    }
}
