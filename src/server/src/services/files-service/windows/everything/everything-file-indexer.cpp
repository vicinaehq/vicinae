#include <QtConcurrent/QtConcurrentRun>
#include <qstring.h>
#include <optional>
#include <span>
#include <string_view>
#include <common/file-category.hpp>
#include "everything-file-indexer.hpp"
#include "everything-pipe-client.hpp"
#include "everything-window-client.hpp"

namespace {

constexpr char REGEX_PREFIX = '@';

std::optional<std::wstring> categoryFilter(vicinae::FileCategory category) {
  using vicinae::FileCategory;

  if (category == FileCategory::Directory) { return L"folder:"; }

  bool const other = category == FileCategory::Other;
  std::span<const FileCategory> const categories =
      other ? std::span<const FileCategory>{vicinae::EXTENSION_CATEGORIES}
            : std::span<const FileCategory>{&category, 1};
  std::wstring filter = other ? L"file: !ext:" : L"ext:";
  bool first = true;

  for (FileCategory c : categories) {
    for (std::string_view ext : vicinae::extensionsForCategory(c)) {
      if (!first) { filter += L';'; }
      first = false;
      filter += std::wstring{ext.begin(), ext.end()};
    }
  }

  if (first) { return std::nullopt; }

  return filter;
}

EverythingSearch buildSearch(std::string_view query, const IndexerQueryParams &params) {
  EverythingSearch search{.maxResults = params.limit};

  if (query.starts_with(REGEX_PREFIX)) {
    search.regex = true;
    query.remove_prefix(1);
  }

  search.text =
      QString::fromUtf8(query.data(), static_cast<qsizetype>(query.size())).trimmed().toStdWString();

  if (search.text.empty()) { search.regex = false; }

  if (params.category && !search.regex) {
    if (auto filter = categoryFilter(*params.category)) {
      search.text = search.text.empty() ? *filter : *filter + L' ' + search.text;
    }
  }

  return search;
}

} // namespace

std::unique_ptr<EverythingClient> connectEverythingClient(const std::wstring &instanceName) {
  if (auto pipe = EverythingPipeClient::connect(instanceName)) { return pipe; }
  return EverythingWindowClient::connect();
}

bool EverythingFileIndexer::ensureClient() const {
  if (m_client && m_client->isConnected()) { return true; }

  m_client = connectEverythingClient(m_instanceName);

  return m_client != nullptr;
}

bool EverythingFileIndexer::isAvailable() const {
  std::scoped_lock lock(m_mutex);
  return ensureClient();
}

void EverythingFileIndexer::setInstanceName(std::wstring name) {
  std::scoped_lock lock(m_mutex);

  if (name == m_instanceName) { return; }

  m_instanceName = std::move(name);
  m_client.reset();
}

std::vector<IndexerFileResult> EverythingFileIndexer::runQuery(const std::string &query,
                                                               const IndexerQueryParams &params) {
  if (params.limit <= 0) { return {}; }

  EverythingSearch const search = buildSearch(query, params);

  if (search.text.empty()) { return {}; }

  std::vector<WinFileCandidate> candidates;

  {
    std::scoped_lock lock(m_mutex);

    if (!ensureClient()) { return {}; }

    auto found = m_client->search(search);

    if (!found) {
      m_client.reset();
      return {};
    }

    candidates = std::move(*found);
  }

  return orderedWinFileResults(std::move(candidates), params);
}

QFuture<std::vector<IndexerFileResult>> EverythingFileIndexer::queryAsync(std::string_view query,
                                                                          const IndexerQueryParams &params) {
  return QtConcurrent::run([this, params, q = std::string(query)]() { return runQuery(q, params); });
}
