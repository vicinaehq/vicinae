#include <qwidget.h>

class SettingsAbout : public QWidget {
  void setupUI();
  void showEvent(QShowEvent *event) override {
    QWidget::showEvent(event);
    clearFocus();
  }

public:
  SettingsAbout();
};
