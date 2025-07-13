#include "app.hpp"
#include "action-panel/action-panel.hpp"
#include "base-view.hpp"
#include "command-builder.hpp"
#include "command-database.hpp"
#include "command-server.hpp"
#include <QGraphicsBlurEffect>
#include "services/clipboard/clipboard-server-factory.hpp"
#include "common.hpp"
#include "services/config/config-service.hpp"
#include "extension/missing-extension-preference-view.hpp"
#include "command.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/alert.hpp"
#include "ui/keyboard.hpp"
#include "ui/toast.hpp"
#include "ui/top_bar.hpp"
#include "wm/window-manager-factory.hpp"
#include "extension/manager/extension-manager.hpp"
#include "image-fetcher.hpp"
#include "omni-command-db.hpp"
#include "omnicast.hpp"
#include "root-command.hpp"
#include "theme.hpp"
#include <QLabel>
#include <QMainWindow>
#include <QThread>
#include <QVBoxLayout>
#include <csetjmp>
#include <memory>
#include <qboxlayout.h>
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qgraphicseffect.h>
#include <qlogging.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpixmapcache.h>
#include <qtmetamacros.h>
#include <qwidget.h>

bool AppWindow::event(QEvent *event) {
  auto ui = ServiceRegistry::instance()->UI();

  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    auto key = keyEvent->key();
    bool isEsc = keyEvent->key() == Qt::Key_Escape;

    if (isEsc || (keyEvent->key() == Qt::Key_Backspace)) {
      ui->popView();
      return true;
    }

    if (keyEvent->keyCombination() == QKeyCombination(Qt::ControlModifier, Qt::Key_Comma)) {
      settings->show();
      return true;
    }

    if (m_statusBar->isActionButtonVisible() &&
        keyEvent == KeyboardShortcut(m_statusBar->actionButtonShortcut())) {
      if (auto panel = ui->topView()->actionPanel()) {
        panel->show();
        return true;
      }
    }
  }

  return QWidget::event(event);
}

void AppWindow::unloadHangingCommand() {
  /*
if (commandStack.size() > 1) {
auto &command = commandStack.at(commandStack.size() - 1);

if (command.viewStack.empty()) {
command.command->unload();
commandStack.pop_back();
qWarning() << "unloading hanging command";
}
}
*/
}

void AppWindow::resizeEvent(QResizeEvent *event) { QMainWindow::resizeEvent(event); }

void AppWindow::paintEvent(QPaintEvent *event) {
  auto &config = ServiceRegistry::instance()->config()->value();
  auto &theme = ThemeService::instance().theme();
  int borderWidth = 2;
  QColor finalBgColor = theme.colors.mainBackground;
  QPainter painter(this);

  finalBgColor.setAlphaF(config.window.opacity);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), config.window.rounding, config.window.rounding);

  painter.setClipPath(path);

  painter.fillPath(path, finalBgColor);

  QPen pen(theme.colors.border, borderWidth);
  painter.setPen(pen);

  painter.drawPath(path);
}

