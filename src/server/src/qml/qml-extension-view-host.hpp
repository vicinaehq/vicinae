#pragma once
#include "extend/model-parser.hpp"
#include "extension/extension-action-panel-builder.hpp"
#include "extension/extension-command-controller.hpp"
#include "qml-bridge-view.hpp"
#include "qml-extension-form-model.hpp"
#include "qml-extension-grid-model.hpp"
#include "qml-extension-list-model.hpp"
#include <QTimer>

class QmlExtensionViewHost : public QmlBridgeViewBase {
  Q_OBJECT
  Q_PROPERTY(QString viewType READ viewType NOTIFY viewTypeChanged)
  Q_PROPERTY(QObject *contentModel READ contentModel NOTIFY contentModelChanged)
  Q_PROPERTY(bool isExtLoading READ isExtLoading NOTIFY isLoadingChanged)
  Q_PROPERTY(bool selectFirstOnReset READ selectFirstOnReset NOTIFY selectFirstOnResetChanged)
  Q_PROPERTY(QString detailMarkdown READ detailMarkdown NOTIFY detailContentChanged)
  Q_PROPERTY(QVariantList detailMetadata READ detailMetadata NOTIFY detailContentChanged)
  Q_PROPERTY(QObject *formModel READ formModel NOTIFY formModelChanged)
  Q_PROPERTY(bool suppressEmptyView READ suppressEmptyView NOTIFY suppressEmptyViewChanged)
  Q_PROPERTY(QString linkAccessoryText READ linkAccessoryText NOTIFY linkAccessoryChanged)
  Q_PROPERTY(QString linkAccessoryHref READ linkAccessoryHref NOTIFY linkAccessoryChanged)

public:
  explicit QmlExtensionViewHost(ExtensionCommandController *controller, QObject *parent = nullptr);

  QUrl qmlComponentUrl() const override;
  QUrl qmlSearchAccessoryUrl() const override;
  QVariantMap qmlProperties() const override;
  void loadInitialData() override;
  void onReactivated() override;

  void render(const RenderModel &model);

  void textChanged(const QString &text) override;
  bool inputFilter(QKeyEvent *event) override;
  void beforePop() override;

  QString viewType() const;
  QObject *contentModel() const;
  bool isExtLoading() const;
  bool selectFirstOnReset() const { return m_selectFirstOnReset; }
  QString detailMarkdown() const;
  QVariantList detailMetadata() const;
  QObject *formModel() const;
  bool suppressEmptyView() const { return m_isLoading && !m_hasSearchText; }
  QString linkAccessoryText() const;
  QString linkAccessoryHref() const;

signals:
  void selectFirstOnResetChanged();
  void viewTypeChanged();
  void contentModelChanged();
  void isLoadingChanged();
  void detailContentChanged();
  void formModelChanged();
  void suppressEmptyViewChanged();
  void linkAccessoryChanged();

private:
  void handleFirstRender(const RenderModel &model);
  void renderList(const ListModel &model);
  void renderGrid(const GridModel &model);
  void renderDetail(const RootDetailModel &model);
  void renderForm(const FormModel &model);
  void notifyExtension(const QString &handler, const QJsonArray &args);
  void handleDebouncedSearch();

  ExtensionCommandController *m_controller;
  QmlExtensionListModel *m_listModel = nullptr;
  QmlExtensionGridModel *m_gridModel = nullptr;
  QmlExtensionFormModel *m_formModel = nullptr;
  QString m_viewType = "loading";
  int m_renderIndex = -1;
  QTimer *m_searchDebounce;
  bool m_isLoading = true;
  bool m_hasSearchText = false;
  bool m_firstRender = true;

  bool m_throttle = false;
  bool m_filtering = false;
  std::optional<std::string> m_onSearchTextChange;
  bool m_shouldResetSelection = false;
  bool m_selectFirstOnReset = true;

  QString m_detailMarkdown;
  QVariantList m_detailMetadata;
  std::optional<ActionPannelModel> m_detailActions;
  mutable ExtensionActionPanelBuilder::SubmenuCache m_submenuCache;

  std::optional<ActionPannelModel> m_formActions;
  QString m_linkAccessoryText;
  QString m_linkAccessoryHref;
};
