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
#include <string>
#include <string_view>
#include <vector>
#include <common/file-category.hpp>
#include "fuzzy/fzf.hpp"
#include "utils/scoped-com.hpp"
#include "win-file-indexer.hpp"

namespace {

using Microsoft::WRL::ComPtr;

constexpr int MIN_CANDIDATE_LIMIT = 250;
constexpr int CANDIDATE_LIMIT_MULTIPLIER = 20;
constexpr DBCOUNTITEM ROW_BATCH_SIZE = 64;
constexpr size_t PATH_BUFFER_CHARS = 2048;
constexpr size_t MIME_BUFFER_CHARS = 256;
constexpr const wchar_t *CONNECTION_STRING =
    L"Provider=Search.CollatorDSO;Extended Properties='Application=Windows'";

int candidateLimitFor(int limit) {
  if (limit <= 0) { return 0; }
  return std::max(MIN_CANDIDATE_LIMIT, limit * CANDIDATE_LIMIT_MULTIPLIER);
}

// Quotes are doubled, LIKE wildcards neutralized with bracket classes.
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
  if (category == vicinae::FileCategory::Directory) { return L"System.ItemType = 'Directory'"; }

  auto extensions = vicinae::extensionsForCategory(category);
  if (extensions.empty()) { return std::nullopt; }

  std::wstring predicate;

  for (std::string_view ext : extensions) {
    if (!predicate.empty()) { predicate += L" OR "; }
    predicate += L"System.ItemType = '." + std::wstring{ext.begin(), ext.end()} + L"'";
  }

  return L"(" + predicate + L")";
}

// Candidate generation only, the real ranking is done by the fuzzy matcher.
// SCOPE='file:' keeps non-file items (emails, contacts) out.
std::wstring buildQuerySql(const std::string &query, const IndexerQueryParams &params, int candidateLimit) {
  std::wstring predicate;

  for (QStringView word : QStringTokenizer{QString::fromUtf8(query), u' ', Qt::SkipEmptyParts}) {
    if (!predicate.empty()) { predicate += L" AND "; }
    predicate += L"System.FileName LIKE '%" + escapeLikeTerm(word) + L"%'";
  }

  if (predicate.empty()) { return {}; }

  if (params.category) {
    if (auto categoryFilter = categoryPredicate(*params.category)) {
      predicate = L"(" + predicate + L") AND " + *categoryFilter;
    }
  }

  return L"SELECT TOP " + std::to_wstring(candidateLimit) +
         L" System.ItemPathDisplay, System.MIMEType, System.FileAttributes"
         L" FROM SystemIndex WHERE SCOPE='file:' AND " +
         predicate;
}

struct Candidate {
  std::wstring path;
  std::optional<std::string> mimeType;
  bool isDirectory = false;
};

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

std::vector<Candidate> fetchCandidates(const std::wstring &sql, int candidateLimit) {
  std::vector<Candidate> candidates;
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

  candidates.reserve(candidateLimit);

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

      Candidate candidate{.path = row.path};

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

std::vector<IndexerFileResult> runQuery(const std::string &query, const IndexerQueryParams &params) {
  std::vector<IndexerFileResult> results;
  int const candidateLimit = candidateLimitFor(params.limit);

  if (candidateLimit == 0) { return results; }

  std::wstring const sql = buildQuerySql(query, params, candidateLimit);

  if (sql.empty()) { return results; }

  ScopedCom com(COINIT_MULTITHREADED);

  struct Scored {
    std::filesystem::path path;
    int score = 0;
    vicinae::FileCategory category = vicinae::FileCategory::Other;
    std::optional<std::string> mimeType;
  };

  const auto &matcher = fzf::threadLocalMatcher();
  std::vector<Scored> scored;

  for (Candidate &candidate : fetchCandidates(sql, candidateLimit)) {
    std::filesystem::path path{std::move(candidate.path)};
    auto category =
        candidate.isDirectory ? vicinae::FileCategory::Directory : vicinae::fileCategoryFor(path, false);

    if (params.category && *params.category != category) { continue; }

    int const fuzzyScore = matcher.fuzzy_match_v2_score_query(path.filename().string(), query);

    if (fuzzyScore > 0) {
      scored.emplace_back(Scored{.path = std::move(path),
                                 .score = fuzzyScore,
                                 .category = category,
                                 .mimeType = std::move(candidate.mimeType)});
    }
  }

  std::ranges::stable_sort(scored, [](const Scored &a, const Scored &b) {
    if (a.score != b.score) { return a.score > b.score; }
    return a.path < b.path;
  });

  int const limit = std::max(0, params.limit);
  size_t const end = std::min(static_cast<size_t>(limit), scored.size());

  results.reserve(end);

  for (size_t i = 0; i < end; ++i) {
    results.emplace_back(IndexerFileResult{.path = std::move(scored[i].path),
                                           .rank = static_cast<double>(scored[i].score),
                                           .category = scored[i].category,
                                           .mimeType = std::move(scored[i].mimeType)});
  }

  return results;
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
