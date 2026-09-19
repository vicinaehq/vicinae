#include <catch2/catch_test_macros.hpp>
#include <algorithm>
#include <array>
#include "services/window-manager/window-layout.hpp"

using namespace WindowLayout;
using Screen = AbstractWindowManager::Screen;
using Bounds = AbstractWindowManager::WindowBounds;

namespace {

class TestWindow : public AbstractWindowManager::AbstractWindow {
public:
  QString identifier = "1";
  std::optional<Bounds> geometry = Bounds{120, 100, 640, 480};
  bool fullscreen = false;
  QString id() const override { return identifier; }
  QString title() const override { return "Test"; }
  QString wmClass() const override { return "test"; }
  std::optional<int> pid() const override { return 42; }
  std::optional<Bounds> bounds() const override { return geometry; }
  bool fullScreen() const override { return fullscreen; }
  QRect rect() const { return {geometry->x, geometry->y, geometry->width, geometry->height}; }
};

class TestProvider : public AbstractWindowManager {
public:
  std::shared_ptr<TestWindow> window = std::make_shared<TestWindow>();
  std::vector<Screen> screens = {
      {.bounds = QRect(0, 0, 1200, 900), .availableBounds = QRect(0, 30, 1200, 820)}};
  bool fail = false;
  bool partialFailure = false;
  bool placementSupported = true;
  QSize minimumSize{1, 1};
  mutable int writes = 0;
  QString id() const override { return "test"; }
  bool ping() const override { return true; }
  bool isActivatable() const override { return true; }
  void start() override {}
  QFlags<Capability> capabilities() const override {
    return placementSupported ? QFlags{Capability::WindowPlacement} : QFlags<Capability>{};
  }
  std::vector<Screen> listScreensSync(QWindow *) const override { return screens; }
  bool setWindowBounds(const AbstractWindow &, const Bounds &bounds) const override {
    ++writes;
    if (partialFailure) {
      window->geometry->x = bounds.x;
      return false;
    }
    if (fail) return false;
    window->geometry = bounds;
    window->geometry->width = std::max(bounds.width, minimumSize.width());
    window->geometry->height = std::max(bounds.height, minimumSize.height());
    return true;
  }
};

} // namespace

TEST_CASE("Tiles cover odd-sized work areas without gaps or overlap") {
  const QRect area(-1511, -920, 1511, 877);
  const QRect window(-1400, -800, 800, 600);
  const auto left = calculateBounds(Kind::LeftHalf, window, area);
  const auto right = calculateBounds(Kind::RightHalf, window, area);
  CHECK(left.united(right) == area);
  CHECK(left.intersected(right).isEmpty());
  CHECK(left.right() + 1 == right.left());

  const auto first = calculateBounds(Kind::FirstThird, window, area);
  const auto center = calculateBounds(Kind::CenterThird, window, area);
  const auto last = calculateBounds(Kind::LastThird, window, area);
  CHECK(first.right() + 1 == center.left());
  CHECK(center.right() + 1 == last.left());
  CHECK(first.width() + center.width() + last.width() == area.width());
  CHECK(first.united(center) == calculateBounds(Kind::FirstTwoThirds, window, area));
  CHECK(center.united(last) == calculateBounds(Kind::LastTwoThirds, window, area));

  const auto topLeft = calculateBounds(Kind::TopLeftQuarter, window, area);
  const auto topRight = calculateBounds(Kind::TopRightQuarter, window, area);
  const auto bottomLeft = calculateBounds(Kind::BottomLeftQuarter, window, area);
  const auto bottomRight = calculateBounds(Kind::BottomRightQuarter, window, area);
  CHECK(topLeft.united(topRight) == calculateBounds(Kind::TopHalf, window, area));
  CHECK(bottomLeft.united(bottomRight) == calculateBounds(Kind::BottomHalf, window, area));
  CHECK(topLeft.bottom() + 1 == bottomLeft.top());
  CHECK(topLeft.united(bottomLeft) == left);
  CHECK(topRight.united(bottomRight) == right);
}

