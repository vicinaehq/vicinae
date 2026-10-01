#define DBINITCONSTANTS
#include <windows.h>
#include <winsvc.h>
#include <oledb.h>
#include <oledberr.h>
#include <msdasc.h>
#include <wrl/client.h>
#include <QtConcurrent/QtConcurrentRun>
#include <qlogging.h>
#include <qstring.h>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <common/file-category.hpp>
#include "fuzzy/fuzzy-searchable.hpp"
#include "utils/scoped-com.hpp"
#include "services/files-service/windows/win-file-candidates.hpp"
#include "win-file-indexer.hpp"

namespace {

using Microsoft::WRL::ComPtr;

constexpr DBCOUNTITEM ROW_BATCH_SIZE = 64;
constexpr size_t PATH_BUFFER_CHARS = 2048;
constexpr size_t MIME_BUFFER_CHARS = 256;
constexpr const wchar_t *CONNECTION_STRING =
    L"Provider=Search.CollatorDSO;Extended Properties='Application=Windows'";
constexpr int MIN_CANDIDATE_LIMIT = 250;
constexpr int CANDIDATE_LIMIT_MULTIPLIER = 20;

int candidateLimit(int limit) { return std::max(MIN_CANDIDATE_LIMIT, limit * CANDIDATE_LIMIT_MULTIPLIER); }

std::wstring escapeLikeTerm(QStringView word) {
  std::wstring escaped;
  escaped.reserve(word.size());

  for (QChar qc : word) {
    switch (wchar_t const c = qc.unicode(); c) {
    case L'\'':
      escaped += L"''";
      break;
    case L'%':
      escaped += L"[%]";
      break;
    case L'_':
      escaped += L"[_]";
      break;
    case L'[':
      escaped += L"[[]";
      break;
    default:
      escaped += c;
      break;
    }
  }

  return escaped;
}

std::optional<std::wstring> categoryPredicate(vicinae::FileCategory category) {
  using vicinae::FileCategory;

  if (category == FileCategory::Directory) { return L"System.ItemType = 'Directory'"; }

  bool const other = category == FileCategory::Other;
  std::span<const FileCategory> const categories =
      other ? std::span<const FileCategory>{vicinae::EXTENSION_CATEGORIES}
            : std::span<const FileCategory>{&category, 1};
  std::wstring predicate;

  for (FileCategory c : categories) {
    for (std::string_view ext : vicinae::extensionsForCategory(c)) {
      if (!predicate.empty()) { predicate += L" OR "; }
      predicate += L"System.ItemType = '." + std::wstring{ext.begin(), ext.end()} + L"'";
    }
  }

  if (predicate.empty()) { return std::nullopt; }
  if (other) { return L"(System.ItemType <> 'Directory' AND NOT (" + predicate + L"))"; }

  return L"(" + predicate + L")";
}

struct QuerySql {
  std::wstring text;
  bool listing = false;
};

QuerySql buildQuerySql(const std::string &query, const IndexerQueryParams &params) {
  std::wstring predicate;

  for (QStringView word : QStringTokenizer{QString::fromUtf8(query), u' ', Qt::SkipEmptyParts}) {
    if (!predicate.empty()) { predicate += L" AND "; }
    predicate += L"System.FileName LIKE '%" + escapeLikeTerm(word) + L"%'";
  }

  std::optional<std::wstring> const categoryFilter =
      params.category ? categoryPredicate(*params.category) : std::nullopt;
  std::wstring const columns = L" System.ItemPathDisplay, System.MIMEType, System.FileAttributes"
                               L" FROM SystemIndex WHERE SCOPE='file:' AND ";

  if (predicate.empty()) {
    if (!categoryFilter) { return {}; }

    return {.text = L"SELECT TOP " + std::to_wstring(params.limit) + columns + *categoryFilter +
                    L" ORDER BY System.DateModified DESC",
            .listing = true};
  }

  if (categoryFilter) { predicate = L"(" + predicate + L") AND " + *categoryFilter; }

  return {.text = L"SELECT TOP " + std::to_wstring(candidateLimit(params.limit)) + columns + predicate};
}

struct RowBuffer {
  DBSTATUS pathStatus;
  DBLENGTH pathLength;
  wchar_t path[PATH_BUFFER_CHARS];
  DBSTATUS mimeStatus;
  DBLENGTH mimeLength;
  wchar_t mime[MIME_BUFFER_CHARS];
  DBSTATUS attributesStatus;
  DBLENGTH attributesLength;
  ULONG attributes;
};

DBBINDING columnBinding(DBORDINAL ordinal, DBTYPE type, size_t obStatus, size_t obLength, size_t obValue,
                        DBLENGTH valueSize) {
  DBBINDING binding{};

  binding.iOrdinal = ordinal;
  binding.obStatus = obStatus;
  binding.obLength = obLength;
  binding.obValue = obValue;
  binding.dwPart = DBPART_VALUE | DBPART_LENGTH | DBPART_STATUS;
  binding.dwMemOwner = DBMEMOWNER_CLIENTOWNED;
  binding.eParamIO = DBPARAMIO_NOTPARAM;
  binding.cbMaxLen = valueSize;
  binding.wType = type;

  return binding;
}

std::vector<WinFileCandidate> fetchCandidates(const std::wstring &sql, int limit) {
  std::vector<WinFileCandidate> candidates;
  ComPtr<IDataInitialize> dataInit;

  if (HRESULT hr =
          CoCreateInstance(CLSID_MSDAINITIALIZE, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dataInit));
      FAILED(hr)) {
    qWarning() << "WinFileIndexer: failed to create OLE DB initializer" << Qt::hex << hr;
    return candidates;
  }

