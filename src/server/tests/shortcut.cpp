#if __has_include(<catch2/catch_test_macros.hpp>)
#include <catch2/catch_test_macros.hpp>
#else
#include <cassert>
#include <iostream>
#include <functional>
#include <vector>

namespace {
inline std::vector<std::pair<const char *, std::function<void()>>> &get_tests() {
  static std::vector<std::pair<const char *, std::function<void()>>> tests;
  return tests;
}
struct AutoRegister {
  AutoRegister(const char *name, std::function<void()> fn) { get_tests().push_back({name, fn}); }
};
} // namespace

#define CONCAT_INNER(a, b) a##b
#define CONCAT(a, b) CONCAT_INNER(a, b)
#define TEST_CASE(name)                                                                                      \
  static void CONCAT(test_func_, __LINE__)();                                                                \
  static AutoRegister CONCAT(reg_, __LINE__)(name, CONCAT(test_func_, __LINE__));                            \
  static void CONCAT(test_func_, __LINE__)()

#define REQUIRE(expr)                                                                                        \
  do {                                                                                                       \
    if (!(expr)) {                                                                                           \
      std::cerr << "Assertion failed: " #expr " at " __FILE__ ":" << __LINE__ << std::endl;                  \
      std::abort();                                                                                          \
    }                                                                                                        \
  } while (0)

#define REQUIRE_FALSE(expr) REQUIRE(!(expr))
#endif

#include "services/shortcut/shortcut.hpp"
#include "service-registry.hpp"
#include <QDateTime>
#include <QCoreApplication>

#if !__has_include(<catch2/catch_test_macros.hpp>)
ServiceRegistry *ServiceRegistry::instance() { return nullptr; }
AbstractSelectionService *ServiceRegistry::selectionService() const { return nullptr; }
#endif

// NOLINTBEGIN(bugprone-throwing-static-initialization)
TEST_CASE("shortcut: basic link parsing without placeholders") {
  Shortcut sc;
  sc.parseLink(QStringLiteral("https://vicinae.com"));

  REQUIRE(sc.arguments().empty());
  REQUIRE(sc.placeholders().empty());
  REQUIRE(sc.expand({}) == "https://vicinae.com");
}

TEST_CASE("shortcut: argument parsing and automatic percent-encoding in URLs") {
  Shortcut sc;
  sc.parseLink(QStringLiteral("https://google.com/search?q={query}"));

  REQUIRE(sc.arguments().size() == 1);
  REQUIRE(sc.arguments()[0].name == "query");

  const std::vector<QString> args = {QStringLiteral("c++ 23 & raycast")};
  const QString expanded = sc.expand(args);

  REQUIRE(expanded == "https://google.com/search?q=c%2B%2B%2023%20%26%20raycast");
}

TEST_CASE("shortcut: raw modifier prevents percent-encoding in URLs") {
  Shortcut sc;
  sc.parseLink(QStringLiteral("https://api.example.com/v1/{path | raw}"));

  const std::vector<QString> args = {QStringLiteral("users/123/profile")};
  const QString expanded = sc.expand(args);

  REQUIRE(expanded == "https://api.example.com/v1/users/123/profile");
}

TEST_CASE("shortcut: modifier chaining (uppercase, trim)") {
  Shortcut sc;
  sc.parseLink(QStringLiteral("git checkout {branch | trim | uppercase}"));

  const std::vector<QString> args = {QStringLiteral("  feature-branch  ")};
  const QString expanded = sc.expand(args);

  REQUIRE(expanded == "git checkout FEATURE-BRANCH");
}

TEST_CASE("shortcut: default argument fallback when value omitted") {
  Shortcut sc;
  sc.parseLink(QStringLiteral("https://github.com/search?q={argument name=\"query\" default=\"vicinae\"}"));

  REQUIRE(sc.arguments().size() == 1);
  REQUIRE(sc.arguments()[0].defaultValue == "vicinae");

  // Omitted argument uses default
  REQUIRE(sc.expand({}) == "https://github.com/search?q=vicinae");

  // Provided argument overrides default
  const std::vector<QString> custom = {QStringLiteral("kde plasma")};
  REQUIRE(sc.expand(custom) == "https://github.com/search?q=kde%20plasma");
}

TEST_CASE("shortcut: argument deduplication in argument list") {
  Shortcut sc;
  sc.parseLink(QStringLiteral("https://example.com/?q1={search}&q2={search}"));

  REQUIRE(sc.arguments().size() == 1);
  REQUIRE(sc.arguments()[0].name == "search");

  const std::vector<QString> args = {QStringLiteral("hello")};
  REQUIRE(sc.expand(args) == "https://example.com/?q1=hello&q2=hello");
}

TEST_CASE("shortcut: date placeholder with offset and formatting") {
  Shortcut sc;
  sc.parseLink(QStringLiteral("log-{date offset=\"+1d\" format=\"yyyy-MM-dd\"}.txt"));

  const QString expanded = sc.expand({});
  const QString tomorrow = QDateTime::currentDateTime().addDays(1).toString("yyyy-MM-dd");

  REQUIRE(expanded == QStringLiteral("log-%1.txt").arg(tomorrow));
}
// NOLINTEND(bugprone-throwing-static-initialization)

#if !__has_include(<catch2/catch_test_macros.hpp>)
int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  int passed = 0;
  for (const auto &[name, fn] : get_tests()) {
    std::cout << "[RUN ] " << name << std::endl;
    fn();
    std::cout << "[PASS] " << name << std::endl;
    passed++;
  }
  std::cout << "All " << passed << " shortcut unit tests passed successfully!" << std::endl;
  return 0;
}
#endif