std::variant<CommandResponse, CommandError> AppWindow::handleCommand(const CommandMessage &message) {
  qDebug() << "received message type" << message.type;
  auto commandDb = ServiceRegistry::instance()->commandDb();
  auto extensionManager = ServiceRegistry::instance()->extensionManager();
  auto ui = ServiceRegistry::instance()->UI();

  if (message.type == "ping") { return "pong"; }
  if (message.type == "toggle") {
    setVisible(!isVisible());
    return true;
  }

  if (message.type == "url-scheme-handler") {
    QUrl url(message.params.asString().c_str());

    // omnicast://extensions/<extension_id>/<command_id>
    if (url.host() == "extensions") {
      auto commandDb = ServiceRegistry::instance()->commandDb();
      auto ss = url.path().slice(1).split('/');
      auto extId = ss.at(0);
      auto commandId = ss.at(1);

      if (ss.size() < 2) {
        qCritical() << "Malformed extensions request";
        return false;
      }

      for (auto &entry : commandDb->commands()) {
        if (entry.command->extensionId() == extId && entry.command->commandId() == commandId) {
          ui->popToRoot();
          ui->launchCommand(entry.command);
          show();
          return true;
        }
      }

      qCritical() << "No command id" << extId << commandId;
    }

    if (url.path() == "/api/extensions/develop/start") {
      QUrlQuery query(url.query());
      QString id = query.queryItemValue("id");

      extensionManager->startDevelopmentSession(id);
      qDebug() << "start develop id" << query.queryItemValue("id");
    }

    else if (url.path() == "/api/extensions/develop/refresh") {
      QUrlQuery query(url.query());
      QString id = query.queryItemValue("id");

      extensionManager->refreshDevelopmentSession(id);
      qDebug() << "refresh develop id" << id;
    }

    qDebug() << "handling URL in daemon" << url.toString();
    return {};
  }

  if (message.type == "command.list") {
    Proto::Array results;

    for (const auto &entry : commandDb->commands()) {
      Proto::Dict result;

      result["id"] = entry.command->uniqueId().toUtf8().constData();
      result["name"] = entry.command->name().toUtf8().constData();
      results.push_back(result);
    }

    return results;
  }

  if (message.type == "command.push") {
    auto args = message.params.asArray();

    if (args.empty()) { return CommandError{"Ill-formed command.push request"}; }

    auto id = args.at(0).asString();

    return CommandError{"No such command"};
  }

  return CommandError{"Unknowm command"};
}

void AppWindow::closeWindow(bool withPopToRoot) { hide(); }

void AppWindow::confirmAlert(AlertWidget *alert) {
  _dialog->setContent(alert);
  _dialog->showDialog();
  _dialog->setFocus();
}

void AppWindow::applyActionPanelState(ActionPanelV2Widget *panel) {
  KeyboardShortcutModel DEFAULT_ACTION_PANEL_SHORTCUT = {.key = "B", .modifiers = {"ctrl"}};
  m_statusBar->setActionButton("Actions", KeyboardShortcutModel{.key = "return"});

  auto actions = panel->actions();
  auto primaryAction = panel->primaryAction();

  m_statusBar->setActionButtonVisibility(!actions.empty() && (!primaryAction || actions.size() > 1));
  m_statusBar->setCurrentActionButtonVisibility(primaryAction);

  if (auto action = panel->primaryAction()) {
    m_statusBar->setCurrentAction(action->title(),
                                  action->shortcut.value_or(KeyboardShortcutModel{.key = "return"}));
    m_statusBar->setActionButton("Actions", DEFAULT_ACTION_PANEL_SHORTCUT);
  } else {
    m_statusBar->setActionButton("Actions", KeyboardShortcutModel{.key = "return"});
  }
}

