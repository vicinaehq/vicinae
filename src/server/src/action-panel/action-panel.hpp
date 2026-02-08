#pragma once
#include "common.hpp"
#include "navigation-controller.hpp"
#include "template-engine/template-engine.hpp"
#include "theme.hpp"
#include "config/config.hpp"
#include "ui/action-pannel/action-list-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/omni-list/omni-list-item-widget.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "service-registry.hpp"
#include "services/keybinding/keybinding-service.hpp"
#include "ui/popover/popover.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qcoreevent.h>
#include <qdnslookup.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qsizepolicy.h>
#include <qstackedlayout.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <algorithm>
#include <memory>

class NoResultListItem : public OmniList::AbstractVirtualItem {
  class NoResultWidget : public OmniListItemWidget {
    TypographyWidget *m_text = new TypographyWidget(this);

  public:
    NoResultWidget() {
      auto layout = new QVBoxLayout(this);

      m_text->setText("No Results");
      m_text->setColor(SemanticColor::TextMuted);
      m_text->setAlignment(Qt::AlignCenter);
      layout->addWidget(m_text, Qt::AlignCenter);
      setLayout(layout);
    }
  };

  OmniListItemWidget *createWidget() const override { return new NoResultWidget; }

  int calculateHeight(int width) const override { return 40; }

  QString generateId() const override { return "empty-view"; }
};

class ActionSectionTitleListItem : public OmniList::AbstractVirtualItem {
  QString m_title;
  class HeaderWidget : public OmniListItemWidget {
    TypographyWidget *m_text = new TypographyWidget(this);

  public:
    HeaderWidget(const QString &title) {
      auto layout = new QVBoxLayout(this);

      layout->setContentsMargins(10, 5, 5, 5);
      layout->addWidget(m_text);
      m_text->setColor(SemanticColor::TextMuted);
      m_text->setText(title);
      m_text->setSize(TextSize::TextSmaller);
      setLayout(layout);
    }
  };

  OmniListItemWidget *createWidget() const override { return new HeaderWidget(m_title); }

  QString generateId() const override { return m_title; }

  bool selectable() const override { return false; }

  ListRole role() const override { return AbstractVirtualItem::ListSection; }

public:
  ActionSectionTitleListItem(const QString &title) : m_title(title) {}
};

class ActionPanelView : public QWidget {
  Q_OBJECT

protected:
  virtual void onActivate() {}
  virtual void onDeactivate() {}
  virtual void onInitialize() {}
  void pop() { emit popCurrentViewRequested(); }
  void pushView(ActionPanelView *view) { emit pushViewRequested(view); }

public:
  virtual QList<AbstractAction *> actions() const { return {}; }
  virtual QString searchText() const { return {}; };
  virtual void setSearchText(const QString &text) {}
  virtual void reset() {}
  void activate() { onActivate(); }
  void deactivate() { onDeactivate(); }
  void initialize() { onInitialize(); }

signals:
  void actionActivated(AbstractAction *action) const;
  void actionsChanged() const;
  void pushViewRequested(ActionPanelView *view) const;
  void popCurrentViewRequested() const;
};

class ActionPanelListView : public ActionPanelView {
  QVBoxLayout *m_layout = new QVBoxLayout(this);
  QLineEdit *m_input = new QLineEdit(this);

protected:
  OmniList *m_list = new OmniList;
  virtual void onSearchChanged(const QString &text) {}

  void reset() override { m_input->clear(); }

  QSize sizeHint() const override {
    QSize size = ActionPanelView::sizeHint();

    // qDebug() << "sizeHint" << size;

    return ActionPanelView::sizeHint();
  }

  void showEvent(QShowEvent *event) override {
    QWidget::showEvent(event);
    m_input->setFocus();
  }

  void onActivate() override {
    m_input->setPlaceholderText("Filter actions");
    if (isVisible()) { m_input->setFocus(); }
  }

  void resizeEvent(QResizeEvent *event) override { ActionPanelView::resizeEvent(event); }

