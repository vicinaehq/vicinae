#pragma once
#include "common.hpp"
#include "omni-icon.hpp"
#include "root-item-manager.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "ui/form/checkbox.hpp"
#include "ui/image/omnimg.hpp"
#include "qheaderview.h"
#include "ui/omni-scroll-bar.hpp"
#include "ui/omni-tree.hpp"
#include "ui/typography.hpp"
#include <libqalculate/Number.h>
#include <memory>
#include <qabstractitemmodel.h>
#include <qboxlayout.h>
#include <qdnslookup.h>
#include <qnamespace.h>
#include <qtreeview.h>
#include <qtreewidget.h>
#include <qlineedit.h>
#include <qwidget.h>
#include <qtreeview.h>
#include <ranges>

class ExtensionSettingRenderable {
public:
  virtual QString title() const = 0;
  virtual OmniIcon icon() const = 0;
  virtual ExtensionSettingRenderable *parent() const = 0;
  virtual std::vector<std::shared_ptr<ExtensionSettingRenderable>> children() const = 0;
  int row() const {
    if (auto p = parent()) {
      auto children = p->children();
      auto it = std::ranges::find_if(children, [&](auto &&child) { return child.get() == this; });

      return it - children.begin();
    }

    return 0;
  }
};

class ExtensionSettingItem : public ExtensionSettingRenderable {
  std::shared_ptr<RootItem> m_item;

  OmniIcon icon() const override { return m_item->iconUrl(); }
  QString title() const override { return m_item->displayName(); }
  ExtensionSettingRenderable *parent() const override { return nullptr; }
  std::vector<std::shared_ptr<ExtensionSettingRenderable>> children() const override { return {}; }

public:
  ExtensionSettingItem(const std::shared_ptr<RootItem> &item) : m_item(item) {}
};

class ExtensionSettingProviderItem : public ExtensionSettingRenderable {
  const RootProvider *m_provider = nullptr;
  std::vector<std::shared_ptr<ExtensionSettingRenderable>> m_items;

  OmniIcon icon() const override { return m_provider->icon(); }
  QString title() const override { return m_provider->displayName(); }
  ExtensionSettingRenderable *parent() const override { return nullptr; }
  std::vector<std::shared_ptr<ExtensionSettingRenderable>> children() const override { return m_items; }

public:
  ExtensionSettingProviderItem(const RootProvider *provider) : m_provider(provider) {
    for (const auto &item : provider->loadItems()) {
      auto shared = std::shared_ptr<ExtensionSettingRenderable>(new ExtensionSettingItem(item));

      m_items.emplace_back(shared);
    }
  }
};

class SettingsModel : public QAbstractItemModel {
  RootItemManager &m_manager;
  std::vector<std::shared_ptr<ExtensionSettingRenderable>> m_providers;
  std::unordered_map<QString, std::vector<std::shared_ptr<RootItem>>> m_items;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
    if (!index.isValid()) return {};
    if (role != Qt::DisplayRole) return {};
    auto setting = static_cast<ExtensionSettingRenderable *>(index.internalPointer());

    switch (index.column()) {
    case 0:
      return "Name";
    case 1:
      return "Type";
    case 2:
      return "Alias";
    case 3:
      return "Enabled";
    }

    return {};
  }

  Qt::ItemFlags flags(const QModelIndex &index) const override { return {}; }

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
      switch (section) {
      case 0:
        return "Name";
      case 1:
        return "Type";
      case 2:
        return "Alias";
      case 3:
        return "Enabled";
      }
    }

    return QVariant();
  }

  QModelIndex parent(const QModelIndex &child) const override {
    auto renderable = static_cast<ExtensionSettingRenderable *>(child.internalPointer());
    auto parent = renderable->parent();

    if (!parent) return QModelIndex();

    return createIndex(parent->row(), 0, parent);
  }

  int rowCount(const QModelIndex &parent = QModelIndex()) const override {
    if (parent.isValid()) {
      auto provider = static_cast<ExtensionSettingRenderable *>(parent.internalPointer());

      return provider->children().size();
    }

    return m_providers.size();
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const override { return 4; }

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
    if (!hasIndex(row, column, parent)) { return QModelIndex(); }

    if (parent.isValid()) {
      auto provider = static_cast<ExtensionSettingRenderable *>(parent.internalPointer());
      auto children = provider->children();

      if (row < children.size()) {
        return createIndex(row, column, children.at(row).get());
      } else {
        return QModelIndex();
      }
    }

    auto provider = m_providers.at(row);

    return createIndex(row, column, provider.get());
  }

  void populate() {
    for (const auto &provider : m_manager.providers()) {
      auto ptr = std::make_shared<ExtensionSettingProviderItem>(provider);

      m_providers.emplace_back(ptr);
    }
  }

