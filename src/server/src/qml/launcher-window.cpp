#include "launcher-window.hpp"
#include "hud-bridge.hpp"
#include "keybind-bridge.hpp"
#include "view-utils.hpp"
#include "action-panel-controller.hpp"
#include "alert-model.hpp"
#include "async-image-provider.hpp"
#include "bridge-view.hpp"
#include "image-source.hpp"
#include "image-url.hpp"
#include "root-search-model.hpp"
#include "config-bridge.hpp"
#include "theme-bridge.hpp"
#include "navigation-controller.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "settings-controller/settings-controller.hpp"
#include "services/keybinding/keybinding-service.hpp"
#include "services/toast/toast-service.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "services/background-effect/background-effect-manager.hpp"
#include "services/window-manager/window-manager.hpp"
#include "environment.hpp"
#include "vicinae.hpp"
#include "lib/keyboard/keyboard.hpp"
#include "ui/views/base-view.hpp"
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickWindow>
#include <QWindow>
#include <QKeyEvent>
#ifdef WAYLAND_LAYER_SHELL
#include <LayerShellQt/Window>
#include <utility>
#endif

LauncherWindow::LauncherWindow(ApplicationContext &ctx, QObject *parent)
    : QObject(parent), m_ctx(ctx), m_actionPanel(new ActionPanelController(ctx, this)),
      m_alertModel(new AlertModel(*ctx.navigation, this)), m_configBridge(new ConfigBridge(this)),
      m_imgSource(new ImageSource(this)), m_keybindProxy(new KeybindBridge(this)),
      m_themeBridge(new ThemeBridge(this)) {

  // Ensure Wayland app_id / X11 WM_CLASS is "vicinae"
  QGuiApplication::setDesktopFileName(QStringLiteral("vicinae"));

  m_searchModel = new RootSearchModel(ViewScope(&ctx, ctx.navigation->topState()->sender), this);

  qRegisterMetaType<ImageUrl>("ImageUrl");

  m_engine.addImageProvider(QStringLiteral("vicinae"), new AsyncImageProvider());

  auto *rootCtx = m_engine.rootContext();
  rootCtx->setContextProperty(QStringLiteral("Nav"), ctx.navigation.get());
  rootCtx->setContextProperty(QStringLiteral("searchModel"), m_searchModel);
  rootCtx->setContextProperty(QStringLiteral("Theme"), m_themeBridge);
  rootCtx->setContextProperty(QStringLiteral("Config"), m_configBridge);
  rootCtx->setContextProperty(QStringLiteral("Img"), m_imgSource);

  rootCtx->setContextProperty(QStringLiteral("launcher"), this);
  rootCtx->setContextProperty(QStringLiteral("actionPanel"), m_actionPanel);
  rootCtx->setContextProperty(QStringLiteral("Keybinds"), m_keybindProxy);

  m_engine.load(QUrl(QStringLiteral("qrc:/Vicinae/LauncherWindow.qml")));

  auto rootObjects = m_engine.rootObjects();
  if (!rootObjects.isEmpty()) { m_window = qobject_cast<QQuickWindow *>(rootObjects.first()); }

  applyWindowConfig();

  if (!Environment::isHudDisabled()) {
    m_hudBridge = new HudBridge(this);
    rootCtx->setContextProperty(QStringLiteral("hud"), m_hudBridge);
    m_engine.load(QUrl(QStringLiteral("qrc:/Vicinae/HudWindow.qml")));
  }

  auto *nav = ctx.navigation.get();

  // Track window activation so toggleWindow() and closeOnFocusLoss work correctly
  if (m_window) {
    connect(m_window, &QQuickWindow::activeChanged, this,
            [this]() { m_ctx.navigation->setWindowActivated(m_window->isActive()); });
    m_window->installEventFilter(this);
  }

  m_closeOnFocusLoss = ctx.services->config()->value().closeOnFocusLoss;

  connect(nav, &NavigationController::windowVisiblityChanged, this, &LauncherWindow::handleVisibilityChanged);

  // View lifecycle - viewPoped fires BEFORE ViewState is destroyed
  connect(nav, &NavigationController::viewPoped, this, &LauncherWindow::handleViewPoped);
  connect(nav, &NavigationController::viewReplaced, this, [this]() { m_viewWasReplaced = true; });

  connect(nav, &NavigationController::currentViewChanged, this,
          [this](const NavigationController::ViewState &) { handleCurrentViewChanged(); });

  connect(nav, &NavigationController::loadingChanged, this, [this](bool loading) {
    if (m_isLoading != loading) {
      m_isLoading = loading;
      emit isLoadingChanged();
    }
  });

  connect(nav, &NavigationController::searchVisibilityChanged, this, [this](bool visible) {
    if (m_searchVisible != visible) {
      m_searchVisible = visible;
      emit searchVisibleChanged();
    }
  });

  // Search interactivity (for form views: bar visible but input disabled)
  connect(nav, &NavigationController::searchInteractiveChanged, this, [this](bool interactive) {
    if (m_searchInteractive != interactive) {
      m_searchInteractive = interactive;
      emit searchInteractiveChanged();
    }
  });

  // Window size override (e.g. dmenu --width/--height)
  connect(nav, &NavigationController::windowSizeRequested, this, [this](QSize size) {
    m_overrideWidth = size.width();
    m_overrideHeight = size.height();
    emit windowSizeOverrideChanged();
  });

  connect(nav, &NavigationController::backButtonVisibilityChanged, this, [this](bool visible) {
    if (m_showBackButton != visible) {
      m_showBackButton = visible;
      emit showBackButtonChanged();
    }
  });

  connect(nav, &NavigationController::statusBarVisiblityChanged, this, [this](bool visible) {
    if (m_statusBarVisible != visible) {
      m_statusBarVisible = visible;
      emit statusBarVisibleChanged();
    }
  });

  // Search state — programmatic text changes (e.g. from extensions)
  connect(nav, &NavigationController::searchTextTampered, this, [this](const QString &text) {
    emit searchTextUpdated(text);
    tryCompaction();
  });

  connect(nav, &NavigationController::searchPlaceholderTextChanged, this, [this](const QString &text) {
    if (m_searchPlaceholder != text) {
      m_searchPlaceholder = text;
      emit searchPlaceholderChanged();
    }
  });

  connect(nav, &NavigationController::actionsChanged, this,
          [this](const ActionPanelState &state) { m_actionPanel->setStateFrom(state); });

  connect(nav, &NavigationController::viewPushed, this, [this](const BaseView *) { m_actionPanel->close(); });

  connect(nav, &NavigationController::navigationStatusChanged, this,
          [this](const QString &title, const ImageURL &icon) {
            qDebug() << "[NAV] title changed" << title;
            m_navigationTitle = title;
            m_navigationIcon = ImageUrl(icon);
            emit navigationStatusChanged();
          });

  connect(nav, &NavigationController::completionCreated, this, [this](const CompleterState &state) {
    m_hasCompleter = true;
    m_completerArgs.clear();
    for (const auto &arg : state.args) {
      QVariantMap map;
      map[QStringLiteral("name")] = arg.name;
      map[QStringLiteral("placeholder")] = arg.placeholder;
      map[QStringLiteral("required")] = arg.required;
      map[QStringLiteral("type")] = arg.type == CommandArgument::Password   ? QStringLiteral("password")
                                    : arg.type == CommandArgument::Dropdown ? QStringLiteral("dropdown")
                                                                            : QStringLiteral("text");
      if (arg.data) {
        QVariantList items;
        for (const auto &d : *arg.data) {
          items.append(QVariantMap{{QStringLiteral("title"), d.title}, {QStringLiteral("value"), d.value}});
        }
        map[QStringLiteral("data")] = items;
      }
      m_completerArgs.append(map);
    }
    m_completerIcon = qml::imageSourceFor(state.icon);
    // Initialize values from state (may already have values from restore)
    m_completerValues.clear();
    for (const auto &arg : state.args) {
      m_completerValues.append(
          QVariantMap{{QStringLiteral("name"), arg.name}, {QStringLiteral("value"), QString()}});
    }
    emit completerChanged();
    emit completerValuesChanged();
  });

  connect(nav, &NavigationController::completionDestroyed, this, [this]() {
    m_hasCompleter = false;
    m_completerArgs.clear();
    m_completerIcon.clear();
    m_completerValues.clear();
    emit completerChanged();
    emit completerValuesChanged();
  });

  connect(nav, &NavigationController::completionValuesChanged, this, [this](const ArgumentValues &values) {
    m_completerValues.clear();
    for (const auto &[name, value] : values) {
      m_completerValues.append(QVariantMap{{QStringLiteral("name"), name}, {QStringLiteral("value"), value}});
    }
    emit completerValuesChanged();
  });

  connect(nav, &NavigationController::invalidCompletionFired, this,
          &LauncherWindow::completerValidationFailed);

  if (m_hudBridge) { connect(nav, &NavigationController::showHudRequested, m_hudBridge, &HudBridge::show); }

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

  connect(m_ctx.services->config(), &config::Manager::configChanged, this,
          [this](const auto &, const auto &) {
            m_closeOnFocusLoss = m_ctx.services->config()->value().closeOnFocusLoss;
            applyWindowConfig();
            tryCompaction();
          });

  connect(ctx.overlay.get(), &OverlayController::overlayChanged, this, [this]() {
    m_hasOverlay = m_ctx.overlay->hasOverlay();
    m_overlayHost = m_ctx.overlay->current();

    if (m_overlayHost) {
      auto url = m_overlayHost->property("qmlComponentUrl");
      m_overlayUrl = url.isValid() ? url.toUrl() : QUrl();
    } else {
      m_overlayUrl.clear();
    }

    emit overlayChanged();
  });
}

