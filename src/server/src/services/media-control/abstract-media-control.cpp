#include <algorithm>
#include <ranges>
#include "services/media-control/abstract-media-control.hpp"

std::optional<MediaPlayer> AbstractMediaControl::defaultPlayer() const {
  auto all = players();

  if (all.empty()) return std::nullopt;

  if (m_lastPlayerId) {
    if (auto it = std::ranges::find(all, *m_lastPlayerId, &MediaPlayer::id); it != all.end()) {
      return std::move(*it);
    }
  }

  if (auto it = std::ranges::find(all, PlaybackStatus::Playing, &MediaPlayer::status); it != all.end()) {
    return std::move(*it);
  }

  return std::move(all.front());
}
