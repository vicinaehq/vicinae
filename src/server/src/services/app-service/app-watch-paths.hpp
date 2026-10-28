#pragma once

#include <deque>
#include <filesystem>
#include <set>
#include <system_error>
#include <utility>
#include <vector>

namespace appWatchPaths {

inline std::vector<std::filesystem::path> directories(const std::vector<std::filesystem::path> &paths) {
  namespace fs = std::filesystem;
  std::set<fs::path> watches;

  for (const auto &path : paths) {
    std::deque<fs::path> pending(path.begin(), path.end());
    fs::path current;
    unsigned int links = 0;
    bool followedLink = false;

    while (!pending.empty() && links < 40) {
      auto next = (current / pending.front()).lexically_normal();
      pending.pop_front();
      std::error_code ec;
      auto status = fs::symlink_status(next, ec);
      if (ec) {
        std::error_code dirEc;
        if (followedLink && fs::is_directory(current, dirEc)) watches.emplace(current);
        break;
      }

      if (fs::is_symlink(status)) {
        // Watching the resolved directory alone misses changes to a profile symlink.
        if (fs::is_directory(current, ec)) watches.emplace(current);
        auto target = fs::read_symlink(next, ec);
        if (ec) break;
        if (target.is_absolute()) current.clear();
        for (auto it = target.end(); it != target.begin();)
          pending.push_front(*--it);
        ++links;
        followedLink = true;
      } else {
        current = std::move(next);
      }
    }

    std::error_code ec;
    if (pending.empty() && fs::is_directory(current, ec)) watches.emplace(std::move(current));
  }

  return {watches.begin(), watches.end()};
}

} // namespace appWatchPaths
