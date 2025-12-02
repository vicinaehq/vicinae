#pragma once
#include "common.hpp"
#include "../ui/image/url.hpp"
#include "layout.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "service-registry.hpp"
#include "settings-controller/settings-controller.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/checkbox.hpp"
#include "ui/image/image.hpp"
#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include "ui/omni-tree/omni-tree.hpp"
#include "ui/typography/typography.hpp"
#include <memory>
#include <qabstractitemmodel.h>
#include <qboxlayout.h>
#include <qcoreevent.h>
#include <qdnslookup.h>
#include <qevent.h>
#include <qjsonobject.h>
#include <qkeysequenceedit.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qscrollarea.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qtreeview.h>
#include <qtreewidget.h>
#include <qlineedit.h>
#include <qwidget.h>
#include <qtreeview.h>

class NameTreeWidget : public QWidget {
  QHBoxLayout *m_layout = new QHBoxLayout;
  ImageWidget *m_icon = new ImageWidget;
  TypographyWidget *m_text = new TypographyWidget;

public:
  void setText(const QString &text) { m_text->setText(text); }
  void setIcon(const ImageURL &icon) { m_icon->setUrl(icon); }

  void setupUI() {
    m_icon->setFixedSize(20, 20);
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
  ImageWidget *m_searchIcon = new ImageWidget;

public:
  QLineEdit *input() const { return m_input->input(); }

  void showEvent(QShowEvent *event) override {
    QWidget::showEvent(event);
    m_input->setFocus();
  }

  ExtensionSettingsToolbar() {
    m_searchIcon->setFixedSize({20, 20});
    m_searchIcon->setUrl(ImageURL::builtin("magnifying-glass"));
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
    setColor(SemanticColor::TextMuted);
  }
};

class CheckboxContainer : public QWidget {
  Checkbox *m_checkbox = new Checkbox;

public:
  Checkbox *checkbox() const { return m_checkbox; }

  CheckboxContainer() {
    auto layout = new QHBoxLayout;

    m_checkbox->setFillColor(Qt::transparent);
    m_checkbox->setFixedSize(20, 20);
    m_checkbox->setFocusPolicy(Qt::ClickFocus);
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

  void keyPressEvent(QKeyEvent *event) override {
    if (event->key() == Qt::Key_Return) return;
    BaseInput::keyPressEvent(event);
  }

public:
  void refreshAlias() {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    auto metadata = manager->itemMetadata(m_id);

    setText(QString::fromStdString(metadata.alias));
  }

  // TODO: we need to move that logic a few level above I think, as that's not pretty
  void handleSave() {
    auto manager = ServiceRegistry::instance()->rootItemManager();

    manager->setAlias(m_id, text());
  }

  AliasInput(const QString &rootItemId) : m_id(rootItemId) {
    setFocusPolicy(Qt::ClickFocus);
    setPlaceholderText("Add alias");
    setTextMargins(QMargins{2, 2, 2, 2});
    connect(focusNotifier(), &FocusNotifier::focusChanged, this, [this](bool value) {
      if (!value) handleSave();
    });
  }
};

class AbstractSettingsDetailPaneItem {
public:
  virtual ImageURL icon() const = 0;
  virtual QString title() const = 0;
  virtual QWidget *content() const = 0;

  virtual ~AbstractSettingsDetailPaneItem() = default;
};

class ProviderDetailPaneItem : public AbstractSettingsDetailPaneItem {
  RootProvider *m_provider = nullptr;

public:
  ImageURL icon() const override { return m_provider->icon(); }
  QString title() const override { return m_provider->displayName(); }
  QWidget *content() const override { return m_provider->settingsDetail(); }

  ProviderDetailPaneItem(RootProvider *provider) : m_provider(provider) {}
};

class RootDetailPaneItem : public AbstractSettingsDetailPaneItem {
  std::shared_ptr<RootItem> m_item;

public:
  ImageURL icon() const override { return m_item->iconUrl(); }
  QString title() const override { return m_item->displayName(); }
  QWidget *content() const override {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    QJsonObject preferences = manager->getItemPreferenceValues(m_item->uniqueId());

    return m_item->settingsDetail(preferences);
  }

  RootDetailPaneItem(const std::shared_ptr<RootItem> &item) : m_item(item) {}
};

class AbstractRootItemDelegate : public VirtualTreeItemDelegate {
public:
  virtual std::unique_ptr<AbstractSettingsDetailPaneItem> generateDetail() const = 0;
  virtual ~AbstractRootItemDelegate() = default;
};

class RootItemDelegate : public QObject, public AbstractRootItemDelegate {
  Q_OBJECT

