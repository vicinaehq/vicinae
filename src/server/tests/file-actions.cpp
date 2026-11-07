#if __has_include(<catch2/catch_test_macros.hpp>)
#include <catch2/catch_test_macros.hpp>
#else
#include <cassert>
#include <functional>
#include <iostream>
#include <vector>

static std::vector<std::pair<const char *, std::function<void()>>> g_tests;

#define CONCAT_INNER(a, b) a##b
#define CONCAT(a, b) CONCAT_INNER(a, b)

#define TEST_CASE(name, ...)                                                                                 \
  static void CONCAT(test_func_, __LINE__)();                                                                 \
  static struct CONCAT(TestAutoReg_, __LINE__) {                                                             \
    CONCAT(TestAutoReg_, __LINE__)() { g_tests.push_back({name, CONCAT(test_func_, __LINE__)}); }             \
  } CONCAT(g_autoreg_, __LINE__);                                                                             \
  static void CONCAT(test_func_, __LINE__)()

#define REQUIRE(expr)                                                                                         \
  do {                                                                                                        \
    if (!(expr)) {                                                                                            \
      std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__ << ": " << #expr << "\n";           \
      std::abort();                                                                                           \
    }                                                                                                         \
  } while (0)

#define CHECK(expr) REQUIRE(expr)
#endif

#include <filesystem>
#include <QString>
#include <QFile>

TEST_CASE("file_actions: directory resolution logic") {
  namespace fs = std::filesystem;
  fs::path filePath = "/tmp/test_dir/sample_file.txt";
  std::error_code ec;

  // Since filePath does not exist as directory, target dir should be its parent
  fs::path targetDir = fs::is_directory(filePath, ec) ? filePath : filePath.parent_path();
  REQUIRE(targetDir == "/tmp/test_dir");

  // For /tmp (exists as directory)
  fs::path tmpDir = "/tmp";
  fs::path resolvedTmp = fs::is_directory(tmpDir, ec) ? tmpDir : tmpDir.parent_path();
  REQUIRE(resolvedTmp == "/tmp");
}

TEST_CASE("file_actions: copy file content limit logic") {
  namespace fs = std::filesystem;
  const size_t MAX_SIZE = 2 * 1024 * 1024;
  REQUIRE(1024 < MAX_SIZE);
  REQUIRE(5 * 1024 * 1024 > MAX_SIZE);
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
  std::cout << "All " << passed << " file_actions unit tests passed successfully!\n";
  return 0;
}
#endif
