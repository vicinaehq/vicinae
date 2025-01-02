#pragma once
#include "filesystem-database.hpp"
#include <memory>
#include <qdir.h>
#include <qfileinfo.h>
#include <qobject.h>
#include <qthread.h>
#include <qthreadpool.h>
#include <qtmetamacros.h>

struct SearchRequest : public QObject {
  Q_OBJECT
public:
  QString query;

  SearchRequest(const QString &query) : query(query) {}

signals:
  void finished(const QList<FileInfo> &results);
};

class IndexWriter : public QObject {
  Q_OBJECT

  std::unique_ptr<FilesystemDatabase> con;

  void writeBatch(const QList<FileInfo> &batch) {
    if (!con->insert(batch)) {
      qDebug() << "failed to insert batch";
    } else {
      qDebug() << "inserted" << batch.size();
    }
  }

public:
  IndexWriter(const QString &path)
      : con(std::make_unique<FilesystemDatabase>(path, "files-writer")) {
    connect(this, &IndexWriter::requestWrite, this, &IndexWriter::writeBatch);
  }

signals:
  void requestWrite(const QList<FileInfo> &batch);
};

class IndexManager : public QObject {
  Q_OBJECT

  std::unique_ptr<FilesystemDatabase> db;
  QThread *writerThread;
  IndexWriter *writer;

  QList<FileInfo> currentBatch;

  void indexDirectory(const QString &base) {
    QStack<QString> dirs;

    dirs.push_back(base);

    while (!dirs.isEmpty()) {
      QString dirPath = dirs.pop();
      QDir dir(dirPath);
      QFileInfo dirInfo(dirPath);

      auto indexed = db->listIndexedDirectory(dirPath);

      qDebug() << "indexed entries for" << dirPath << indexed.size();
      for (auto entry : dir.entryList()) {
        if (entry.startsWith('.'))
          continue;

        QString path = dir.path() + QDir::separator() + entry;
        QFileInfo info(path);

        if (info.isDir())
          dirs.push_back(path);

        std::optional<IndexedEntry> indexedEntry;

        for (const auto &entry : indexed) {
          if (entry.path == path) {
            indexedEntry = entry;
            break;
          }
        }

        auto currentLastModified = info.lastModified().toSecsSinceEpoch();

        if (indexedEntry && indexedEntry->mtime == currentLastModified) {
          qDebug() << "Skipping" << path << "last modified did not change";
          continue;
        }

        if (info.isFile())
          saveFile(info);
      }

      saveFile(dirInfo);
    }
  }

  void saveFile(const QFileInfo &info) {
    FileType type;

    if (info.isDir())
      type = FileType::Directory;
    else if (info.isFile())
      type = FileType::RegularFile;
    else
      return;

    currentBatch << FileInfo{.name = info.fileName(),
                             .path = info.absoluteFilePath(),
                             .mtime = info.lastModified(),
                             .type = type,
                             .parentPath = info.dir().absolutePath()};
  }

  void flushInserts() {
    emit writer->requestWrite(currentBatch);
    currentBatch.clear();
  }

  void indexFile(const QString &file) {
    QFileInfo f(file);
    FileType type;

    if (f.isDir())
      type = FileType::Directory;
    else if (f.isFile())
      type = FileType::RegularFile;
    else
      return;

    currentBatch << FileInfo{.name = f.fileName(),
                             .path = file,
                             .mtime = f.lastModified(),
                             .type = type};

    if (currentBatch.size() > 1000) {
      flushInserts();
    }

    // qDebug() << "inserted " << file;
  }

public:
  void indexWork(const QString &path) {
    qDebug() << "index work";
    QFileInfo info(path);

    if (info.isDir()) {
      indexDirectory(path);
    } else {
      indexFile(path);
    }

    flushInserts();
  }

  void search(SearchRequest *request) {
    auto results = db->search(request->query);

    emit request->finished(results);
  }

  IndexManager(const QString &dbPath)
      : db(std::make_unique<FilesystemDatabase>(dbPath, "files-reader")),
        writer(new IndexWriter(dbPath)), writerThread(new QThread) {

    writer->moveToThread(writerThread);
    writerThread->start();

    connect(this, &IndexManager::requestIndex, this, &IndexManager::indexWork);
    connect(this, &IndexManager::requestSearch, this, &IndexManager::search);
  }

signals:
  void requestIndex(const QString &path);
  void requestSearch(SearchRequest *request);
};

class IndexerService {

  QThread *managerThread;
  IndexManager *manager;

public:
  IndexerService(const QString &text) : managerThread(new QThread) {
    QFile file(text);

    /*
if (file.exists()) {
  qDebug() << "Removed " << text;
  file.remove();
}
    */

    manager = new IndexManager(text);
    manager->moveToThread(managerThread);
    managerThread->start();
  }

  SearchRequest *search(const QString &query) {
    auto request = new SearchRequest(query);

    emit manager->requestSearch(request);

    return request;
  }

  void index(const QString &path) { emit manager->requestIndex(path); }
};
