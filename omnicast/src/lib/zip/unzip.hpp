#pragma once
#include <QtCore>
#include <algorithm>
#include <array>
#include <fstream>
#include <ranges>
#include <qlogging.h>
#include <string>
#include <minizip/unzip.h>
#include <filesystem>
#include <vector>

struct UnzipHandle {
  unzFile file = nullptr;
  unz_global_info info;

public:
  bool positionToIndex(int idx);
};

class ZipedFile {
  UnzipHandle m_handle;
  std::filesystem::path m_path;
  int m_idx;

public:
  const std::filesystem::path path() const { return m_path; }
  std::string readAll();

public:
  ZipedFile(UnzipHandle handle, const std::filesystem::path &path, int idx);
};

class Unzipper {

public:
  struct ExtractOptions {
    std::optional<int> stripComponents;
  };

private:
  UnzipHandle m_handle;
  std::unique_ptr<QTemporaryFile> m_tmpFile;

public:
  operator bool() const { return m_handle.file; }

  void extract(const std::filesystem::path &target, const ExtractOptions &opts = {});
  std::vector<ZipedFile> listFiles();

  // Handling memory reading directly can be quite tricky so we use a temp file instead
  // to keep things simple.
  Unzipper(std::string_view path);
  Unzipper(const std::filesystem::path &path);

  ~Unzipper();
};
