#pragma once
#include "list-view-host.hpp"
#include "search-files-model.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include <QTimer>
#include <qfuturewatcher.h>
#include <qmimedatabase.h>

class SearchFilesViewHost : public ListViewHost {
  Q_OBJECT
  Q_PROPERTY(bool hasDetail READ hasDetail NOTIFY detailChanged)
  Q_PROPERTY(QString detailName READ detailName NOTIFY detailChanged)
  Q_PROPERTY(QString detailPath READ detailPath NOTIFY detailChanged)
  Q_PROPERTY(QString detailMimeType READ detailMimeType NOTIFY detailChanged)
  Q_PROPERTY(QString detailLastModified READ detailLastModified NOTIFY detailChanged)
  Q_PROPERTY(QString detailImageSource READ detailImageSource NOTIFY detailChanged)
  Q_PROPERTY(QString detailTextContent READ detailTextContent NOTIFY detailChanged)

  Q_PROPERTY(bool isIndexing READ isIndexing NOTIFY indexingStateChanged)
  Q_PROPERTY(bool showReadyPulse READ showReadyPulse NOTIFY indexingStateChanged)
  Q_PROPERTY(quint64 indexedFilesCount READ indexedFilesCount NOTIFY indexingStateChanged)
  Q_PROPERTY(QString emptyTitle READ emptyTitle NOTIFY indexingStateChanged)
  Q_PROPERTY(QString emptyDescription READ emptyDescription NOTIFY indexingStateChanged)

signals:
  void detailChanged();
  void indexingStateChanged();

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;

  bool hasDetail() const { return m_hasDetail; }
  QString detailName() const { return m_detailName; }
  QString detailPath() const { return m_detailPath; }
  QString detailMimeType() const { return m_detailMimeType; }
  QString detailLastModified() const { return m_detailLastModified; }
  QString detailImageSource() const { return m_detailImageSource; }
  QString detailTextContent() const { return m_detailTextContent; }

  bool isIndexing() const { return m_isIndexing; }
  bool showReadyPulse() const { return m_showReadyPulse; }
  quint64 indexedFilesCount() const { return m_indexedFilesCount; }
  QString emptyTitle() const;
  QString emptyDescription() const;

private:
  void renderRecentFiles();
  void clearSection();
  void handleDebounce();
  void handleSearchResults();
  void handleScanState(quint64 scanned, bool scanning, bool ready);
  void loadDetail(const std::filesystem::path &path);
  void clearDetail();

  using Watcher = QFutureWatcher<std::vector<IndexerFileResult>>;

  SearchFilesSection m_section;
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

  bool m_isIndexing = false;
  bool m_indexReady = false;
  // Brief pulse window after the scan flips to ready, used to render a
  // "Ready — N files indexed" message before recents take over.
  bool m_showReadyPulse = false;
  quint64 m_indexedFilesCount = 0;
  quint64 m_readyAnnounceCount = 0;
  QTimer m_readyPulseTimer;
};
