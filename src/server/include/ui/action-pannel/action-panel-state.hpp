#pragma once
#include "common/types.hpp"
#include "lib/keyboard/keyboard.hpp"
#include "ui/action-pannel/action.hpp"
#include <QString>
#include <cstdint>
#include <memory>
#include <numeric>
#include <vector>

struct ActionPanelSectionState {
  QString m_name;
  std::vector<std::shared_ptr<AbstractAction>> m_actions;

  auto actions() const { return m_actions; }
  QString name() const { return m_name; };
  void setName(const QString &text) { m_name = text; }
  void addAction(AbstractAction *action) { m_actions.emplace_back(action); }
};

class ActionPanelState : public NonCopyable {
public:
  enum class ShortcutPreset : std::uint8_t {
    None,
    List,
    Form,
  };

  AbstractAction *primaryAction() const { return m_primary; }

  /**
   * Apply shortcut presets and other things that need to be computed
   * at a given time.
   */
  void finalize() {
    computePrimaryAction();
    applyShortcuts();
  }

  const std::vector<std::unique_ptr<ActionPanelSectionState>> &sections() const { return m_sections; }

  ActionPanelSectionState *createSection(const QString &name = "") {
    auto section = std::make_unique<ActionPanelSectionState>();
    auto handle = section.get();

    section->setName(name);
    m_sections.emplace_back(std::move(section));

    return handle;
  }

  /**
   * The first action will be considered as the primary one, unless another
   * action was explicitly marked as primary.
   */
  void setAutoSelectPrimary(bool value = true) { m_autoSelectPrimary = value; }

  void setTitle(const QString &title) { m_title = title; }
  QString title() const { return m_title; }

  void setId(const QString &id) { m_id = id; }
  QString id() const { return m_id; }

  void setShortcutPreset(ShortcutPreset preset) { m_defaultShortcuts = shortcutsForPreset(preset); }

  ActionPanelState() { setShortcutPreset(ShortcutPreset::None); }

  int actionCount() const {
    auto acc = [](int acc, auto &&cur) { return acc + cur->actions().size(); };

    return std::accumulate(m_sections.begin(), m_sections.end(), 0, acc);
  }

  void setDirty(bool value) { m_dirty = value; }
  bool dirty() const { return m_dirty; }

private:
  void computePrimaryAction() {
    AbstractAction *first = nullptr;

    for (const auto &section : m_sections) {
      for (const auto &action : section->actions()) {
        if (!first) first = action.get();
        if (action->isPrimary()) {
          m_primarySection = section.get();
          m_primary = action.get();
          return;
        }
      }
    }

    if (m_autoSelectPrimary) {
      m_primary = first;
      if (first) { first->setPrimary(true); }
    }

    m_primarySection = !m_sections.empty() ? m_sections.front().get() : nullptr;
  }

  void applyShortcuts() {
    if (!m_primarySection) return;

    auto actions = m_primarySection->actions();

    for (size_t i = 0; i != actions.size() && i != m_defaultShortcuts.size(); ++i) {
      auto &action = actions[i];
      auto &shortcut = m_defaultShortcuts[i];
      auto existing = action->shortcut();

      // always prioritize default shortcut, but still keeps the one
      // that was set before as a secondary shortcut (usually not shown in UI).
      action->setShortcut(shortcut);
      if (existing) { action->addShortcut(*existing); }
    }
  }

  std::vector<Keyboard::Shortcut> shortcutsForPreset(ShortcutPreset preset) {
    switch (preset) {
    case ShortcutPreset::List:
      return {Keyboard::Shortcut::enter(), Keyboard::Shortcut::submit()};
    case ShortcutPreset::Form:
      return {Keyboard::Shortcut::submit()};
    default:
      return {};
    }
  }

  bool m_autoSelectPrimary = true;
  bool m_dirty = true;
  QString m_title;
  QString m_id;
  std::vector<std::unique_ptr<ActionPanelSectionState>> m_sections;
  std::vector<Keyboard::Shortcut> m_defaultShortcuts;
  AbstractAction *m_primary = nullptr;
  ActionPanelSectionState *m_primarySection = nullptr;
};

class ListActionPanelState : public ActionPanelState {
public:
  ListActionPanelState() { setShortcutPreset(ShortcutPreset::List); }
};

class FormActionPanelState : public ActionPanelState {
public:
  FormActionPanelState() { setShortcutPreset(ShortcutPreset::Form); }
};
