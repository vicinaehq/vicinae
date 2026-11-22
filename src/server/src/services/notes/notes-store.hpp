#pragma once

#include <QFuture>
#include <QObject>
#include <QThread>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>
#include "notes-attachments.hpp"
#include "notes-preferences.hpp"

namespace Notes {

struct Note {
  std::string id;
  std::string title;
  std::string markdown;
  std::int64_t createdAt = 0;
  std::int64_t updatedAt = 0;
  std::string path;
  std::string revision;
  bool readOnly = false;
};

struct AttachmentResult {
  Note note;
  std::expected<std::string, std::string> markdown;
};

template <typename T> using Result = std::expected<T, std::string>;

class Store : public QObject {
  Q_OBJECT

signals:
  void changed();
  void removed(const QString &id);

public:
  explicit Store(std::filesystem::path directory, Preferences preferences = {});
  ~Store() override;
  void configure(Preferences preferences);
  static std::string normalizeDirectory(const std::string &path);
  std::vector<std::string> directories() const;
  std::string defaultDirectory() const;
  QFuture<Result<std::vector<Note>>> list();
  QFuture<Result<Note>> load(std::string id);
  QFuture<Result<Note>> save(Note note, bool create);
  QFuture<Result<AttachmentResult>> attach(Note note, bool create, std::vector<Attachment> attachments);
  QFuture<Result<void>> remove(std::string id);

private:
  class Worker;
  std::filesystem::path m_directory;
  Preferences m_preferences;
  QThread m_thread;
  Worker *m_worker;
};

} // namespace Notes
