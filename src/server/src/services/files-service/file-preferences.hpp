#pragma once
#include <string>
#include <vector>
#include <glaze/core/meta.hpp>
#include <QtGlobal>
#include "utils.hpp"

#ifdef Q_OS_WIN
enum class FileSearchBackend { Auto, WindowsSearch, Everything };

template <> struct glz::meta<FileSearchBackend> {
  using enum FileSearchBackend;
  static constexpr auto value =
      glz::enumerate("auto", Auto, "windows-search", WindowsSearch, "everything", Everything);
};
#endif

struct FilePreferences {
#if defined(Q_OS_LINUX)
  bool autoIndexing = true;
  std::vector<std::string> indexingPaths = {homeDir().string()};
  std::vector<std::string> excludedIndexingPaths;
#elif defined(Q_OS_WIN)
  FileSearchBackend searchBackend = FileSearchBackend::Auto;
  std::string everythingInstance;
#endif
};
