#pragma once
#include "omni-icon.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/icon-button.hpp"
#include "ui/input_completer.hpp"
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class SearchBar : public QLineEdit {
  Q_OBJECT

protected:
  bool event(QEvent *event) override {
    if (event->type() == QEvent::KeyPress) {
      auto keyEvent = static_cast<QKeyEvent *>(event);

      if (keyEvent->key() == Qt::Key_Backspace && text().isEmpty()) {
        emit pop();
        return true;
      }
    }

    return QLineEdit::event(event);
  }

public:
  SearchBar(QWidget *parent = nullptr) : QLineEdit(parent) {
    auto debounce = new QTimer(this);

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
  OmniIconUrl iconUrl;
};

struct TopBar : public QWidget {
  IconButton *backButton = nullptr;
  QHBoxLayout *layout;
  SearchBar *input;
  InputCompleter *quickInput = nullptr;
  std::optional<CompleterData> completerData;

public:
  TopBar(QWidget *parent = nullptr);

  bool eventFilter(QObject *obj, QEvent *event) override;
  void showBackButton();
  void hideBackButton();
  void destroyQuicklinkCompleter();
  void activateQuicklinkCompleter(const CompleterData &data);
};
