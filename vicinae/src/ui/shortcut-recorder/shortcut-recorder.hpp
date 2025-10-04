#pragma once
#include "layout.hpp"
#include "theme.hpp"
#include "ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.hpp"
#include "ui/keyboard.hpp"
#include "ui/popover/popover.hpp"
#include "ui/typography/typography.hpp"
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ShortcutRecorder : public Popover {
  Q_OBJECT

signals:
  void shortcutChanged(const KeyboardShortcut &shortcut);

public:
  ShortcutRecorder() {
    setWindowFlags(Qt::Popup);
    VStack()
        .center()
        .add(VStack().margins(10).spacing(5).add(m_indicator, 0, Qt::AlignHCenter).add(m_text))
        .imbue(this);
    setFocusPolicy(Qt::StrongFocus);
    m_text->setText("Recording...");
    setFixedSize(250, 80);

    m_text->setAlignment(Qt::AlignHCenter);
    m_indicator->setBackgroundColor(SemanticColor::TertiaryBackground);
    m_indicator->hide();
  }

  void attach(QWidget *widget) {
    auto targetPos = widget->mapToGlobal(QPoint(widget->width() / 2 - width() / 2, -(height() + 10)));
    move(targetPos);
    show();
  }

protected:
  void setError(const QString &text) {
    m_text->setText(text);
    m_text->setColor(SemanticColor::Red);
  }

  void clear() {
    m_text->setText("Recording...");
    m_text->setColor(SemanticColor::TextPrimary);
    m_indicator->hide();
  }

  static bool isModKey(Qt::Key key) { return key >= Qt::Key_Shift && key <= Qt::Key_Alt; }

  void keyPressEvent(QKeyEvent *event) override {
    if (event->key() == Qt::Key_Escape) { return QWidget::keyPressEvent(event); }

    clear();

    KeyboardShortcut shortcut(event);

    qDebug() << event->key();

    if (event->modifiers().toInt() == 0) {
      setError("A modifier is required");
      return;
    }

    m_indicator->setShortcut(shortcut.toModel());
    m_indicator->show();

    if (isModKey(static_cast<Qt::Key>(event->key()))) {
      m_text->setText("Recording...");
    } else {
      close();
      emit shortcutChanged(shortcut);
    }
  }

private:
  KeyboardShortcutIndicatorWidget *m_indicator = new KeyboardShortcutIndicatorWidget;
  TypographyWidget *m_text = new TypographyWidget;
};
