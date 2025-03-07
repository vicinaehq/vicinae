#pragma once
#include <qtmetamacros.h>
#include <qwidget.h>

class OmniListItemWidget : public QWidget {
  Q_OBJECT;

public:
  virtual void selectionChanged(bool selected) { Q_UNUSED(selected); }

  OmniListItemWidget(QWidget *parent = nullptr) : QWidget(parent) {}

signals:
  void clicked();
  void doubleClicked();
};
