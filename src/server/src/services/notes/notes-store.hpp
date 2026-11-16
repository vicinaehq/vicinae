#pragma once

#include <QFuture>
#include <QObject>
#include <QThread>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace Notes {

struct Note {
  std::string id;
  std::string title;
  std::string markdown;
  std::int64_t createdAt = 0;
  std::int64_t updatedAt = 0;
};

template <typename T> using Result = std::expected<T, std::string>;

class Store : public QObject {
  Q_OBJECT

signals:
  void changed();
  void removed(const QString &id);

public:
  explicit Store(std::filesystem::path directory);
  ~Store() override;
  QFuture<Result<std::vector<Note>>> list();
  QFuture<Result<Note>> load(std::string id);
  QFuture<Result<void>> save(Note note, bool create);
  QFuture<Result<void>> remove(std::string id);

private:
  class Worker;
  QThread m_thread;
  Worker *m_worker;
};

} // namespace Notes