public:
  SettingsModel(RootItemManager &manager) : m_manager(manager) { populate(); }
};

class NameTreeWidget : public QWidget {
  QHBoxLayout *m_layout = new QHBoxLayout;
  Omnimg::ImageWidget *m_icon = new Omnimg::ImageWidget;
  TypographyWidget *m_text = new TypographyWidget;

public:
  void setText(const QString &text) { m_text->setText(text); }
  void setIcon(const OmniIconUrl &icon) { m_icon->setUrl(icon); }

  void setupUI() {
    m_icon->setFixedSize(25, 25);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(10);
    m_layout->addWidget(m_icon);
    m_layout->addWidget(m_text);
    setLayout(m_layout);
  }

  NameTreeWidget() { setupUI(); }
};

class ExtensionSettingsToolbar : public QWidget {
  QHBoxLayout *m_layout = new QHBoxLayout;
  QLineEdit *m_input = new QLineEdit;

public:
  QLineEdit *input() const { return m_input; }

  ExtensionSettingsToolbar() {
    m_layout->setContentsMargins(5, 5, 5, 5);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_input);
    m_input->setPlaceholderText("Search...");
    setLayout(m_layout);
  }
};

class NonApplicableTextPlaceholder : public TypographyWidget {
public:
  NonApplicableTextPlaceholder() {
    setText("--");
    setAlignment(Qt::AlignCenter);
    setColor(ColorTint::TextSecondary);
  }
};

class RootItemDelegate : public VirtualTreeItemDelegate {
  std::shared_ptr<RootItem> m_item = nullptr;

  bool expandable() const override { return false; }

  QWidget *widgetForColumn(int column) const override {
    if (column == 0) {
      auto widget = new NameTreeWidget();

      widget->setText(m_item->displayName());
      widget->setIcon(m_item->iconUrl());

      return widget;
    }

    if (column == 1) {
      auto widget = new TypographyWidget;
      widget->setText("Command");
      return widget;
    }

    if (column == 2) { return new NonApplicableTextPlaceholder; }

    if (column == 3) {
      auto checkbox = new Checkbox;

      checkbox->setFixedSize(20, 20);
      return checkbox;
    }

    return nullptr;
  }

public:
  RootItemDelegate(const std::shared_ptr<RootItem> &item) : m_item(item) {}
};

class ProviderItemDelegate : public VirtualTreeItemDelegate {
  RootProvider *m_provider = nullptr;

  bool expandable() const override { return true; }

  QWidget *widgetForColumn(int column) const override {
    if (column == 0) {
      auto widget = new NameTreeWidget();

      widget->setText(m_provider->displayName());
      widget->setIcon(m_provider->icon());

      return widget;
    }

    if (column == 1) {
      auto widget = new TypographyWidget;
      widget->setText(m_provider->typeAsString());
      return widget;
    }

    if (column == 2) { return new NonApplicableTextPlaceholder; }

    if (column == 3) {
      auto checkbox = new Checkbox;

      checkbox->setFixedSize(20, 20);
      return checkbox;
    }

    return nullptr;
  }

  std::vector<VirtualTreeItemDelegate *> children() const override {
    std::vector<VirtualTreeItemDelegate *> child;

    for (const auto &item : m_provider->loadItems()) {
      child.emplace_back(new RootItemDelegate(item));
    }

    return child;
  }

public:
  ProviderItemDelegate(RootProvider *provider) : m_provider(provider) {}
};

class ExtensionSettingsContextLeftPane : public QWidget {
  ExtensionSettingsToolbar *m_toolbar = new ExtensionSettingsToolbar();
  OmniTree *m_tree = new OmniTree;
  QTimer m_searchDebounce;

  void handleTextChange(const QString &text) { m_searchDebounce.start(); }

