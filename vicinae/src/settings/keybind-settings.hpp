#pragma once
#include "keyboard/keybind-manager.hpp"
#include "keyboard/keyboard.hpp"
#include "layout.hpp"
#include "theme.hpp"
#include "ui/form/base-input.hpp"
#include "ui/icon-button/icon-button.hpp"
#include "ui/image/url.hpp"
#include "ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "ui/omni-tree/omni-tree.hpp"
#include "ui/shortcut-recorder/shortcut-recorder.hpp"
#include "ui/typography/typography.hpp"
#include "ui/shortcut-recorder-input/shortcut-recorder-input.hpp"
#include <qnamespace.h>
#include <qpainterpath.h>
#include <qwidget.h>

class KeybindField : public QWidget {
public:
  KeybindField() {
    // m_removeBtn->setFixedSize(16, 16);
    // m_removeBtn->layout()->setContentsMargins(0, 0, 0, 0);
    // m_removeBtn->setUrl(ImageURL::builtin("xmark"));
    // m_removeBtn->setBackgroundColor(Qt::transparent);
    m_indicator->setBackgroundColor(Qt::transparent);
    m_indicator->hide();

    HStack().mx(10).add(m_indicator).justifyBetween().imbue(this);

    m_recorder->setValidator([this](const Keyboard::Shortcut &shortcut) {
      if (!shortcut.hasMods()) return "Modifier required";
      if (auto existing = KeybindManager::instance()->isBound(shortcut)) { return "Already bound"; }
      return "";
    });

    connect(m_recorder, &ShortcutRecorder::shortcutChanged, this, [this](const Keyboard::Shortcut &shortcut) {
      m_indicator->setShortcut(shortcut);
      KeybindManager::instance()->setKeybind(m_bind, shortcut);
      update();
      m_indicator->show();
    });
    setFixedHeight(30);
  }

  void setBind(Keybind bind) { m_bind = bind; }

  void setShortcut(const Keyboard::Shortcut &shortcut) {
    m_indicator->setShortcut(shortcut);
    update();
    m_indicator->show();
  }

protected:
  void paintEvent(QPaintEvent *event) override {
    {
      OmniPainter painter(this);
      QPainterPath path;

      painter.setRenderHint(QPainter::RenderHint::Antialiasing);
      painter.setBrush(Qt::NoBrush);
      painter.setThemePen(hasFocus() ? SemanticColor::InputBorderFocus : SemanticColor::Border, 3);
      path.addRoundedRect(rect(), 6, 6);
      painter.setClipPath(path);
      painter.drawPath(path);

      if (!m_indicator->isVisible()) {
        QRect contentRect = rect().marginsRemoved(layout()->contentsMargins());
        painter.setThemePen(SemanticColor::TextSecondary);
        painter.drawText(contentRect, Qt::AlignLeft | Qt::AlignVCenter, "Record shortcut");
      }
    }
  }

  void mousePressEvent(QMouseEvent *event) override { m_recorder->attach(this); }

private:
  KeyboardShortcutIndicatorWidget *m_indicator = new KeyboardShortcutIndicatorWidget;
  // IconButton *m_removeBtn = new IconButton;
  ShortcutRecorder *m_recorder = new ShortcutRecorder;
  Keybind m_bind;
};

class KeybindTreeItem : public VirtualTreeItemDelegate {
public:
  KeybindTreeItem(Keybind bind, const KeybindInfo &info) : m_bind(bind), m_info(info) {
    m_shortcut = KeybindManager::instance()->resolve(bind);
  }

  QString id() const override { return m_info.name; }

  QWidget *widgetForColumn(int column) const override {
    switch (column) {
    case 0:
      return new TypographyWidget;
    case 1:
      return new KeybindField;
    default:
      return nullptr;
    }
  }

  void refreshForColumn(QWidget *widget, int column) const override {
    switch (column) {
    case 0: {
      static_cast<TypographyWidget *>(widget)->setText(m_info.name);
      break;
    }
    case 1: {
      auto w = static_cast<KeybindField *>(widget);
      w->setBind(m_bind);
      if (m_shortcut) { w->setShortcut(m_shortcut.value()); }
      break;
    }
    }
  }

private:
  std::optional<Keyboard::Shortcut> m_shortcut;
  KeybindInfo m_info;
  Keybind m_bind;
};

class KeybindSettingsView : public QWidget {
public:
  KeybindSettingsView() { setupUI(); }

protected:
  void resizeEvent(QResizeEvent *event) override {
    m_leftPane->setFixedWidth(width() * 0.6);
    m_rightPane->setFixedWidth(width() * 0.4);
    QWidget::resizeEvent(event);
  }

  void setupUI() {
    m_leftPane = VStack().add(VStack().margins(10).add(m_input)).add(m_tree).buildWidget();
    m_rightPane = new QWidget;
    m_tree->setColumns({"Name", "Shortcut"});
    m_input->setRightIcon(ImageURL::builtin("magnifying-glass"));

    HStack().divided(1).add(m_leftPane).add(m_rightPane).imbue(this);

    std::vector<std::shared_ptr<VirtualTreeItemDelegate>> rows;

    rows.reserve(100);

    for (const auto &[id, info] : KeybindManager::instance()->list()) {
      rows.emplace_back(std::make_shared<KeybindTreeItem>(id, info));
    }

    m_tree->addRows(rows);
  }

private:
  QWidget *m_leftPane = new QWidget;
  QWidget *m_rightPane = new QWidget;
  BaseInput *m_input = new BaseInput;
  OmniTree *m_tree = new OmniTree;
};