bool LauncherWindow::eventFilter(QObject *obj, QEvent *event) {
  // only works on some compositors.
  // we could probably make it work everywhere layer shell is supported by adding a layer behind ours.
  if (obj == m_window && event->type() == QEvent::MouseMove && m_closeOnFocusLoss) {
    auto *me = static_cast<QMouseEvent *>(event); // NOLINT
    QRect const contentRect(0, 0, m_window->width(), m_window->height());
    if (!contentRect.contains(me->position().toPoint())) { m_ctx.navigation->closeWindow(); }
  }
  return QObject::eventFilter(obj, event);
}

void LauncherWindow::handleVisibilityChanged(bool visible) {
  if (!m_window) return;

  if (visible) {
    applyWindowConfig();
    tryCompaction();
    m_window->show();
    m_window->raise();
    m_window->requestActivate();
  } else {
    m_window->hide();
  }
}

void LauncherWindow::handleViewPoped(const BaseView *view) {
  m_actionPanel->close();
  emit commandViewPopped();
  m_viewWasPopped = true;
}

void LauncherWindow::handleCurrentViewChanged() {
  auto *nav = m_ctx.navigation.get();
  bool const wasPopped = m_viewWasPopped;
  bool const wasReplaced = m_viewWasReplaced;
  m_viewWasPopped = false;
  m_viewWasReplaced = false;

  if (nav->viewStackSize() == 1) {
    disconnect(m_searchAccessoryConnection);
    if (!m_searchAccessoryUrl.isEmpty()) {
      m_searchAccessoryUrl.clear();
      emit searchAccessoryChanged();
    }
    if (m_commandViewHost) {
      m_commandViewHost = nullptr;
      emit commandViewHostChanged();
    }
    if (!m_isRootSearch) {
      m_isRootSearch = true;
      emit isRootSearchChanged();
    }
    if (!m_showBackButton) {
      m_showBackButton = true;
      emit showBackButtonChanged();
    }
    if (m_overrideWidth != 0 || m_overrideHeight != 0) {
      m_overrideWidth = 0;
      m_overrideHeight = 0;
      emit windowSizeOverrideChanged();
    }
    emit commandStackCleared();
    tryCompaction();
    return;
  }

  auto *state = nav->topState();
  if (!state || !state->sender) return;

  auto *bridge = dynamic_cast<ViewHostBase *>(state->sender);
  if (!bridge) return;

  disconnect(m_searchAccessoryConnection);
  m_searchAccessoryConnection =
      connect(bridge, &ViewHostBase::searchAccessoryUrlChanged, this, [this, bridge]() {
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
    if (wasReplaced) {
      emit commandViewReplaced(bridge->qmlComponentUrl(), bridge->qmlProperties());
    } else {
      emit commandViewPushed(bridge->qmlComponentUrl(), bridge->qmlProperties());
    }
    bridge->loadInitialData();
  } else {
    bridge->onReactivated();
  }

  if (m_isRootSearch) {
    m_isRootSearch = false;
    emit isRootSearchChanged();
  }
  tryCompaction();
}

void LauncherWindow::forwardSearchText(const QString &text) {
  m_ctx.navigation->broadcastSearchText(text);
  tryCompaction();
}

void LauncherWindow::handleReturn() {
  if (!m_isRootSearch) {
    m_actionPanel->executePrimaryAction();
  } else {
    m_searchModel->activateSelected();
  }
}

bool LauncherWindow::forwardKey(int key, int modifiers) {
  auto mods = static_cast<Qt::KeyboardModifiers>(modifiers);

  if (mods == Qt::NoModifier) return false;

  switch (key) {
  case Qt::Key_Shift:
  case Qt::Key_Control:
  case Qt::Key_Alt:
  case Qt::Key_Meta:
    return false;
  default:
    break;
  }

  // shortcuts that actions cannot override
  if (mods == Qt::ControlModifier) {
    switch (key) {
    case Qt::Key_A:
    case Qt::Key_C:
    case Qt::Key_V:
    case Qt::Key_Z:
      return false;
    default:
      break;
    }
  }

  QKeyEvent const event(QEvent::KeyPress, key, mods);

  if (auto *action = m_actionPanel->findBoundAction(&event)) {
    m_actionPanel->executeAction(action);
    return true;
  }

  if (Keyboard::Shortcut(Keybind::OpenSettings) == &event) {
    m_ctx.navigation->closeWindow();
    m_ctx.settings->openWindow();
    return true;
  }

  return false;
}

void LauncherWindow::goBack() {
  m_ctx.navigation->goBack();
  emit viewNavigatedBack();
}

void LauncherWindow::handleEscape() {
  if (m_ctx.services->config()->value().escapeKeyBehavior == "close_window") {
    m_ctx.navigation->closeWindow();
    return;
  }

  goBack();
}

void LauncherWindow::popToRoot() {
  m_ctx.navigation->popToRoot();
  emit viewNavigatedBack();
}

bool LauncherWindow::popOnBackspace() { return m_ctx.services->config()->value().popOnBackspace; }

bool LauncherWindow::tryAliasFastTrack() { return m_searchModel->tryAliasFastTrack(); }

int LauncherWindow::matchNavigationKey(int key, int modifiers) {
  return KeyBindingService::matchNavigation(key, modifiers, m_ctx.services->config()->value().keybinding);
}

void LauncherWindow::setCompleterValue(int index, const QString &value) {
  auto *nav = m_ctx.navigation.get();
  auto values = nav->completionValues();
  if (index < 0 || std::cmp_greater_equal(index, values.size())) return;
  values[index].second = value;
  nav->setCompletionValues(values);
}

void LauncherWindow::expand() { setCompacted(false); }

void LauncherWindow::setCompacted(bool value) {
  if (m_compacted == value) return;
  m_compacted = value;
  emit compactedChanged();
  updateBlur();
}

void LauncherWindow::tryCompaction() {
  auto &cfg = m_ctx.services->config()->value().launcherWindow.compactMode;
  setCompacted(cfg.enabled && m_ctx.navigation->searchText().isEmpty() &&
               m_ctx.navigation->viewStackSize() == 1);
}

void LauncherWindow::updateBlur() {
  if (!m_window) return;
  auto &cfg = m_ctx.services->config()->value().launcherWindow;
  auto *bgEffect = m_ctx.services->backgroundEffectManager();
  int const rounding = cfg.clientSideDecorations.enabled ? cfg.clientSideDecorations.rounding : 0;

  if (!bgEffect->supportsBlur()) return;

  if (cfg.blur.enabled) {
    QRect region(0, 0, m_window->width(), m_window->height());
    if (m_compacted) region.setHeight(60 + 2 * cfg.clientSideDecorations.borderWidth);
    bgEffect->setBlur(m_window, {.radius = rounding, .region = region});
  } else {
    bgEffect->clearBlur(m_window);
  }
}

void LauncherWindow::applyWindowConfig() {
  if (!m_window) return;
  auto &cfg = m_ctx.services->config()->value().launcherWindow;

  updateBlur();
  m_ctx.services->windowManager()->provider()->setDimAround(cfg.dimAround);

#ifdef WAYLAND_LAYER_SHELL
  const auto &lc = cfg.layerShell;
  if (Environment::isLayerShellSupported() && lc.enabled) {
    namespace Shell = LayerShellQt;
    if (auto *lshell = Shell::Window::get(m_window)) {
      lshell->setLayer(lc.layer == "overlay" ? Shell::Window::LayerOverlay : Shell::Window::LayerTop);
      lshell->setScope(Omnicast::LAYER_SCOPE);
      // TODO: migrate to this when newer version is widely available
      // lshell->setWantsToBeOnActiveScreen(true);
      lshell->setScreenConfiguration(Shell::Window::ScreenConfiguration::ScreenFromCompositor);
      lshell->setAnchors(Shell::Window::AnchorNone);
      lshell->setKeyboardInteractivity(lc.keyboardInteractivity == "exclusive"
                                           ? Shell::Window::KeyboardInteractivityExclusive
                                           : Shell::Window::KeyboardInteractivityOnDemand);
    }
  }
#endif
}