  std::shared_ptr<RootItem> m_item = nullptr;
  bool m_enabled = false;

public:
  std::unique_ptr<AbstractSettingsDetailPaneItem> generateDetail() const override {
    return std::make_unique<RootDetailPaneItem>(m_item);
  }

  bool expandable() const override { return false; }

  bool disabled() const override { return !m_enabled; }

  void setEnabled(bool value) { m_enabled = value; }

  QString id() const override { return m_item->uniqueId(); }

  void refreshForColumn(QWidget *widget, int column) const override {
    if (column == 3) {
      auto checkbox = static_cast<CheckboxContainer *>(widget)->checkbox();

      checkbox->blockSignals(true);
      checkbox->setValue(m_enabled);
      checkbox->blockSignals(false);
    }
  }

  void attached(QWidget *widget, int column) override {

    if (column == 3) {
      auto manager = ServiceRegistry::instance()->rootItemManager();
      Checkbox *checkbox = static_cast<CheckboxContainer *>(widget)->checkbox();

      connect(checkbox, &Checkbox::valueChanged, this, [this, manager](bool value) {
        manager->setItemEnabled(m_item->uniqueId(), value);

        emit itemEnabledChanged(m_item.get(), value);
      });
    }
  }

  void detached(QWidget *widget, int column) override {
    if (column == 3) {
      Checkbox *checkbox = static_cast<CheckboxContainer *>(widget)->checkbox();

      disconnect(checkbox);
    }
  }

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
      auto checkbox = new CheckboxContainer;

      checkbox->checkbox()->setValue(m_enabled);

      return checkbox;
    }

    return nullptr;
  }

public:
  RootItemDelegate(const std::shared_ptr<RootItem> &item) : m_item(item) {}

signals:
  void itemEnabledChanged(const RootItem *item, bool value) const;
};

class ProviderItemDelegate : public QObject, public AbstractRootItemDelegate {
  Q_OBJECT

public:
  enum CheckboxState { CHECKED, UNCHECKED, PARTIAL };

private:
  RootProvider *m_provider = nullptr;
  CheckboxState m_checkboxState = UNCHECKED;
  std::vector<std::shared_ptr<RootItem>> items;
  std::vector<std::shared_ptr<VirtualTreeItemDelegate>> child;

public:
  std::unique_ptr<AbstractSettingsDetailPaneItem> generateDetail() const override {
    return std::make_unique<ProviderDetailPaneItem>(m_provider);
  }

  bool expandable() const override { return true; }

  QString id() const override { return m_provider->uniqueId(); }

  void styleCheckbox(Checkbox *checkbox) const {
    checkbox->blockSignals(true);
    checkbox->setValue(m_checkboxState == CHECKED || m_checkboxState == PARTIAL);
    checkbox->blockSignals(false);
  }

  void attached(QWidget *widget, int column) override {
    if (column == 3) {
      Checkbox *checkbox = static_cast<CheckboxContainer *>(widget)->checkbox();

      connect(checkbox, &Checkbox::valueChanged, this,
              [this](bool value) { emit providerEnabledChanged(m_provider, value); });
    }
  }

  void detached(QWidget *widget, int column) override {
    if (column == 3) {
      Checkbox *checkbox = static_cast<CheckboxContainer *>(widget)->checkbox();

      disconnect(checkbox);
    }
  }

  void refreshForColumn(QWidget *widget, int column) const override {
    if (column == 3) {
      Checkbox *box = static_cast<CheckboxContainer *>(widget)->checkbox();

      styleCheckbox(box);
    }
  }

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

      styleCheckbox(checkbox->checkbox());

      return checkbox;
    }

    return nullptr;
  }

  std::vector<std::shared_ptr<VirtualTreeItemDelegate>> children() const override { return child; }

  void setCheckboxState(CheckboxState state) { m_checkboxState = state; }

public:
  ProviderItemDelegate(RootProvider *provider, const std::vector<std::shared_ptr<RootItem>> &items)
      : m_provider(provider), items(items) {
    size_t disabledCount = 0;

    for (const auto &item : items) {
      auto manager = ServiceRegistry::instance()->rootItemManager();
      auto metadata = manager->itemMetadata(item->uniqueId());
      auto delegate = new RootItemDelegate(item);

      disabledCount += !metadata.isEnabled;
      delegate->setEnabled(metadata.isEnabled);

      connect(delegate, &RootItemDelegate::itemEnabledChanged, this,
              [delegate, this](auto a, bool value) { emit itemEnabledChanged(delegate, value); });

      child.emplace_back(delegate);
    }

    if (disabledCount == items.size()) {
      m_checkboxState = UNCHECKED;
    } else if (disabledCount > 0) {
      m_checkboxState = PARTIAL;
    } else {
      m_checkboxState = CHECKED;
    }
  }

