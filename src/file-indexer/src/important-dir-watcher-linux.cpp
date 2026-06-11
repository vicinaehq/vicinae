#include "file-indexer/important-dir-watcher.hpp"
#include "file-indexer/log.hpp"
#include "file-indexer/util.hpp"
#include "xdgpp/env/env.hpp"
#include <cstdint>
#include <mutex>
#include <poll.h>
#include <ranges>
#include <sys/eventfd.h>
#include <sys/inotify.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

namespace fs = std::filesystem;

namespace {

class LinuxImportantDirectoryWatcher : public ImportantDirectoryWatcher {
  static constexpr uint32_t WATCH_MASK =
      IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO | IN_DELETE_SELF | IN_ONLYDIR | IN_EXCL_UNLINK;

  Callback m_cb;
  int m_inFd = -1;
  int m_wakeFd = -1;
  mutable std::mutex m_watchesMtx;
  std::unordered_map<int, fs::path> m_watches;
  std::thread m_thread;

  static std::vector<fs::path> importantDirectories() {
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

  void addWatch(const fs::path &dir) {
    int const wd = inotify_add_watch(m_inFd, dir.c_str(), WATCH_MASK);

    if (wd < 0) {
      flog::warn() << "Failed to watch directory" << dir.c_str();
      return;
    }

    std::scoped_lock const l(m_watchesMtx);
    m_watches.emplace(wd, dir);
  }

  void handleEvent(const inotify_event &ev) {
    if (ev.mask & IN_Q_OVERFLOW) {
      m_cb({Event::Kind::Degraded, {}});
      return;
    }

    std::optional<fs::path> dir;
    {
      std::scoped_lock const l(m_watchesMtx);
      auto it = m_watches.find(ev.wd);
      if (it == m_watches.end()) return;
      if (ev.mask & IN_IGNORED) {
        m_watches.erase(it);
        return;
      }
      dir = it->second;
    }

    m_cb({Event::Kind::DirectoryChanged, std::move(*dir)});
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

    for (const auto &dir : importantDirectories()) {
      addWatch(dir);
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

  std::vector<fs::path> watchedDirectories() const override {
    std::scoped_lock const l(m_watchesMtx);
    return m_watches | std::views::values | std::ranges::to<std::vector>();
  }
};

} // namespace

std::unique_ptr<ImportantDirectoryWatcher> ImportantDirectoryWatcher::create(Callback cb) {
  return std::make_unique<LinuxImportantDirectoryWatcher>(std::move(cb));
}
