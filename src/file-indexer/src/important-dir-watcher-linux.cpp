#include "file-indexer/important-dir-watcher.hpp"
#include "file-indexer/entry-filter.hpp"
#include "file-indexer/log.hpp"
#include "file-indexer/util.hpp"
#include "xdgpp/env/env.hpp"
#include <cstdint>
#include <deque>
#include <mutex>
#include <poll.h>
#include <ranges>
#include <sys/eventfd.h>
#include <sys/inotify.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>

namespace fs = std::filesystem;

namespace {

class LinuxImportantDirectoryWatcher : public ImportantDirectoryWatcher {
  static constexpr uint32_t WATCH_MASK =
      IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO | IN_DELETE_SELF | IN_ONLYDIR | IN_EXCL_UNLINK;

  // directories created later inherit their parent's depth + 1; past the budget,
  // coverage degrades to sweep cadence
  static constexpr int MAX_WATCH_DEPTH = 2;
  static constexpr size_t WATCH_BUDGET = 8192;

  struct WatchInfo {
    fs::path path;
    int depth = 0;
  };

  Callback m_cb;
  EntryFilter m_filter;
  int m_inFd = -1;
  int m_wakeFd = -1;
  mutable std::mutex m_watchesMtx;
  std::unordered_map<int, WatchInfo> m_watches;
  std::unordered_map<fs::path, int> m_watchByPath;
  std::unordered_set<fs::path> m_dynamicPaths;
  std::vector<fs::path> m_roots;
  bool m_budgetExhausted = false;
  std::thread m_thread;

  static std::vector<fs::path> computeRoots() {
    std::vector<fs::path> dirs;
    std::error_code ec;
    auto const home = file_indexer::homeDir();

    if (home.empty()) return dirs;

    dirs.reserve(64);
    dirs.emplace_back(home);

    for (const auto &entry : fs::directory_iterator(home, ec)) {
      if (!entry.is_directory(ec)) continue;
      if (entry.is_symlink(ec)) continue;
      if (file_indexer::isHiddenPath(entry.path())) continue;
      dirs.emplace_back(entry.path());
    }

    // hidden, so missed by the enumeration above, but indexed nonetheless
    for (auto &&dir : {xdgpp::configHome(), xdgpp::dataHome()}) {
      if (fs::is_directory(dir, ec)) dirs.emplace_back(dir);
    }

    return dirs;
  }

  bool addWatch(const fs::path &dir, int depth) {
    {
      std::scoped_lock const l(m_watchesMtx);

      if (m_watches.size() >= WATCH_BUDGET) {
        if (!m_budgetExhausted) {
          m_budgetExhausted = true;
          flog::warn() << "Watch budget exhausted (" << WATCH_BUDGET
                       << "), remaining directories fall back to periodic scans";
        }
        return false;
      }
    }

    int const wd = inotify_add_watch(m_inFd, dir.c_str(), WATCH_MASK);

    if (wd < 0) {
      if (errno == ENOSPC && !m_budgetExhausted) {
        m_budgetExhausted = true;
        flog::warn() << "Out of inotify watches (fs.inotify.max_user_watches), remaining "
                        "directories fall back to periodic scans";
      }
      return false;
    }

    std::scoped_lock const l(m_watchesMtx);
    m_watches.emplace(wd, WatchInfo{dir, depth});
    m_watchByPath.emplace(dir, wd);
    return true;
  }

  void buildWatchSet() {
    std::deque<std::pair<fs::path, int>> queue;
    std::unordered_set<fs::path> visited;
    std::error_code ec;

    for (const auto &root : m_roots) {
      queue.emplace_back(root, 0);
      visited.insert(root);
    }

    while (!queue.empty() && !m_budgetExhausted) {
      auto [dir, depth] = std::move(queue.front());
      queue.pop_front();

      if (!addWatch(dir, depth)) continue;
      if (depth >= MAX_WATCH_DEPTH) continue;

      for (const auto &entry : fs::directory_iterator(dir, ec)) {
        if (!entry.is_directory(ec)) continue;
        if (!m_filter.shouldVisit(entry)) continue;
        if (!visited.insert(entry.path()).second) continue;
        queue.emplace_back(entry.path(), depth + 1);
      }
    }
  }

  void maybeWatchNewDirectory(const fs::path &parent, int parentDepth, const char *name) {
    if (parentDepth >= MAX_WATCH_DEPTH) return;

    std::error_code ec;
    fs::directory_entry const entry{parent / name, ec};

    if (ec || !entry.is_directory(ec) || !m_filter.shouldVisit(entry)) return;

    addWatch(entry.path(), parentDepth + 1);
  }

