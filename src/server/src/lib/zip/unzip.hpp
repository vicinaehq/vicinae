#pragma once
#include <QTemporaryFile>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <memory>

struct zip_t;

class ZipedFile {
  zip_t *m_zip;
  std::filesystem::path m_path;
  int m_idx;

public:
  const std::filesystem::path &path() const { return m_path; }
  std::string readAll();

  ZipedFile(zip_t *zip, const std::filesystem::path &path, int idx);
};

class Unzipper {

public:
  struct ExtractOptions {
    std::optional<int> stripComponents;
  };

private:
  zip_t *m_zip = nullptr;
  std::unique_ptr<QTemporaryFile> m_tmpFile;

public:
  operator bool() const { return m_zip != nullptr; }

  void extract(const std::filesystem::path &target, const ExtractOptions &opts = {});
  std::vector<ZipedFile> listFiles();

  Unzipper(std::string_view data);
  Unzipper(const std::filesystem::path &path);

  ~Unzipper();
};
