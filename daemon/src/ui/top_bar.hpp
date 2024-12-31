#pragma once
#include "extend/image-model.hpp"
#include "ui/input_completer.hpp"
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <qapplication.h>
#include <qboxlayout.h>

struct CompleterData {
  QList<QString> placeholders;
  ImageLikeModel model;
};

struct TopBar : QWidget {
  QLabel *backButtonLabel = nullptr;
  QHBoxLayout *layout;
  QLineEdit *input;
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
