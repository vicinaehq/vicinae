#pragma once
#include <qboxlayout.h>
#include <qwidget.h>

class QVBoxLayout;

class DialogContentWidget : public QWidget {
  Q_OBJECT

public:
  DialogContentWidget(QWidget *parent = nullptr);

signals:
  void closeRequested() const;
};

class DialogWidget : public QWidget {
  QVBoxLayout *_layout;
  QWidget *_content;

  void paintEvent(QPaintEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

public:
  void setContent(DialogContentWidget *content);
  void showDialog();

  DialogWidget(QWidget *parent = nullptr);
};
