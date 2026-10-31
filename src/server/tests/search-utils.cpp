#if __has_include(<catch2/catch_test_macros.hpp>)
#include <catch2/catch_test_macros.hpp>
#else
#include <iostream>
#include <cassert>
#include <vector>
#include <functional>

static std::vector<std::pair<const char *, std::function<void()>>> g_tests;

#define CONCAT_INNER(a, b) a##b
#define CONCAT(a, b) CONCAT_INNER(a, b)

#define TEST_CASE(name, ...)                                                                                 \
  static void CONCAT(test_func_, __LINE__)();                                                                \
  static struct CONCAT(TestAutoReg_, __LINE__) {                                                             \
    CONCAT(TestAutoReg_, __LINE__)() { g_tests.push_back({name, CONCAT(test_func_, __LINE__)}); }            \
  } CONCAT(g_autoreg_, __LINE__);                                                                            \
  static void CONCAT(test_func_, __LINE__)()

#define REQUIRE(expr)                                                                                        \
  do {                                                                                                       \
    if (!(expr)) {                                                                                           \
      std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__ << ": " << #expr << "\n";           \
      std::abort();                                                                                          \
    }                                                                                                        \
  } while (0)

#define CHECK(expr) REQUIRE(expr)
#endif

#include "utils/search-utils.hpp"

TEST_CASE("search_utils: basic arithmetic operations") {
  CHECK(search_utils::hasMathIntent("12 * 4"));
  CHECK(search_utils::hasMathIntent("100 / 5"));
  CHECK(search_utils::hasMathIntent("2.5 + 3.1"));
  CHECK(search_utils::hasMathIntent("100 - 50"));
  CHECK(search_utils::hasMathIntent("2^8"));
  CHECK(search_utils::hasMathIntent("(10 + 20) * 3"));
  CHECK(search_utils::hasMathIntent("=42"));
}

TEST_CASE("search_utils: percentages and mathematical functions") {
  CHECK(search_utils::hasMathIntent("50% of 200"));
  CHECK(search_utils::hasMathIntent("15% * 80"));
  CHECK(search_utils::hasMathIntent("sqrt(144)"));
  CHECK(search_utils::hasMathIntent("sin(3.14)"));
  CHECK(search_utils::hasMathIntent("ceil(4.2)"));
}

TEST_CASE("search_utils: unit and currency conversions") {
  CHECK(search_utils::hasMathIntent("100 usd to eur"));
  CHECK(search_utils::hasMathIntent("50 km in miles"));
  CHECK(search_utils::hasMathIntent("$50 to eur"));
  CHECK(search_utils::hasMathIntent("€100 in usd"));
  CHECK(search_utils::hasMathIntent("30c in f"));
}

TEST_CASE("search_utils: non-math queries must not trigger intent") {
  CHECK(!search_utils::hasMathIntent("firefox"));
  CHECK(!search_utils::hasMathIntent("google-chrome"));
  CHECK(!search_utils::hasMathIntent("vlc 3.0"));
  CHECK(!search_utils::hasMathIntent("blender 4.1"));
  CHECK(!search_utils::hasMathIntent("back to the future"));
  CHECK(!search_utils::hasMathIntent("open terminal"));
  CHECK(!search_utils::hasMathIntent("x"));
  CHECK(!search_utils::hasMathIntent(""));
}

#if !__has_include(<catch2/catch_test_macros.hpp>)
int main(int argc, char **argv) {
  int passed = 0;
  for (const auto &[name, func] : g_tests) {
    std::cout << "[RUN ] " << name << "\n";
    func();
    std::cout << "[PASS] " << name << "\n";
    ++passed;
  }
  std::cout << "All " << passed << " search_utils unit tests passed successfully!\n";
  return 0;
}
#endif
