#pragma once
#include "extend/image-model.hpp"
#include "ui/input_completer.hpp"
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qtmetamacros.h>

class SearchBar : public QLineEdit {
  Q_OBJECT

public:
  void keyPressEvent(QKeyEvent *event) override {
    if (event->key() == Qt::Key_Backspace && text().isEmpty()) { emit pop(); }

    QLineEdit::keyPressEvent(event);
  }

signals:
  void pop();
};

struct CompleterData {
  QList<QString> placeholders;
  ImageLikeModel model;
};

struct TopBar : QWidget {
  QLabel *backButtonLabel = nullptr;
  QHBoxLayout *layout;
  SearchBar *input;
  InputCompleter *quickInput = nullptr;
  QWidget *backWidget = nullptr;

public:
  TopBar(QWidget *parent = nullptr);

  bool eventFilter(QObject *obj, QEvent *event) override;
  void showBackButton();
  void hideBackButton();
  void destroyQuicklinkCompleter();
  void activateQuicklinkCompleter(const CompleterData &data);
};
