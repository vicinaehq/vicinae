#pragma once

#include "builtins/screenshots/screenshot-grid-model.hpp"
#include "ui/views/bridge-view.hpp"

class ScreenshotsViewHost : public ViewHostBase {
  Q_OBJECT

public:
  QUrl qmlComponentUrl() const override { return qml::componentUrl(u"ScreenshotsView"); }
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void onReactivated() override;
  void textChanged(const QString &text) override;

private:
  ScreenshotGridModel m_model{this};
};
