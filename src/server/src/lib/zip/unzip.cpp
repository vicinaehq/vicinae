#include "unzip.hpp"
#include "utils.hpp"
#include <qlogging.h>

namespace fs = std::filesystem;

bool UnzipHandle::positionToIndex(int idx) {
  if (info.number_entry <= idx) {
    qCritical() << "Invalid idx" << idx;
    return false;
  }

  unzGoToFirstFile(file);

  for (int i = 0; i != idx; ++i) {
    unzGoToNextFile(file);
  }

  return true;
}

ZipedFile::ZipedFile(UnzipHandle handle, const std::filesystem::path &path, int idx)
    : m_handle(handle), m_path(path), m_idx(idx) {}

std::string ZipedFile::readAll() {
  m_handle.positionToIndex(m_idx);

  std::array<char, 8192> buffer;
  std::string data;
  int bytesRead;

  if (unzOpenCurrentFile(m_handle.file) != UNZ_OK) {
    qWarning() << "Failed to open ziped file" << path();
    return {};
  }

  while ((bytesRead = unzReadCurrentFile(m_handle.file, buffer.data(), buffer.size())) > 0) {
    data += std::string(buffer.data(), bytesRead);
  }

  return data;
}

void Unzipper::extract(const std::filesystem::path &target, const Unzipper::ExtractOptions &opts) {
  int sc = opts.stripComponents.value_or(0);

  for (auto &file : listFiles()) {
    std::filesystem::path path = target / stripPathComponents(file.path(), sc);
    std::filesystem::create_directories(path.parent_path());
    std::ofstream ofs(path);
    auto data = file.readAll();

    ofs.write(data.data(), data.size());
  }
}

std::vector<ZipedFile> Unzipper::listFiles() {
  if (!m_handle.file) return {};

  std::vector<ZipedFile> files;

  files.reserve(m_handle.info.number_entry);
  unzGoToFirstFile(m_handle.file);

  for (int i = 0; i != m_handle.info.number_entry; ++i) {
    unz_file_info fileInfo;
    char filename[256];

    if (unzGetCurrentFileInfo(m_handle.file, &fileInfo, filename, sizeof(filename), nullptr, 0, nullptr, 0) !=
        UNZ_OK) {
      continue;
    }

    files.emplace_back(ZipedFile(m_handle, filename, i));
    unzGoToNextFile(m_handle.file);
  }

  return files;
}

Unzipper::Unzipper(std::string_view path) {
  m_tmpFile = std::make_unique<QTemporaryFile>();
  if (!m_tmpFile->open()) {
    qCritical() << "Failed to open unzip temp file";
    return;
  }

  m_tmpFile->write(path.data(), path.size());
  m_tmpFile->flush();
  m_handle.file = unzOpen(m_tmpFile->filesystemFileName().c_str());
  unzGetGlobalInfo(m_handle.file, &m_handle.info);
}

Unzipper::Unzipper(const std::filesystem::path &path) {
  m_handle.file = unzOpen(path.c_str());
  unzGetGlobalInfo(m_handle.file, &m_handle.info);
}

Unzipper::~Unzipper() {
  if (m_handle.file) unzClose(m_handle.file);
}