TEST_CASE("Center preserves size and maximize respects reserved system UI") {
  const QRect area(50, 30, 1200, 800);
  const QRect window(20, 40, 600, 400);
  CHECK(calculateBounds(Kind::Center, window, area) == QRect(350, 230, 600, 400));
  CHECK(calculateBounds(Kind::Maximize, window, area) == area);
  CHECK(calculateBounds(Kind::AlmostMaximize, window, area) == QRect(110, 70, 1080, 720));
}

TEST_CASE("Incremental resize preserves the center and stays within the work area") {
  const QRect area(0, 30, 1200, 800);
  const QRect window(300, 230, 600, 400);
  const auto smaller = calculateBounds(Kind::MakeSmaller, window, area);
  CHECK(smaller == QRect(360, 270, 480, 320));
  CHECK(calculateBounds(Kind::MakeLarger, smaller, area) == window);
  CHECK(calculateBounds(Kind::MakeSmaller, smaller, area).width() < smaller.width());
  CHECK(calculateBounds(Kind::MakeLarger, area, area) == area);
  CHECK(area.contains(calculateBounds(Kind::MakeLarger, QRect(1000, 700, 200, 130), area)));
  CHECK(calculateBounds(Kind::MakeSmaller, QRect(0, 30, 1, 1), area).size() == QSize(1, 1));
}

TEST_CASE("Screen selection follows the window, including negative origins and disconnected displays") {
  const auto screens = std::to_array<Screen>({
      {.bounds = QRect(0, 0, 1200, 900), .availableBounds = QRect(0, 30, 1200, 820), .active = true},
      {.bounds = QRect(-1600, -300, 1600, 1200), .availableBounds = QRect(-1600, -270, 1600, 1170)},
      {.bounds = QRect(0, -900, 1200, 900), .availableBounds = QRect(0, -870, 1200, 870)},
  });
  CHECK(screenForWindow(QRect(-900, 100, 1000, 500), screens) == &screens[1]);
  CHECK(screenForWindow(QRect(100, -800, 600, 400), screens) == &screens[2]);
  CHECK(screenForWindow(QRect(-3000, 0, 600, 400), screens) == &screens[1]);
  CHECK(screenForWindow(QRect(2500, 0, 600, 400), screens) == &screens[0]);
  CHECK(screenForWindow(QRect(0, 0, 600, 400), {}) == nullptr);
}

TEST_CASE("Restore undoes the last change and repeated placement keeps the undo point") {
  TestProvider provider;
  Manager manager;
  const auto original = provider.window->rect();
  REQUIRE(manager.apply(provider, *provider.window, Kind::RightHalf) == Result::Success);
  REQUIRE(manager.apply(provider, *provider.window, Kind::RightHalf) == Result::Success);
  CHECK(provider.writes == 1);
  REQUIRE(manager.apply(provider, *provider.window, Kind::Restore) == Result::Success);
  CHECK(provider.window->rect() == original);
  CHECK(manager.apply(provider, *provider.window, Kind::Restore) == Result::NothingToRestore);

  REQUIRE(manager.apply(provider, *provider.window, Kind::MakeSmaller) == Result::Success);
  const auto smaller = provider.window->rect();
  REQUIRE(manager.apply(provider, *provider.window, Kind::MakeSmaller) == Result::Success);
  CHECK(provider.window->rect().width() < smaller.width());
  REQUIRE(manager.apply(provider, *provider.window, Kind::Restore) == Result::Success);
  CHECK(provider.window->rect() == smaller);
}

