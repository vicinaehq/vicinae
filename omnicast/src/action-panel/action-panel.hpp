#pragma once
#include "common.hpp"
#include "ui/action-pannel/action-list-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/omni-list.hpp"
#include "ui/popover.hpp"
#include "ui/top_bar.hpp"
#include <qboxlayout.h>
#include <qdnslookup.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qsizepolicy.h>
#include <qstackedlayout.h>
#include <qwidget.h>

class ActionPanelView : public QWidget {
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
    qWarning() << "minimum size" << minimumSize();
    qWarning() << "maximum size" << maximumSize();
    qWarning() << "minimum size hint" << minimumSizeHint();
    ActionPanelView::resizeEvent(event);
  }

public:
  ActionPanelListView() {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_list, 0, Qt::AlignTop);
    m_layout->addWidget(new HDivider);
    m_layout->addWidget(m_input, 0, Qt::AlignBottom);
    m_input->setContentsMargins(5, 5, 5, 5);

    setLayout(m_layout);
    connect(m_input, &SearchBar::textChanged, this, &ActionPanelListView::onSearchChanged);
    connect(m_list, &OmniList::virtualHeightChanged, this, [this](int height) {
      qCritical() << "vheight" << height;
      m_list->setFixedHeight(height);
      updateGeometry();
    });
  }
};

class ActionPanelStaticListView : public ActionPanelListView {
  QList<AbstractAction *> m_actions;

public:
  QList<AbstractAction *> actions() const override { return m_actions; }

  void onInitialize() override { onSearchChanged(""); }

  void onSearchChanged(const QString &text) override {
    m_list->beginResetModel();
    auto &section = m_list->addSection();

    for (const auto &action : m_actions) {
      if (action->title().contains(text, Qt::CaseInsensitive)) {
        section.addItem(std::make_unique<ActionListItem>(action));
      }
    }

    m_list->endResetModel(OmniList::SelectFirst);
  }

  ActionPanelStaticListView(const QList<AbstractAction *> &actions) : m_actions(actions) {}
};

class ActionPanelV2Widget : public Popover {
  std::stack<ActionPanelView *> m_viewStack;
  QStackedLayout *m_layout = new QStackedLayout(this);

  bool eventFilter(QObject *obj, QEvent *event) override {
    if (!m_viewStack.empty() && obj == m_viewStack.top() &&
        (event->type() == QEvent::Resize || event->type() == QEvent::LayoutRequest)) {
      QTimer::singleShot(0, this, [this]() {
        if (!m_viewStack.empty()) {
          QSize contentSize = m_viewStack.top()->sizeHint();
          qDebug() << "resize panel to" << contentSize;
          setFixedHeight(contentSize.height());
        }
      });
    }
    return Popover::eventFilter(obj, event);
  }

  void showEvent(QShowEvent *event) override {
    qWarning() << "show ActionPanelV2Widget";
    move(0, 0);
    QWidget::showEvent(event);
  }

  void resizeEvent(QResizeEvent *event) override {
    qCritical() << "list view resize" << event->size();
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
    m_layout->addWidget(view);
    m_viewStack.push(view);
    view->initialize();
    view->activate();
  }

  QTimer timer;

  ActionPanelV2Widget(QWidget *parent = nullptr) : Popover(parent) {
    setFixedWidth(400);
    timer.setInterval(1000);
    timer.start();
    connect(&timer, &QTimer::timeout, this, [this]() {
      qDebug() << "adjust size";
      // if (!m_viewStack.empty()) { setFixedHeight(m_viewStack.top()->sizeHint().height()); }
    });
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);
  }
};
