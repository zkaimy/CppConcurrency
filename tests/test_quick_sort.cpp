#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <list>
#include <algorithm>
#include <random>
#include "sequential_quick_sort.h"

static std::list<int> createRandomList(size_t size)
{
    std::list<int> result;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, static_cast<int>(size * 2));
    for (size_t i = 0; i < size; ++i)
    {
        result.emplace_back(dist(gen));
    }
    return result;
}

static bool isSorted(const std::list<int>& lst)
{
    if (lst.size() <= 1)
        return true;
    auto it = lst.begin();
    auto prev = *it;
    ++it;
    for (; it != lst.end(); ++it)
    {
        if (*it < prev)
            return false;
        prev = *it;
    }
    return true;
}

TEST_CASE("sequential_quick_sort - empty list", "[quick_sort]")
{
    std::list<int> empty;
    auto result = sequential_quick_sort(empty);
    REQUIRE(result.empty());
}

TEST_CASE("sequential_quick_sort - single element", "[quick_sort]")
{
    std::list<int> single = {42};
    auto result = sequential_quick_sort(single);
    REQUIRE(result.size() == 1);
    REQUIRE(result.front() == 42);
}

TEST_CASE("sequential_quick_sort - already sorted", "[quick_sort]")
{
    std::list<int> sorted = {1, 2, 3, 4, 5};
    auto result = sequential_quick_sort(sorted);
    REQUIRE(isSorted(result));
    REQUIRE(result.size() == 5);
}

TEST_CASE("sequential_quick_sort - reverse sorted", "[quick_sort]")
{
    std::list<int> reverse = {5, 4, 3, 2, 1};
    auto result = sequential_quick_sort(reverse);
    REQUIRE(isSorted(result));
    REQUIRE(result.size() == 5);
}

TEST_CASE("sequential_quick_sort - duplicates", "[quick_sort]")
{
    std::list<int> dupes = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    auto result = sequential_quick_sort(dupes);
    REQUIRE(isSorted(result));
    REQUIRE(result.size() == 10);
}

TEST_CASE("sequential_quick_sort - random list", "[quick_sort]")
{
    auto randomList = createRandomList(100);
    auto original = randomList;
    auto result = sequential_quick_sort(randomList);
    REQUIRE(isSorted(result));
    REQUIRE(result.size() == original.size());
}

TEST_CASE("sequential_quick_sort_inplace - empty", "[quick_sort]")
{
    std::list<int> empty;
    sequential_quick_sort_inplace(empty);
    REQUIRE(empty.empty());
}

TEST_CASE("sequential_quick_sort_inplace - single", "[quick_sort]")
{
    std::list<int> single = {42};
    sequential_quick_sort_inplace(single);
    REQUIRE(single.size() == 1);
    REQUIRE(single.front() == 42);
}

TEST_CASE("sequential_quick_sort_inplace - random", "[quick_sort]")
{
    auto randomList = createRandomList(100);
    sequential_quick_sort_inplace(randomList);
    REQUIRE(isSorted(randomList));
}

TEST_CASE("sequential_quick_sort_inplace_V2 - empty", "[quick_sort]")
{
    std::list<int> empty;
    sequential_quick_sort_inplace_V2(empty);
    REQUIRE(empty.empty());
}

TEST_CASE("sequential_quick_sort_inplace_V2 - random", "[quick_sort]")
{
    auto randomList = createRandomList(100);
    sequential_quick_sort_inplace_V2(randomList);
    REQUIRE(isSorted(randomList));
}

TEST_CASE("sequential_quick_sort - all same elements", "[quick_sort]")
{
    std::list<int> same(50, 7);
    auto result = sequential_quick_sort(same);
    REQUIRE(isSorted(result));
    REQUIRE(result.size() == 50);
    for (const auto& val : result)
    {
        REQUIRE(val == 7);
    }
}

TEST_CASE("sequential_quick_sort - negative numbers", "[quick_sort]")
{
    std::list<int> negatives = {-5, -1, -3, -4, -2};
    auto result = sequential_quick_sort(negatives);
    REQUIRE(isSorted(result));
    REQUIRE(result.front() == -5);
    REQUIRE(result.back() == -1);
}