signals:
  void itemEnabledChanged(RootItemDelegate *delegate, bool value) const;
  void providerEnabledChanged(const RootProvider *provider, bool value) const;
};

class ExtensionSettingsDetailPane : public QWidget {
  ImageWidget *m_icon = new ImageWidget;
  TypographyWidget *m_title = new TypographyWidget;
  QWidget *m_header = nullptr;
  QScrollArea *m_content = new VerticalScrollArea(this);
  HDivider *m_divider = new HDivider;

public:
  void setData(std::unique_ptr<AbstractSettingsDetailPaneItem> data) {
    m_title->setText(data->title());
    m_icon->setUrl(data->icon());
    m_icon->show();
    m_divider->show();

    QWidget *content = data->content();

    if (auto previous = m_content->widget()) {
      // previous->deleteLater();
      previous->hide();
    }

    m_content->setWidget(content);
    m_divider->show();
    m_content->show();
    m_header->show();
    updateGeometry();
  }

  void clearData() {
    m_content->hide();
    m_divider->hide();
    m_header->hide();
  }

  void setupUI() {
    m_header = HStack().margins(20).spacing(10).add(m_icon).add(m_title).addStretch().buildWidget();
    m_content->setWidgetResizable(true);
    m_content->setVerticalScrollBar(new OmniScrollBar);
    m_content->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    m_icon->hide();
    m_divider->hide();
    m_icon->setFixedSize(30, 30);
    m_header->setFixedHeight(76);

    VStack().add(m_header).add(m_divider).add(m_content, 1).imbue(this);
  }

  ExtensionSettingsDetailPane() { setupUI(); }
};

class ExtensionSettingsContextLeftPane : public QWidget {
  Q_OBJECT

signals:
  void itemSelectionChanged(AbstractRootItemDelegate *next, AbstractRootItemDelegate *previous);

public:
  ExtensionSettingsToolbar *m_toolbar = new ExtensionSettingsToolbar();
  OmniTree *m_tree = new OmniTree;
  QTimer m_searchDebounce;

  void handleTextChange(const QString &text) { populateTreeFromQuery(text); }

  void providerEnabledChanged(ProviderItemDelegate *delegate, bool value) {
    auto manager = ServiceRegistry::instance()->rootItemManager();

    if (!manager->setProviderEnabled(delegate->id(), value)) {
      qWarning() << "Failed to enable/disable provider" << delegate->id();
      return;
    }

    for (auto &item : delegate->children()) {
      std::static_pointer_cast<RootItemDelegate>(item)->setEnabled(value);
    }

    if (value) {
      delegate->setCheckboxState(ProviderItemDelegate::CheckboxState::CHECKED);
    } else {
      delegate->setCheckboxState(ProviderItemDelegate::CheckboxState::UNCHECKED);
    }

    m_tree->refresh();
  }

  void itemEnabledChanged(RootItemDelegate *delegate, bool value) {
    delegate->setEnabled(value);
    m_tree->refresh();
  }

  bool eventFilter(QObject *watched, QEvent *event) override {
    if (watched == m_toolbar->input() && event->type() == QEvent::KeyPress) {
      auto ev = static_cast<QKeyEvent *>(event);

      if (ev->modifiers().toInt() == 0) {
        switch (ev->key()) {
        case Qt::Key_Up:
          return m_tree->selectUp();
          break;
        case Qt::Key_Down:
          return m_tree->selectDown();
          break;
        case Qt::Key_Escape: {
          if (!m_toolbar->input()->text().isEmpty()) {
            m_toolbar->input()->clear();
            return true;
          }
          break;
        }
        case Qt::Key_Return:
        case Qt::Key_Enter:
          m_tree->activateCurrentSelection();
          break;
        }
      }
    }

    return QWidget::eventFilter(watched, event);
  }

