#pragma once
#include <QTimer>
#include <QtQml/qqmlregistration.h>
#include <optional>
#include <string>
#include "builtins/browser/browser-history-model.hpp"
#include "ui/image/image-url.hpp"
#include "ui/views/list-view-host.hpp"

class BrowserHistoryViewHost : public ListViewHost {
  Q_OBJECT
  QML_NAMED_ELEMENT(BrowserHistoryViewHost)
  QML_UNCREATABLE("")
  Q_PROPERTY(QString emptyTitle READ emptyTitle NOTIFY accessChanged)
  Q_PROPERTY(QString emptyDescription READ emptyDescription NOTIFY accessChanged)
  Q_PROPERTY(ImageUrl emptyIcon READ emptyIcon NOTIFY accessChanged)
  Q_PROPERTY(bool detailVisible READ detailVisible NOTIFY detailChanged)
  Q_PROPERTY(QVariantList detailMetadata READ detailMetadata NOTIFY detailChanged)

signals:
  void accessChanged();
  void detailChanged();

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void beforePop() override;

  QString emptyTitle() const;
  QString emptyDescription() const;
  ImageUrl emptyIcon() const;
  bool detailVisible() const { return m_detailVisible && m_selected.has_value(); }
  QVariantList detailMetadata() const;

protected:
  std::unique_ptr<ActionPanelState> emptyActionPanel() override;

private:
  using Access = BrowserExtensionService::PermissionAccess;

  static constexpr std::chrono::milliseconds SEARCH_THROTTLE{100};

  void handleBrowsersChanged();
  void search();
  void reload();
  void toggleDetail();

  BrowserHistorySection m_section;
  QTimer m_searchThrottle;
  bool m_searchPending = false;
  std::string m_query;
  std::string m_searchedQuery;
  std::string m_appliedQuery;
  Access m_access = Access::Granted;
  std::optional<BrowserHistoryItem> m_selected;
  bool m_detailVisible = false;
};
