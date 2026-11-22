#include <QCryptographicHash>
#include <QDateTime>
#include <QDirIterator>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPromise>
#include <QRegularExpression>
#include <QSaveFile>
#include <QTimer>
#include <QUuid>
#include <algorithm>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include "notes-store.hpp"
#include "db/database.hpp"

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

void fail(const QString &message) { throw std::runtime_error(message.toStdString()); }
QByteArray read(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) fail(file.errorString());
  auto bytes = file.readAll();
  if (file.error() != QFileDevice::NoError) fail(file.errorString());
  return bytes;
}
void write(const QString &path, const QByteArray &bytes) {
  QSaveFile file(path);
  if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit())
    fail(file.errorString());
}
std::string revision(const QByteArray &bytes) {
  return QCryptographicHash::hash(bytes, QCryptographicHash::Sha256).toHex().toStdString();
}
bool readOnly(const QFileInfo &file) {
  return file.isSymLink() || !file.isWritable() || !QFileInfo(file.absolutePath()).isWritable();
}
QString filename(const std::string &title) {
  auto name = QString::fromStdString(title).trimmed();
  name.replace(QRegularExpression(QStringLiteral(R"([\x00-\x1f<>:"/\\|?*])")), QStringLiteral("-"));
  while (name.endsWith(u'.') || name.endsWith(u' '))
    name.chop(1);
  // Leave room for collision suffixes within the 255-byte filename limit.
  while (name.toUtf8().size() > 180)
    name.chop(1);
  if (name.isEmpty()) name = Store::tr("Untitled Note");
  const QRegularExpression reserved(QStringLiteral(R"(^(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])($|\.))"),
                                    QRegularExpression::CaseInsensitiveOption);
  if (reserved.match(name).hasMatch()) name.prepend(u'_');
  return name;
}
QString availablePath(const QString &directory, const QString &name) {
  auto path = QDir(directory).filePath(name + QStringLiteral(".md"));
  for (int suffix = 2; QFileInfo::exists(path); ++suffix)
    path = QDir(directory).filePath(QStringLiteral("%1 (%2).md").arg(name).arg(suffix));
  return path;
}
} // namespace

class Store::Worker : public QObject {
public:
  Worker(std::filesystem::path directory, std::vector<std::string> directories, std::string defaultDirectory,
         Store *store)
      : m_directory(std::move(directory)), m_directories(std::move(directories)),
        m_defaultDirectory(std::move(defaultDirectory)), m_store(store) {}