  void itemActivated(const OmniList::AbstractVirtualItem &item) {
    if (auto actionItem = dynamic_cast<const ActionListItem *>(&item)) {
      emit actionActivated(actionItem->action);
    }
  }

  bool eventFilter(QObject *watched, QEvent *event) override {
    if (watched == m_input && event->type() == QEvent::KeyPress) {
      auto keyEvent = static_cast<QKeyEvent *>(event);

      if (keyEvent->modifiers() == Qt::ControlModifier) {
        auto config = ServiceRegistry::instance()->config();
        const std::string &keybinding = config->value().keybinding;

        if (KeyBindingService::isDownKey(keyEvent, keybinding)) { return m_list->selectDown(); }
        if (KeyBindingService::isUpKey(keyEvent, keybinding)) { return m_list->selectUp(); }
        if (KeyBindingService::isLeftKey(keyEvent, keybinding)) {
          pop();
          return true;
        }
        if (KeyBindingService::isRightKey(keyEvent, keybinding)) {
          m_list->activateCurrentSelection();
          return true;
        }
      }

      if (keyEvent->modifiers().toInt() == 0) {
        switch (keyEvent->key()) {
        case Qt::Key_Up:
          return m_list->selectUp();
        case Qt::Key_Down:
          return m_list->selectDown();
        case Qt::Key_Return:
        case Qt::Key_Enter:
          m_list->activateCurrentSelection();
          return true;
        case Qt::Key_Escape:
          pop();
          return true;
        }
      }

      for (const auto &action : actions()) {
        if (action->isBoundTo(keyEvent)) {
          emit actionActivated(action);
          return true;
        }
      }
    }

    return ActionPanelView::eventFilter(watched, event);
  }

  void keyPressEvent(QKeyEvent *event) override { return ActionPanelView::keyPressEvent(event); }

  QString searchText() const override { return m_input->text(); }

  void updateStyle() {
    TemplateEngine engine;
    double pointSize = ThemeService::instance().pointSize(TextSize::TextRegular);
    engine.setVar("FONT_SIZE", QString::number(pointSize));
    auto stylesheet = engine.build(R"(
	  	QLineEdit { font-size: {FONT_SIZE}pt; }
	)");
    setStyleSheet(stylesheet);
  }

public:
  ActionPanelListView() {
    m_layout->setContentsMargins(2, 2, 2, 2);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_list);
    m_layout->addWidget(new HDivider);
    m_layout->addWidget(m_input);
    m_input->setContentsMargins(10, 10, 10, 10);
    m_list->setMargins(3, 3, 3, 3);
    m_input->installEventFilter(this);
    updateStyle();

    setLayout(m_layout);
    connect(m_input, &QLineEdit::textChanged, this, &ActionPanelListView::onSearchChanged);
    connect(m_list, &OmniList::itemActivated, this, &ActionPanelListView::itemActivated);
    connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() { updateStyle(); });
    connect(m_list, &OmniList::virtualHeightChanged, this, [this](int height) {
      m_list->setFixedHeight(std::min(height, 180));
      updateGeometry();
    });
  }
};

class ActionPanelStaticListView : public ActionPanelListView {
  struct ActionSection {
    QString title;
    std::vector<std::shared_ptr<AbstractAction>> actions;
  };

  std::vector<ActionSection> m_sections;
  QString m_title;
  QString m_id;

  void onInitialize() override { onSearchChanged(""); }

public:
  QList<AbstractAction *> actions() const override {
    QList<AbstractAction *> actions;

    for (const auto &section : m_sections) {
      for (const auto &action : section.actions) {
        actions.emplace_back(action.get());
      }
    }

    return actions;
  }

  void setTitle(const QString &title) { m_title = title; }

  void addSection(const QString &title = "") { m_sections.emplace_back(ActionSection{}); }

  void clear() { m_sections.clear(); }

  void addAction(const std::shared_ptr<AbstractAction> &action) {
    if (m_sections.empty()) { addSection(m_title); }
    m_sections.at(m_sections.size() - 1).actions.emplace_back(action);
  }

