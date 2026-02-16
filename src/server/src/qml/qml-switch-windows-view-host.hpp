#pragma once
#include "qml-bridge-view.hpp"
#include <chrono>

class QmlSwitchWindowsModel;

class QmlSwitchWindowsViewHost : public QmlBridgeViewBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;
  void beforePop() override;

  QObject *listModel() const;

private:
  void refreshWindows();

  QmlSwitchWindowsModel *m_model = nullptr;
  std::chrono::time_point<std::chrono::steady_clock> m_lastFetch{};
};
