#include <qjsonobject.h>
#include "win-file-indexer-selector.hpp"

namespace {

constexpr auto BACKEND_KEY = "searchBackend";
constexpr auto EVERYTHING_INSTANCE_KEY = "everythingInstance";
constexpr auto BACKEND_WINDOWS_SEARCH = "windows-search";
constexpr auto BACKEND_EVERYTHING = "everything";

} // namespace

void WinFileIndexerSelector::preferenceValuesChanged(const QJsonObject &preferences) {
  QString const backend = preferences.value(BACKEND_KEY).toString();

  if (backend == BACKEND_WINDOWS_SEARCH) {
    m_backend = Backend::WindowsSearch;
  } else if (backend == BACKEND_EVERYTHING) {
    m_backend = Backend::Everything;
  } else {
    m_backend = Backend::Auto;
  }

  m_everything.setInstanceName(
      preferences.value(EVERYTHING_INSTANCE_KEY).toString().trimmed().toStdWString());
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

QFuture<std::vector<IndexerFileResult>> WinFileIndexerSelector::queryAsync(std::string_view query,
                                                                           const IndexerQueryParams &params) {
  return active().queryAsync(query, params);
}
