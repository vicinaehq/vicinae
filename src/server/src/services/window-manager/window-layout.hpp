#pragma once
#include <QRect>
#include <span>
#include <string>
#include <unordered_map>
#include "abstract-window-manager.hpp"

namespace WindowLayout {

enum class Kind {
  LeftHalf,
  RightHalf,
  TopHalf,
  BottomHalf,
  TopLeftQuarter,
  TopRightQuarter,
  BottomLeftQuarter,
  BottomRightQuarter,
  FirstThird,
  CenterThird,
  LastThird,
  FirstTwoThirds,
  LastTwoThirds,
  Center,
  Maximize,
  AlmostMaximize,
  MakeSmaller,
  MakeLarger,
  NextDisplay,
  PreviousDisplay,
  Restore
};

enum class Result { Success, NoBounds, NoScreen, NoOtherDisplay, Fullscreen, NothingToRestore, Failed };

const AbstractWindowManager::Screen *screenForWindow(const QRect &window,
                                                     std::span<const AbstractWindowManager::Screen> screens);
QRect calculateBounds(Kind kind, const QRect &window, const QRect &available);
QRect boundsOnDisplay(const QRect &window, const QRect &source, const QRect &destination);

class Manager {
public:
  Result apply(AbstractWindowManager &provider, const AbstractWindowManager::AbstractWindow &window,
               Kind kind);
  void forgetClosedWindows(std::span<const AbstractWindowManager::WindowPtr> windows);

private:
  std::unordered_map<std::string, QRect> m_previousBounds;
};

} // namespace WindowLayout
