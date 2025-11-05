#include "watcher-scanner.hpp"
#include "scan.hpp"
#include <QtLogging>
#include <filesystem>

void WatcherScanner::handleMessage(const wtr::event &ev) {
  auto err_case = [ev](const char *str) { return ev.path_name.native().starts_with(str); };

  switch (ev.path_name.native().front()) {
    // TODO: Handle common messages specially

  case 's':
    if (err_case("s/self/live@")) {
      qInfo() << "Creating inotify watchers in" << scan.path.c_str();
      start(scan);
      return;
    }
    break;
  case 'w':
    // TODO
    if (err_case("w/sys/not_watched@")) {
      qCritical()
          << "Ran out of inotify watchers.\n"
          << "    Please increase /proc/sys/fs/inotify/max_user_watches, or set it parmanently:\n"
          << "    `echo fs.inotify.max_user_watches=524288 | sudo tee -a /etc/sysctl.conf && sudo sysctl -p`";
      fail();
      return;
    }
    qWarning() << "Watcher error:" << ev.path_name.c_str();
    break;
  case 'e':
    if (err_case("e/self/die@")) {
      // Follow-up message for prior errors, ignore
      return;
    }
    if (err_case("e/self/live@")) {
      // Failed to start
      qWarning() << "Watcher failed to start: is" << scan.path.c_str() << "a correct path?";
    } else {
      qWarning() << "Fatal Watcher error:" << ev.path_name.c_str();
    }
    fail();
    break;
  }
}

void WatcherScanner::handleEvent(const wtr::event &ev) {
  if (ev.path_type == wtr::event::path_type::watcher) {
    handleMessage(ev);
    return;
  }

  for (const auto &excludedPath : scan.excludedFilenames) {
    auto eventPathStr = ev.path_name.string();
    auto excludedPathStr = excludedPath;
    if (eventPathStr.ends_with(excludedPathStr)) { return; }
  }

  // WHY
  auto toFileTimeType = [](long long t) {
    using namespace std::chrono;
    using namespace std::filesystem;
    return file_time_type::clock::now() + duration_cast<file_time_type::duration>(nanoseconds(t));
  };

  switch (ev.effect_type) {
  case wtr::event::effect_type::create:
  case wtr::event::effect_type::modify:
    m_writer->indexEvents({FileEvent(FileEventType::Modify, ev.path_name, toFileTimeType(ev.effect_time))});
    break;

  case wtr::event::effect_type::destroy:
    m_writer->indexEvents({FileEvent(FileEventType::Delete, ev.path_name, toFileTimeType(ev.effect_time))});
    break;

  case wtr::event::effect_type::rename:
    m_writer->indexEvents({FileEvent(FileEventType::Delete, ev.path_name, toFileTimeType(ev.effect_time))});
    if (ev.associated) { // Sometimes we don't get the associated event, looking more into it
      m_writer->indexEvents({FileEvent(FileEventType::Modify, ev.associated->path_name,
                                       toFileTimeType(ev.associated->effect_time))});
    } else {
      qWarning() << "Got rename event for" << ev.path_name.c_str() << ", but didn't get any associated event";
    }
    break;

  case wtr::event::effect_type::owner:
  case wtr::event::effect_type::other:
    break;
  }
}

WatcherScanner::WatcherScanner(std::shared_ptr<DbWriter> writer, const Scan &scan, FinishCallback callback)
    : AbstractScanner(writer, scan, callback), scan(scan) {
  m_watch = std::make_unique<wtr::watch>(scan.path, [this](const wtr::event &ev) { handleEvent(ev); });
}

void WatcherScanner::interrupt() {
  setInterruptFlag();
  // m_watch->close() can take a long time, so do it in join()
  finish();
}

void WatcherScanner::join() { m_watch->close(); }
