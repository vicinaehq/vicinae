#pragma once
#include "filesystem-database.hpp"
#include <fnmatch.h>
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

struct GitIgnorePattern {
  bool negated;
  QString pattern;

  GitIgnorePattern() : negated(false) {}
};

class IgnoreFile {
  QList<GitIgnorePattern> list;

  bool evaluatePattern(const QString &target, const GitIgnorePattern &ignore) {
    size_t cursor = 0;
    auto &pattern = ignore.pattern;
    bool starFlag = false;

    for (const auto &ch : ignore.pattern) {
      if (ch == '*')
        starFlag = true;
      else if (starFlag) {
        while (cursor < target.size() && target.at(cursor) != ch)
          ++cursor;

        if (cursor == target.size())
          return false;

        starFlag = false;
      } else {
        if (target.at(cursor) != ch) {
          return false;
        }

        ++cursor;
      }
    }

    auto matches = cursor == target.size();

    if (ignore.negated)
      matches = !matches;
    if (target.endsWith("node_modules")) {
      qDebug() << target << "VS" << ignore.pattern << matches
               << "negated=" << ignore.negated;
    }

    return matches;
  }

public:
  IgnoreFile() {}
  IgnoreFile(const QString &path) {
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      return;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
      auto line = in.readLine().trimmed();
      size_t idx = 0;

      GitIgnorePattern pattern;

      if (line.isEmpty() || line.startsWith('#'))
        continue;

      if (line.size() > idx && line.at(idx) == '!') {
        pattern.negated = true;
        ++idx;
      }

      if (line.size() > idx && line.at(idx) == '/') {
        pattern.pattern =
            QFileInfo(path).dir().absolutePath() + line.sliced(idx);
      } else {
        pattern.pattern = line.sliced(idx);
      }

      list << pattern;
    }
  }

  void append(const IgnoreFile &file) { list << file.list; }

  bool matches(const QFileInfo &info) {

    if (info.absoluteFilePath().endsWith("api/node_modules")) {
      qDebug() << "compare target against" << list.size() << "patterns";
    }

    for (const auto &pattern : list) {
      QString compared = pattern.pattern.startsWith('/')
                             ? info.absoluteFilePath()
                             : info.fileName();

      if (evaluatePattern(compared, pattern)) {
        return true;
      }
    }

    return false;
  }
};

class IndexManager : public QObject {
  Q_OBJECT

  std::unique_ptr<FilesystemDatabase> db;
  QThread *writerThread;
  IndexWriter *writer;

  QList<FileInfo> currentBatch;

  void indexDirectory(const QString &base) {
    QStack<QString> dirs;

    dirs << base;

    while (!dirs.isEmpty()) {
      QString dirPath = dirs.pop();
      QDir dir(dirPath);
      QFileInfo dirInfo(dirPath);
      QDir parent(dir);

      IgnoreFile ignoreFile;

      while (!parent.isRoot()) {
        QString gitignorePath = parent.absoluteFilePath(".gitignore");
        QFile gitignore(gitignorePath);

        if (gitignore.exists()) {
          ignoreFile.append(gitignorePath);
        }

        parent.cdUp();
      }

      for (auto entry : dir.entryList()) {
        if (entry.startsWith('.'))
          continue;

        QString path = dir.absoluteFilePath(entry);
        QFileInfo info(path);

        if (ignoreFile.matches(info))
          continue;

        if (info.isDir())
          dirs.push_back(path);

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
    qDebug() << "indexed" << info.absoluteFilePath();
  }

  void flushInserts() {
    emit writer->requestWrite(currentBatch);
    currentBatch.clear();
  }

public:
  void indexWork(const QString &path) {
    qDebug() << "index work";
    QFileInfo info(path);

    if (info.isDir()) {
      indexDirectory(path);
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

    if (file.exists()) {
      qDebug() << "Removed " << text;
      file.remove();
    }

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
