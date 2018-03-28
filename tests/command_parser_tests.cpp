#include "catch.hpp"

#include <stdexcept>
#include <functional>
#include <cstdint>

#include "command_parser.hpp"

TEST_CASE("Command Parser")
{
    CommandParser parser;

    SECTION("Cannot have an empty command name")
    {
        std::function<bool()> action = [](){return true;};
        REQUIRE_THROWS_AS(
                parser.add_command<>("", action),
                std::invalid_argument);
    }

    SECTION("Parses 1 command without arguments correctly")
    {
        bool result = false;
        std::function<bool()> action = [&result](){return result = true;};
        parser.add_command<>("test", action);
        parser.parse("test");
        REQUIRE(result);
    }

    SECTION("Parse 1 string correctly")
    {
        bool result = false;
        std::function<bool(std::string)> action = [&result](std::string str)
            { return result = (str == "valid"); };
        parser.add_command<std::string>("test", action);
        parser.parse("test valid");
        REQUIRE(result);
    }

    SECTION("Parse 1 int32 correctly")
    {
        bool result = false;
        std::function<bool(int32_t)> action = [&result](int32_t value)
            { return result = (value == 42); };
        parser.add_command<int32_t>("test", action);
        parser.parse("test 42");
        REQUIRE(result);
    }

    SECTION("Parses 2 arguments")
    {
        bool result = false;
        std::function<bool(std::string, int32_t)> action =
            [&result](std::string str, int32_t value)
            { return result = ((value == 31)
                    && (str == "valid2")); };
        parser.add_command<std::string, int32_t>("test", action);
        parser.parse("test valid2 31");
        REQUIRE(result);
    }

    SECTION("Fails with too many arguments")
    {
        std::function<bool(int32_t)> action =
            [](int32_t){return true;};
        parser.add_command("test", action);
        REQUIRE_THROWS_AS(parser.parse("test 31 too many"), parse_error);
    }

    SECTION("Fails with too little arguments")
    {
        std::function<bool(int32_t, std::string)> action =
            [](int32_t, std::string){return true;};
        parser.add_command("test", action);
        REQUIRE_THROWS_AS(parser.parse("test 31"), parse_error);
    }

    SECTION("Does not fail on empty lines")
    {
        std::function<bool(void)> action = [](){return true;};
        parser.add_command("test", action);
        REQUIRE_NOTHROW(parser.parse("test\n\ntest"));
    }
}
