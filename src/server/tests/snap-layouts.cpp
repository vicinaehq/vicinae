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

#include <QRect>
#include "services/window-manager/abstract-window-manager.hpp"

struct SnapHelper {
  static AbstractWindowManager::WindowBounds calculate(const QRect &screen, const std::string &layout) {
    int w = screen.width();
    int h = screen.height();
    int sx = screen.x();
    int sy = screen.y();

    if (layout == "left-half") return {sx, sy, w / 2, h};
    if (layout == "right-half") return {sx + w / 2, sy, w - w / 2, h};
    if (layout == "top-half") return {sx, sy, w, h / 2};
    if (layout == "bottom-half") return {sx, sy + h / 2, w, h - h / 2};
    if (layout == "maximize") return {sx, sy, w, h};
    if (layout == "almost-maximize") {
      int aw = static_cast<int>(w * 0.90);
      int ah = static_cast<int>(h * 0.90);
      return {sx + (w - aw) / 2, sy + (h - ah) / 2, aw, ah};
    }
    if (layout == "first-third") return {sx, sy, w / 3, h};
    if (layout == "center-third") return {sx + w / 3, sy, w / 3, h};
    if (layout == "last-third") return {sx + 2 * (w / 3), sy, w - 2 * (w / 3), h};
    if (layout == "first-two-thirds") return {sx, sy, 2 * (w / 3), h};
    if (layout == "last-two-thirds") return {sx + w / 3, sy, w - w / 3, h};

    return {sx, sy, w, h};
  }
};

TEST_CASE("snap_layouts: single monitor halves and maximize") {
  QRect screen(0, 0, 1920, 1080);

  auto left = SnapHelper::calculate(screen, "left-half");
  REQUIRE(left.x == 0);
  REQUIRE(left.y == 0);
  REQUIRE(left.width == 960);
  REQUIRE(left.height == 1080);

  auto right = SnapHelper::calculate(screen, "right-half");
  REQUIRE(right.x == 960);
  REQUIRE(right.y == 0);
  REQUIRE(right.width == 960);
  REQUIRE(right.height == 1080);

  auto top = SnapHelper::calculate(screen, "top-half");
  REQUIRE(top.x == 0);
  REQUIRE(top.y == 0);
  REQUIRE(top.width == 1920);
  REQUIRE(top.height == 540);

  auto bottom = SnapHelper::calculate(screen, "bottom-half");
  REQUIRE(bottom.x == 0);
  REQUIRE(bottom.y == 540);
  REQUIRE(bottom.width == 1920);
  REQUIRE(bottom.height == 540);

  auto max = SnapHelper::calculate(screen, "maximize");
  REQUIRE(max.x == 0);
  REQUIRE(max.y == 0);
  REQUIRE(max.width == 1920);
  REQUIRE(max.height == 1080);
}

TEST_CASE("snap_layouts: multi-monitor offsets and almost-maximize") {
  QRect secondScreen(1920, 100, 2560, 1440);

  auto left = SnapHelper::calculate(secondScreen, "left-half");
  REQUIRE(left.x == 1920);
  REQUIRE(left.y == 100);
  REQUIRE(left.width == 1280);
  REQUIRE(left.height == 1440);

  auto almost = SnapHelper::calculate(secondScreen, "almost-maximize");
  REQUIRE(almost.width == static_cast<int>(2560 * 0.90));
  REQUIRE(almost.height == static_cast<int>(1440 * 0.90));
  REQUIRE(almost.x == 1920 + (2560 - almost.width) / 2);
  REQUIRE(almost.y == 100 + (1440 - almost.height) / 2);
}

TEST_CASE("snap_layouts: thirds calculations") {
  QRect screen(0, 0, 1920, 1080);

  auto first = SnapHelper::calculate(screen, "first-third");
  REQUIRE(first.x == 0);
  REQUIRE(first.width == 640);

  auto center = SnapHelper::calculate(screen, "center-third");
  REQUIRE(center.x == 640);
  REQUIRE(center.width == 640);

  auto last = SnapHelper::calculate(screen, "last-third");
  REQUIRE(last.x == 1280);
  REQUIRE(last.width == 640);

  auto firstTwo = SnapHelper::calculate(screen, "first-two-thirds");
  REQUIRE(firstTwo.x == 0);
  REQUIRE(firstTwo.width == 1280);

  auto lastTwo = SnapHelper::calculate(screen, "last-two-thirds");
  REQUIRE(lastTwo.x == 640);
  REQUIRE(lastTwo.width == 1280);
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
  std::cout << "All " << passed << " snap_layouts unit tests passed successfully!\n";
  return 0;
}
#endif
