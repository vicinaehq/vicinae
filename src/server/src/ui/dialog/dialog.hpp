#pragma once
#include <qboxlayout.h>
#include <qwidget.h>

class QVBoxLayout;

class DialogContentWidget : public QWidget {
  Q_OBJECT

public:
  /**
   * Called if the dialog is dismissed by an external source.
   * Typically this happens if the user presses escape while the dialog is shown or
   * if navigation is changed.
   * Dismissing the dialog as a result of `closeRequested` being emitted does NOT call this.
   */
  virtual void interrupted() {}

  DialogContentWidget(QWidget *parent = nullptr);

signals:
  void closeRequested() const;
};

class DialogWidget : public QWidget {
  QVBoxLayout *_layout;
  DialogContentWidget *_content = nullptr;

  void paintEvent(QPaintEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

public:
  void setContent(DialogContentWidget *content);
  void showDialog();

  DialogWidget(QWidget *parent = nullptr);
};
