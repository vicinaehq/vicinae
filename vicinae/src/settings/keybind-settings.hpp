#pragma once
#include "keyboard/keybind-manager.hpp"
#include "keyboard/keyboard.hpp"
#include "layout.hpp"
#include "service-registry.hpp"
#include "services/config/config-service.hpp"
#include "services/keybinding/keybinding-service.hpp"
#include "theme.hpp"
#include "ui/form/base-input.hpp"
#include "ui/icon-button/icon-button.hpp"
#include "ui/image/image.hpp"
#include "ui/image/url.hpp"
#include "ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "ui/omni-tree/omni-tree.hpp"
#include "ui/shortcut-recorder/shortcut-recorder.hpp"
#include "ui/typography/typography.hpp"
#include "ui/shortcut-recorder-input/shortcut-recorder-input.hpp"
#include <qcoreevent.h>
#include <qgraphicseffect.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainterpath.h>
#include <qwidget.h>

class KeybindFirstField : public QWidget {
public:
  KeybindFirstField() {
    m_icon->setFixedSize(20, 20);
    HStack().spacing(10).add(m_icon).add(m_name).imbue(this);
  }

  void setKeybind(const KeybindInfo &info) {
    m_name->setText(info.name);
    m_icon->setUrl(ImageURL::builtin(info.icon).setBackgroundTint(SemanticColor::Orange));
  }

private:
  TypographyWidget *m_name = new TypographyWidget;
  ImageWidget *m_icon = new ImageWidget;
};

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
      if (!shortcut.hasMods()) return QString("Modifier required");
      if (auto existing = KeybindManager::instance()->findBoundInfo(shortcut)) {
        return QString("Already bound to \"%1\"").arg(existing->name);
      }
      return QString();
    });

    connect(m_recorder, &ShortcutRecorder::shortcutChanged, this, [this](const Keyboard::Shortcut &shortcut) {
      m_indicator->setShortcut(shortcut);
      KeybindManager::instance()->setKeybind(m_bind, shortcut);
      update();
      m_indicator->show();
    });
    setFixedHeight(25);
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

      m_opacityEffect->setOpacity(underMouse() ? 0.8 : 1);
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
  QGraphicsOpacityEffect *m_opacityEffect = new QGraphicsOpacityEffect;
  KeyboardShortcutIndicatorWidget *m_indicator = new KeyboardShortcutIndicatorWidget;
  // IconButton *m_removeBtn = new IconButton;
  ShortcutRecorder *m_recorder = new ShortcutRecorder(this);
  Keybind m_bind;
};

class KeybindTreeItem : public VirtualTreeItemDelegate {
public:
  KeybindTreeItem(Keybind bind, const KeybindInfo &info) : m_bind(bind), m_info(info) {
    m_shortcut = KeybindManager::instance()->resolve(bind);
  }

  const KeybindInfo &info() const { return m_info; }

  bool expandable() const override { return false; }

  QString id() const override { return m_info.name; }

  QWidget *widgetForColumn(int column) const override {
    switch (column) {
    case 0:
      return new KeybindFirstField;
    case 1:
      return new KeybindField;
    default:
      return nullptr;
    }
  }

  void refreshForColumn(QWidget *widget, int column) const override {
    switch (column) {
    case 0: {
      static_cast<KeybindFirstField *>(widget)->setKeybind(m_info);
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

class KeybindSidePannel {
public:
  KeybindSidePannel() {}
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

  void textChanged(const QString &text) {
    std::vector<std::shared_ptr<VirtualTreeItemDelegate>> rows;

    for (const auto &[id, info] : KeybindManager::instance()->orderedInfoList()) {
      auto matches = text.isEmpty() || info->name.contains(text, Qt::CaseInsensitive) ||
                     info->description.contains(text, Qt::CaseInsensitive);

      if (matches) { rows.emplace_back(std::make_shared<KeybindTreeItem>(id, *info)); }
    }

    m_tree->addRows(rows);
    m_tree->selectFirst();
  }

  bool eventFilter(QObject *sender, QEvent *event) override {
    auto &keybinding = ServiceRegistry::instance()->config()->value().keybinding;

    if (sender == m_input && event->type() == QEvent::KeyPress) {
      auto keyEvent = static_cast<QKeyEvent *>(event);
      auto &keybinding = ServiceRegistry::instance()->config()->value().keybinding;

      if (keyEvent->modifiers() == Qt::ControlModifier) {
        if (KeyBindingService::isUpKey(keyEvent, keybinding)) { return m_tree->selectUp(); }
        if (KeyBindingService::isDownKey(keyEvent, keybinding)) { return m_tree->selectDown(); }
      }

      if (keyEvent->modifiers().toInt() == 0) {
        switch (keyEvent->key()) {
        case Qt::Key_Up:
          return m_tree->selectUp();
        case Qt::Key_Down:
          return m_tree->selectDown();
        case Qt::Key_Return:
          m_tree->activateCurrentSelection();
          return true;
        }
      }
    }

    return QWidget::eventFilter(sender, event);
  }

  void selectionUpdated(VirtualTreeItemDelegate *next, VirtualTreeItemDelegate *prev) {
    if (next) {
      auto treeItem = static_cast<KeybindTreeItem *>(next);
      m_currentIcon->setUrl(
          ImageURL::builtin(treeItem->info().icon).setBackgroundTint(SemanticColor::Orange));
      m_currentName->setText(treeItem->info().name);
      m_currentDescription->setText(treeItem->info().description);
    }

    m_rightPane->setVisible(next);
  }

  void setupUI() {
    auto inputBox = VStack().margins(10).add(m_input).buildWidget();

    inputBox->setFixedHeight(HEIGHT);

    m_leftPane = VStack().add(inputBox).add(m_tree).buildWidget();

    auto rightBox = HStack().margins(10).spacing(10).add(m_currentIcon).add(m_currentName).buildWidget();
    rightBox->setFixedHeight(HEIGHT);

    m_rightPane = VStack()
                      .divided(1)
                      .add(rightBox)
                      .add(VStack()
                               .margins(10)
                               .spacing(10)
                               .add(UI::Text("Description").secondary())
                               .add(m_currentDescription))
                      .addStretch(1)
                      .buildWidget();

    m_currentIcon->setFixedSize(25, 25);
    m_currentDescription->setWordWrap(true);

    m_tree->setColumns({"Name", "Shortcut"});
    m_tree->setAlternateBackgroundColor(SemanticColor::MainHoverBackground);
    m_input->setRightIcon(ImageURL::builtin("magnifying-glass"));
    m_input->installEventFilter(this);

    HStack().divided(1).add(m_leftPane).add(m_rightPane).imbue(this);

    connect(m_input, &BaseInput::textChanged, this, &KeybindSettingsView::textChanged);
    connect(m_tree, &OmniTree::selectionUpdated, this, &KeybindSettingsView::selectionUpdated);

    textChanged("");
  }

private:
  QWidget *m_leftPane = new QWidget;
  QWidget *m_rightPane = new QWidget;
  BaseInput *m_input = new BaseInput;
  OmniTree *m_tree = new OmniTree;

  // side pannel
  ImageWidget *m_currentIcon = new ImageWidget;
  TypographyWidget *m_currentName = new TypographyWidget;
  TypographyWidget *m_currentDescription = new TypographyWidget;

  static constexpr size_t HEIGHT = 60;
};
