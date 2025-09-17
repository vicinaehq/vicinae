#include "watcher-scanner.hpp"
#include <QtLogging>
#include <filesystem>

void WatcherScanner::handleMessage(const wtr::event &ev) {
  switch (ev.path_name.native().front()) {
    // TODO: Handle common messages specially

  case 's':
    break;
  case 'w':
    // TODO
    if (ev.path_name.native().starts_with("w/sys/not_watched@")) {
      qCritical() << "Watcher for" << ev.path_name.c_str() << ": Ran out of inotify watchers.";
      fail();
      return;
    }
    qWarning() << "Watcher error:" << ev.path_name.c_str();
    break;
  case 'e':
    qWarning() << "Fatal Watcher error:" << ev.path_name.c_str();
    // TODO: Log error
    fail();
    break;
  }
}

void WatcherScanner::handleEvent(const wtr::event &ev) {
  if (ev.path_type == wtr::event::path_type::watcher) {
    handleMessage(ev);
    return;
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
    m_writer->indexEvents({
        FileEvent(FileEventType::Modify, ev.path_name, toFileTimeType(ev.effect_time))
        });
    break;

  case wtr::event::effect_type::destroy:
    m_writer->indexEvents({
        FileEvent(FileEventType::Delete, ev.path_name, toFileTimeType(ev.effect_time))
        });
    break;

  case wtr::event::effect_type::rename:
    m_writer->indexEvents({
        FileEvent(FileEventType::Delete, ev.path_name, toFileTimeType(ev.effect_time)),
        FileEvent(FileEventType::Modify, ev.associated->path_name, toFileTimeType(ev.associated->effect_time))
        });
    break;

  case wtr::event::effect_type::owner:
  case wtr::event::effect_type::other:
    break;
  }
}

WatcherScanner::WatcherScanner(std::shared_ptr<DbWriter> writer, const Scan& scan, FinishCallback callback):
  AbstractScanner(writer, scan, callback) {

  qInfo() << "Creating inotify watchers in" << scan.path.c_str();
  start(scan);

  m_watch = std::make_unique<wtr::watch>(scan.path, [this](const wtr::event& ev) {
      handleEvent(ev);
      });
}

void WatcherScanner::interrupt() {
  setInterruptFlag();
  m_watch->close();
  finish();
}

void WatcherScanner::join() {
  // Everything was set up in `interrupt()`
}
