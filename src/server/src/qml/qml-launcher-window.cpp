#include "qml-launcher-window.hpp"
#include "qml-action-panel-model.hpp"
#include "qml-alert-model.hpp"
#include "qml-async-image-provider.hpp"
#include "qml-bridge-view.hpp"
#include "qml-image-source.hpp"
#include "qml-image-url.hpp"
#include "qml-root-search-model.hpp"
#include "qml-source-blend-rect.hpp"
#include "qml-config-bridge.hpp"
#include "qml-theme-bridge.hpp"
#include "navigation-controller.hpp"
#include "services/toast/toast-service.hpp"
#include "service-registry.hpp"
#include "lib/keyboard/keyboard.hpp"
#include "ui/views/base-view.hpp"
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickWindow>
#include <QQuickItem>
#include <QWindow>
#include <QKeyEvent>

QmlLauncherWindow::QmlLauncherWindow(ApplicationContext &ctx, QObject *parent)
    : QObject(parent), m_ctx(ctx) {

  // Ensure Wayland app_id / X11 WM_CLASS is "vicinae"
  QGuiApplication::setDesktopFileName(QStringLiteral("vicinae"));

  m_searchModel = new QmlRootSearchModel(ctx, this);
  m_themeBridge = new QmlThemeBridge(this);
  m_configBridge = new QmlConfigBridge(this);
  m_alertModel = new QmlAlertModel(*ctx.navigation, this);

  qRegisterMetaType<QmlImageUrl>("QmlImageUrl");

  // The engine takes ownership of the image provider
  m_engine.addImageProvider(QStringLiteral("vicinae"), new QmlAsyncImageProvider());

  m_imgSource = new QmlImageSource(this);

  auto *rootCtx = m_engine.rootContext();
  rootCtx->setContextProperty(QStringLiteral("Nav"), ctx.navigation.get());
  rootCtx->setContextProperty(QStringLiteral("searchModel"), m_searchModel);
  rootCtx->setContextProperty(QStringLiteral("Theme"), m_themeBridge);
  rootCtx->setContextProperty(QStringLiteral("Config"), m_configBridge);
  rootCtx->setContextProperty(QStringLiteral("Img"), m_imgSource);
  rootCtx->setContextProperty(QStringLiteral("launcher"), this);

  m_engine.load(QUrl(QStringLiteral("qrc:/Vicinae/LauncherWindow.qml")));

  // Cache the QQuickWindow and content area item
  auto rootObjects = m_engine.rootObjects();
  if (!rootObjects.isEmpty()) {
    m_window = qobject_cast<QQuickWindow *>(rootObjects.first());
    if (m_window) {
      m_contentArea = m_window->findChild<QQuickItem *>(QStringLiteral("contentArea"));
      if (m_contentArea) {
        connect(m_contentArea, &QQuickItem::widthChanged, this, &QmlLauncherWindow::repositionWidget);
        connect(m_contentArea, &QQuickItem::heightChanged, this, &QmlLauncherWindow::repositionWidget);
        connect(m_contentArea, &QQuickItem::xChanged, this, &QmlLauncherWindow::repositionWidget);
        connect(m_contentArea, &QQuickItem::yChanged, this, &QmlLauncherWindow::repositionWidget);
      }
    }
  }

  auto *nav = ctx.navigation.get();

  // Track window activation so toggleWindow() and closeOnFocusLoss work correctly
  if (m_window) {
    connect(m_window, &QQuickWindow::activeChanged, this, [this]() {
      m_ctx.navigation->setWindowActivated(m_window->isActive());
    });
  }

  // Connect navigation visibility
  connect(nav, &NavigationController::windowVisiblityChanged,
          this, &QmlLauncherWindow::handleVisibilityChanged);

  // View lifecycle — viewPoped fires BEFORE ViewState is destroyed
  connect(nav, &NavigationController::viewPoped,
          this, &QmlLauncherWindow::handleViewPoped);

  // currentViewChanged fires after view push/pop to indicate the new active view
  connect(nav, &NavigationController::currentViewChanged,
          this, [this](const NavigationController::ViewState &) {
            handleCurrentViewChanged();
          });

  // Search visibility (for views that don't support search, e.g. form views)
  connect(nav, &NavigationController::searchVisibilityChanged,
          this, [this](bool visible) {
            if (m_searchVisible != visible) {
              m_searchVisible = visible;
              emit searchVisibleChanged();
            }
          });

  // Search interactivity (for form views: bar visible but input disabled)
  connect(nav, &NavigationController::searchInteractiveChanged,
          this, [this](bool interactive) {
            if (m_searchInteractive != interactive) {
              m_searchInteractive = interactive;
              emit searchInteractiveChanged();
            }
          });

  // Search state
  connect(nav, &NavigationController::searchTextTampered,
          this, &QmlLauncherWindow::searchTextUpdated);

  connect(nav, &NavigationController::searchPlaceholderTextChanged,
          this, [this](const QString &text) {
            if (m_searchPlaceholder != text) {
              m_searchPlaceholder = text;
              emit searchPlaceholderChanged();
            }
          });

  // Action panel changes (for footer in command views)
  connect(nav, &NavigationController::actionsChanged,
          this, [this](const ActionPanelState &) {
            emit commandActionChanged();
            updateActionPanelModel();
          });

  // Close action panel on view push/pop
  connect(nav, &NavigationController::viewPushed,
          this, [this](const BaseView *) { closeActionPanel(); });

  // Navigation status (left side of footer)
  connect(nav, &NavigationController::navigationStatusChanged, this,
          [this](const QString &title, const ImageURL &icon) {
            m_navigationTitle = title;
            m_navigationIcon = QmlImageUrl(icon);
            emit navigationStatusChanged();
          });

  // Toast service
  auto *toast = m_ctx.services->toastService();
  connect(toast, &ToastService::toastActivated, this, [this](const Toast *t) {
    m_toastActive = true;
    m_toastTitle = t->title();
    m_toastMessage = t->message();
    m_toastStyle = static_cast<int>(t->priority());
    emit toastChanged();
    emit toastActiveChanged();
  });
  connect(toast, &ToastService::toastHidden, this, [this]() {
    m_toastActive = false;
    emit toastActiveChanged();
  });
}

