#pragma once
#include <QObject>

class DialogContentWidget : public QObject {
  Q_OBJECT

public:
  virtual void interrupted() {}

  DialogContentWidget(QObject *parent = nullptr);

signals:
  void closeRequested() const;
};
