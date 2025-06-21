#pragma once
#include "common.hpp"
#include "omni-icon.hpp"
#include "root-item-manager.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/checkbox.hpp"
#include "ui/icon-button.hpp"
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
#include <qevent.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qtmetamacros.h>
#include <qtreeview.h>
#include <qtreewidget.h>
#include <qlineedit.h>
#include <qwidget.h>
#include <qtreeview.h>

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
  BaseInput *m_input = new BaseInput;
  Omnimg::ImageWidget *m_searchIcon = new Omnimg::ImageWidget;

public:
  QLineEdit *input() const { return m_input; }

  ExtensionSettingsToolbar() {
    m_searchIcon->setFixedSize({20, 20});
    m_searchIcon->setUrl(BuiltinOmniIconUrl("magnifying-glass"));
    m_layout->setContentsMargins(10, 10, 10, 10);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_input);
    m_input->setPlaceholderText("Search...");
    m_input->setRightAccessory(m_searchIcon);
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

class CheckboxContainer : public QWidget {
  Checkbox *m_checkbox = new Checkbox;

public:
  Checkbox *checkbox() const { return m_checkbox; }

  CheckboxContainer() {
    auto layout = new QHBoxLayout;

    m_checkbox->setFixedSize(20, 20);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_checkbox, 0, Qt::AlignCenter);
    setLayout(layout);
  }
};

class AliasInput : public BaseInput {
  QString m_id;

  void showEvent(QShowEvent *event) override {
    refreshAlias();
    BaseInput::showEvent(event);
  }

public:
  void refreshAlias() {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    auto metadata = manager->itemMetadata(m_id);

    setText(metadata.alias);
  }

  void focusOutEvent(QFocusEvent *event) override {
    auto manager = ServiceRegistry::instance()->rootItemManager();

    manager->setAlias(m_id, text());
    BaseInput::focusOutEvent(event);
  }

  AliasInput(const QString &rootItemId) : m_id(rootItemId) {
    setContentsMargins(2, 2, 2, 2);
    setPlaceholderText("Add alias");
  }
};

class RootItemDelegate : public QObject, public VirtualTreeItemDelegate {
  Q_OBJECT

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
      widget->setText(m_item->typeDisplayName());
      return widget;
    }

    if (column == 2) { return new AliasInput(m_item->uniqueId()); }

    if (column == 3) {
      auto manager = ServiceRegistry::instance()->rootItemManager();
      auto checkbox = new CheckboxContainer;
      auto metadata = manager->itemMetadata(m_item->uniqueId());

      checkbox->checkbox()->setValue(metadata.isEnabled);

      connect(checkbox->checkbox(), &Checkbox::valueChanged, this, [this, manager](bool value) {
        manager->setItemEnabled(m_item->uniqueId(), value);
        qDebug() << "value changed";
        emit itemEnabledChanged(m_item.get(), value);
      });

      return checkbox;
    }

    return nullptr;
  }

public:
  RootItemDelegate(const std::shared_ptr<RootItem> &item) : m_item(item) {}

signals:
  void itemEnabledChanged(const RootItem *item, bool value) const;
};

class ProviderItemDelegate : public QObject, public VirtualTreeItemDelegate {
  Q_OBJECT

  RootProvider *m_provider = nullptr;
  std::vector<std::shared_ptr<RootItem>> items;

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
      auto checkbox = new CheckboxContainer;

      return checkbox;
    }

    return nullptr;
  }

  std::vector<VirtualTreeItemDelegate *> children() const override {
    std::vector<VirtualTreeItemDelegate *> child;

    for (const auto &item : items) {
      auto delegate = new RootItemDelegate(item);

      connect(delegate, &RootItemDelegate::itemEnabledChanged, this,
              &ProviderItemDelegate::itemEnabledChanged);

      child.emplace_back(delegate);
    }

    return child;
  }

public:
  ProviderItemDelegate(RootProvider *provider, const std::vector<std::shared_ptr<RootItem>> &items)
      : m_provider(provider), items(items) {}

signals:
  void itemEnabledChanged(const RootItem *item, bool value) const;
};

class ExtensionSettingsContextLeftPane : public QWidget {
  ExtensionSettingsToolbar *m_toolbar = new ExtensionSettingsToolbar();
  OmniTree *m_tree = new OmniTree;
  QTimer m_searchDebounce;

  void handleTextChange(const QString &text) { m_searchDebounce.start(); }

  void populateTreeFromQuery(const QString &query) {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    RootItemPrefixSearchOptions opts;
    std::map<QString, std::vector<std::shared_ptr<RootItem>>> map;

    opts.includeDisabled = true;

    for (const auto &item : manager->prefixSearch(query, opts)) {
      QString providerId = manager->getItemProviderId(item->uniqueId());

      if (providerId.isEmpty()) continue;

      map[providerId].emplace_back(item);
    }

    std::vector<VirtualTreeItemDelegate *> delegates;

    delegates.reserve(map.size());

    for (const auto &[providerId, items] : map) {
      auto provider = manager->provider(providerId);

      if (!provider) continue;

      auto delegate = new ProviderItemDelegate(provider, items);

      delegate->setExpandable(!query.isEmpty());

      connect(delegate, &ProviderItemDelegate::itemEnabledChanged, this,
              [](const RootItem *item, bool value) { qDebug() << "enabled" << item->uniqueId() << value; });

      delegates.emplace_back(delegate);
    }

    m_tree->setColumns({"Name", "Type", "Alias", "Enabled"});
    m_tree->setColumnWidth(1, 100);
    m_tree->setColumnWidth(2, 100);
    m_tree->setColumnWidth(3, 80);
    m_tree->addRows(delegates);
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

    populateTreeFromQuery(text);
  }

  void setupUI() {
    auto layout = new QVBoxLayout;
    // m_tree->setHeaderLabels({"Name", "Type", "Alias", "Enabled"});
    m_searchDebounce.setInterval(50);
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
    populateTreeFromQuery("");
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
