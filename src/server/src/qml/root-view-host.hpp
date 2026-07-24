#pragma once
#include "bridge-view.hpp"
#include "ui/action-pannel/action.hpp"
#include <qtimer.h>

class RootSearchModel;

class RootViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  QString initialSearchPlaceholderText() const override { return QStringLiteral("Search for anything..."); }
  bool showBackButton() const override { return false; }

  void initialize() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;
  void beforePop() override;

  QObject *listModel() const;

protected:
  bool inputFilter(QKeyEvent *) override;
  void beforeActionExecuted(const AbstractAction *action) override;
  bool tryAliasFastTrack();
  void scheduleNextClockTick();

private:
  bool m_textChangedByHistory = false;
  std::optional<int> m_historyOffset;
  QTimer *m_clockTimer = new QTimer(this);
  RootSearchModel *m_model = nullptr;
};