  void addAction(AbstractAction *action) {
    if (m_sections.empty()) { addSection(m_title); }
    m_sections.at(m_sections.size() - 1).actions.emplace_back(std::shared_ptr<AbstractAction>(action));
  }

  QString title() const { return m_title; }

  QString id() const { return m_id; }
  void setId(const QString &id) { m_id = id; }

  QString currentSearchText() const { return ActionPanelListView::searchText(); }

  void copyFrom(const ActionPanelStaticListView &other) {
    const QString currentSearch = currentSearchText();
    m_title = other.m_title;
    m_sections = other.m_sections;
    m_id = other.m_id;
    // Invalidate the list cache to ensure widgets are recreated with new action properties
    m_list->invalidateCache();
    // Force a rebuild of the list to show updated action properties
    onSearchChanged(currentSearch);
  }

  std::vector<AbstractAction *> filterActions(const QString &text) {
    auto filterAction = [&](const std::shared_ptr<AbstractAction> &action) -> bool {
      auto words = action->title().split(" ");
      return std::ranges::any_of(
          words, [&](const QString &word) { return word.startsWith(text, Qt::CaseInsensitive); });
    };

    std::vector<AbstractAction *> actions;

    for (const auto &section : m_sections) {
      for (const auto &action : section.actions) {
        if (!filterAction(action)) continue;
        actions.emplace_back(action.get());
      }
    }

    return actions;
  }

  void buildEmpty() {
    m_list->beginResetModel();

    if (!m_title.isEmpty()) {
      auto &section = m_list->addSection();

      section.addItem(std::make_unique<ActionSectionTitleListItem>(m_title));
    }

    for (const auto &actionSection : m_sections) {
      auto &section = m_list->addSection();

      if (!actionSection.title.isEmpty() && !actionSection.actions.empty()) {
        section.addItem(std::make_unique<ActionSectionTitleListItem>(actionSection.title));
      }

      for (const auto &action : actionSection.actions) {
        section.addItem(std::make_unique<ActionListItem>(action.get()));
      }

      m_list->addDivider();
    }

    m_list->endResetModel(OmniList::SelectFirst);
    m_list->scrollToTop();
  }

  void buildFiltered(const QString &query) {
    m_list->beginResetModel();
    auto actions = filterActions(query);
    auto &titleSection = m_list->addSection();

    if (!m_title.isEmpty()) { titleSection.addItem(std::make_unique<ActionSectionTitleListItem>(m_title)); }
    if (actions.empty()) {
      titleSection.addItem(std::make_unique<NoResultListItem>());
    } else {
      std::ranges::for_each(filterActions(query), [&](AbstractAction *action) {
        titleSection.addItem(std::make_unique<ActionListItem>(action));
      });
    }

    m_list->endResetModel(OmniList::SelectFirst);
    m_list->scrollToTop();
  }

  void onSearchChanged(const QString &text) override {
    QString query = text.trimmed();

    if (query.isEmpty()) return buildEmpty();
    return buildFiltered(text);
  }

  ActionPanelStaticListView() {}
};

class ActionPanelV2Widget : public Popover {
  Q_OBJECT

  std::stack<ActionPanelView *> m_viewStack;
  ActionPanelStaticListView *m_rootView = nullptr;
  QStackedLayout *m_layout = new QStackedLayout(this);

  bool eventFilter(QObject *obj, QEvent *event) override {
    if (!m_viewStack.empty() && obj == m_viewStack.top() &&
        (event->type() == QEvent::Resize || event->type() == QEvent::LayoutRequest)) {
      if (event->type() == QEvent::Resize) {
        QTimer::singleShot(0, this, [this]() { resizeView(); });
        return false;
      }

      QTimer::singleShot(0, this, [this]() {
        if (!m_viewStack.empty()) {
          QSize contentSize = m_viewStack.top()->sizeHint();
          setFixedHeight(contentSize.height());
          // resizeView();
        }
      });
      return false;
    }
    return Popover::eventFilter(obj, event);
  }