void QmlLauncherWindow::handleVisibilityChanged(bool visible) {
  if (!m_window) return;

  if (visible) {
    m_window->show();
    m_window->raise();
    m_window->requestActivate();
  } else {
    m_window->hide();
  }
}

void QmlLauncherWindow::handleViewPoped(const BaseView *view) {
  closeActionPanel();

  // Clean up BEFORE the ViewState is destroyed (which calls deleteLater on the view)
  if (m_activeWidget == view) {
    m_activeWidget->hide();
    if (auto *wh = m_activeWidget->windowHandle()) {
      wh->setParent(nullptr);
    }
    m_activeWidget = nullptr;
  }

  if (dynamic_cast<const QmlBridgeViewBase *>(view)) {
    emit commandViewPopped();
  }

  m_viewWasPopped = true;
}

void QmlLauncherWindow::handleCurrentViewChanged() {
  auto *nav = m_ctx.navigation.get();
  bool wasPopped = m_viewWasPopped;
  m_viewWasPopped = false;

  if (nav->viewStackSize() == 1) {
    // Back to root search
    removeWidget();
    if (!m_searchAccessoryUrl.isEmpty()) {
      m_searchAccessoryUrl.clear();
      emit searchAccessoryChanged();
    }
    if (m_commandViewHost) {
      m_commandViewHost = nullptr;
      emit commandViewHostChanged();
    }
    emit commandStackCleared();
    return;
  }

  // Command view — check if it's a QML bridge or a widget
  auto *state = nav->topState();
  if (!state || !state->sender) return;

  if (auto *bridge = dynamic_cast<QmlBridgeViewBase *>(state->sender)) {
    // Detach any previous widget without changing hasCommandView
    if (m_activeWidget) {
      m_activeWidget->hide();
      if (auto *wh = m_activeWidget->windowHandle()) wh->setParent(nullptr);
      m_activeWidget = nullptr;
    }

    // Update search accessory and host for this bridge view
    auto newAccessoryUrl = bridge->qmlSearchAccessoryUrl();
    if (m_searchAccessoryUrl != newAccessoryUrl) {
      m_searchAccessoryUrl = newAccessoryUrl;
      emit searchAccessoryChanged();
    }
    if (m_commandViewHost != bridge) {
      m_commandViewHost = bridge;
      emit commandViewHostChanged();
    }

    if (!wasPopped) {
      // New bridge view pushed — add to QML stack
      emit commandViewPushed(bridge->qmlComponentUrl(), bridge->qmlProperties());
      bridge->loadInitialData();
    } else {
      // Popped back — QML component preserved by StackView; let the view re-establish state.
      bridge->onReactivated();
    }
  } else if (state->sender != m_activeWidget) {
    // Widget view — embed on top; command stack stays behind
    embedWidget(state->sender);
  }

  if (!m_hasCommandView) {
    m_hasCommandView = true;
    emit hasCommandViewChanged();
  }

  emit commandActionChanged();
}

void QmlLauncherWindow::embedWidget(BaseView *view) {
  if (m_activeWidget && m_activeWidget != view) {
    m_activeWidget->hide();
    if (auto *wh = m_activeWidget->windowHandle()) {
      wh->setParent(nullptr);
    }
  }

  m_activeWidget = view;
  if (!m_window) return;

  // Force native window creation and reparent as child of the QQuickWindow
  view->winId();
  if (auto *wh = view->windowHandle()) {
    wh->setParent(m_window);
  }

  repositionWidget();
  view->show();
}

void QmlLauncherWindow::removeWidget() {
  if (m_activeWidget) {
    m_activeWidget->hide();
    if (auto *wh = m_activeWidget->windowHandle()) {
      wh->setParent(nullptr);
    }
    m_activeWidget = nullptr;
  }

  if (m_hasCommandView) {
    m_hasCommandView = false;
    emit hasCommandViewChanged();
  }
}

