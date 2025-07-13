#include "base-view.hpp"
#include "common.hpp"
#include "extension/extension-command.hpp"
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include "ui/ui-controller.hpp"
#include "extension/missing-extension-preference-view.hpp"

void UIController::handleTextEdited(const QString &text) {
  if (auto view = topView()) {
    updateViewState(view, [&](ViewState &state) { state.text = text; });
    view->textChanged(text);
  }
}

bool UIController::eventFilter(QObject *watched, QEvent *event) {
  if (watched == m_topBar->input && event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    if (auto view = topView()) {
      if (view->inputFilter(keyEvent)) { return true; }
    }

    if (keyEvent->key() == Qt::Key_Escape) {
      if (m_stateStack.size() > 1) {
        popView();
        return true;
      }

      if (m_topBar->input->text().isEmpty()) {
        closeWindow();
        return true;
      }

      m_topBar->input->clear();
      emit m_topBar->input->textEdited("");
      return true;
    }
  }

  return false;
}

void UIController::replaceView(BaseView *previous, BaseView *next) {
  auto it = std::ranges::find_if(m_stateStack, [&](auto &&state) { return state.sender == previous; });

  if (it == m_stateStack.end()) {
    qCritical() << "replaceView: previous does not exist";
    return;
  }

  if (auto accessory = previous->searchBarAccessory()) { accessory->deleteLater(); }

  *it = ViewState{.sender = next};

  if (topView() == next) { applyViewState(*it); }

  emit replaceViewRequested(previous, next);
}

void UIController::launchCommand(const QString &id, const LaunchProps &opts) {
  auto commandDb = ServiceRegistry::instance()->commandDb();

  if (auto command = commandDb->findCommand(id)) { launchCommand(command->command, opts); }
}

void UIController::launchCommand(const std::shared_ptr<AbstractCmd> &cmd, const LaunchProps &opts) {
  auto itemId = QString("extension.%1").arg(cmd->uniqueId());
  auto manager = ServiceRegistry::instance()->rootItemManager();
  auto preferences = manager->getMergedItemPreferences(itemId);
  auto preferenceValues = manager->getPreferenceValues(itemId);

  for (const auto &preference : preferences) {
    if (preference.required() && !preferenceValues.contains(preference.name()) &&
        preference.defaultValue().isUndefined()) {
      if (cmd->type() == CommandType::CommandTypeExtension) {
        auto extensionCommand = std::static_pointer_cast<ExtensionCommand>(cmd);

        pushView(new MissingExtensionPreferenceView(extensionCommand, preferences, preferenceValues),
                 {.navigation = NavigationStatus{.title = cmd->name(), .iconUrl = cmd->iconUrl()}});
        return;
      }

      qDebug() << "MISSING PREFERENCE" << preference.title();
    }
  }

  // TODO: unloadHangingCommand();

  auto ctx = cmd->createContext(cmd);

  if (!ctx) { return; }

  if (cmd->isNoView() && cmd->type() == CommandType::CommandTypeBuiltin) {
    qCritical() << "Running no view command";
    ctx->load(opts);
  } else {
    m_commandStack.push_back({.command = std::unique_ptr<CommandContext>(ctx)});
    ctx->load(opts);
  }
}

void UIController::pushView(BaseView *view, const PushViewOptions &opts) {
  qCritical() << "push view";

  if (auto view = topView()) {
    auto &state = m_stateStack.back();

    if (auto panel = state.actionPanel) {
      disconnect(panel, nullptr, this, nullptr);
      panel->close();
    }

    if (auto accessory = view->searchBarAccessory()) {
      m_topBar->clearAccessoryWidget();
      accessory->hide();
    }

    state.navigation.title = m_statusBar->navigationTitle();
    state.navigation.icon = m_statusBar->navigationIcon();

    m_topBar->destroyCompleter();

    view->deactivate();
    view->hide();
    view->clearFocus();
  }

  ViewState state;

  state.sender = view;
  state.supportsSearch = view->supportsSearch();
  state.needsTopBar = view->needsGlobalTopBar();
  state.needsStatusBar = view->needsGlobalStatusBar();
  state.actionPanel = view->actionPanel();
  state.searchAccessory = view->searchBarAccessory();
  m_stateStack.emplace_back(state);
  qCritical() << "search accessory" << state.searchAccessory;

  m_topBar->setBackButtonVisiblity(m_stateStack.size() > 1);
  applyViewState(m_stateStack.back());

  if (m_commandStack.empty()) {
    qDebug() << "AppWindow::pushView called with empty command stack";
    return;
  }

  auto &currentCommand = m_commandStack.at(m_commandStack.size() - 1);

  currentCommand.viewStack.push(state);

  if (auto navigation = opts.navigation) {
    qDebug() << "set navigation";
    m_statusBar->setNavigation(navigation->title, navigation->iconUrl);
  }

  emit currentViewChanged(view);
  emit viewStackSizeChanged(m_stateStack.size());

  view->show();
  view->setFocus();
  view->createInitialize();
  view->onActivate();

  if (state.supportsSearch) { m_topBar->input->setFocus(); }
}

