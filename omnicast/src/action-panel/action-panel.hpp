#pragma once
#include "common.hpp"
#include "theme.hpp"
#include "ui/action-pannel/action-list-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/empty-view.hpp"
#include "ui/omni-list-item-widget.hpp"
#include "ui/omni-list.hpp"
#include "ui/popover.hpp"
#include "ui/top_bar.hpp"
#include "ui/typography.hpp"
#include <atomic>
#include <qboxlayout.h>
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

class NoResultListItem : public OmniList::AbstractVirtualItem {
  class NoResultWidget : public OmniListItemWidget {
    TypographyWidget *m_text = new TypographyWidget(this);

  public:
    NoResultWidget() {
      auto layout = new QVBoxLayout(this);

      m_text->setText("No Results");
      m_text->setColor(ColorTint::TextSecondary);
      layout->addWidget(m_text, Qt::AlignCenter);
      setLayout(layout);
    }
  };

  OmniListItemWidget *createWidget() const override { return new NoResultWidget; }

  int calculateHeight(int width) const override { return 40; }

  QString id() const override { return "empty-view"; }
};

class ActionPanelView : public QWidget {
  Q_OBJECT

protected:
  virtual void onActivate() {}
  virtual void onDeactivate() {}
  virtual void onInitialize() {}

public:
  virtual QList<AbstractAction *> actions() const { return {}; }
  virtual QString searchText() const { return {}; };
  virtual void setSearchText(const QString &text) {}
  void activate() { onActivate(); }
  void deactivate() { onDeactivate(); }
  void initialize() { onInitialize(); }

signals:
  void actionActivated(AbstractAction *action) const;
};

class ActionPanelListView : public ActionPanelView {
  QVBoxLayout *m_layout = new QVBoxLayout(this);
  QLineEdit *m_input = new QLineEdit(this);

protected:
  OmniList *m_list = new OmniList;
  virtual void onSearchChanged(const QString &text) {}

  QSize sizeHint() const override {
    QSize size = ActionPanelView::sizeHint();

    qDebug() << "sizeHint" << size;

    return ActionPanelView::sizeHint();
  }

  void onActivate() override {
    m_input->setPlaceholderText("Filter actions");
    m_input->setFocus();
  }

  void resizeEvent(QResizeEvent *event) override {
    qWarning() << "list view resize" << event->size();
    ActionPanelView::resizeEvent(event);
  }

  void itemActivated(const OmniList::AbstractVirtualItem &item) {
    if (auto actionItem = dynamic_cast<const ActionListItem *>(&item)) {
      emit actionActivated(actionItem->action);
    }
  }

public:
  ActionPanelListView() {
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_list);
    m_layout->addWidget(new HDivider);
    m_layout->addWidget(m_input);
    m_input->setContentsMargins(10, 10, 10, 10);

    setLayout(m_layout);
    connect(m_input, &SearchBar::textChanged, this, &ActionPanelListView::onSearchChanged);
    connect(m_list, &OmniList::itemActivated, this, &ActionPanelListView::itemActivated);
    connect(m_list, &OmniList::virtualHeightChanged, this, [this](int height) {
      m_list->setFixedHeight(std::min(height, 180));

      // qCritical() << "vheight" << height;
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

  void addSection(const QString &title = "") { m_sections.emplace_back(ActionSection{.title = title}); }

  void addAction(AbstractAction *action) {
    if (m_sections.empty()) { addSection(m_title); }
    m_sections.at(m_sections.size() - 1).actions.emplace_back(std::shared_ptr<AbstractAction>(action));
  }

  void onSearchChanged(const QString &text) override {
    m_list->beginResetModel();

    for (const auto &actionSection : m_sections) {
      auto &section = m_list->addSection(actionSection.title);

      for (const auto &action : actionSection.actions) {
        if (action->title().contains(text, Qt::CaseInsensitive)) {
          section.addItem(std::make_unique<ActionListItem>(action.get()));
        }
      }

      m_list->addDivider();
    }

    m_list->endResetModel(OmniList::SelectFirst);
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

    qWarning() << "Resize view";

    auto parentGeo = window->geometry();

    auto x = parentGeo.width() - width() - 10;
    auto y = parentGeo.height() - height() - 50;
    QPoint global = window->mapToGlobal(QPoint(x, y));

    move(global);
  }

  void showEvent(QShowEvent *event) override {
    qWarning() << "show ActionPanelV2Widget";
    resizeView();
    QWidget::showEvent(event);
  }

  void resizeEvent(QResizeEvent *event) override {
    qCritical() << "panel resize" << event->size();
    qCritical() << "minimum size" << minimumSize();
    qCritical() << "maximum size" << maximumSize();
    qCritical() << "minimum size hint" << minimumSizeHint();

    QWidget::resizeEvent(event);
  }

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

  void popCurrentView() {
    if (m_viewStack.empty()) return;

    auto view = m_viewStack.top();

    m_viewStack.pop();

    if (!m_viewStack.empty()) {
      auto next = m_viewStack.top();
      next->show();
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
    m_layout->addWidget(view);
    m_viewStack.push(view);
    view->initialize();
    view->activate();
  }

  ActionPanelV2Widget(QWidget *parent = nullptr) : Popover(parent) {
    setFixedWidth(400);
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);
  }

signals:
  void actionActivated(AbstractAction *action) const;
};
