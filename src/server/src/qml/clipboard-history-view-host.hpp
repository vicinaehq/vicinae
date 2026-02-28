#pragma once
#include "bridge-view.hpp"
#include "services/clipboard/clipboard-db.hpp"
#include <QTemporaryFile>
#include <memory>

class ClipboardHistoryModel;
class ClipboardHistoryController;
class ClipboardService;

class ClipboardHistoryViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)
  Q_PROPERTY(QString itemCountText READ itemCountText NOTIFY itemCountTextChanged)
  Q_PROPERTY(QString clipboardStatusText READ clipboardStatusText NOTIFY clipboardStatusChanged)
  Q_PROPERTY(QString clipboardStatusIcon READ clipboardStatusIcon NOTIFY clipboardStatusChanged)
  Q_PROPERTY(bool canToggleMonitoring READ canToggleMonitoring CONSTANT)
  Q_PROPERTY(int currentKindFilter READ currentKindFilter NOTIFY currentKindFilterChanged)
  Q_PROPERTY(bool hasDetail READ hasDetail NOTIFY detailChanged)
  Q_PROPERTY(bool hasDetailError READ hasDetailError NOTIFY detailChanged)
  Q_PROPERTY(QString detailMimeType READ detailMimeType NOTIFY detailChanged)
  Q_PROPERTY(QString detailTextContent READ detailTextContent NOTIFY detailChanged)
  Q_PROPERTY(QString detailImageSource READ detailImageSource NOTIFY detailChanged)
  Q_PROPERTY(QString detailSize READ detailSize NOTIFY detailChanged)
  Q_PROPERTY(QString detailCopiedAt READ detailCopiedAt NOTIFY detailChanged)
  Q_PROPERTY(QString detailMd5 READ detailMd5 NOTIFY detailChanged)
  Q_PROPERTY(QString detailEncryptionIcon READ detailEncryptionIcon NOTIFY detailChanged)
  Q_PROPERTY(QString detailErrorTitle READ detailErrorTitle NOTIFY detailChanged)
  Q_PROPERTY(QString detailErrorDescription READ detailErrorDescription NOTIFY detailChanged)

public:
  explicit ClipboardHistoryViewHost();

  QUrl qmlComponentUrl() const override;
  QUrl qmlSearchAccessoryUrl() const override;
  QVariantMap qmlProperties() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void initialize() override;
  void onReactivated() override;
  void beforePop() override;

  Q_INVOKABLE void toggleMonitoring();
  Q_INVOKABLE void setKindFilter(int kind);

  QObject *listModel() const;
  QString itemCountText() const { return m_itemCountText; }
  QString clipboardStatusText() const { return m_clipboardStatusText; }
  QString clipboardStatusIcon() const { return m_clipboardStatusIcon; }
  bool canToggleMonitoring() const { return m_canToggleMonitoring; }
  int currentKindFilter() const { return m_currentKindFilter; }
  bool hasDetail() const { return m_hasDetail; }
  bool hasDetailError() const { return m_hasDetailError; }
  QString detailMimeType() const { return m_detailMimeType; }
  QString detailTextContent() const { return m_detailTextContent; }
  QString detailImageSource() const { return m_detailImageSource; }
  QString detailSize() const { return m_detailSize; }
  QString detailCopiedAt() const { return m_detailCopiedAt; }
  QString detailMd5() const { return m_detailMd5; }
  QString detailEncryptionIcon() const { return m_detailEncryptionIcon; }
  QString detailErrorTitle() const { return m_detailErrorTitle; }
  QString detailErrorDescription() const { return m_detailErrorDescription; }

signals:
  void itemCountTextChanged();
  void clipboardStatusChanged();
  void currentKindFilterChanged();
  void detailChanged();

private:
  void handleMonitoringChanged(bool monitoring);
  void handleDataRetrieved(int totalCount);
  void loadDetail(const ClipboardHistoryEntry &entry);
  void clearDetail();
  void saveDropdownFilter(const QString &value);
  std::optional<QString> getSavedDropdownFilter();

  ClipboardHistoryModel *m_model = nullptr;
  ClipboardHistoryController *m_controller = nullptr;
  ClipboardService *m_clipman = nullptr;

  QString m_itemCountText = QStringLiteral("Loading...");
  QString m_clipboardStatusText;
  QString m_clipboardStatusIcon;
  bool m_canToggleMonitoring = false;
  int m_currentKindFilter = 0;

  bool m_hasDetail = false;
  bool m_hasDetailError = false;
  QString m_detailMimeType;
  QString m_detailTextContent;
  QString m_detailImageSource;
  QString m_detailSize;
  QString m_detailCopiedAt;
  QString m_detailMd5;
  QString m_detailEncryptionIcon;
  QString m_detailErrorTitle;
  QString m_detailErrorDescription;

  std::unique_ptr<QTemporaryFile> m_tmpFile;
};
