#pragma once
#include "fuzzy-section.hpp"
#include "services/shortcut/shortcut.hpp"
#include <memory>

template <> struct fuzzy::FuzzySearchable<std::shared_ptr<Shortcut>> {
  static int score(const std::shared_ptr<Shortcut> &s, std::string_view query) {
    auto name = s->name().toStdString();
    return fuzzy::scoreWeighted({{name, 1.0}}, query);
  }
};

class ManageShortcutsSection : public FuzzySection<std::shared_ptr<Shortcut>> {
public:
  QString sectionName() const override { return QStringLiteral("Shortcuts ({count})"); }

  void setOnShortcutSelected(std::function<void(const std::shared_ptr<Shortcut> &)> cb) {
    m_onShortcutSelected = std::move(cb);
  }

  void onSelected(int i) override {
    if (m_onShortcutSelected) m_onShortcutSelected(at(i));
  }

protected:
  QString displayTitle(const std::shared_ptr<Shortcut> &item) const override;
  QString displayIconSource(const std::shared_ptr<Shortcut> &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const std::shared_ptr<Shortcut> &item) const override;

private:
  std::function<void(const std::shared_ptr<Shortcut> &)> m_onShortcutSelected;
};
