#include "watcher-scanner.hpp"
#include "abstract-scanner.hpp"
#include "file-indexer-db.hpp"
#include <QDebug>
#include <qlogging.h>

void WatcherScanner::handleMessage(size_t index, const wtr::event &ev) {
  switch (ev.path_name.native().front()) {
    // TODO: Obtain a list of these messages

  case 's':
  case 'w':
    qWarning() << "Watcher error:" << ev.path_name.native();
  case 'e':
    qWarning() << "Fatal Watcher error:" << ev.path_name.native();
    m_db.setScanError(m_watches[index].m_record.id, QString::fromStdString(ev.path_name.native()));
    m_watches[index].m_failed = true;
    break;
  }
}

void WatcherScanner::handleEvent(size_t index, const wtr::event &ev) {
  if (ev.path_type == wtr::event::path_type::watcher) {
    handleMessage(index, ev);
    return;
  }

  switch (ev.effect_type) {
  case wtr::event::effect_type::create:
    m_db.indexFiles({ev.path_name});
    break;

  case wtr::event::effect_type::destroy:
    m_db.deleteIndexedFiles({ev.path_name});
    break;

  case wtr::event::effect_type::rename:
    m_db.deleteIndexedFiles({ev.path_name});
    m_db.indexFiles({ev.associated->path_name});
    break;

  case wtr::event::effect_type::modify:
    m_db.indexFiles({ev.path_name});
    break;

  case wtr::event::effect_type::owner:
  case wtr::event::effect_type::other:
    break;
  }
}

void WatcherScanner::run() {
  AbstractScanner::run();

  while (true) {
    auto expected = awaitScan();

    if (!expected.has_value()) break;

    Scan scan = *expected;

    auto record = m_db.createScan(scan.path, scan.type);

    if (!record) {
      qWarning() << "Not scanning" << scan.path.c_str() << "because scan record creation failed with error"
                 << record.error();
      continue;
    }

    m_db.updateScanStatus(record->id, FileIndexerDatabase::ScanStatus::Started);
    size_t index = m_watches.size();
    m_watches.emplace_back(*record,
                           std::make_unique<wtr::watch>(
                               scan.path, [this, index](const wtr::event &ev) { handleEvent(index, ev); }),
                           false);
  }
}

void WatcherScanner::stop() {
  AbstractScanner::stop();
  for (auto &[record, watch, failed] : m_watches) {
    watch->close();
    if (!failed) { m_db.updateScanStatus(record.id, FileIndexerDatabase::ScanStatus::Finished); }
  }
}