  void initialize() {
    if (m_ready) return;
    std::filesystem::create_directories(m_directory);
    migrate();
    m_watcher = new QFileSystemWatcher(this);
    m_debounce = new QTimer(this);
    m_debounce->setSingleShot(true);
    m_debounce->setInterval(200);
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, m_debounce, qOverload<>(&QTimer::start));
    connect(m_watcher, &QFileSystemWatcher::fileChanged, m_debounce, qOverload<>(&QTimer::start));
    connect(m_debounce, &QTimer::timeout, this, [this] {
      watch();
      QMetaObject::invokeMethod(m_store, &Store::changed, Qt::QueuedConnection);
    });
    m_ready = true;
    watch();
  }

  void configure(std::vector<std::string> directories, std::string defaultDirectory) {
    m_directories = std::move(directories);
    m_defaultDirectory = std::move(defaultDirectory);
    if (m_ready) watch();
  }

  std::vector<QString> paths() const {
    std::vector<QString> result;
    result.reserve(64);
    std::set<QString> seen;
    for (const auto &directory : m_directories) {
      QDirIterator it(QString::fromStdString(directory), QDir::Files | QDir::Readable,
                      QDirIterator::Subdirectories);
      while (it.hasNext()) {
        const auto path = it.next();
        const auto info = it.fileInfo();
        if (info.suffix().compare(QStringLiteral("md"), Qt::CaseInsensitive) != 0) continue;
        const auto relative = QDir(QString::fromStdString(directory)).relativeFilePath(path).split(u'/');
        if (std::ranges::any_of(relative, [](const auto &part) { return part.endsWith(u".assets"); }))
          continue;
        if (seen.emplace(info.canonicalFilePath()).second) result.emplace_back(path);
      }
    }
    return result;
  }

  void watch() {
    std::set<QString> desired;
    for (const auto &directory : m_directories) {
      const auto root = QString::fromStdString(directory);
      // Watch the parent too so a removed directory can be discovered again.
      auto ancestor = QFileInfo(root).absolutePath();
      while (!QFileInfo::exists(ancestor) && ancestor != QFileInfo(ancestor).absolutePath())
        ancestor = QFileInfo(ancestor).absolutePath();
      desired.emplace(ancestor);
      if (!QFileInfo(root).isDir()) continue;
      desired.emplace(QFileInfo(root).canonicalFilePath());
      QDirIterator it(root, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
      while (it.hasNext())
        desired.emplace(it.next());
    }
    for (const auto &path : paths())
      desired.emplace(path);
    for (const auto &[id, path] : m_paths)
      if (QFileInfo::exists(path)) desired.emplace(path);
    const auto current = m_watcher->files() + m_watcher->directories();
    for (const auto &path : current) {
      if (!desired.erase(path)) m_watcher->removePath(path);
    }
    QStringList additions;
    for (const auto &path : desired)
      if (QFileInfo::exists(path)) additions.append(path);
    if (!additions.isEmpty()) m_watcher->addPaths(additions);
  }

  std::vector<Note> list() {
    initialize();
    std::vector<Note> notes;
    const auto files = paths();
    notes.reserve(files.size());
    for (const auto &path : files) {
      auto [it, inserted] = m_ids.try_emplace(path.toStdString());
      if (inserted) {
        it->second = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
        m_paths.emplace(it->second, path);
      }
      try {
        notes.emplace_back(load(it->second));
      } catch (const std::exception &error) { qWarning() << "Read note:" << path << error.what(); }
    }
    std::ranges::sort(notes, [](const Note &a, const Note &b) {
      return a.updatedAt != b.updatedAt ? a.updatedAt > b.updatedAt : a.id < b.id;
    });
    return notes;
  }

  Note load(const std::string &id) {
    initialize();
    const auto path = pathFor(id);
    const auto bytes = read(path);
    const QFileInfo info(path);
    return {id,
            info.completeBaseName().toStdString(),
            bytes.toStdString(),
            info.birthTime().toMSecsSinceEpoch(),
            info.lastModified().toMSecsSinceEpoch(),
            path.toStdString(),
            revision(bytes),
            readOnly(info)};
  }

  Note save(const Note &note, bool create) {
    initialize();
    QString path;
    if (create) {
      if (m_paths.contains(note.id)) fail(Store::tr("This note already exists."));
      const QFileInfo directory(QString::fromStdString(m_defaultDirectory));
      if (!directory.isDir() || !directory.isWritable())
        fail(Store::tr(
            "The default notes directory is unavailable or read-only. Choose another in Settings."));
      path = availablePath(directory.absoluteFilePath(), filename(note.title));
    } else {
      path = pathFor(note.id);
      const QFileInfo info(path);
      if (!info.exists())
        fail(Store::tr("This note was moved or deleted outside Vicinae. Save a copy to keep your changes."));
      if (readOnly(info)) fail(Store::tr("This note is read-only. Save a copy to keep your changes."));
      if (revision(read(path)) != note.revision)
        fail(Store::tr(
            "This note changed outside Vicinae. Save a copy to keep your changes, or reload the file."));
    }
    const auto bytes = QByteArray::fromStdString(note.markdown);
    if (!create && QString::fromStdString(note.title) != QFileInfo(path).completeBaseName()) {
      const auto renamed = availablePath(QFileInfo(path).absolutePath(), filename(note.title));
      if (!QFile::rename(path, renamed)) fail(Store::tr("Could not rename this note."));
      try {
        write(renamed, bytes);
      } catch (...) {
        QFile::rename(renamed, path);
        throw;
      }
      m_ids.erase(path.toStdString());
      path = renamed;
    } else {
      if (create) {
        QFile reservation(path);
        if (!reservation.open(QIODevice::WriteOnly | QIODevice::NewOnly)) fail(reservation.errorString());
        reservation.close();
      }
      try {
        if (create) copyAttachments(note, QFileInfo(path).absolutePath());
        write(path, bytes);
      } catch (...) {
        if (create) QFile::remove(path);
        throw;
      }
    }
    m_paths.insert_or_assign(note.id, path);
    m_ids.insert_or_assign(path.toStdString(), note.id);
    watch();
    return load(note.id);
  }

  void remove(const std::string &id) {
    initialize();
    const auto path = pathFor(id);
    if (readOnly(QFileInfo(path))) fail(Store::tr("This note is read-only."));
    QFile file(path);
    if (file.exists() && !file.remove()) fail(file.errorString());
    m_paths.erase(id);
    m_ids.erase(path.toStdString());
  }

private:
  QString pathFor(const std::string &id) const {
    const auto it = m_paths.find(id);
    if (it == m_paths.end()) fail(Store::tr("This note no longer exists."));
    return it->second;
  }

  void migrate() {
    const auto databasePath = m_directory / "notes.db";
    if (!std::filesystem::exists(databasePath)) return;
    const auto journalPath = QString::fromStdString((m_directory / ".database-import.json").string());
    QJsonObject journal;
    if (QFileInfo::exists(journalPath)) {
      QJsonParseError error;
      const auto document = QJsonDocument::fromJson(read(journalPath), &error);
      if (error.error != QJsonParseError::NoError || !document.isObject())
        fail(Store::tr(
            "Could not read the notes migration record. The original database has been preserved."));
      journal = document.object();
    }
    if (journal.value(QStringLiteral("complete")).toBool()) return;
    auto database = db::Database::open(databasePath);
    if (!database) throw std::runtime_error(database.error());
    auto query = database->prepare("SELECT id, title, markdown, updated_at FROM note ORDER BY id");
    if (!query) throw std::runtime_error(database->lastError());
    const auto destination = QString::fromStdString(m_defaultDirectory);
    if (!QFileInfo(destination).isDir() || !QFileInfo(destination).isWritable())
      fail(Store::tr("Choose an available, writable default directory to import your existing notes."));
    auto imported = journal.value(QStringLiteral("notes")).toObject();
    while (query.step()) {
      const auto id = QString::fromStdString(query.columnText(0));
      auto entry = imported.value(id).toObject();
      if (entry.value(QStringLiteral("complete")).toBool()) continue;
      const auto bytes = QByteArray::fromStdString(query.columnText(2));
      auto path = entry.value(QStringLiteral("path")).toString();
      if (path.isEmpty()) {
        path = availablePath(destination, filename(query.columnText(1)));
        entry.insert(QStringLiteral("path"), path);
        imported.insert(id, entry);
        journal.insert(QStringLiteral("notes"), imported);
        write(journalPath, QJsonDocument(journal).toJson());
      }
      if (QFileInfo::exists(path)) {
        if (read(path) != bytes)
          fail(Store::tr("An imported note was changed before migration finished. The original database has "
                         "been preserved."));
      } else {
        write(path, bytes);
        QFile file(path);
        if (file.open(QIODevice::ReadWrite))
          file.setFileTime(QDateTime::fromMSecsSinceEpoch(query.columnInt64(3)),
                           QFileDevice::FileModificationTime);
      }
      entry.insert(QStringLiteral("complete"), true);
      imported.insert(id, entry);
      journal.insert(QStringLiteral("notes"), imported);
      write(journalPath, QJsonDocument(journal).toJson());
    }
    const auto code = sqlite3_errcode(database->handle());
    if (code != SQLITE_OK && code != SQLITE_DONE) throw std::runtime_error(database->lastError());
    journal.insert(QStringLiteral("complete"), true);
    write(journalPath, QJsonDocument(journal).toJson());
  }

  std::filesystem::path m_directory;
  std::vector<std::string> m_directories;
  std::string m_defaultDirectory;
  Store *m_store;
  QFileSystemWatcher *m_watcher = nullptr;
  QTimer *m_debounce = nullptr;
  std::unordered_map<std::string, QString> m_paths;
  std::unordered_map<std::string, std::string> m_ids;
  bool m_ready = false;
};

