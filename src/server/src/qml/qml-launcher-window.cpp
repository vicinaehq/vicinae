#include "qml-launcher-window.hpp"
#include "qml-utils.hpp"
#include "qml-action-panel-controller.hpp"
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
  m_actionPanel = new ActionPanelController(ctx, this);

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
  rootCtx->setContextProperty(QStringLiteral("actionPanel"), m_actionPanel);

  m_engine.load(QUrl(QStringLiteral("qrc:/Vicinae/LauncherWindow.qml")));

  auto rootObjects = m_engine.rootObjects();
  if (!rootObjects.isEmpty()) {
    m_window = qobject_cast<QQuickWindow *>(rootObjects.first());
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

  // Loading state
  connect(nav, &NavigationController::loadingChanged,
          this, [this](bool loading) {
            if (m_isLoading != loading) {
              m_isLoading = loading;
              emit isLoadingChanged();
            }
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

  // Action panel changes — delegate to controller
  connect(nav, &NavigationController::actionsChanged,
          this, [this](const ActionPanelState &state) {
            m_actionPanel->setStateFrom(state);
          });

  // Close action panel on view push
  connect(nav, &NavigationController::viewPushed,
          this, [this](const BaseView *) { m_actionPanel->close(); });

  // Navigation status (left side of footer)
  connect(nav, &NavigationController::navigationStatusChanged, this,
          [this](const QString &title, const ImageURL &icon) {
            m_navigationTitle = title;
            m_navigationIcon = QmlImageUrl(icon);
            emit navigationStatusChanged();
          });

  // Completer (argument completion for root search items)
  connect(nav, &NavigationController::completionCreated,
          this, [this](const CompleterState &state) {
            m_hasCompleter = true;
            m_completerArgs.clear();
            for (const auto &arg : state.args) {
              QVariantMap map;
              map[QStringLiteral("name")] = arg.name;
              map[QStringLiteral("placeholder")] = arg.placeholder;
              map[QStringLiteral("required")] = arg.required;
              map[QStringLiteral("type")] = arg.type == CommandArgument::Password ? QStringLiteral("password")
                                          : arg.type == CommandArgument::Dropdown ? QStringLiteral("dropdown")
                                          : QStringLiteral("text");
              if (arg.data) {
                QVariantList items;
                for (const auto &d : *arg.data) {
                  items.append(QVariantMap{{QStringLiteral("title"), d.title},
                                          {QStringLiteral("value"), d.value}});
                }
                map[QStringLiteral("data")] = items;
              }
              m_completerArgs.append(map);
            }
            m_completerIcon = qml::imageSourceFor(state.icon);
            // Initialize values from state (may already have values from restore)
            m_completerValues.clear();
            for (const auto &arg : state.args) {
              m_completerValues.append(QVariantMap{{QStringLiteral("name"), arg.name},
                                                  {QStringLiteral("value"), QString()}});
            }
            emit completerChanged();
            emit completerValuesChanged();
          });

  connect(nav, &NavigationController::completionDestroyed,
          this, [this]() {
            m_hasCompleter = false;
            m_completerArgs.clear();
            m_completerIcon.clear();
            m_completerValues.clear();
            emit completerChanged();
            emit completerValuesChanged();
          });

  connect(nav, &NavigationController::completionValuesChanged,
          this, [this](const ArgumentValues &values) {
            m_completerValues.clear();
            for (const auto &[name, value] : values) {
              m_completerValues.append(QVariantMap{{QStringLiteral("name"), name},
                                                  {QStringLiteral("value"), value}});
            }
            emit completerValuesChanged();
          });

  connect(nav, &NavigationController::invalidCompletionFired,
          this, &QmlLauncherWindow::completerValidationFailed);

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
  m_actionPanel->close();
  emit commandViewPopped();
  m_viewWasPopped = true;
}

void QmlLauncherWindow::handleCurrentViewChanged() {
  auto *nav = m_ctx.navigation.get();
  bool wasPopped = m_viewWasPopped;
  m_viewWasPopped = false;

  if (nav->viewStackSize() == 1) {
    // Back to root search
    disconnect(m_searchAccessoryConnection);
    if (!m_searchAccessoryUrl.isEmpty()) {
      m_searchAccessoryUrl.clear();
      emit searchAccessoryChanged();
    }
    if (m_commandViewHost) {
      m_commandViewHost = nullptr;
      emit commandViewHostChanged();
    }
    if (m_hasCommandView) {
      m_hasCommandView = false;
      emit hasCommandViewChanged();
    }
    emit commandStackCleared();
    return;
  }

  auto *state = nav->topState();
  if (!state || !state->sender) return;

  auto *bridge = dynamic_cast<QmlBridgeViewBase *>(state->sender);
  if (!bridge) return;

  disconnect(m_searchAccessoryConnection);
  m_searchAccessoryConnection = connect(bridge, &QmlBridgeViewBase::searchAccessoryUrlChanged,
                                        this, [this, bridge]() {
    auto url = bridge->qmlSearchAccessoryUrl();
    if (m_searchAccessoryUrl != url) {
      m_searchAccessoryUrl = url;
      emit searchAccessoryChanged();
    }
  });

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
    emit commandViewPushed(bridge->qmlComponentUrl(), bridge->qmlProperties());
    bridge->loadInitialData();
  } else {
    bridge->onReactivated();
  }

  if (!m_hasCommandView) {
    m_hasCommandView = true;
    emit hasCommandViewChanged();
  }
}

void QmlLauncherWindow::forwardSearchText(const QString &text) {
  if (m_hasCommandView) {
    m_ctx.navigation->setSearchText(text);
  } else {
    // broadcastSearchText stores text without emitting searchTextTampered
    // (avoids unnecessary signal round-trip for root search)
    m_ctx.navigation->broadcastSearchText(text);
  }
}

void QmlLauncherWindow::handleReturn() {
  if (m_hasCommandView) {
    m_actionPanel->executePrimaryAction();
  } else {
    m_searchModel->activateSelected();
  }
}

bool QmlLauncherWindow::forwardKey(int key, int modifiers) {
  auto mods = static_cast<Qt::KeyboardModifiers>(modifiers);
  QKeyEvent event(QEvent::KeyPress, key, mods);

  // Check for action shortcuts first
  if (auto *action = m_actionPanel->findBoundAction(&event)) {
    m_actionPanel->executeAction(action);
    return true;
  }

  // Open search accessory selector (e.g. clipboard kind filter dropdown)
  if (Keyboard::Shortcut(Keybind::OpenSearchAccessorySelector) == &event
      && !m_searchAccessoryUrl.isEmpty()) {
    emit openSearchAccessoryRequested();
    return true;
  }

  return false;
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

void QmlLauncherWindow::setCompleterValue(int index, const QString &value) {
  auto *nav = m_ctx.navigation.get();
  auto values = nav->completionValues();
  if (index < 0 || index >= static_cast<int>(values.size())) return;
  values[index].second = value;
  nav->setCompletionValues(values);
}