  ComPtr<IDBInitialize> dbInit;

  if (FAILED(dataInit->GetDataSource(nullptr, CLSCTX_INPROC_SERVER, CONNECTION_STRING, IID_IDBInitialize,
                                     reinterpret_cast<IUnknown **>(dbInit.GetAddressOf()))) ||
      FAILED(dbInit->Initialize())) {
    qWarning() << "WinFileIndexer: failed to connect to the Windows Search catalog";
    return candidates;
  }

  ComPtr<IDBCreateSession> createSession;
  ComPtr<IDBCreateCommand> createCommand;
  ComPtr<ICommandText> commandText;
  ComPtr<IRowset> rowset;

  if (FAILED(dbInit.As(&createSession)) ||
      FAILED(createSession->CreateSession(nullptr, IID_IDBCreateCommand,
                                          reinterpret_cast<IUnknown **>(createCommand.GetAddressOf()))) ||
      FAILED(createCommand->CreateCommand(nullptr, IID_ICommandText,
                                          reinterpret_cast<IUnknown **>(commandText.GetAddressOf()))) ||
      FAILED(commandText->SetCommandText(DBGUID_DEFAULT, sql.c_str()))) {
    qWarning() << "WinFileIndexer: failed to prepare Windows Search query";
    return candidates;
  }

  if (HRESULT hr = commandText->Execute(nullptr, IID_IRowset, nullptr, nullptr,
                                        reinterpret_cast<IUnknown **>(rowset.GetAddressOf()));
      FAILED(hr)) {
    qWarning() << "WinFileIndexer: query execution failed" << Qt::hex << hr;
    return candidates;
  }

  ComPtr<IAccessor> accessor;

  if (FAILED(rowset.As(&accessor))) { return candidates; }

  DBBINDING bindings[] = {
      columnBinding(1, DBTYPE_WSTR, offsetof(RowBuffer, pathStatus), offsetof(RowBuffer, pathLength),
                    offsetof(RowBuffer, path), sizeof(RowBuffer::path)),
      columnBinding(2, DBTYPE_WSTR, offsetof(RowBuffer, mimeStatus), offsetof(RowBuffer, mimeLength),
                    offsetof(RowBuffer, mime), sizeof(RowBuffer::mime)),
      columnBinding(3, DBTYPE_UI4, offsetof(RowBuffer, attributesStatus),
                    offsetof(RowBuffer, attributesLength), offsetof(RowBuffer, attributes),
                    sizeof(RowBuffer::attributes)),
  };
  HACCESSOR hAccessor = DB_NULL_HACCESSOR;

  if (FAILED(accessor->CreateAccessor(DBACCESSOR_ROWDATA, std::size(bindings), bindings, 0, &hAccessor,
                                      nullptr))) {
    qWarning() << "WinFileIndexer: failed to create row accessor";
    return candidates;
  }

