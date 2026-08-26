#pragma once
#include "abstract-wallpaper-backend.hpp"
#include <QString>

namespace wallpaper {

// Shared by GNOME, Cinnamon and MATE: same picture-options vocabulary.
inline QString gsettingsPictureOption(WallpaperFit fit) {
  switch (fit) {
  case WallpaperFit::Cover:
    return "zoom";
  case WallpaperFit::Contain:
    return "scaled";
  case WallpaperFit::Stretch:
    return "stretched";
  case WallpaperFit::Center:
    return "centered";
  case WallpaperFit::Tile:
    return "wallpaper";
  }
  return "zoom";
}

} // namespace wallpaper