  void handleEvent(const inotify_event &ev) {
    if (ev.mask & IN_Q_OVERFLOW) {
      m_cb({Event::Kind::Degraded, {}});
      return;
    }

    fs::path dir;
    int depth = 0;
    {
      std::scoped_lock const l(m_watchesMtx);
      auto it = m_watches.find(ev.wd);
      if (it == m_watches.end()) return;
      if (ev.mask & IN_IGNORED) {
        m_watchByPath.erase(it->second.path);
        m_dynamicPaths.erase(it->second.path);
        m_watches.erase(it);
        return;
      }
      dir = it->second.path;
      depth = it->second.depth;
    }

    if ((ev.mask & IN_ISDIR) && (ev.mask & (IN_CREATE | IN_MOVED_TO)) && ev.len > 0) {
      maybeWatchNewDirectory(dir, depth, ev.name);
    }

    m_cb({Event::Kind::DirectoryChanged, std::move(dir)});
  }

  void drainEvents() {
    alignas(inotify_event) char buf[4096];
    ssize_t len = 0;

    while ((len = read(m_inFd, buf, sizeof(buf))) > 0) {
      for (char *p = buf; p < buf + len;) {
        auto *ev = reinterpret_cast<inotify_event *>(p);
        p += sizeof(inotify_event) + ev->len;
        handleEvent(*ev);
      }
    }
  }

  void eventLoop() {
    pollfd fds[2] = {{m_inFd, POLLIN, 0}, {m_wakeFd, POLLIN, 0}};

    while (true) {
      if (poll(fds, 2, -1) < 0 && errno != EINTR) break;
      if (fds[1].revents & POLLIN) break;
      if (fds[0].revents & POLLIN) drainEvents();
    }
  }

public:
  LinuxImportantDirectoryWatcher(Callback cb) : m_cb(std::move(cb)) {
    m_inFd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
    m_wakeFd = eventfd(0, EFD_CLOEXEC);

    if (m_inFd < 0 || m_wakeFd < 0) {
      flog::error() << "Failed to initialize inotify based directory watcher";
      return;
    }

    m_roots = computeRoots();
    buildWatchSet();

    {
      std::scoped_lock const l(m_watchesMtx);
      flog::info() << "Watching " << m_watches.size() << " directories";
    }

    m_thread = std::thread([this] { eventLoop(); });
  }

  ~LinuxImportantDirectoryWatcher() override {
    if (m_thread.joinable()) {
      uint64_t const one = 1;
      std::ignore = write(m_wakeFd, &one, sizeof(one));
      m_thread.join();
    }
    if (m_inFd >= 0) close(m_inFd);
    if (m_wakeFd >= 0) close(m_wakeFd);
  }

  std::vector<fs::path> rootDirectories() const override { return m_roots; }

  void setDynamicDirectories(const std::vector<fs::path> &dirs) override {
    std::unordered_set<fs::path> const desired{dirs.begin(), dirs.end()};
    std::vector<fs::path> toAdd;
    std::vector<int> toRemove;

    {
      std::scoped_lock const l(m_watchesMtx);

      for (const auto &path : m_dynamicPaths) {
        if (!desired.contains(path)) {
          if (auto it = m_watchByPath.find(path); it != m_watchByPath.end()) {
            toRemove.emplace_back(it->second);
          }
        }
      }

      for (const auto &path : desired) {
        if (!m_watchByPath.contains(path)) { toAdd.emplace_back(path); }
      }
    }

    // removal triggers IN_IGNORED, which cleans the maps up on the event thread
    for (int const wd : toRemove) {
      inotify_rm_watch(m_inFd, wd);
    }

    std::error_code ec;

    for (const auto &path : toAdd) {
      if (!fs::is_directory(path, ec)) continue;
      // dynamic watches are leaves: no automatic extension below them
      if (!addWatch(path, MAX_WATCH_DEPTH)) break;

      std::scoped_lock const l(m_watchesMtx);
      m_dynamicPaths.insert(path);
    }
  }

  std::vector<fs::path> watchedDirectories() const override {
    std::scoped_lock const l(m_watchesMtx);
    return m_watches | std::views::values |
           std::views::transform([](const WatchInfo &info) { return info.path; }) |
           std::ranges::to<std::vector>();
  }
};

} // namespace

std::unique_ptr<ImportantDirectoryWatcher> ImportantDirectoryWatcher::create(Callback cb) {
  return std::make_unique<LinuxImportantDirectoryWatcher>(std::move(cb));
}
