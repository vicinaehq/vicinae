#pragma once
#include "bridge-view.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include <QTimer>
#include <qfuturewatcher.h>
#include <qmimedatabase.h>

class SearchFilesModel;

class SearchFilesViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)
  Q_PROPERTY(bool hasDetail READ hasDetail NOTIFY detailChanged)
  Q_PROPERTY(QString detailName READ detailName NOTIFY detailChanged)
  Q_PROPERTY(QString detailPath READ detailPath NOTIFY detailChanged)
  Q_PROPERTY(QString detailMimeType READ detailMimeType NOTIFY detailChanged)
  Q_PROPERTY(QString detailLastModified READ detailLastModified NOTIFY detailChanged)
  Q_PROPERTY(QString detailImageSource READ detailImageSource NOTIFY detailChanged)
  Q_PROPERTY(QString detailTextContent READ detailTextContent NOTIFY detailChanged)

signals:
  void detailChanged();

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;

  QObject *listModel() const;
  bool hasDetail() const { return m_hasDetail; }
  QString detailName() const { return m_detailName; }
  QString detailPath() const { return m_detailPath; }
  QString detailMimeType() const { return m_detailMimeType; }
  QString detailLastModified() const { return m_detailLastModified; }
  QString detailImageSource() const { return m_detailImageSource; }
  QString detailTextContent() const { return m_detailTextContent; }

private:
  void renderRecentFiles();
  void handleDebounce();
  void handleSearchResults();
  void loadDetail(const std::filesystem::path &path);
  void clearDetail();

  using Watcher = QFutureWatcher<std::vector<IndexerFileResult>>;

  SearchFilesModel *m_model = nullptr;
  QTimer m_debounce;
  Watcher m_pendingResults;
  QString m_lastSearchText;
  QMimeDatabase m_mimeDb;

  bool m_hasDetail = false;
  QString m_detailName;
  QString m_detailPath;
  QString m_detailMimeType;
  QString m_detailLastModified;
  QString m_detailImageSource;
  QString m_detailTextContent;
};