  /*
  void populateTree() {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    m_tree->clear();

    for (const auto &[idx, provider] : manager->providers() | std::views::enumerate) {
      auto rootItem = new QTreeWidgetItem(m_tree);
      auto widget = new NameTreeWidget();
      auto typeWidget = new TypographyWidget;
      auto aliasWidget = new NonApplicableTextPlaceholder;
      auto checkbox = new NonApplicableTextPlaceholder;

      widget->setText(provider->displayName());
      widget->setIcon(provider->icon());
      m_tree->setItemWidget(rootItem, 0, widget);
      m_tree->setItemWidget(rootItem, 1, typeWidget);
      m_tree->setItemWidget(rootItem, 2, aliasWidget);
      m_tree->setItemWidget(rootItem, 3, checkbox);

      for (const auto &item : provider->loadItems()) {
        auto childItem = new QTreeWidgetItem(rootItem);
        auto metadata = manager->itemMetadata(item->uniqueId());
        auto name = new NameTreeWidget;
        auto type = new TypographyWidget;
        auto alias = new NonApplicableTextPlaceholder;
        auto checkbox = new Checkbox;

        checkbox->setFixedSize(20, 20);
        name->setText(item->displayName());
        name->setIcon(item->iconUrl());
        type->setText("Command");
        type->setAlignment(Qt::AlignCenter);

        checkbox->setValue(metadata.isEnabled);
        m_tree->setItemWidget(childItem, 0, name);
        m_tree->setItemWidget(childItem, 1, type);
        m_tree->setItemWidget(childItem, 2, alias);
        m_tree->setItemWidget(childItem, 3, checkbox);
      }
    }
  }
  */

  void populateTree() {
    auto manager = ServiceRegistry::instance()->rootItemManager();

    m_tree->setColumns({"Name", "Type", "Alias", "Enabled"});
    m_tree->setColumnWidth(1, 80);
    m_tree->setColumnWidth(2, 80);
    m_tree->setColumnWidth(3, 80);

    auto rows = manager->providers() |
                std::views::transform([](auto &&provider) -> VirtualTreeItemDelegate * {
                  return new ProviderItemDelegate(provider);
                }) |
                std::ranges::to<std::vector>();
    m_tree->addRows(rows);
  }

  void applyTheme() {
    auto &theme = ThemeService::instance().theme();
    auto style = QString(R"(
        QTreeView {
            background-color: transparent;
            color: #ffffff;
            alternate-background-color: %1;
            border: 1px solid %2;
        }
        
        QHeaderView::section {
            background-color: %1;
            color: #ffffff;
            border: 1px solid %2;
			font-size: 10px;
            padding: 8px;
        }

		QTreeView::item {
			height: 30px;
			padding: 5px 0;
		}
        
        QTreeView::item:hover {
            background-color: %3;
        }
        
        QTreeView::item:selected {
            background-color: %1;
        }
    )")
                     .arg(theme.colors.mainSelectedBackground.name())
                     .arg(theme.colors.border.name())
                     .arg(theme.colors.mainHoveredBackground.name());

    m_tree->setStyleSheet(style);
  }

  void handleDebouncedSearch() {
    QString text = m_toolbar->input()->text();

    populateTree();
  }

  void setupUI() {
    auto layout = new QVBoxLayout;
    // m_tree->setHeaderLabels({"Name", "Type", "Alias", "Enabled"});
    m_searchDebounce.setInterval(500);
    m_searchDebounce.setSingleShot(true);

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_toolbar);
    layout->addWidget(m_tree, 1);

    connect(m_toolbar->input(), &QLineEdit::textChanged, this,
            &ExtensionSettingsContextLeftPane::handleTextChange);
    connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() { applyTheme(); });
    connect(&m_searchDebounce, &QTimer::timeout, this,
            &ExtensionSettingsContextLeftPane::handleDebouncedSearch);

    setLayout(layout);
    applyTheme();
    populateTree();
  }

public:
  ExtensionSettingsContextLeftPane() { setupUI(); }
};

class ExtensionSettingsContent : public QWidget {
  QHBoxLayout *m_layout = new QHBoxLayout;
  ExtensionSettingsContextLeftPane *m_left = new ExtensionSettingsContextLeftPane;

public:
  void setupUI() {
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_left, 2);
    m_layout->addWidget(new VDivider);
    m_layout->addWidget(new QWidget, 1);
    setLayout(m_layout);
  }

  ExtensionSettingsContent() { setupUI(); }
};