  void populateTreeFromQuery(const QString &query) {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    RootItemPrefixSearchOptions opts;

    // we use a vector of pairs so that providers are ranked by their best ranked
    // items
    std::vector<std::pair<QString, std::vector<std::shared_ptr<RootItem>>>> map;

    opts.includeDisabled = true;

    for (const auto &item : manager->search(query, opts)) {
      QString providerId = manager->getItemProviderId(item.item.get()->uniqueId());

      if (providerId.isEmpty()) continue;

      auto pred = [&](auto &&pair) { return pair.first == providerId; };

      if (auto it = std::ranges::find_if(map, pred); it != map.end()) {
        it->second.emplace_back(item.item.get());
      } else {
        map.push_back({providerId, {item.item.get()}});
      }
    }

    std::vector<std::shared_ptr<VirtualTreeItemDelegate>> delegates;

    delegates.reserve(map.size());

    for (const auto &[providerId, items] : map) {
      auto provider = manager->provider(providerId);

      if (!provider) continue;

      auto delegate = std::make_shared<ProviderItemDelegate>(provider, items);
      auto delegatePtr = delegate.get();

      delegate->setExpandable(!query.isEmpty());

      connect(delegate.get(), &ProviderItemDelegate::providerEnabledChanged, this,
              [this, delegatePtr](auto provider, bool value) { providerEnabledChanged(delegatePtr, value); });
      connect(delegate.get(), &ProviderItemDelegate::itemEnabledChanged, this,
              &ExtensionSettingsContextLeftPane::itemEnabledChanged);

      delegates.emplace_back(delegate);
    }

    m_tree->setColumns({"Name", "Type", "Alias", "Enabled"});
    m_tree->setColumnWidth(1, 100);
    m_tree->setColumnWidth(2, 100);
    m_tree->setColumnWidth(3, 80);
    m_tree->addRows(delegates);
    m_tree->selectFirst();
  }

  void handleDebouncedSearch() {
    QString text = m_toolbar->input()->text();

    populateTreeFromQuery(text);
  }

  void selectionUpdated(VirtualTreeItemDelegate *next, VirtualTreeItemDelegate *previous) {
    emit itemSelectionChanged(static_cast<AbstractRootItemDelegate *>(next),
                              static_cast<AbstractRootItemDelegate *>(previous));
  }

  void setupUI() {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    auto theme = ThemeService::instance().theme();
    m_searchDebounce.setInterval(50);
    m_searchDebounce.setSingleShot(true);
    m_toolbar->input()->installEventFilter(this);
    m_tree->setAlternateBackgroundColor(SemanticColor::ListItemHoverBackground);

    VStack().add(m_toolbar).add(m_tree, 1).imbue(this);

    connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this](const ThemeFile &theme) {
      m_tree->setAlternateBackgroundColor(theme.resolve(SemanticColor::ListItemHoverBackground));
    });
    connect(m_tree, &OmniTree::selectionUpdated, this, &ExtensionSettingsContextLeftPane::selectionUpdated);
    connect(manager, &RootItemManager::itemsChanged, this,
            [this]() { populateTreeFromQuery(m_toolbar->input()->text()); });
    connect(m_toolbar->input(), &QLineEdit::textChanged, this,
            &ExtensionSettingsContextLeftPane::handleTextChange);
    connect(&m_searchDebounce, &QTimer::timeout, this,
            &ExtensionSettingsContextLeftPane::handleDebouncedSearch);

    QTimer::singleShot(0, this, [this]() { populateTreeFromQuery(""); });
  }

public:
  ExtensionSettingsContextLeftPane() { setupUI(); }

  auto selected() { return static_cast<AbstractRootItemDelegate *>(m_tree->value()); }

  void select(const QString &id) {
    m_toolbar->input()->clear();
    m_tree->select(id);
  }
};

class ExtensionSettingsContent : public QWidget {
  ExtensionSettingsContextLeftPane *m_left = new ExtensionSettingsContextLeftPane;
  ExtensionSettingsDetailPane *m_detail = new ExtensionSettingsDetailPane;

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    QSize size = event->size();

    m_left->setFixedWidth(width() * 0.60);
    m_detail->setFixedWidth(width() * 0.40);
  }

  void itemSelectionChanged(AbstractRootItemDelegate *next, AbstractRootItemDelegate *previous) {
    if (next) {
      m_detail->setData(next->generateDetail());
    } else {
      m_detail->clearData();
    }
  }

public:
  void setupUI() {
    HStack().divided(1).add(m_left).add(m_detail).imbue(this);

    connect(m_left, &ExtensionSettingsContextLeftPane::itemSelectionChanged, this,
            &ExtensionSettingsContent::itemSelectionChanged);
  }

  ExtensionSettingsContent(const ApplicationContext *ctx) {
    connect(ctx->settings.get(), &SettingsController::openExtensionPreferencesRequested, this,
            [this](const QString &id) { m_left->select(id); });
    setupUI();
  }
};
