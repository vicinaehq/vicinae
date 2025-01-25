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
#include <qtimer.h>
#include <qtmetamacros.h>

class SearchBar : public QLineEdit {
  Q_OBJECT

public:
  void keyPressEvent(QKeyEvent *event) override {
    if (event->key() == Qt::Key_Backspace && text().isEmpty()) { emit pop(); }

    QLineEdit::keyPressEvent(event);
  }

  SearchBar() {
    auto debounce = new QTimer();

    debounce->setInterval(10);
    debounce->setSingleShot(true);
    connect(debounce, &QTimer::timeout, this, &SearchBar::debounce);
    connect(this, &QLineEdit::textEdited, debounce, [debounce]() { debounce->start(); });
  }

  void debounce() { emit debouncedTextEdited(text()); }

signals:
  void debouncedTextEdited(const QString &text);
  void pop();
};

struct CompleterData {
  QList<QString> placeholders;
  QList<QString> values;
  ImageLikeModel model;
};

struct TopBar : QWidget {
  QLabel *backButtonLabel = nullptr;
  QHBoxLayout *layout;
  SearchBar *input;
  InputCompleter *quickInput = nullptr;
  QWidget *backWidget = nullptr;
  std::optional<CompleterData> completerData;

public:
  TopBar(QWidget *parent = nullptr);

  bool eventFilter(QObject *obj, QEvent *event) override;
  void showBackButton();
  void hideBackButton();
  void destroyQuicklinkCompleter();
  void activateQuicklinkCompleter(const CompleterData &data);
};
