#pragma once
#include "extend/model-parser.hpp"
#include "extension/extension-command-controller.hpp"
#include "qml-bridge-view.hpp"
#include "qml-extension-grid-model.hpp"
#include "qml-extension-list-model.hpp"
#include <QTimer>

class QmlExtensionViewHost : public QmlBridgeViewBase {
  Q_OBJECT
  Q_PROPERTY(QString viewType READ viewType NOTIFY viewTypeChanged)
  Q_PROPERTY(QObject *contentModel READ contentModel NOTIFY contentModelChanged)
  Q_PROPERTY(bool isExtLoading READ isExtLoading NOTIFY isLoadingChanged)

public:
  explicit QmlExtensionViewHost(ExtensionCommandController *controller, QObject *parent = nullptr);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void loadInitialData() override;
  void onReactivated() override;

  void render(const RenderModel &model);

  // BaseView overrides
  void textChanged(const QString &text) override;
  bool inputFilter(QKeyEvent *event) override;
  void beforePop() override;

  // Properties for QML
  QString viewType() const;
  QObject *contentModel() const;
  bool isExtLoading() const;

signals:
  void viewTypeChanged();
  void contentModelChanged();
  void isLoadingChanged();
  void fallbackRequired(const RenderModel &model);

private:
  void handleFirstRender(const RenderModel &model);
  void renderList(const ListModel &model);
  void renderGrid(const GridModel &model);
  void notifyExtension(const QString &handler, const QJsonArray &args);
  void handleDebouncedSearch();

  ExtensionCommandController *m_controller;
  QmlExtensionListModel *m_listModel = nullptr;
  QmlExtensionGridModel *m_gridModel = nullptr;
  QString m_viewType = "loading";
  int m_renderIndex = -1;
  QTimer *m_searchDebounce;
  bool m_isLoading = true;
  bool m_firstRender = true;

  // State from model
  bool m_throttle = false;
  bool m_filtering = false;
  std::optional<std::string> m_onSearchTextChange;
  bool m_shouldResetSelection = false;
};
