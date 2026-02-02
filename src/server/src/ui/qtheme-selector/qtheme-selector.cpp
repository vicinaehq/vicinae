#include "ui/qtheme-selector/qtheme-selector.hpp"
#include <memory>
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
  std::vector<std::shared_ptr<AbstractItem>> items;

  for (const auto &theme : iconThemeDb.themes()) {
    items.emplace_back(std::make_shared<QThemeSelectorItem>(theme.name));
  }

  addSection("", items);
  updateModel();
}
