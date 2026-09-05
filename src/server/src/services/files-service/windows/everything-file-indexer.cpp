#include <QtConcurrent/QtConcurrentRun>
#include <qstring.h>
#include <optional>
#include <string_view>
#include <common/file-category.hpp>
#include "everything-file-indexer.hpp"
#include "everything-pipe-client.hpp"
#include "everything-window-client.hpp"

namespace {

// Flow Launcher convention, kept for muscle memory. Everything's own `regex:` prefix works too.
constexpr char REGEX_PREFIX = '@';

std::optional<std::wstring> categoryFilter(vicinae::FileCategory category) {
  if (category == vicinae::FileCategory::Directory) { return L"folder:"; }

  auto extensions = vicinae::extensionsForCategory(category);
  if (extensions.empty()) { return std::nullopt; }

  std::wstring filter = L"ext:";

  for (std::string_view ext : extensions) {
    if (filter.back() != L':') { filter += L';'; }
    filter += std::wstring{ext.begin(), ext.end()};
  }

  return filter;
}

vicinae::FileCategory categoryOf(const WinFileCandidate &candidate) {
  return candidate.isDirectory ? vicinae::FileCategory::Directory
                               : vicinae::fileCategoryFor(candidate.path, false);
}

// The query goes to Everything as typed so its search syntax and wildcards keep working. A regex
// covers the whole search text, so the category filter cannot be spliced in and is applied to the
// results instead.
EverythingSearch buildSearch(std::string_view query, const IndexerQueryParams &params) {
  EverythingSearch search{.maxResults = params.limit};

  if (query.starts_with(REGEX_PREFIX)) {
    search.regex = true;
    query.remove_prefix(1);
  }

  search.text =
      QString::fromUtf8(query.data(), static_cast<qsizetype>(query.size())).trimmed().toStdWString();

  if (search.text.empty()) { return search; }

  if (params.category && !search.regex) {
    if (auto filter = categoryFilter(*params.category)) { search.text = *filter + L' ' + search.text; }
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

  std::vector<IndexerFileResult> results;

  results.reserve(candidates.size());

  // Everything already sorted, rank only mirrors that order.
  for (WinFileCandidate &candidate : candidates) {
    auto const category = categoryOf(candidate);

    if (params.category && *params.category != category) { continue; }

    results.emplace_back(IndexerFileResult{.path = std::move(candidate.path),
                                           .rank = static_cast<double>(candidates.size() - results.size()),
                                           .category = category,
                                           .mimeType = std::move(candidate.mimeType)});
  }

  return results;
}

QFuture<std::vector<IndexerFileResult>> EverythingFileIndexer::queryAsync(std::string_view query,
                                                                          const IndexerQueryParams &params) {
  return QtConcurrent::run([this, params, q = std::string(query)]() { return runQuery(q, params); });
}
