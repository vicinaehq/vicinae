#include <QPromise>
#include <stdexcept>
#include "notes-store.hpp"
#include "db/database.hpp"
#include "utils/migration-manager/migration-manager.hpp"

namespace Notes {
namespace {
template <typename F> auto submit(QObject *worker, F operation) {
  using T = std::invoke_result_t<F>;
  auto promise = std::make_shared<QPromise<Result<T>>>();
  promise->start();
  auto future = promise->future();
  QMetaObject::invokeMethod(
      worker,
      [promise, operation = std::move(operation)]() mutable {
        try {
          if constexpr (std::is_void_v<T>) {
            operation();
            promise->addResult(Result<void>{});
          } else {
            promise->addResult(Result<T>{operation()});
          }
        } catch (const std::exception &error) {
          promise->addResult(Result<T>{std::unexpected(std::string(error.what()))});
        }
        promise->finish();
      },
      Qt::QueuedConnection);
  return future;
}

Note readNote(const db::Statement &statement) {
  return {statement.columnText(0), statement.columnText(1), statement.columnText(2), statement.columnInt64(3),
          statement.columnInt64(4)};
}
} // namespace

class Store::Worker : public QObject {
public:
  explicit Worker(std::filesystem::path directory) : m_directory(std::move(directory)) {}

  void open() {
    if (m_ready) return;
    std::filesystem::create_directories(m_directory);
    auto database = db::Database::open(m_directory / "notes.db");
    if (!database) throw std::runtime_error(database.error());
    m_db = std::move(*database);
    if (!m_db.exec("PRAGMA journal_mode=WAL;")) throw std::runtime_error(m_db.lastError());
    MigrationManager migrations(m_db, "notes");
    migrations.runMigrations();
    m_ready = true;
  }

  void close() { m_db = {}; }

  std::vector<Note> list() {
    open();
    auto query =
        prepare("SELECT id, title, markdown, created_at, updated_at FROM note ORDER BY updated_at DESC, id");
    std::vector<Note> notes;
    notes.reserve(64);
    while (query.step())
      notes.emplace_back(readNote(query));
    return notes;
  }

  Note load(const std::string &id) {
    open();
    auto query = prepare("SELECT id, title, markdown, created_at, updated_at FROM note WHERE id=:id");
    query.bind(":id", id);
    if (!query.step()) throw std::runtime_error("Note could not be loaded");
    return readNote(query);
  }

  void save(const Note &note, bool create) {
    open();
    auto query =
        prepare(create ? "INSERT INTO note(id,title,markdown,created_at,updated_at) "
                         "VALUES(:id,:title,:markdown,:created,:updated)"
                       : "UPDATE note SET title=:title,markdown=:markdown,updated_at=:updated WHERE id=:id");
    query.bind(":id", note.id);
    query.bind(":title", note.title);
    query.bind(":markdown", note.markdown);
    if (create) query.bind(":created", note.createdAt);
    query.bind(":updated", note.updatedAt);
    if (!query.exec()) throw std::runtime_error(m_db.lastError());
    if (m_db.changes() != 1) throw std::runtime_error("Note no longer exists");
  }

  void remove(const std::string &id) {
    open();
    auto query = prepare("DELETE FROM note WHERE id=:id");
    query.bind(":id", id);
    if (!query.exec()) throw std::runtime_error(m_db.lastError());
  }

private:
  db::Statement prepare(std::string_view sql) {
    auto statement = m_db.prepare(sql);
    if (!statement) throw std::runtime_error(m_db.lastError());
    return statement;
  }
  std::filesystem::path m_directory;
  db::Database m_db;
  bool m_ready = false;
};

Store::Store(std::filesystem::path directory) : m_worker(new Worker(std::move(directory))) {
  m_worker->moveToThread(&m_thread);
  connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);
  m_thread.setObjectName(QStringLiteral("NotesStore"));
  m_thread.start();
}

Store::~Store() {
  QMetaObject::invokeMethod(
      m_worker,
      [worker = m_worker] {
        worker->close();
        QThread::currentThread()->quit();
      },
      Qt::QueuedConnection);
  m_thread.wait();
}

QFuture<Result<std::vector<Note>>> Store::list() {
  return submit(m_worker, [worker = m_worker] { return worker->list(); });
}

QFuture<Result<Note>> Store::load(std::string id) {
  return submit(m_worker, [worker = m_worker, id = std::move(id)] { return worker->load(id); });
}

QFuture<Result<void>> Store::save(Note note, bool create) {
  return submit(m_worker, [this, note = std::move(note), create] {
    m_worker->save(note, create);
    QMetaObject::invokeMethod(this, &Store::changed, Qt::QueuedConnection);
  });
}

QFuture<Result<void>> Store::remove(std::string id) {
  return submit(m_worker, [this, id = std::move(id)] {
    m_worker->remove(id);
    QMetaObject::invokeMethod(
        this,
        [this, id] {
          emit removed(QString::fromStdString(id));
          emit changed();
        },
        Qt::QueuedConnection);
  });
}

} // namespace Notes
