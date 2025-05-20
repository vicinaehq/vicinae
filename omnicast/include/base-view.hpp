#pragma once
#include "common.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action-pannel-widget.hpp"
#include <qevent.h>
#include <ranges>
#include "ui/action-pannel/action.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/status_bar.hpp"
#include "ui/toast.hpp"
#include "ui/top_bar.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qlogging.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class BaseView : public QWidget {
  Q_OBJECT

public:
  /**
   * Called before the view is first shown on screen.
   * You can use this hook to setup UI.
   */
  virtual void initialize() {}

  void activate() { onActivate(); }
  void deactivate() { onDeactivate(); }

  /**
   * Called when the view becomes visible. This is called the first time the view is shown
   * (right after `initialize`) but also after a view that was pushed on top of it was poped.
   */
  virtual void onActivate() {}

  /**
   * Called when the view becomes hidden. This is called before the view is poped or when
   * another view is pushed on top of it.
   */
  virtual void onDeactivate() {}

  /**
   * Called on the view when a toast needs to be displayed. It's up to the specific view to decide
   * how the toast should be shown.
   */
  virtual void setToast(const QString &title, ToastPriority priority) {}

  /**
   * Clear the content of the search bar. If this is not applicable to the view
   * (for instance, a form view doesn't have a search field) this method should not be overriden.
   */
  virtual void clearSearchBar() { qWarning() << "clearSearchBar() is not implemented for this view"; }

  /**
   * The current search text for this view. If not applicable, do not implement.
   */
  virtual QString searchText() const { return {}; }

  /**
   * The dynamic arguments for this view. Used by some actions.
   */
  virtual std::vector<QString> argumentValues() const { return {}; }

  BaseView(QWidget *parent = nullptr) : QWidget(parent) {}

signals:
  void requestLaunchCommand(const std::shared_ptr<AbstractCmd> &command, const LaunchCommandOptions &opts,
                            const LaunchProps &props) const;
  void requestViewPush(View *view, const PushViewOptions &options) const;
};

class SimpleView : public BaseView {
protected:
  QVBoxLayout *m_layout = new QVBoxLayout(this);
  TopBar *m_topBar = new TopBar(this);
  StatusBar *m_statusBar = new StatusBar(this);
  HorizontalLoadingBar *m_loadingBar = new HorizontalLoadingBar(this);
  ActionPannelWidget *m_actionPannel = new ActionPannelWidget(this);

  bool eventFilter(QObject *obj, QEvent *event) override {
    if (event->type() == QEvent::KeyPress) {
      auto keyEvent = static_cast<QKeyEvent *>(event);
      bool isEsc = keyEvent->key() == Qt::Key_Escape;

      if (obj == m_topBar->input) { return inputFilter(keyEvent); }
    }

    return false;
  }

  virtual bool inputFilter(QKeyEvent *event) { return false; }

  bool event(QEvent *event) override {
    if (event->type() == QEvent::KeyPress) {
      auto keyEvent = static_cast<QKeyEvent *>(event);
      auto key = keyEvent->key();

      bool isEsc = keyEvent->key() == Qt::Key_Escape;

      if (isEsc) {
        escapePressed();
        return true;
      }

      if (keyEvent->modifiers().testFlag(Qt::ControlModifier) && keyEvent->key() == Qt::Key_B) {
        m_actionPannel->showActions();

        return true;
      }

      if (auto action = m_actionPannel->findBoundAction(keyEvent)) {
        executeAction(action);
        return true;
      }
    }

    return QWidget::event(event);
  }

  void actionPannelOpened() {
    m_statusBar->setActionButtonHighlight(true);
    onActionPanelOpened();
  }

  virtual void escapePressed() {
    qDebug() << "escape pressed";
    ServiceRegistry::instance()->UI()->popView();
  }

  void actionPannelClosed() {
    m_statusBar->setActionButtonHighlight(false);
    onActionPanelClosed();
  }

  void executeAction(AbstractAction *action) {
    m_actionPannel->close();
    action->execute();
    onActionExecuted(action);
  }

  void actionButtonClicked() { m_actionPannel->showActions(); }

  void currentActionButtonClicked() {
    if (auto action = m_actionPannel->primaryAction()) { executeAction(action); }
  }

  virtual QWidget *centerWidget() const {
    qCritical() << "default centerWidget()";
    return new QWidget;
  }

  std::vector<QString> argumentValues() const override {
    return m_topBar->m_completer->collect() | std::views::transform([](const auto &p) { return p.second; }) |
           std::ranges::to<std::vector>();
  }

  void clearSearchBar() override { m_topBar->input->clear(); }

  void setToast(const QString &title, ToastPriority priority) override {
    m_statusBar->setToast(title, priority);
  }

  virtual void onSearchChanged(const QString &text) {}
  virtual void onActionPanelClosed() {}
  virtual void onActionPanelOpened() {}
  virtual void onActionExecuted(AbstractAction *action) {}

  void onActivate() override {
    if (!m_topBar->input->text().isEmpty()) { m_topBar->input->selectAll(); }

    m_topBar->input->setFocus();
  }

  void activatePrimaryAction() {
    if (auto action = m_actionPannel->primaryAction()) { executeAction(action); }
  }

  QString searchText() const override { return m_topBar->input->text(); }

  void setSearchText(const QString &text) { m_topBar->input->setText(text); }
  QString searchPlaceholderText() const { return m_topBar->input->placeholderText(); }
  void setSearchPlaceholderText(const QString &value) const {
    return m_topBar->input->setPlaceholderText(value);
  }

  QString navigationTitle() const { return m_statusBar->navigationTitle(); }
  void setNavigationTitle(const QString &title) { m_statusBar->setNavigationTitle(title); }

  void setLoading(bool value) { m_loadingBar->setStarted(value); }

  void setActions(const QList<AbstractAction *> &actions) {
    if (!actions.isEmpty()) { actions.at(0)->setShortcut({.key = "return"}); }
    if (actions.size() > 1) { actions.at(1)->setShortcut({.key = "return", .modifiers = {"shift"}}); }

    m_actionPannel->setSignalActions(actions);

    if (!actions.isEmpty()) {
      m_statusBar->setAction(*actions.at(0));
    } else {
      m_statusBar->clearAction();
    }
  }

  void setupUI() {
    m_topBar->setFixedHeight(Omnicast::TOP_BAR_HEIGHT);
    m_statusBar->setFixedHeight(Omnicast::STATUS_BAR_HEIGHT);

    m_loadingBar->setFixedHeight(1);
    m_loadingBar->setBarWidth(100);
    m_topBar->input->installEventFilter(this);

    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_topBar);
    m_layout->addWidget(m_loadingBar);
    m_layout->addWidget(centerWidget(), 1);
    m_layout->addWidget(m_statusBar);
    setLayout(m_layout);

    connect(m_topBar->input, &SearchBar::textChanged, this, &SimpleView::onSearchChanged);
    connect(m_actionPannel, &ActionPannelWidget::opened, this, &SimpleView::actionPannelOpened);
    connect(m_actionPannel, &ActionPannelWidget::closed, this, &SimpleView::actionPannelClosed);
    connect(m_actionPannel, &ActionPannelWidget::actionExecuted, this, &SimpleView::executeAction);
    connect(m_statusBar, &StatusBar::actionButtonClicked, this, &SimpleView::actionButtonClicked);
  }

  void initialize() override { setupUI(); }

public:
  SimpleView(QWidget *parent = nullptr) : BaseView(parent) {}
};