AppWindow::AppWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground, true);
  setMinimumSize(Omnicast::WINDOW_SIZE);

  FaviconService::initialize(new FaviconService((Omnicast::dataDir() / "favicon.db").c_str()));

  _commandServer = new CommandServer(this);

  if (!_commandServer->start(Omnicast::commandSocketPath())) {
    qDebug() << "could not start the command server";
    return;
  }

  _commandServer->setHandler(this);

  ImageFetcher::instance();

  connect(ServiceRegistry::instance()->config(), &ConfigService::configChanged, this,
          [](const ConfigService::Value &next, const ConfigService::Value &prev) {
            if (next.theme.name.value_or("") != prev.theme.name.value_or("")) {
              ThemeService::instance().setTheme(*next.theme.name);
            }

            if (QIcon::themeName() == "hicolor") {
              if (ThemeService::instance().theme().appearance == "light") {
                QIcon::setThemeName("Reversal");
              } else {
                QIcon::setThemeName("Reversal-dark");
              }
            }

            if (next.font.normal && *next.font.normal != prev.font.normal.value_or("")) {
              QApplication::setFont(*next.font.normal);
              qApp->setStyleSheet(qApp->styleSheet());
            }
          });

  auto rootCommand =
      CommandBuilder("root").withIcon(BuiltinOmniIconUrl("omnicast")).toSingleView<RootSearchView>();
  auto ui = ServiceRegistry::instance()->UI();

  connect(ui, &UIController::currentViewChanged, this, [this](BaseView *view) {
    qCritical() << "CURRENT VIEW CHANGED" << view;
    if (auto widget = m_viewContainer->widget(0); widget != view) {
      if (widget) m_viewContainer->removeWidget(widget);
      m_viewContainer->addWidget(view);
    }

    m_viewContainer->setCurrentWidget(view);
    m_topBar->input->setVisible(view->supportsSearch());
    m_topBar->setVisible(view->needsGlobalTopBar());
    m_statusBar->setVisible(view->needsGlobalStatusBar());
    m_topBar->input->setFocus();
    m_topBar->input->selectAll();
  });

  connect(ui, &UIController::closeWindowRequested, this, [this]() { closeWindow(false); });
  connect(ui, &UIController::openSettingsRequested, this, [this]() { settings->show(); });

  connect(ui, &UIController::searchTextChanged, m_topBar->input, &SearchBar::setText);
  connect(ui, &UIController::searchTextPlaceholderChanged, m_topBar->input, &SearchBar::setPlaceholderText);
  connect(ui, &UIController::navigationTitleChanged, m_statusBar, &StatusBar::setNavigationTitle);
  connect(ui, &UIController::navigationIconChanged, m_statusBar, &StatusBar::setNavigationIcon);
  connect(ui, &UIController::loadingChanged, m_topBar, &TopBar::setLoading);
  connect(ui, &UIController::searchAccessoryChanged, this, [this](QWidget *widget) {
    if (widget)
      m_topBar->setAccessoryWidget(widget);
    else
      m_topBar->clearAccessoryWidget();
  });

  connect(ui, &UIController::actionPanelStateChanged, this, &AppWindow::applyActionPanelState);

  connect(ui, &UIController::viewStackSizeChanged, this,
          [this](size_t size) { m_topBar->setBackButtonVisiblity(size > 1); });

  connect(m_topBar->input, &SearchBar::textEdited, ui, &UIController::setSearchText);

  auto toast = ServiceRegistry::instance()->toastService();

  connect(ui, &UIController::showToastRequested, toast,
          [this, toast](const QString &title, ToastPriority priority) { toast->setToast(title, priority); });

  connect(toast, &ToastService::toastActivated, this,
          [this](const Toast *toast) { m_statusBar->setToast(toast); });

  connect(ui, &UIController::alertRequested, this, [this, ui](AlertWidget *widget) {
    if (auto panel = ui->topView()->actionPanel()) panel->close();
    confirmAlert(widget);
  });

  connect(toast, &ToastService::toastHidden, this, [this](const Toast *toast) { m_statusBar->clearToast(); });

  m_statusBar->setFixedHeight(Omnicast::STATUS_BAR_HEIGHT);
  m_statusBar->setActionButtonVisibility(false);
  m_statusBar->setCurrentActionButtonVisibility(false);
  m_topBar->setFixedHeight(Omnicast::TOP_BAR_HEIGHT);

  ui->setStatusBar(m_statusBar);
  ui->setTopBar(m_topBar);
  m_layout->setSpacing(0);
  m_layout->setContentsMargins(0, 0, 0, 0);
  m_layout->addWidget(m_topBar);
  m_layout->addWidget(m_viewContainer, 1);
  m_layout->addWidget(m_statusBar);

  QWidget *m_widget = new QWidget;

  m_widget->setLayout(m_layout);
  setCentralWidget(m_widget);

  ui->launchCommand(rootCommand);
}
