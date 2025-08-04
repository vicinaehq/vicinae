#pragma once
#include "argument.hpp"
#include "../image/url.hpp"
#include <qcontainerfwd.h>
#include <qlineedit.h>
#include <qtimer.h>

struct CompleterData {
  QList<QString> placeholders;
  QList<QString> values;
  ImageURL iconUrl;
  ArgumentList arguments;
};

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

    setFrame(false);
    setProperty("search-input", true);
    debounce->setInterval(10);
    debounce->setSingleShot(true);
    connect(debounce, &QTimer::timeout, this, &SearchBar::debounce);
    connect(this, &QLineEdit::textEdited, debounce, [debounce]() { debounce->start(); });
  }

  void setInline(bool isInline) {
    if (isInline) {
      auto fm = fontMetrics();
      QString sizedText = text();

      if (sizedText.isEmpty()) sizedText = placeholderText();

      int width = fm.horizontalAdvance(sizedText) + 10;

      setFixedWidth(width);
      return;
    }

    setFixedWidth(QWIDGETSIZE_MAX);
  }

  void debounce() { emit debouncedTextEdited(text()); }

signals:
  void debouncedTextEdited(const QString &text);
  void pop();
};
