#include "ui/qtheme-selector/qtheme-selector.hpp"
#include <ranges>
#include <QIcon>
#include "icon-theme-db/icon-theme-db.hpp"
#include "qtheme-selector.hpp"

namespace fs = std::filesystem;

class QThemeSelectorItem : public SelectorInput::AbstractItem {
  QString m_theme;

  QString displayName() const override { return m_theme; }

  QString generateId() const override { return m_theme; }

  AbstractItem *clone() const override { return new QThemeSelectorItem(*this); }

public:
  const QString &theme() const { return m_theme; }

  QThemeSelectorItem(const QString &path) : m_theme(path) {}
};

QThemeSelector::QThemeSelector() {
  IconThemeDatabase iconThemeDb;

  auto items = iconThemeDb.themes() | std::views::transform([](auto &&info) { return info.name; }) |
               std::views::transform([](auto &&path) -> std::shared_ptr<SelectorInput::AbstractItem> {
                 return std::make_shared<QThemeSelectorItem>(path);
               }) |
               std::ranges::to<std::vector>();

  addSection("", items);
  updateModel();
}
