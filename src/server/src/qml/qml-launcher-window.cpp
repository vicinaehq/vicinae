#include "qml-launcher-window.hpp"
#include "qml-bridge-view.hpp"
#include "qml-image-provider.hpp"
#include "qml-root-search-model.hpp"
#include "qml-source-blend-rect.hpp"
#include "qml-config-bridge.hpp"
#include "qml-theme-bridge.hpp"
#include "navigation-controller.hpp"
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

  // Register custom QML types
  qmlRegisterType<QmlSourceBlendRect>("Vicinae", 1, 0, "SourceBlendRect");

  // The engine takes ownership of the image provider
  m_engine.addImageProvider(QStringLiteral("vicinae"), new QmlImageProvider());

  auto *rootCtx = m_engine.rootContext();
  rootCtx->setContextProperty(QStringLiteral("Nav"), ctx.navigation.get());
  rootCtx->setContextProperty(QStringLiteral("searchModel"), m_searchModel);
  rootCtx->setContextProperty(QStringLiteral("Theme"), m_themeBridge);
  rootCtx->setContextProperty(QStringLiteral("Config"), m_configBridge);
  rootCtx->setContextProperty(QStringLiteral("launcher"), this);

  m_engine.load(QUrl(QStringLiteral("qrc:/qml/LauncherWindow.qml")));

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

void QmlLauncherWindow::forwardKey(int key, int modifiers) {
  if (!m_activeWidget) return;
  auto mods = static_cast<Qt::KeyboardModifiers>(modifiers);
  QKeyEvent event(QEvent::KeyPress, key, mods);
  m_activeWidget->inputFilter(&event);
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
