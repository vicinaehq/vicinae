#pragma once
#include "ui/views/bridge-view.hpp"
#include "ui/action-panel/action.hpp"
#include <qtimer.h>

class RootSearchModel;
class SectionListModel;

class RootViewHost : public ViewHostBase {
  Q_OBJECT

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  QString initialSearchPlaceholderText() const override { return tr("Search for anything..."); }
  bool showBackButton() const override { return false; }

  void initialize() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;
  void beforePop() override;

  SectionListModel *listModel() const override;

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