  void resizeView() {
    auto parent = parentWidget();

    if (!parent) {
      qWarning() << "Failed to resize action panel: no parent";
      return;
    }

    QRect parentGeo = parent->geometry();
    int x = parentGeo.width() - width() - 10;
    int y = parentGeo.height() - height() - 50;

    move(parent->mapToGlobal(QPoint(x, y)));
  }

  void showEvent(QShowEvent *event) override {
    emit opened();
    emit openChanged(true);
    resizeView();
    QWidget::showEvent(event);
  }

  void closeEvent(QCloseEvent *event) override {
    reset();
    emit closed();
    emit openChanged(false);
    Popover::closeEvent(event);
  }

  void resizeEvent(QResizeEvent *event) override { QWidget::resizeEvent(event); }

public:
  void popToRoot() {
    while (!m_viewStack.empty()) {
      popCurrentView();
    }
    m_rootView = nullptr;
  }

  QList<AbstractAction *> actions() const {
    if (m_viewStack.empty()) return {};
    return m_viewStack.top()->actions();
  }

  AbstractAction *primaryAction() const {
    for (const auto &action : actions()) {
      if (action->isPrimary()) return action;
    }

    return nullptr;
  }

  /**
   * Discard the existing view stack and replace it with `view`.
   */
  void setView(ActionPanelView *view) {
    popToRoot();
    pushView(view);
  }

  void reset() {
    while (m_viewStack.size() > 1) {
      popCurrentView();
    }

    if (!m_viewStack.empty()) { m_viewStack.top()->reset(); }
  }

  void handlePop() {
    if (m_viewStack.size() == 1) {
      close();
      return;
    }

    popCurrentView();
  }

  void popCurrentView() {
    if (m_viewStack.empty()) return;

    auto view = m_viewStack.top();

    m_viewStack.pop();

    if (!m_viewStack.empty()) {
      auto next = m_viewStack.top();
      next->show();
      next->activate();
      QSize contentSize = next->sizeHint();
      setFixedHeight(contentSize.height());
      resizeView();
    }

    view->hide();
    view->deleteLater();
  }

  void pushView(ActionPanelView *view) {
    if (!m_viewStack.empty()) {
      auto previous = m_viewStack.top();

      previous->hide();
    }

    view->installEventFilter(this);
    connect(view, &ActionPanelView::actionActivated, this, &ActionPanelV2Widget::actionActivated);
    connect(view, &ActionPanelView::popCurrentViewRequested, this, &ActionPanelV2Widget::handlePop);
    connect(view, &ActionPanelView::pushViewRequested, this, &ActionPanelV2Widget::pushView);

    m_layout->addWidget(view);
    m_layout->setCurrentWidget(view);
    m_viewStack.push(view);

    // Store root view pointer when it's first pushed
    if (m_viewStack.size() == 1) {
      m_rootView = dynamic_cast<ActionPanelStaticListView *>(view);
      connect(view, &ActionPanelView::actionsChanged, this, &ActionPanelV2Widget::actionsChanged);
      emit actionsChanged();
    }

    view->initialize();
    view->activate();

    resizeView();
  }

  /**
   * Checks if the root view structure matches the new state by comparing action IDs.
   * This is used to determine if we can update the existing root view in-place
   * instead of rebuilding the entire view stack, which preserves open submenus.
   *
   * @param state The new action panel state to compare against
   */
  bool isRootStructureSame(const ActionPanelState &state) const {
    if (!m_rootView) return false;

    const auto currentActions = m_rootView->actions();
    std::vector<QString> stateActionIds;
    stateActionIds.reserve(state.actionCount());

    for (const auto &section : state.sections()) {
      for (const auto &action : section->actions()) {
        stateActionIds.emplace_back(action->id());
      }
    }

    if (stateActionIds.size() != static_cast<size_t>(currentActions.size())) { return false; }

    for (size_t index = 0; index < stateActionIds.size(); ++index) {
      if (currentActions.at(index)->id() != stateActionIds.at(index)) { return false; }
    }

    return true;
  }

