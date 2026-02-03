#pragma once
#include "theme.hpp"
#include "ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.hpp"
#include "lib/keyboard/keyboard.hpp"
#include "ui/popover/popover.hpp"
#include "ui/typography/typography.hpp"
#include <qcoreevent.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ShortcutRecorder : public Popover {
  Q_OBJECT

signals:
  void shortcutChanged(const Keyboard::Shortcut &shortcut);

public:
  using Validator = std::function<QString(const Keyboard::Shortcut &shortcut)>;

  ShortcutRecorder(QWidget *parent);

  void attach(QWidget *widget);

  /**
   * Set a validator function that is to be run every time the
   * shortcut is changed. If a non empty string is returned, the error
   * message will be shown by the recorder.
   */
  void setValidator(const Validator &validator);

protected:
  static bool isModKey(Qt::Key key);
  static bool isCloseKey(Qt::Key key);

  void setupUI();
  void setColor(const ColorLike &color);
  void setError(const QString &text);
  void setSuccess();
  void clear();
  void recompute();

  void hideEvent(QHideEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  bool eventFilter(QObject *sender, QEvent *event) override;

private:
  QTimer m_closeTimer;
  KeyboardShortcutIndicatorWidget *m_indicator = new KeyboardShortcutIndicatorWidget;
  TypographyWidget *m_text = new TypographyWidget;
  size_t m_successDismissTimeout = 2000;
  static constexpr const QSize m_defaultSize = {250, 80};
  Validator m_validator;
  QWidget *m_target = nullptr;
};
