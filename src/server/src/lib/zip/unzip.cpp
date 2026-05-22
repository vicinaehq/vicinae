#include "unzip.hpp"
#include "utils.hpp"
#include <zip/zip.h>
#include <qlogging.h>
#include <fstream>

ZipedFile::ZipedFile(zip_t *zip, const std::filesystem::path &path, int idx)
    : m_zip(zip), m_path(path), m_idx(idx) {}

std::string ZipedFile::readAll() {
  if (zip_entry_openbyindex(m_zip, m_idx) < 0) {
    qWarning() << "Failed to open zipped file" << m_path.c_str();
    return {};
  }

  auto size = zip_entry_size(m_zip);
  std::string data(static_cast<size_t>(size), '\0');
  zip_entry_noallocread(m_zip, data.data(), data.size());
  zip_entry_close(m_zip);
  return data;
}

void Unzipper::extract(const std::filesystem::path &target, const Unzipper::ExtractOptions &opts) {
  int const sc = opts.stripComponents.value_or(0);

  for (auto &file : listFiles()) {
    std::filesystem::path const path = target / stripPathComponents(file.path(), sc);
    std::filesystem::create_directories(path.parent_path());
    auto data = file.readAll();
    std::ofstream ofs(path, std::ios::binary);
    ofs.write(data.data(), static_cast<std::streamsize>(data.size()));
  }
}

std::vector<ZipedFile> Unzipper::listFiles() {
  if (!m_zip) return {};

  auto total = zip_entries_total(m_zip);
  std::vector<ZipedFile> files;
  files.reserve(static_cast<size_t>(total));

  for (ssize_t i = 0; i < total; ++i) {
    if (zip_entry_openbyindex(m_zip, i) < 0) continue;
    if (!zip_entry_isdir(m_zip)) { files.emplace_back(m_zip, zip_entry_name(m_zip), static_cast<int>(i)); }
    zip_entry_close(m_zip);
  }

  return files;
}

Unzipper::Unzipper(std::string_view data) {
  m_tmpFile = std::make_unique<QTemporaryFile>();
  if (!m_tmpFile->open()) {
    qCritical() << "Failed to open unzip temp file";
    return;
  }

  m_tmpFile->write(data.data(), data.size());
  m_tmpFile->flush();
  m_zip = zip_open(m_tmpFile->filesystemFileName().c_str(), 0, 'r');
}

Unzipper::Unzipper(const std::filesystem::path &path) { m_zip = zip_open(path.c_str(), 0, 'r'); }

Unzipper::~Unzipper() {
  if (m_zip) zip_close(m_zip);
}
