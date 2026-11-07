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

#include "services/snippet/snippet-expander.hpp"
#include <QGuiApplication>
#include <QDateTime>

// Test stubs
ServiceRegistry *ServiceRegistry::instance() { return nullptr; }
AbstractSelectionService *ServiceRegistry::selectionService() const { return nullptr; }
std::unique_ptr<QProcess> AppService::shellProcess(const QString &) const { return nullptr; }

TEST_CASE("snippet: basic expansion and cursor positioning") {
  SnippetExpander expander;
  auto res = expander.expand(QStringLiteral("Hello {cursor}world!"), {});
  REQUIRE(res.cursorPos.has_value());
  REQUIRE(*res.cursorPos == 6);
  QString str = expander.expandToString(QStringLiteral("Hello {cursor}world!"), {});
  REQUIRE(str == QStringLiteral("Hello world!"));
}

TEST_CASE("snippet: argument substitution and defaults") {
  SnippetExpander expander;
  // With argument provided
  std::vector<std::pair<QString, QString>> args = {{"name", "Alice"}};
  QString str1 = expander.expandToString(QStringLiteral("Hi {name}!"), args);
  REQUIRE(str1 == QStringLiteral("Hi Alice!"));

  // Without argument, default value used
  QString str2 =
      expander.expandToString(QStringLiteral("Hi {argument name=\"title\" default=\"Friend\"}!"), {});
  REQUIRE(str2 == QStringLiteral("Hi Friend!"));
}

TEST_CASE("snippet: modifiers chaining") {
  SnippetExpander expander;
  std::vector<std::pair<QString, QString>> args = {{"text", "  hello world  "}};

  QString str = expander.expandToString(QStringLiteral("Result: {text | trim | uppercase}"), args);
  REQUIRE(str == QStringLiteral("Result: HELLO WORLD"));
}

TEST_CASE("snippet: fallback chains") {
  SnippetExpander expander;
  // Argument missing, fallback to literal
  QString str1 = expander.expandToString(QStringLiteral("Key: {missing ?? DefaultValue}"), {});
  REQUIRE(str1 == QStringLiteral("Key: DefaultValue"));

  // Argument missing, fallback to another argument
  std::vector<std::pair<QString, QString>> args = {{"backup", "FoundMe"}};
  QString str2 = expander.expandToString(QStringLiteral("Key: {missing ?? backup}"), args);
  REQUIRE(str2 == QStringLiteral("Key: FoundMe"));
}

TEST_CASE("snippet: date math offsets and relative units") {
  SnippetExpander expander;

  // Tomorrow date offset
  QDateTime tomorrow = QDateTime::currentDateTime().addDays(1);
  QString expectedTomorrow = tomorrow.toString(QStringLiteral("yyyy-MM-dd"));
  QString strTomorrow =
      expander.expandToString(QStringLiteral("{date offset=\"+1d\" format=\"YYYY-MM-DD\"}"), {});
  REQUIRE(strTomorrow == expectedTomorrow);

  // Relative unit {time}
  QString expectedTime = QDateTime::currentDateTime().toString(QStringLiteral("HH:mm"));
  QString strTime = expander.expandToString(QStringLiteral("{time}"), {});
  REQUIRE(strTime == expectedTime);

  // Relative unit {day}
  QString expectedDay = QDateTime::currentDateTime().toString(QStringLiteral("dddd"));
  QString strDay = expander.expandToString(QStringLiteral("{day}"), {});
  REQUIRE(strDay == expectedDay);
}

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
  std::cout << "All " << passed << " snippet unit tests passed successfully!\n";
  return 0;
}
#endif
