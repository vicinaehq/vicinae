#pragma once
#include <QtQml/qqmlregistration.h>
#include <optional>
#include <string>
#include <vector>
#include "builtins/browser/browser-bookmarks-model.hpp"
#include "ui/image/image-url.hpp"
#include "ui/quick/completion-model.hpp"
#include "ui/views/list-view-host.hpp"

class BrowserBookmarksViewHost : public ListViewHost {
  Q_OBJECT
  QML_NAMED_ELEMENT(BrowserBookmarksViewHost)
  QML_UNCREATABLE("")
  Q_PROPERTY(CompletionModel *folderFilterModel READ folderFilterModel CONSTANT)
  Q_PROPERTY(QString currentFolderFilter READ currentFolderFilter NOTIFY currentFolderFilterChanged)
  Q_PROPERTY(QString emptyTitle READ emptyTitle NOTIFY accessChanged)
  Q_PROPERTY(QString emptyDescription READ emptyDescription NOTIFY accessChanged)
  Q_PROPERTY(ImageUrl emptyIcon READ emptyIcon NOTIFY accessChanged)
  Q_PROPERTY(bool detailVisible READ detailVisible NOTIFY detailChanged)
  Q_PROPERTY(QVariantList detailMetadata READ detailMetadata NOTIFY detailChanged)

signals:
  void currentFolderFilterChanged();
  void accessChanged();
  void detailChanged();

public:
  QUrl qmlComponentUrl() const override;
  QUrl qmlSearchAccessoryUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;

  CompletionModel *folderFilterModel() { return &m_folderFilterModel; }
  QString currentFolderFilter() const;
  QString emptyTitle() const;
  QString emptyDescription() const;
  ImageUrl emptyIcon() const;
  bool detailVisible() const { return m_detailVisible && m_selected.has_value(); }
  QVariantList detailMetadata() const;

  Q_INVOKABLE void setFolderFilter(const QString &id);

protected:
  std::unique_ptr<ActionPanelState> emptyActionPanel() override;

private:
  enum class Access : std::uint8_t { Granted, NeedsPermission, NeedsUpdate, NoBrowser };

  Access computeAccess() const;
  void handleBrowsersChanged();
  void reload();
  void rebuildFolderFilter();
  void applyFolderFilter();
  void toggleDetail();

  BrowserBookmarksSection m_section;
  CompletionModel m_folderFilterModel{this};
  std::vector<BrowserBookmark> m_bookmarks;
  std::optional<std::vector<std::string>> m_folderFilter;
  Access m_access = Access::Granted;
  std::optional<BrowserBookmark> m_selected;
  bool m_detailVisible = false;
};