  candidates.reserve(limit);

  while (true) {
    HROW rowHandles[ROW_BATCH_SIZE];
    HROW *rows = rowHandles;
    DBCOUNTITEM obtained = 0;

    if (FAILED(rowset->GetNextRows(DB_NULL_HCHAPTER, 0, ROW_BATCH_SIZE, &obtained, &rows)) || obtained == 0) {
      break;
    }

    for (DBCOUNTITEM i = 0; i < obtained; ++i) {
      RowBuffer row{};

      if (FAILED(rowset->GetData(rowHandles[i], hAccessor, &row))) { continue; }
      if (row.pathStatus != DBSTATUS_S_OK) { continue; }

      WinFileCandidate candidate{.path = std::wstring_view{row.path}};

      if (row.mimeStatus == DBSTATUS_S_OK && row.mime[0]) {
        candidate.mimeType = QString::fromWCharArray(row.mime).toStdString();
      }
      if (row.attributesStatus == DBSTATUS_S_OK) {
        candidate.isDirectory = row.attributes & FILE_ATTRIBUTE_DIRECTORY;
      }

      candidates.emplace_back(std::move(candidate));
    }

    rowset->ReleaseRows(obtained, rowHandles, nullptr, nullptr, nullptr);
  }

  accessor->ReleaseAccessor(hAccessor, nullptr);

  return candidates;
}

std::vector<IndexerFileResult> rankCandidates(std::vector<WinFileCandidate> candidates,
                                              const std::string &query, const IndexerQueryParams &params) {
  struct Scored {
    std::filesystem::path path;
    int score = 0;
    vicinae::FileCategory category = vicinae::FileCategory::Other;
    std::optional<std::string> mimeType;
  };

  std::vector<Scored> scored;
  fuzzy::Query const fuzzyQuery{query};

  scored.reserve(candidates.size());

  for (WinFileCandidate &candidate : candidates) {
    auto const category = winFileCategory(candidate);

    if (params.category && *params.category != category) { continue; }

    auto const m = fuzzy::scoreWeighted({{candidate.path.filename().string(), 1.0}}, fuzzyQuery);

    if (m.accepted()) {
      scored.emplace_back(Scored{.path = std::move(candidate.path),
                                 .score = m.score,
                                 .category = category,
                                 .mimeType = std::move(candidate.mimeType)});
    }
  }

  std::ranges::stable_sort(scored, [](const Scored &a, const Scored &b) {
    if (a.score != b.score) { return a.score > b.score; }
    return a.path < b.path;
  });

  std::vector<IndexerFileResult> results;
  size_t const end = std::min(static_cast<size_t>(params.limit), scored.size());

  results.reserve(end);

  for (Scored &item : scored | std::views::take(end)) {
    results.emplace_back(IndexerFileResult{.path = std::move(item.path),
                                           .rank = static_cast<double>(item.score),
                                           .category = item.category,
                                           .mimeType = std::move(item.mimeType)});
  }

  return results;
}

std::vector<IndexerFileResult> runQuery(const std::string &query, const IndexerQueryParams &params) {
  if (params.limit <= 0) { return {}; }

  QuerySql const sql = buildQuerySql(query, params);

  if (sql.text.empty()) { return {}; }

  ScopedCom com(COINIT_MULTITHREADED);

  if (sql.listing) { return orderedWinFileResults(fetchCandidates(sql.text, params.limit), params); }

  return rankCandidates(fetchCandidates(sql.text, candidateLimit(params.limit)), query, params);
}

} // namespace

bool WinFileIndexer::isAvailable() const {
  SC_HANDLE const scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);

  if (!scm) { return false; }

  bool running = false;

  if (SC_HANDLE const service = OpenServiceW(scm, L"WSearch", SERVICE_QUERY_STATUS)) {
    SERVICE_STATUS status{};

    if (QueryServiceStatus(service, &status)) { running = status.dwCurrentState == SERVICE_RUNNING; }
    CloseServiceHandle(service);
  }

  CloseServiceHandle(scm);

  return running;
}

QFuture<std::vector<IndexerFileResult>> WinFileIndexer::queryAsync(std::string_view query,
                                                                   const IndexerQueryParams &params) {
  return QtConcurrent::run([params, q = std::string(query)]() { return runQuery(q, params); });
}
