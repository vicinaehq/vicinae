#include "common.hpp"
#include "layout.hpp"
#include "ui/button/button.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.hpp"
#include "ui/shortcut-button/shortcut-button.hpp"
#include "ui/shortcut-recorder/shortcut-recorder.hpp"
#include <qnamespace.h>

class ShortcutRecorderInput : public JsonFormItemWidget {
public:
  ShortcutRecorderInput() {
    VStack().add(m_button).imbue(this);
    m_button->setLeftAccessory(m_indicator);
    m_button->setText("No shortcut");
    m_button->setColor(ButtonBase::ButtonColor::Transparent);
    m_indicator->hide();

    m_indicator->setBorderColor(Qt::transparent);
    connect(m_button, &ShortcutButton::clicked, this, [this]() { m_recorder->attach(this); });
    connect(m_recorder, &ShortcutRecorder::shortcutChanged, this, [this](auto shortcut) {
      m_indicator->setShortcut(shortcut);
      m_indicator->show();
      m_button->setText("");
    });
  }

  QJsonValue asJsonValue() const override { return ""; }
  void setValueAsJson(const QJsonValue &value) override {}
  FocusNotifier *focusNotifier() const override { return m_notifier; }

private:
  KeyboardShortcutIndicatorWidget *m_indicator = new KeyboardShortcutIndicatorWidget;
  ShortcutRecorder *m_recorder = new ShortcutRecorder(this);
  ButtonWidget *m_button = new ButtonWidget;
  FocusNotifier *m_notifier = new FocusNotifier(this);
};
