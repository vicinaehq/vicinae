#pragma once

#include <QtQml/qqmlregistration.h>
#include <string>
#include "services/browser-extension-service.hpp"
#include "ui/quick/completion-model.hpp"
#include "ui/views/bridge-view.hpp"

class BookmarkTabFormViewHost : public FormViewBase {
  Q_OBJECT
  QML_NAMED_ELEMENT(BookmarkTabFormViewHost)
  QML_UNCREATABLE("")

  Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY formChanged)
  Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY formChanged)
  Q_PROPERTY(QString urlError READ urlError NOTIFY errorsChanged)
  Q_PROPERTY(CompletionModel *folderModel READ folderModel CONSTANT)
  Q_PROPERTY(QVariantMap selectedFolder READ selectedFolder NOTIFY folderChanged)

public:
  explicit BookmarkTabFormViewHost(const BrowserExtensionService::BrowserTab &tab);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;

  Q_INVOKABLE void submit();
  Q_INVOKABLE void selectFolder(const QVariantMap &item);

  QString title() const { return m_title; }
  QString url() const { return m_url; }
  QString urlError() const { return m_urlError; }
  CompletionModel *folderModel() { return &m_folderModel; }
  QVariantMap selectedFolder() const { return m_folderModel.itemDataById(m_folderId); }

  void setTitle(const QString &title) {
    if (m_title == title) return;
    m_title = title;
    emit formChanged();
  }

  void setUrl(const QString &url) {
    if (m_url == url) return;
    m_url = url;
    emit formChanged();
  }

signals:
  void formChanged();
  void errorsChanged();
  void folderChanged();

private:
  void refreshFolders();

  std::string m_browserId;
  int m_tabId;
  QString m_title;
  QString m_url;
  QString m_urlError;
  QString m_folderId;
  CompletionModel m_folderModel{this};
};
