#include "fff-library.hpp"

extern "C" {
#include <fff.h>
}

#include <QDebug>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

namespace vicinae::fff {

namespace {

struct FffResultDeleter {
  void operator()(::FffResult *r) const noexcept {
    if (r) fff_free_result(r);
  }
};
using FffResultPtr = std::unique_ptr<::FffResult, FffResultDeleter>;

struct FffSearchResultDeleter {
  void operator()(::FffSearchResult *r) const noexcept {
    if (r) fff_free_search_result(r);
  }
};
using FffSearchResultPtr = std::unique_ptr<::FffSearchResult, FffSearchResultDeleter>;

struct FffScanProgressDeleter {
  void operator()(::FffScanProgress *r) const noexcept {
    if (r) fff_free_scan_progress(r);
  }
};
using FffScanProgressPtr = std::unique_ptr<::FffScanProgress, FffScanProgressDeleter>;

std::string takeError(::FffResult *result) {
  if (!result || !result->error) return "unknown fff error";
  return std::string(result->error);
}

const char *cstr(const fs::path &p) { return p.empty() ? "" : p.c_str(); }

} // namespace

FffInstance::FffInstance(void *handle, fs::path basePath)
    : m_handle(handle), m_basePath(std::move(basePath)) {}

FffInstance::FffInstance(FffInstance &&other) noexcept
    : m_handle(other.m_handle), m_basePath(std::move(other.m_basePath)) {
  other.m_handle = nullptr;
}

FffInstance &FffInstance::operator=(FffInstance &&other) noexcept {
  if (this == &other) return *this;
  if (m_handle) fff_destroy(m_handle);
  m_handle = other.m_handle;
  m_basePath = std::move(other.m_basePath);
  other.m_handle = nullptr;
  return *this;
}

FffInstance::~FffInstance() {
  if (m_handle) fff_destroy(m_handle);
}

std::expected<std::unique_ptr<FffInstance>, std::string> FffInstance::create(const Config &config) {
  if (config.basePath.empty()) { return std::unexpected("fff: base_path must not be empty"); }

  FffResultPtr result(fff_create_instance(config.basePath.c_str(), cstr(config.frecencyDbPath),
                                          cstr(config.historyDbPath),
                                          /*use_unsafe_no_lock=*/false, config.enableMmapCache,
                                          config.enableContentIndexing, config.watch, config.aiMode));

  if (!result || !result->success || !result->handle) { return std::unexpected(takeError(result.get())); }

  void *handle = result->handle;
  // Ownership of handle is transferred to us; FffResult itself is freed by
  // the unique_ptr deleter. fff_free_result does *not* free the handle.
  return std::unique_ptr<FffInstance>(new FffInstance(handle, config.basePath));
}

bool FffInstance::waitForScan(std::chrono::milliseconds timeout) const {
  if (!m_handle) return false;

  std::uint64_t const timeoutMs = timeout.count() > 0 ? static_cast<std::uint64_t>(timeout.count())
                                                      : std::numeric_limits<std::uint64_t>::max();

  FffResultPtr result(fff_wait_for_scan(m_handle, timeoutMs));
  if (!result || !result->success) {
    qWarning() << "fff: wait_for_scan failed:" << (result ? result->error : "null result");
    return false;
  }
  return result->int_value == 1;
}

FffInstance::ScanProgress FffInstance::progress() const {
  ScanProgress out{};
  if (!m_handle) return out;

  FffResultPtr result(fff_get_scan_progress(m_handle));
  if (!result || !result->success || !result->handle) {
    if (result && result->error) { qWarning() << "fff: get_scan_progress failed:" << result->error; }
    return out;
  }

  auto *raw = static_cast<::FffScanProgress *>(result->handle);
  FffScanProgressPtr owned(raw);

  out.scannedFilesCount = owned->scanned_files_count;
  out.isScanning = owned->is_scanning;
  out.isWatcherReady = owned->is_watcher_ready;
  out.isWarmupComplete = owned->is_warmup_complete;
  return out;
}

std::vector<IndexerFileResult> FffInstance::search(std::string_view query,
                                                   const SearchOptions &options) const {
  std::vector<IndexerFileResult> out;
  if (!m_handle) return out;

  // fff_search takes C strings; copy into a null-terminated buffer.
  std::string const queryStr(query);

  FffResultPtr result(fff_search(m_handle, queryStr.c_str(), /*current_file=*/"",
                                 static_cast<std::uint32_t>(std::max(0, options.maxThreads)),
                                 static_cast<std::uint32_t>(std::max(0, options.pageIndex)),
                                 static_cast<std::uint32_t>(std::max(0, options.pageSize)),
                                 /*combo_boost_multiplier=*/0, /*min_combo_count=*/0));
  if (!result || !result->success || !result->handle) {
    if (result && result->error) { qWarning() << "fff: search failed:" << result->error; }
    return out;
  }

  auto *rawSearch = static_cast<::FffSearchResult *>(result->handle);
  FffSearchResultPtr owned(rawSearch);
  out.reserve(owned->count);

  for (std::uint32_t i = 0; i < owned->count; ++i) {
    const ::FffFileItem *item = fff_search_result_get_item(owned.get(), i);
    const ::FffScore *score = fff_search_result_get_score(owned.get(), i);
    if (!item) continue;

    const char *relative = fff_file_item_get_relative_path(item);
    if (!relative) continue;

    fs::path absolute = m_basePath / relative;
    double rank = score ? static_cast<double>(score->total) : 0.0;

    out.push_back(IndexerFileResult{.path = std::move(absolute), .rank = rank});
  }

  return out;
}

bool FffInstance::rescan() {
  if (!m_handle) return false;
  FffResultPtr result(fff_scan_files(m_handle));
  if (!result || !result->success) {
    qWarning() << "fff: scan_files failed:" << (result ? result->error : "null result");
    return false;
  }
  return true;
}

bool FffInstance::restartIndex(const fs::path &newBase) {
  if (!m_handle) return false;
  FffResultPtr result(fff_restart_index(m_handle, newBase.c_str()));
  if (!result || !result->success) {
    qWarning() << "fff: restart_index failed:" << (result ? result->error : "null result");
    return false;
  }
  m_basePath = newBase;
  return true;
}

} // namespace vicinae::fff
