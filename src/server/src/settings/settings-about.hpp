#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include <qwidget.h>

class SettingsAbout : public VerticalScrollArea {
  void setupUI();
  void showEvent(QShowEvent *event) override {
    QWidget::showEvent(event);
    clearFocus();
  }

public:
  SettingsAbout();
};