std::string Store::normalizeDirectory(const std::string &path) {
  auto value = QString::fromStdString(path);
  if (value == QStringLiteral("~"))
    value = QDir::homePath();
  else if (value.startsWith(QStringLiteral("~/")))
    value.replace(0, 1, QDir::homePath());
  const QFileInfo info(QDir::cleanPath(value));
  return QDir::cleanPath(info.absoluteFilePath()).toStdString();
}

std::vector<std::string> Store::directories() const {
  std::vector<std::string> directories;
  directories.reserve(m_preferences.directories.size() + 1);
  directories.emplace_back(defaultDirectory());
  for (const auto &path : m_preferences.directories) {
    if (path.empty()) continue;
    auto directory = normalizeDirectory(path);
    if (std::ranges::find(directories, directory) == directories.end())
      directories.emplace_back(std::move(directory));
  }
  return directories;
}
std::string Store::defaultDirectory() const {
  return normalizeDirectory(m_preferences.defaultDirectory && !m_preferences.defaultDirectory->empty()
                                ? *m_preferences.defaultDirectory
                                : m_directory.string());
}

Store::Store(std::filesystem::path directory, Preferences preferences)
    : m_directory(std::move(directory)), m_preferences(std::move(preferences)),
      m_worker(new Worker(m_directory, directories(), defaultDirectory(), this)) {
  m_worker->moveToThread(&m_thread);
  connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);
  m_thread.setObjectName(QStringLiteral("NotesStore"));
  m_thread.start();
  // Start watching before the first search.
  list();
}
Store::~Store() {
  QMetaObject::invokeMethod(m_worker, [] { QThread::currentThread()->quit(); }, Qt::QueuedConnection);
  m_thread.wait();
}
void Store::configure(Preferences preferences) {
  m_preferences = std::move(preferences);
  QMetaObject::invokeMethod(m_worker,
                            [this, directories = directories(), defaultDirectory = defaultDirectory()] {
                              m_worker->configure(directories, defaultDirectory);
                              QMetaObject::invokeMethod(this, &Store::changed, Qt::QueuedConnection);
                            });
}
QFuture<Result<std::vector<Note>>> Store::list() {
  return submit(m_worker, [worker = m_worker] { return worker->list(); });
}
QFuture<Result<Note>> Store::load(std::string id) {
  return submit(m_worker, [worker = m_worker, id = std::move(id)] { return worker->load(id); });
}
QFuture<Result<Note>> Store::save(Note note, bool create) {
  return submit(m_worker, [this, note = std::move(note), create] {
    auto saved = m_worker->save(note, create);
    QMetaObject::invokeMethod(this, &Store::changed, Qt::QueuedConnection);
    return saved;
  });
}
QFuture<Result<AttachmentResult>> Store::attach(Note note, bool create, std::vector<Attachment> attachments) {
  return submit(m_worker, [this, note = std::move(note), create, attachments = std::move(attachments)] {
    auto saved = m_worker->save(note, create);
    QMetaObject::invokeMethod(this, &Store::changed, Qt::QueuedConnection);
    try {
      auto markdown = importAttachments(saved, attachments);
      return AttachmentResult{std::move(saved), std::move(markdown)};
    } catch (const std::exception &error) {
      return AttachmentResult{std::move(saved), std::unexpected(std::string(error.what()))};
    }
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