TEST_CASE("Failures preserve existing restore history and partial changes can be undone") {
  TestProvider provider;
  Manager manager;
  const auto original = provider.window->rect();
  REQUIRE(manager.apply(provider, *provider.window, Kind::Maximize) == Result::Success);
  provider.fail = true;
  CHECK(manager.apply(provider, *provider.window, Kind::LeftHalf) == Result::Failed);
  CHECK(manager.apply(provider, *provider.window, Kind::Restore) == Result::Failed);
  provider.fail = false;
  REQUIRE(manager.apply(provider, *provider.window, Kind::Restore) == Result::Success);
  CHECK(provider.window->rect() == original);

  provider.partialFailure = true;
  REQUIRE(manager.apply(provider, *provider.window, Kind::RightHalf) == Result::Failed);
  CHECK(provider.window->rect() != original);
  provider.partialFailure = false;
  REQUIRE(manager.apply(provider, *provider.window, Kind::Restore) == Result::Success);
  CHECK(provider.window->rect() == original);
}

TEST_CASE("Restore history belongs to the individual window") {
  TestProvider provider;
  Manager manager;
  const auto firstWindow = provider.window;
  const auto firstBounds = firstWindow->rect();
  REQUIRE(manager.apply(provider, *firstWindow, Kind::LeftHalf) == Result::Success);

  provider.window = std::make_shared<TestWindow>();
  provider.window->identifier = "2";
  provider.window->geometry = Bounds{320, 300, 500, 400};
  const auto secondBounds = provider.window->rect();
  CHECK(manager.apply(provider, *provider.window, Kind::Restore) == Result::NothingToRestore);
  REQUIRE(manager.apply(provider, *provider.window, Kind::RightHalf) == Result::Success);
  REQUIRE(manager.apply(provider, *provider.window, Kind::Restore) == Result::Success);
  CHECK(provider.window->rect() == secondBounds);

  provider.window = firstWindow;
  REQUIRE(manager.apply(provider, *provider.window, Kind::Restore) == Result::Success);
  CHECK(provider.window->rect() == firstBounds);
}

TEST_CASE("Application minimum sizes do not push right-hand tiles off screen") {
  TestProvider provider;
  Manager manager;
  provider.minimumSize = QSize(800, 400);
  REQUIRE(manager.apply(provider, *provider.window, Kind::LastThird) == Result::Success);
  CHECK(provider.window->rect() == QRect(400, 30, 800, 820));
}

TEST_CASE("Unavailable windows and screens do not produce resize requests") {
  TestProvider provider;
  Manager manager;
  SECTION("fullscreen") {
    provider.window->fullscreen = true;
    CHECK(manager.apply(provider, *provider.window, Kind::LeftHalf) == Result::Fullscreen);
  }
  SECTION("missing geometry") {
    provider.window->geometry.reset();
    CHECK(manager.apply(provider, *provider.window, Kind::LeftHalf) == Result::NoBounds);
  }
  SECTION("missing display") {
    provider.screens.clear();
    CHECK(manager.apply(provider, *provider.window, Kind::LeftHalf) == Result::NoScreen);
  }
  SECTION("unsupported backend") {
    provider.placementSupported = false;
    CHECK(manager.apply(provider, *provider.window, Kind::LeftHalf) == Result::Failed);
  }
  CHECK(provider.writes == 0);
}

TEST_CASE("Restore recovers windows after a display disconnect and forgets closed windows") {
  TestProvider provider;
  Manager manager;
  provider.window->geometry = Bounds{-1400, -200, 800, 600};
  provider.screens.emplace_back(
      Screen{.bounds = QRect(-1600, -300, 1600, 1200), .availableBounds = QRect(-1600, -270, 1600, 1170)});
  REQUIRE(manager.apply(provider, *provider.window, Kind::Maximize) == Result::Success);
  provider.screens.pop_back();
  REQUIRE(manager.apply(provider, *provider.window, Kind::Restore) == Result::Success);
  CHECK(provider.screens[0].availableBounds.contains(provider.window->rect()));

  REQUIRE(manager.apply(provider, *provider.window, Kind::Maximize) == Result::Success);
  manager.forgetClosedWindows({});
  CHECK(manager.apply(provider, *provider.window, Kind::Restore) == Result::NothingToRestore);
}
