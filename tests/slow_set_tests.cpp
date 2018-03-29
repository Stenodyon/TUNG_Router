#include "catch.hpp"

#if 0

#include "slow_set.hpp"

TEST_CASE("Slow Set class")
{
    set<int> test_set;

    SECTION("Default empty")
    {
        REQUIRE(test_set.empty());
        REQUIRE(test_set.size() == 0);
    }
    SECTION("Adding 1 element")
    {
        test_set.add(1);
        REQUIRE_FALSE(test_set.empty());
        REQUIRE(test_set.size() == 1);
    }
    SECTION("Adding 2 different elements")
    {
        test_set.add(1);
        test_set.add(42);
        REQUIRE_FALSE(test_set.empty());
        REQUIRE(test_set.size() == 2);
    }
    SECTION("Adding the same element twice")
    {
        test_set.add(31);
        test_set.add(31);
        REQUIRE_FALSE(test_set.empty());
        REQUIRE(test_set.size() == 1);
    }
    SECTION("Removing the only element")
    {
        test_set.add(25);
        CHECK(test_set.remove(25));
        REQUIRE(test_set.empty());
        REQUIRE(test_set.size() == 0);
    }
    SECTION("Removing 1 of 2 different element")
    {
        test_set.add(43);
        test_set.add(99);
        CHECK(test_set.remove(43));
        REQUIRE(test_set.size() == 1);
    }
    SECTION("Removing 1 element after adding it twice")
    {
        test_set.add(45);
        test_set.add(45);
        CHECK(test_set.remove(45));
        REQUIRE(test_set.size() == 0);
    }
    SECTION("Attempting to remove a element from an empty set")
    {
        REQUIRE_FALSE(test_set.remove(33));
    }
    SECTION("Attempting to remove an element not in the set")
    {
        test_set.add(65);
        test_set.add(34);
        REQUIRE_FALSE(test_set.remove(22));
    }
    SECTION("Set contains added elements")
    {
        test_set.add(43);
        test_set.add(138);
        REQUIRE(test_set.contains(43));
        REQUIRE(test_set.contains(138));
    }
    SECTION("Set does not contain elements")
    {
        test_set.add(43);
        test_set.add(138);
        REQUIRE_FALSE(test_set.contains(19));
        REQUIRE_FALSE(test_set.contains(97897));
    }
    SECTION("Set contains included set")
    {
        test_set.add(158);
        test_set.add(249);
        test_set.add(9707);
        set<int> other;
        other.add(249);
        other.add(158);
        REQUIRE(test_set.contains(other));
    }
    SECTION("Set does not contain a completely different set (null intersection)")
    {
        test_set.add(158);
        test_set.add(249);
        test_set.add(9707);
        set<int> other;
        other.add(297);
        other.add(1128);
        REQUIRE_FALSE(test_set.contains(other));
    }
    SECTION("Set does not contain an overlapping but different set")
    {
        test_set.add(158);
        test_set.add(249);
        test_set.add(9707);
        set<int> other;
        other.add(249);
        other.add(1128);
        REQUIRE_FALSE(test_set.contains(other));
    }
    SECTION("Set equality")
    {
        test_set.add(158);
        test_set.add(249);
        test_set.add(9707);
        set<int> other;
        other.add(158);
        other.add(249);
        other.add(9707);
        REQUIRE(test_set == other);
    }
    SECTION("Union of same set")
    {
        test_set.add(158);
        test_set.add(249);
        test_set.add(9707);
        set<int> other;
        other.add(158);
        other.add(249);
        other.add(9707);
        set<int> _union = test_set.set_union(other);
        REQUIRE(_union == test_set);
        REQUIRE(_union == other);
    }
    SECTION("Union of overlapping sets")
    {
        test_set.add(158);
        test_set.add(249);
        test_set.add(9707);
        set<int> other;
        other.add(158);
        other.add(213);
        other.add(9717);
        set<int> _union = test_set.set_union(other);

        set<int> result;
        result.add(158);
        result.add(249);
        result.add(9707);
        result.add(213);
        result.add(9717);
        REQUIRE(_union == result);
    }
    SECTION("Union of different sets")
    {
        test_set.add(158);
        test_set.add(249);
        test_set.add(9707);
        set<int> other;
        other.add(191);
        other.add(213);
        other.add(9717);
        set<int> _union = test_set.set_union(other);

        set<int> result;
        result.add(158);
        result.add(191);
        result.add(249);
        result.add(9707);
        result.add(213);
        result.add(9717);
        REQUIRE(_union == result);
    }
}

#endif
