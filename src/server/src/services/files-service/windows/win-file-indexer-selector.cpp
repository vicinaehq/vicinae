#include "win-file-indexer-selector.hpp"

void WinFileIndexerSelector::preferencesChanged(const FilePreferences &preferences) {
  switch (preferences.searchBackend) {
  case FileSearchBackend::WindowsSearch:
    m_backend = Backend::WindowsSearch;
    break;
  case FileSearchBackend::Everything:
    m_backend = Backend::Everything;
    break;
  case FileSearchBackend::Auto:
    m_backend = Backend::Auto;
    break;
  }

  m_everything.setInstanceName(
      QString::fromStdString(preferences.everythingInstance).trimmed().toStdWString());
}

AbstractFileIndexer &WinFileIndexerSelector::active() {
  switch (m_backend) {
  case Backend::WindowsSearch:
    return m_windowsSearch;
  case Backend::Everything:
    return m_everything;
  case Backend::Auto:
    break;
  }

  if (m_everything.isAvailable()) { return m_everything; }
  return m_windowsSearch;
}

const AbstractFileIndexer &WinFileIndexerSelector::active() const {
  switch (m_backend) {
  case Backend::WindowsSearch:
    return m_windowsSearch;
  case Backend::Everything:
    return m_everything;
  case Backend::Auto:
    break;
  }

  if (m_everything.isAvailable()) { return m_everything; }
  return m_windowsSearch;
}

bool WinFileIndexerSelector::isAvailable() const { return active().isAvailable(); }

std::chrono::milliseconds WinFileIndexerSelector::queryDebounce() const { return active().queryDebounce(); }

QFuture<std::vector<IndexerFileResult>> WinFileIndexerSelector::queryAsync(std::string_view query,
                                                                           const IndexerQueryParams &params) {
  return active().queryAsync(query, params);
}
