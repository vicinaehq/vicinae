#pragma once
#include "common.hpp"
#include "theme.hpp"
#include "ui/action-pannel/action-list-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/omni-list-item-widget.hpp"
#include "ui/omni-list.hpp"
#include "ui/popover.hpp"
#include "ui/top_bar.hpp"
#include "ui/typography.hpp"
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
#include <ranges>

class NoResultListItem : public OmniList::AbstractVirtualItem {
  class NoResultWidget : public OmniListItemWidget {
    TypographyWidget *m_text = new TypographyWidget(this);

  public:
    NoResultWidget() {
      auto layout = new QVBoxLayout(this);

      m_text->setText("No Results");
      m_text->setColor(ColorTint::TextSecondary);
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
      m_text->setColor(ColorTint::TextSecondary);
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
  void activate() { onActivate(); }
  void deactivate() { onDeactivate(); }
  void initialize() { onInitialize(); }

signals:
  void actionActivated(AbstractAction *action) const;
  void pushViewRequested(ActionPanelView *view) const;
  void popCurrentViewRequested() const;
};

class ActionPanelListView : public ActionPanelView {
  QVBoxLayout *m_layout = new QVBoxLayout(this);
  QLineEdit *m_input = new QLineEdit(this);

protected:
  OmniList *m_list = new OmniList;
  virtual void onSearchChanged(const QString &text) {}

  QSize sizeHint() const override {
    QSize size = ActionPanelView::sizeHint();

    // qDebug() << "sizeHint" << size;

    return ActionPanelView::sizeHint();
  }

  void onActivate() override {
    m_input->setPlaceholderText("Filter actions");
    m_input->setFocus();
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

      switch (keyEvent->key()) {
      case Qt::Key_Up:
        return m_list->selectUp();
      case Qt::Key_Down:
        return m_list->selectDown();
      case Qt::Key_Return:
        m_list->activateCurrentSelection();
        return true;
      case Qt::Key_Escape:
        pop();
        return true;
      }
    }

    return ActionPanelView::eventFilter(watched, event);
  }

  void keyPressEvent(QKeyEvent *event) override { return ActionPanelView::keyPressEvent(event); }

  QString searchText() const override { return m_input->text(); }

public:
  ActionPanelListView() {
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_list);
    m_layout->addWidget(new HDivider);
    m_layout->addWidget(m_input);
    m_input->setContentsMargins(10, 10, 10, 10);
    m_list->setMargins(5, 5, 5, 5);
    m_input->installEventFilter(this);

    setLayout(m_layout);
    connect(m_input, &SearchBar::textChanged, this, &ActionPanelListView::onSearchChanged);
    connect(m_list, &OmniList::itemActivated, this, &ActionPanelListView::itemActivated);
    connect(m_list, &OmniList::virtualHeightChanged, this, [this](int height) {
      qDebug() << "HEIGHT CHANGED TO" << height;
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

  void addAction(AbstractAction *action) {
    if (m_sections.empty()) { addSection(m_title); }
    m_sections.at(m_sections.size() - 1).actions.emplace_back(std::shared_ptr<AbstractAction>(action));
  }

  std::vector<AbstractAction *> filterActions(const QString &text) {
    auto filterAction = [&](const std::shared_ptr<AbstractAction> &action) -> bool {
      auto words = action->title().split(" ");
      return std::ranges::any_of(
          words, [&](const QString &word) { return word.startsWith(text, Qt::CaseInsensitive); });
    };

    auto filteredActions = m_sections |
                           std::views::transform([](const auto &section) { return section.actions; }) |
                           std::views::join | std::views::filter(filterAction) |
                           std::views::transform([](const auto &action) { return action.get(); }) |
                           std::ranges::to<std::vector>();

    return filteredActions;
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
    auto window = QApplication::activeWindow();

    if (!window) {
      qDebug() << "showActions: no active window, won't show popover";
      return;
    }

    auto parentGeo = window->geometry();

    auto x = parentGeo.width() - width() - 10;
    auto y = parentGeo.height() - height() - 50;
    QPoint global = window->mapToGlobal(QPoint(x, y));

    move(global);
  }

  void showEvent(QShowEvent *event) override {
    resizeView();
    QWidget::showEvent(event);
  }

  void resizeEvent(QResizeEvent *event) override { QWidget::resizeEvent(event); }

public:
  void popToRoot() {
    while (!m_viewStack.empty()) {
      popCurrentView();
    }
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
    } else {
      close();
    }

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
    view->initialize();
    view->activate();
    resizeView();
  }

  ActionPanelV2Widget(QWidget *parent = nullptr) : Popover(parent) {
    setFixedWidth(400);
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);
  }

signals:
  void actionActivated(AbstractAction *action) const;
};