void QmlLauncherWindow::repositionWidget() {
  if (!m_activeWidget || !m_contentArea || !m_window) return;

  QPointF scenePos = m_contentArea->mapToScene(QPointF(0, 0));
  m_activeWidget->setGeometry(
      qRound(scenePos.x()), qRound(scenePos.y()),
      qRound(m_contentArea->width()), qRound(m_contentArea->height()));
}

void QmlLauncherWindow::forwardSearchText(const QString &text) {
  if (m_hasCommandView) {
    // setSearchText stores text AND emits searchTextTampered
    m_ctx.navigation->setSearchText(text);
    // Forward to the current view (handles both widget and QML bridge views)
    if (auto *state = m_ctx.navigation->topState(); state && state->sender) {
      state->sender->textChanged(text);
    }
  } else {
    // broadcastSearchText stores text without emitting searchTextTampered
    // (avoids unnecessary signal round-trip for root search)
    m_ctx.navigation->broadcastSearchText(text);
  }
}

void QmlLauncherWindow::handleReturn() {
  if (m_hasCommandView) {
    m_ctx.navigation->executePrimaryAction();
  } else {
    m_searchModel->activateSelected();
  }
}

bool QmlLauncherWindow::forwardKey(int key, int modifiers) {
  auto mods = static_cast<Qt::KeyboardModifiers>(modifiers);
  QKeyEvent event(QEvent::KeyPress, key, mods);

  // Check for action shortcuts first
  if (auto *action = m_ctx.navigation->findBoundAction(&event)) {
    m_ctx.navigation->executeAction(action);
    return true;
  }

  // Open search accessory selector (e.g. clipboard kind filter dropdown)
  if (Keyboard::Shortcut(Keybind::OpenSearchAccessorySelector) == &event
      && !m_searchAccessoryUrl.isEmpty()) {
    emit openSearchAccessoryRequested();
    return true;
  }

  if (!m_activeWidget) return false;
  return m_activeWidget->inputFilter(&event);
}

void QmlLauncherWindow::goBack() {
  m_ctx.navigation->goBack();
  emit viewNavigatedBack();
}

void QmlLauncherWindow::popToRoot() {
  m_ctx.navigation->popToRoot();
  emit viewNavigatedBack();
}

bool QmlLauncherWindow::tryAliasFastTrack() {
  return m_searchModel->tryAliasFastTrack();
}

QString QmlLauncherWindow::commandActionTitle() const {
  auto *state = m_ctx.navigation->topState();
  if (!state || !state->actionPanelState) return {};
  auto *action = state->actionPanelState->primaryAction();
  return action ? action->title() : QString();
}

QString QmlLauncherWindow::commandActionShortcut() const {
  auto *state = m_ctx.navigation->topState();
  if (!state || !state->actionPanelState) return {};
  auto *action = state->actionPanelState->primaryAction();
  if (!action) return {};
  auto shortcut = action->shortcut().value_or(Keyboard::Shortcut::enter());
  return shortcut.toDisplayString();
}

void QmlLauncherWindow::toggleActionPanel() {
  if (m_actionPanelOpen) {
    closeActionPanel();
  } else {
    if (!m_actionPanelModel) return;
    m_actionPanelOpen = true;
    emit actionPanelOpenChanged();
  }
}

void QmlLauncherWindow::closeActionPanel() {
  if (!m_actionPanelOpen) return;
  m_actionPanelOpen = false;
  emit actionPanelOpenChanged();
}

void QmlLauncherWindow::updateActionPanelModel() {
  auto *state = m_ctx.navigation->topState();
  bool newHasActions = state && state->actionPanelState && state->actionPanelState->actionCount() > 0;
  bool newHasMultiple = state && state->actionPanelState && state->actionPanelState->actionCount() > 1;

  if (newHasActions != m_hasActions) {
    m_hasActions = newHasActions;
    emit hasActionsChanged();
  }

  if (newHasMultiple != m_hasMultipleActions) {
    m_hasMultipleActions = newHasMultiple;
    emit hasMultipleActionsChanged();
  }

  if (!state || !state->actionPanelState) {
    if (m_actionPanelModel) {
      m_actionPanelModel->deleteLater();
      m_actionPanelModel = nullptr;
      emit actionPanelModelChanged();
      closeActionPanel();
    }
    return;
  }

  auto *newModel = new QmlActionPanelModel(state->actionPanelState.get(), this);
  connectActionPanelModel(newModel);

  if (m_actionPanelModel) {
    m_actionPanelModel->deleteLater();
  }
  m_actionPanelModel = newModel;
  emit actionPanelModelChanged();
}

void QmlLauncherWindow::connectActionPanelModel(QmlActionPanelModel *model) {
  connect(model, &QmlActionPanelModel::actionExecuted, this, [this](AbstractAction *action) {
    m_ctx.navigation->executeAction(action);
    closeActionPanel();
  });

  connect(model, &QmlActionPanelModel::submenuRequested, this, [this](QmlActionPanelModel *subModel) {
    connectActionPanelModel(subModel);
    emit actionPanelSubmenuPushed(subModel);
  });

  connect(model, &QmlActionPanelModel::closeRequested, this, [this]() {
    closeActionPanel();
  });
}