  void updateRootView(const ActionPanelState &state) {
    if (!m_rootView) return;

    auto rebuiltRoot = std::make_unique<ActionPanelStaticListView>();
    rebuiltRoot->setId(state.id());

    for (const auto &section : state.sections()) {
      rebuiltRoot->addSection(section->name());

      for (const auto &action : section->actions()) {
        rebuiltRoot->addAction(action);
      }
    }

    m_rootView->copyFrom(*rebuiltRoot);

    refreshOpenSubmenus();

    if (!state.actionCount()) close();
  }

  void setNewActions(const ActionPanelState &state) {
    // If a submenu is open and the root structure hasn't changed, update in-place
    // This preserves the open submenu stack instead of resetting it
    if (m_viewStack.size() > 1 && m_rootView && isRootStructureSame(state)) {
      updateRootView(state);
      return;
    }

    auto panel = new ActionPanelStaticListView;
    panel->setId(state.id());
    size_t count = 0;

    for (const auto &section : state.sections()) {
      panel->addSection(section->name());

      for (const auto &action : section->actions()) {
        count++;
        panel->addAction(action);
      }
    }

    setView(panel);

    if (!count) close();
  }

  ActionPanelV2Widget(QWidget *parent = nullptr) : Popover(parent) {
    setFocusPolicy(Qt::NoFocus);
    setFixedWidth(400);
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);
  }

  ~ActionPanelV2Widget() { popToRoot(); }

signals:
  void actionActivated(AbstractAction *action) const;
  void opened() const;
  void closed() const;
  void openChanged(bool value);
  void actionsChanged() const;

private:
  std::vector<ActionPanelView *> stackViews() const {
    std::vector<ActionPanelView *> views;
    auto copy = m_viewStack;
    while (!copy.empty()) {
      views.push_back(copy.top());
      copy.pop();
    }
    std::reverse(views.begin(), views.end());
    return views;
  }

  SubmenuAction *findSubmenuAction(ActionPanelStaticListView *view, const QString &id) const {
    if (!view) return nullptr;

    auto match = [&](auto *action) {
      if (id.isEmpty()) { return action->title() == view->title(); }
      return action->id() == id;
    };

    for (auto action : view->actions()) {
      if (!action || !action->isSubmenu()) continue;
      if (match(action)) { return static_cast<SubmenuAction *>(action); }
    }

    return nullptr;
  }

  /**
   * Refreshes all open submenus in the view stack with their latest content.
   * When the root view is updated, we need to refresh nested submenus to reflect
   * any changes in their parent actions. This walks through the view stack from
   * root to deepest submenu, recreating each submenu view from its parent action.
   */
  void refreshOpenSubmenus() {
    auto views = stackViews();
    if (views.size() <= 1) return;

    // Start with the root view
    auto *parent = dynamic_cast<ActionPanelStaticListView *>(views.front());
    if (!parent) return;

    // Walk through each submenu in the stack and refresh it
    for (size_t index = 1; index < views.size(); ++index) {
      auto submenuView = dynamic_cast<ActionPanelStaticListView *>(views[index]);
      if (!submenuView) break;

      // Find the action that created this submenu
      const QString submenuId = submenuView->id();
      auto submenuAction = findSubmenuAction(parent, submenuId);
      if (!submenuAction) break;

      // Create a fresh submenu view from the parent action (without triggering onOpen)
      std::unique_ptr<ActionPanelView> fresh(submenuAction->createSubmenuStealthily());
      auto freshStatic = dynamic_cast<ActionPanelStaticListView *>(fresh.get());
      if (!freshStatic) break;

      // Copy the fresh content to the existing submenu view to preserve its state
      submenuView->copyFrom(*freshStatic);
      submenuView->setId(freshStatic->id());

      // Clean up the temporary fresh view
      if (auto raw = fresh.release()) { raw->deleteLater(); }

      // Move to the next level in the hierarchy
      parent = submenuView;
    }
  }
};
