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

#include "utils/text-transforms.hpp"

TEST_CASE("text_transforms: case conversions") {
  REQUIRE(text_transforms::toUppercase("hello world") == "HELLO WORLD");
  REQUIRE(text_transforms::toLowercase("HELLO WORLD") == "hello world");
  REQUIRE(text_transforms::toTitleCase("the quick brown fox") == "The Quick Brown Fox");
}

TEST_CASE("text_transforms: slug and trim") {
  REQUIRE(text_transforms::toSlug("Hello, World! 123") == "hello-world-123");
  REQUIRE(text_transforms::toSlug("  some -- spaced text  ") == "some-spaced-text");
  REQUIRE(text_transforms::trim("   padded string \t\n") == "padded string");
}

TEST_CASE("text_transforms: color parsing and formatting") {
  auto hexColor = text_transforms::parseColor("#3b82f6");
  REQUIRE(hexColor.has_value());
  REQUIRE(text_transforms::formatHex(*hexColor) == "#3b82f6");
  REQUIRE(text_transforms::formatRgb(*hexColor) == "rgb(59, 130, 246)");
  REQUIRE(text_transforms::formatHsl(*hexColor) == "hsl(217, 91%, 60%)");

  auto rgbColor = text_transforms::parseColor("rgb(255, 0, 128)");
  REQUIRE(rgbColor.has_value());
  REQUIRE(text_transforms::formatHex(*rgbColor) == "#ff0080");

  auto hslColor = text_transforms::parseColor("hsl(120, 100%, 50%)");
  REQUIRE(hslColor.has_value());
  REQUIRE(text_transforms::formatHex(*hslColor) == "#00ff00");

  auto invalid = text_transforms::parseColor("not-a-color-string");
  REQUIRE(!invalid.has_value());
}

#include <QGuiApplication>

#if !__has_include(<catch2/catch_test_macros.hpp>)
int main(int argc, char **argv) {
  qputenv("QT_QPA_PLATFORM", "offscreen");
  QGuiApplication app(argc, argv);
  int passed = 0;
  for (const auto &[name, func] : g_tests) {
    std::cout << "[RUN ] " << name << "\n";
    func();
    std::cout << "[PASS] " << name << "\n";
    ++passed;
  }
  std::cout << "All " << passed << " text_transforms unit tests passed successfully!\n";
  return 0;
}
#endif
