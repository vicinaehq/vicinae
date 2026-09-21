#include <algorithm>
#include <cstdint>
#include <format>
#include <limits>
#include <ranges>
#include <utility>
#include "window-layout.hpp"

namespace WindowLayout {
namespace {

constexpr double ALMOST_MAXIMIZE_SCALE = 0.9;
constexpr double RESIZE_STEP = 0.1;

QRect fitToScreen(const QRect &window, const QRect &available) {
  const QSize size = window.size().boundedTo(available.size());
  return {std::clamp(window.x(), available.x(), available.x() + available.width() - size.width()),
          std::clamp(window.y(), available.y(), available.y() + available.height() - size.height()),
          size.width(), size.height()};
}

QRect centered(const QSize &size, const QRect &available) {
  return {available.x() + (available.width() - size.width()) / 2,
          available.y() + (available.height() - size.height()) / 2, size.width(), size.height()};
}

QRect toRect(const AbstractWindowManager::WindowBounds &bounds) {
  return {bounds.x, bounds.y, bounds.width, bounds.height};
}

std::string windowKey(const AbstractWindowManager::AbstractWindow &window) {
  return std::format("{}:{}", window.pid().value_or(0), window.id().toStdString());
}

} // namespace

const AbstractWindowManager::Screen *screenForWindow(const QRect &window,
                                                     std::span<const AbstractWindowManager::Screen> screens) {
  const AbstractWindowManager::Screen *best = nullptr;
  std::int64_t largestArea = -1;
  double nearestDistance = std::numeric_limits<double>::max();
  const auto center = window.center();

  for (const auto &screen : screens) {
    if (!screen.bounds.isValid() || !screen.availableBounds.isValid()) continue;
    const auto intersection = window.intersected(screen.bounds);
    const auto area = std::int64_t{intersection.width()} * intersection.height();
    const double dx = center.x() - std::clamp(center.x(), screen.bounds.left(), screen.bounds.right());
    const double dy = center.y() - std::clamp(center.y(), screen.bounds.top(), screen.bounds.bottom());
    const double distance = dx * dx + dy * dy;
    if (area > largestArea || (area == largestArea && distance < nearestDistance)) {
      best = &screen;
      largestArea = area;
      nearestDistance = distance;
    }
  }
  return best;
}

QRect calculateBounds(Kind kind, const QRect &window, const QRect &available) {
  if (!window.isValid() || !available.isValid()) return {};

  // Round shared edges, so adjacent tiles fill odd-sized screens without gaps or overlaps.
  auto tile = [&](int column, int row, int columns, int rows, int width = 1, int height = 1) {
    const int left = qRound(static_cast<double>(available.width()) * column / columns);
    const int right = qRound(static_cast<double>(available.width()) * (column + width) / columns);
    const int top = qRound(static_cast<double>(available.height()) * row / rows);
    const int bottom = qRound(static_cast<double>(available.height()) * (row + height) / rows);
    return QRect(available.x() + left, available.y() + top, right - left, bottom - top);
  };

  switch (kind) {
  case Kind::LeftHalf:
    return tile(0, 0, 2, 1);
  case Kind::RightHalf:
    return tile(1, 0, 2, 1);
  case Kind::TopHalf:
    return tile(0, 0, 1, 2);
  case Kind::BottomHalf:
    return tile(0, 1, 1, 2);
  case Kind::TopLeftQuarter:
    return tile(0, 0, 2, 2);
  case Kind::TopRightQuarter:
    return tile(1, 0, 2, 2);
  case Kind::BottomLeftQuarter:
    return tile(0, 1, 2, 2);
  case Kind::BottomRightQuarter:
    return tile(1, 1, 2, 2);
  case Kind::FirstThird:
    return tile(0, 0, 3, 1);
  case Kind::CenterThird:
    return tile(1, 0, 3, 1);
  case Kind::LastThird:
    return tile(2, 0, 3, 1);
  case Kind::FirstTwoThirds:
    return tile(0, 0, 3, 1, 2);
  case Kind::LastTwoThirds:
    return tile(1, 0, 3, 1, 2);
  case Kind::Center:
    return centered(window.size(), available);
  case Kind::Maximize:
    return available;
  case Kind::AlmostMaximize:
    return centered(QSize(qRound(available.width() * ALMOST_MAXIMIZE_SCALE),
                          qRound(available.height() * ALMOST_MAXIMIZE_SCALE)),
                    available);
  case Kind::MakeSmaller:
  case Kind::MakeLarger: {
    const int direction = kind == Kind::MakeLarger ? 1 : -1;
    const QSize size(
        std::clamp(window.width() + direction * std::max(1, qRound(available.width() * RESIZE_STEP)), 1,
                   available.width()),
        std::clamp(window.height() + direction * std::max(1, qRound(available.height() * RESIZE_STEP)), 1,
                   available.height()));
    return fitToScreen(centered(size, window), available);
  }
  case Kind::Restore:
    return fitToScreen(window, available);
  case Kind::NextDisplay:
  case Kind::PreviousDisplay:
    return window;
  }
  std::unreachable();
}

QRect boundsOnDisplay(const QRect &window, const QRect &source, const QRect &destination) {
  if (!window.isValid() || !source.isValid() || !destination.isValid()) return {};
  const double horizontalScale = static_cast<double>(destination.width()) / source.width();
  const double verticalScale = static_cast<double>(destination.height()) / source.height();
  return fitToScreen({destination.x() + qRound((window.x() - source.x()) * horizontalScale),
                      destination.y() + qRound((window.y() - source.y()) * verticalScale),
                      std::max(1, qRound(window.width() * horizontalScale)),
                      std::max(1, qRound(window.height() * verticalScale))},
                     destination);
}

Result Manager::apply(AbstractWindowManager &provider, const AbstractWindowManager::AbstractWindow &window,
                      Kind kind) {
  if (!provider.supports(AbstractWindowManager::Capability::WindowPlacement)) return Result::Failed;
  if (window.fullScreen()) return Result::Fullscreen;
  const auto bounds = window.bounds();
  if (!bounds || !toRect(*bounds).isValid()) return Result::NoBounds;
  const QRect current = toRect(*bounds);
  const auto key = windowKey(window);
  QRect reference = current;
  if (kind == Kind::Restore) {
    const auto previous = m_previousBounds.find(key);
    if (previous == m_previousBounds.end()) return Result::NothingToRestore;
    reference = previous->second;
  }

  auto screens = provider.listScreensSync();
  std::erase_if(screens, [](const auto &screen) {
    return !screen.bounds.isValid() || !screen.availableBounds.isValid();
  });
  // Use desktop arrangement order so cycling does not depend on which monitor is primary.
  std::ranges::stable_sort(screens, [](const auto &left, const auto &right) {
    return std::pair(left.bounds.x(), left.bounds.y()) < std::pair(right.bounds.x(), right.bounds.y());
  });
  const auto *screen = screenForWindow(reference, screens);
  if (!screen) return Result::NoScreen;
  QRect target;
  if (kind == Kind::NextDisplay || kind == Kind::PreviousDisplay) {
    if (screens.size() < 2) return Result::NoOtherDisplay;
    const auto index = static_cast<std::size_t>(screen - screens.data());
    const auto next = kind == Kind::NextDisplay ? (index + 1) % screens.size()
                                                : (index + screens.size() - 1) % screens.size();
    const auto &destination = screens.at(next);
    target = boundsOnDisplay(current, screen->availableBounds, destination.availableBounds);
    screen = &destination;
  } else {
    target = calculateBounds(kind, reference, screen->availableBounds);
  }
  if (target == current) {
    if (kind == Kind::Restore) m_previousBounds.erase(key);
    return Result::Success;
  }

  bool ok = provider.setWindowBounds(
      window, {.x = target.x(), .y = target.y(), .width = target.width(), .height = target.height()});
  if (const auto actual = window.bounds(); ok && actual && kind != Kind::Center) {
    // Apps may enforce a minimum size larger than the requested tile. Keep the result on screen.
    const auto &area = screen->availableBounds;
    const int x = std::clamp(actual->x, area.x(), area.x() + std::max(0, area.width() - actual->width));
    const int y = std::clamp(actual->y, area.y(), area.y() + std::max(0, area.height() - actual->height));
    if (x != actual->x || y != actual->y) {
      ok = provider.setWindowBounds(window,
                                    {.x = x, .y = y, .width = actual->width, .height = actual->height});
    }
  }
  // Keep an undo point even if an app accepted only part of the move/resize request.
  const auto actual = window.bounds();
  if (kind != Kind::Restore && actual && toRect(*actual) != current) {
    m_previousBounds.insert_or_assign(key, current);
  }
  if (!ok) return Result::Failed;
  if (kind == Kind::Restore) m_previousBounds.erase(key);
  return Result::Success;
}

void Manager::forgetClosedWindows(std::span<const AbstractWindowManager::WindowPtr> windows) {
  std::erase_if(m_previousBounds, [&](const auto &entry) {
    return std::ranges::none_of(windows,
                                [&](const auto &window) { return windowKey(*window) == entry.first; });
  });
}

} // namespace WindowLayout