void UIController::handleCurrentActionButtonClick() { executeDefaultAction(); }

void UIController::executeAction(AbstractAction *action) {
  qCritical() << "execute action" << action->title();
  if (action->isPrimary()) {
    if (m_topBar->m_completer->isVisible()) {
      for (int i = 0; i != m_topBar->m_completer->m_args.size(); ++i) {
        auto &arg = m_topBar->m_completer->m_args.at(i);
        auto input = m_topBar->m_completer->m_inputs.at(i);

        qCritical() << "required" << arg.required << input->text();

        if (arg.required && input->text().isEmpty()) {
          input->setError("Required");
          input->setFocus();
          if (auto view = topView()) {
            if (auto panel = view->actionPanel()) { panel->close(); }
          }
          return;
        }
      }
    }
  }

  if (m_stateStack.empty()) return;

  m_stateStack.back().sender->executeAction(action);
}

void UIController::executeDefaultAction() {
  if (m_stateStack.empty()) return;

  auto &state = m_stateStack.back();

  if (auto panel = state.actionPanel) {
    if (auto action = panel->primaryAction()) {
      executeAction(action);
    } else {
      panel->show();
    }
  }
}

void UIController::handleActionButtonClick() {
  if (m_stateStack.empty()) return;

  auto &state = m_stateStack.back();

  if (auto panel = state.actionPanel) { panel->show(); }
}

void UIController::popView() {
  if (m_stateStack.size() == 1) {
    m_topBar->input->setText("");
    emit m_topBar->input->textEdited("");
    return;
  }

  if (auto view = topView()) {
    auto &state = m_stateStack.back();

    view->deactivate();
    view->deleteLater();

    if (auto accessory = state.searchAccessory) { m_topBar->clearAccessoryWidget(); }
  }

  if (m_stateStack.size() == 1) {
    qDebug() << "can't pop base view";
    return;
  }

  m_stateStack.pop_back();
  auto &state = m_stateStack.back();

  applyViewState(state);

  auto &activeCommand = m_commandStack.at(m_commandStack.size() - 1);

  qDebug() << "pop requested";

  if (activeCommand.viewStack.empty()) {
    qDebug() << "active command view stack empty";
    return;
  }

  m_topBar->setBackButtonVisiblity(m_stateStack.size() > 1);

  state.sender->activate();
  emit currentViewChanged(state.sender);
  emit viewStackSizeChanged(m_stateStack.size());

  if (activeCommand.viewStack.size() == 1) {
    qCritical() << "UNLOAD COMMAND";
    activeCommand.command->unload();
    activeCommand.command->deleteLater();
    m_commandStack.pop_back();
    qDebug() << "popping cmd stack now" << m_commandStack.size();
  } else {
    activeCommand.viewStack.pop();
  }
}

void UIController::handleCompleterArgumentsChanged(const std::vector<std::pair<QString, QString>> &values) {
  m_stateStack.back().completer.values = values;

  if (auto view = topView()) { view->argumentValuesChanged(values); }
}

void UIController::applyViewState(const ViewState &state) {
  qCritical() << "restore navigation" << state.navigation.title;

  setNavigationTitle(state.navigation.title);
  setNavigationIcon(state.navigation.icon);
  setSearchText(state.text);
  setSearchPlaceholderText(state.placeholderText);

  if (auto data = state.completer.data) {
    m_topBar->activateCompleter(*data, state.completer.values);
  } else {
    m_topBar->destroyCompleter();
  }

  if (auto accessory = state.searchAccessory) {
    qDebug() << "set accessory" << accessory;
    m_topBar->setAccessoryWidget(accessory);
  } else {
    m_topBar->clearAccessoryWidget();
  }

  qDebug() << "accessory set";

  if (auto panel = state.actionPanel) {
    connect(panel, &ActionPanelV2Widget::openChanged, this,
            [this](bool value) { m_statusBar->setActionButtonHighlight(value); });
    connect(panel, &ActionPanelV2Widget::actionsChanged, this,
            [this, panel]() { emit actionPanelStateChanged(panel); });
    connect(panel, &ActionPanelV2Widget::actionActivated, this, &UIController::executeAction);
    emit actionPanelStateChanged(panel);
  } else {
    clearActionPanel();
  }
}

void UIController::setActionPanelWidget(BaseView *sender, ActionPanelV2Widget *panel) {
  auto state = findViewState(sender);

  if (!state) return;
  if (auto panel = state->actionPanel) { disconnect(panel); }

  updateViewState(sender, [&](ViewState &state) { state.actionPanel = panel; });

  if (topView() == sender) {
    connect(panel, &ActionPanelV2Widget::openChanged, this,
            [this](bool value) { m_statusBar->setActionButtonHighlight(value); });
    connect(panel, &ActionPanelV2Widget::actionsChanged, this, [this, panel]() { setActionPanel(panel); });
    connect(panel, &ActionPanelV2Widget::actionActivated, this, &UIController::executeAction);

    setActionPanel(panel);
  }
}
