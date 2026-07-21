#include "launcher-window.hpp"
#include "launcher-window-platform.hpp"
#include "hud-bridge.hpp"
#include "keybind-bridge.hpp"
#include "keyboard-bridge.hpp"
#include "view-utils.hpp"
#include "action-panel-controller.hpp"
#include "ui/image/image-renderer.hpp"
#include "services/news/news-service.hpp"
#include "alert-model.hpp"
#include "bridge-view.hpp"
#include "image-source.hpp"
#include "image-url.hpp"
#include "config-bridge.hpp"
#include "platform-bridge.hpp"
#include "theme-bridge.hpp"
#include "navigation-controller.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "extensions/vicinae/bug-report-url.hpp"
#include "qml/vicinae-store-view-host.hpp"
#include "settings-controller/settings-controller.hpp"
#include "services/toast/toast-service.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/file-chooser/file-chooser-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "environment.hpp"
#include "vicinae.hpp"
#include "internal/keyboard/keyboard.hpp"
#include "ui/views/base-view.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include <QCursor>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickWindow>
#include <QScreen>
#include <QWindow>
#include <QKeyEvent>
#include <qcoreevent.h>
#include <qlogging.h>
#include <memory>

#ifdef __GLIBC__
#include <malloc.h>
#endif

LauncherWindow::LauncherWindow(ApplicationContext &ctx, QObject *parent)
    : QObject(parent), m_ctx(ctx), m_actionPanel(new ActionPanelController(ctx, this)),
      m_footerPanel(new ActionPanelController(ctx, this)),
      m_alertModel(new AlertModel(*ctx.navigation, this)), m_configBridge(new ConfigBridge(this)),
      m_imgSource(new ImageSource(this)), m_keybindProxy(new KeybindBridge(this)),
      m_keyboardBridge(new KeyboardBridge(this)), m_platformBridge(new PlatformBridge(this)),
      m_themeBridge(new ThemeBridge(this)) {

#ifndef Q_OS_MACOS
  // Ensure Wayland app_id / X11 WM_CLASS is "vicinae"
  QGuiApplication::setDesktopFileName(QStringLiteral("vicinae"));
#endif

  qRegisterMetaType<ImageUrl>("ImageUrl");

  auto *rootCtx = m_engine.rootContext();
  rootCtx->setContextProperty(QStringLiteral("Nav"), ctx.navigation.get());
  rootCtx->setContextProperty(QStringLiteral("Theme"), m_themeBridge);
  rootCtx->setContextProperty(QStringLiteral("Config"), m_configBridge);
  rootCtx->setContextProperty(QStringLiteral("Platform"), m_platformBridge);
  rootCtx->setContextProperty(QStringLiteral("Img"), m_imgSource);

  rootCtx->setContextProperty(QStringLiteral("launcher"), this);
  rootCtx->setContextProperty(QStringLiteral("actionPanel"), m_actionPanel);
  rootCtx->setContextProperty(QStringLiteral("footerPanel"), m_footerPanel);
  rootCtx->setContextProperty(QStringLiteral("Keybinds"), m_keybindProxy);
  rootCtx->setContextProperty(QStringLiteral("Keyboard"), m_keyboardBridge);
  rootCtx->setContextProperty(QStringLiteral("FileChooser"), ctx.services->fileChooserService());

  updateLayerShellProps();
  buildFooterMenu();

  m_engine.load(QUrl(
#if defined(Q_OS_MACOS)
      QStringLiteral("qrc:/Vicinae/LauncherWindowMacOS.qml")
#elif defined(Q_OS_WIN)
      QStringLiteral("qrc:/Vicinae/LauncherWindowWindows.qml")
#else
      isLayerShellActive() ? QStringLiteral("qrc:/Vicinae/LauncherWindowLayerShell.qml")
                           : QStringLiteral("qrc:/Vicinae/LauncherWindow.qml")
#endif
          ));

  auto rootObjects = m_engine.rootObjects();
  if (!rootObjects.isEmpty()) { m_window = qobject_cast<QQuickWindow *>(rootObjects.first()); }

  applyWindowConfig();

  if (!Environment::isHudDisabled()) {
    m_hudBridge = new HudBridge(this);
    rootCtx->setContextProperty(QStringLiteral("hud"), m_hudBridge);
    m_engine.load(QUrl(
#if defined(Q_OS_MACOS)
        QStringLiteral("qrc:/Vicinae/HudWindowMacOS.qml")
#elif defined(Q_OS_WIN)
        QStringLiteral("qrc:/Vicinae/HudWindowWindows.qml")
#else
        QStringLiteral("qrc:/Vicinae/HudWindowLayerShell.qml")
#endif
            ));
  }

  auto *nav = ctx.navigation.get();

  // Track window activation so toggleWindow() and closeOnFocusLoss work correctly
  if (m_window) {
    nav->setWindow(m_window);
    connect(m_window, &QQuickWindow::activeChanged, this,
            [this]() { m_ctx.navigation->setWindowActivated(m_window->isActive()); });
    m_window->installEventFilter(this);
  }

  using namespace std::chrono_literals;

  // Sometime after we close the window, we release some of our cached resources to lower
  // memory usage.
  static constexpr auto CACHE_EVICTION_DELAY = 10s;

  m_cacheEvictionTimer.setSingleShot(true);
  m_cacheEvictionTimer.setInterval(CACHE_EVICTION_DELAY);

  connect(&m_cacheEvictionTimer, &QTimer::timeout, this, [this]() {
    if (m_window) m_window->releaseResources();
    m_engine.trimComponentCache();
    ImageRendering::clearCache();
#ifdef __GLIBC__
    malloc_trim(0);
#endif
  });

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

  // Search state: programmatic text changes (e.g. from extensions)
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

  connect(nav, &NavigationController::activeActionPanelChanged, this,
          [this, nav]() { m_actionPanel->syncToView(nav->topState()->sender); });

  connect(nav, &NavigationController::viewPushed, this, [this](const BaseView *) {
    m_actionPanel->close();
    m_footerPanel->close();
  });

  connect(m_footerPanel, &ActionPanelController::openChanged, this, [this]() {
    if (m_footerPanel->isOpen()) m_actionPanel->close();
  });
  connect(m_actionPanel, &ActionPanelController::openChanged, this, [this]() {
    if (m_actionPanel->isOpen()) m_footerPanel->close();
  });

  connect(nav, &NavigationController::navigationStatusChanged, this,
          [this](const QString &title, const ImageURL &icon) {
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
    tryCompaction(); // we don't want to compact if there is a toast to show
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

  auto *fileChooser = m_ctx.services->fileChooserService();
  connect(fileChooser, &FileChooserService::dialogOpened, this, [this]() {
    if (!m_window || !m_window->isActive()) return;
    m_pendingLauncherFileChoice = true;
    setExclusiveFocus(false);
    if (isLayerShellActive()) { m_ctx.navigation->closeWindow({.popToRootType = PopToRootType::Suspended}); }
  });
  connect(fileChooser, &FileChooserService::dialogClosed, this, [this]() {
    if (m_pendingLauncherFileChoice) {
      setExclusiveFocus(true);
      if (isLayerShellActive()) { m_ctx.navigation->showWindow(); }
      m_pendingLauncherFileChoice = false;
    }
  });
  connect(nav, &NavigationController::viewPoped, this, [this, fileChooser](const BaseView *) {
    if (m_window && m_window->isActive() && fileChooser->isActive()) fileChooser->cancel();
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
  if (obj != m_window) return QObject::eventFilter(obj, event);

  // makes sure we keep our state in sync if the compositor is the one raising/closing the window for some
  // reason. typically useful to handle close on focus loss on gnome with the gnome extension
  if (event->type() == QEvent::Show) {
    m_ctx.navigation->showWindow();
  } else if (event->type() == QEvent::Hide) {
    m_ctx.navigation->closeWindow();
  }

  else if (event->type() == QEvent::KeyPress) {
    auto *ke = static_cast<QKeyEvent *>(event); // NOLINT
    // KeypadModifier marks key origin, not user intent; strip it so numpad
    // arrows compare equal to main-keyboard arrows downstream.
    if (ke->modifiers().testFlag(Qt::KeypadModifier)) {
      ke->setModifiers(ke->modifiers() & ~Qt::KeypadModifier);
    }
    // the current view host gets first pick at any key press, unless a component
    // that owns the keyboard (overlay, alert, action panel) is up.
    const bool viewOwnsInput =
        !m_hasOverlay && !m_alertModel->visible() && !m_actionPanel->isOpen() && !m_footerPanel->isOpen();
    if (viewOwnsInput) {
      auto *host = dynamic_cast<ViewHostBase *>(m_commandViewHost);
      if (host && host->inputFilter(ke)) return true;
    }
    // unmodified keys (return included) belong to the focused component: forwarding
    // them globally would steal text input or returns meant for local widgets such as
    // text areas, overlays or the opened action panel.
    if (ke->modifiers() != Qt::NoModifier && forwardKey(ke->key(), static_cast<int>(ke->modifiers()))) {
      return true;
    }
  }

  // only works on some compositors.
  // we could probably make it work everywhere layer shell is supported by adding a layer behind ours.
  else if (event->type() == QEvent::MouseMove && m_closeOnFocusLoss) {
    auto *me = static_cast<QMouseEvent *>(event); // NOLINT
    QRect const contentRect(0, 0, m_window->width(), m_window->height());
    if (!contentRect.contains(me->position().toPoint())) { m_ctx.navigation->closeWindow(); }
  }

  return QObject::eventFilter(obj, event);
}

void LauncherWindow::handleVisibilityChanged(bool visible) {
  if (!m_window) return;

  if (visible) {
    m_cacheEvictionTimer.stop();
    applyWindowConfig();
    tryCompaction();
    m_window->show();
    m_window->raise();
    LauncherWindowPlatform::grantForeground();
    m_window->requestActivate();
  } else {
    LauncherWindowPlatform::suppressHeldKeyReleases();
    m_window->hide();
    m_cacheEvictionTimer.start();
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

  auto *state = nav->topState();
  if (!state || !state->sender) return;

  auto *bridge = dynamic_cast<ViewHostBase *>(state->sender);
  if (!bridge) return;

  bool const isRoot = nav->viewStackSize() == 1;
  if (m_atRoot != isRoot) {
    m_atRoot = isRoot;
    emit atRootChanged();
  }

  if (m_overrideWidth != 0 || m_overrideHeight != 0) {
    m_overrideWidth = 0;
    m_overrideHeight = 0;
    emit windowSizeOverrideChanged();
  }

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
  tryCompaction();
}

void LauncherWindow::forwardSearchText(const QString &text) {
  m_ctx.navigation->broadcastSearchText(text);
  tryCompaction();
}

bool LauncherWindow::forwardKey(int key, int modifiers) {
  auto mods = static_cast<Qt::KeyboardModifiers>(modifiers);
  const bool isReturn = key == Qt::Key_Return || key == Qt::Key_Enter;
  const bool unmodified = (mods & ~Qt::KeypadModifier) == Qt::NoModifier;

  // unmodified keys are regular text input, except return which the action panel
  // knows how to handle
  if (unmodified && !isReturn) return false;

  if (unmodified && m_compacted) {
    expand();
    return true;
  }

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

  if (m_actionPanel->activateBoundAction(&event)) return true;

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

void LauncherWindow::setCompleterValue(int index, const QString &value) {
  auto *nav = m_ctx.navigation.get();
  auto values = nav->completionValues();
  if (index < 0 || std::cmp_greater_equal(index, values.size())) return;
  values[index].second = value;
  nav->setCompletionValues(values);
}

QRect LauncherWindow::cursorScreenGeometry() const {
  auto *screen = QGuiApplication::screenAt(QCursor::pos());
  if (!screen) screen = QGuiApplication::primaryScreen();
  return screen ? screen->geometry() : QRect();
}

bool LauncherWindow::canPositionWindow() { return Environment::supportsArbitraryWindowPlacement(); }

void LauncherWindow::positionOnCursorScreen() {
  if (!m_window || !canPositionWindow()) return;
  const QRect g = cursorScreenGeometry();
  m_window->setX(g.x() + (g.width() - m_window->width()) / 2);
  m_window->setY(g.y() + (g.height() - m_window->height()) / 3);
}

void LauncherWindow::openFooterMenu() { m_footerPanel->toggle(true); }

void LauncherWindow::buildFooterMenu() {
  auto state = std::make_unique<ActionPanelState>();
  state->setAutoSelectPrimary(false);
  state->setTitle(QStringLiteral("Vicinae"));
  state->setId(QStringLiteral("footer-menu"));

  auto *appSection = state->createSection();
  appSection->addAction(
      new StaticAction(tr("Open Settings"), ImageURL::builtin("cog"), [](ApplicationContext *ctx) {
        ctx->navigation->closeWindow();
        ctx->settings->openWindow();
      }));
  appSection->addAction(
      new StaticAction(tr("Keyboard Shortcuts"), ImageURL::builtin("keyboard"), [](ApplicationContext *ctx) {
        ctx->navigation->closeWindow();
        ctx->settings->openTab(QStringLiteral("shortcuts"));
      }));
  appSection->addAction(
      new StaticAction(tr("Extension Store"), ImageURL::builtin("cart"), [](ApplicationContext *ctx) {
        ctx->navigation->popToRoot();
        ctx->navigation->clearSearchText();
        ctx->navigation->pushView(new VicinaeStoreViewHost);
      }));

  auto *helpSection = state->createSection();
  helpSection->addAction(
      new StaticAction(tr("Documentation"), ImageURL::builtin("book"), [](ApplicationContext *ctx) {
        ctx->services->appDb()->openTarget(Omnicast::DOC_URL);
        ctx->navigation->showHud(tr("Opened in browser"));
      }));
  helpSection->addAction(
      new StaticAction(tr("Report a Bug"), ImageURL::builtin("bug"), [](ApplicationContext *ctx) {
        ctx->services->appDb()->openTarget(makeVicinaeBugReportUrl());
        ctx->navigation->showHud(tr("Opened in browser"));
      }));
  helpSection->addAction(
      new StaticAction(tr("About Vicinae"), ImageURL::builtin("info-01"), [](ApplicationContext *ctx) {
        ctx->navigation->closeWindow();
        ctx->settings->openTab(QStringLiteral("about"));
      }));

  m_footerPanel->setActions(std::move(state));
}

void LauncherWindow::expand() { setCompacted(false); }

void LauncherWindow::setCompacted(bool value) {
  if (m_compacted == value) return;
  m_compacted = value;
  emit compactedChanged();
}

void LauncherWindow::tryCompaction() {
  auto &cfg = m_ctx.services->config()->value().launcherWindow.compactMode;

  setCompacted(!m_ctx.services->newsService()->hasUnreadNews() && cfg.enabled &&
               m_ctx.navigation->searchText().isEmpty() && m_ctx.navigation->viewStackSize() == 1 &&
               !m_toastActive);
}

bool LauncherWindow::isLayerShellActive() const {
#ifdef WAYLAND_LAYER_SHELL
  return Environment::isLayerShellSupported() &&
         m_ctx.services->config()->value().launcherWindow.layerShell.enabled;
#else
  return false;
#endif
}

void LauncherWindow::setExclusiveFocus(bool exclusive) {
  if (!isLayerShellActive()) return;

  const auto &lc = m_ctx.services->config()->value().launcherWindow.layerShell;
  int ki = (exclusive && lc.keyboardInteractivity == "exclusive") ? 1 : 2; // Exclusive : OnDemand
  if (m_lsKeyboardInteractivity != ki) {
    m_lsKeyboardInteractivity = ki;
    emit lsChanged();
  }
}

void LauncherWindow::applyWindowConfig() {
  if (!m_window) return;
  auto &wcfg = m_ctx.services->config()->value().launcherWindow;

  updateLayerShellProps();
}

void LauncherWindow::updateLayerShellProps() {
  if (!isLayerShellActive()) return;

  auto &cfg = m_ctx.services->config()->value();
  const auto &lc = cfg.launcherWindow.layerShell;

  int layer = (lc.layer == "overlay") ? 3 : 2;                                  // LayerOverlay : LayerTop
  int ki = 2;                                                                   // OnDemand
  if (lc.keyboardInteractivity == "exclusive" && !cfg.closeOnFocusLoss) ki = 1; // Exclusive

  bool changed = false;
  if (m_lsLayer != layer) {
    m_lsLayer = layer;
    changed = true;
  }
  if (m_lsKeyboardInteractivity != ki) {
    m_lsKeyboardInteractivity = ki;
    changed = true;
  }
  if (changed) emit lsChanged();
}
