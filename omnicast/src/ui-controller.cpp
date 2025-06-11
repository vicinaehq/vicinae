#include "base-view.hpp"
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include "ui/ui-controller.hpp"

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

  *it = ViewState{.sender = next};
  applyViewState(*it);
  emit replaceViewRequested(previous, next);
}

void UIController::pushView(BaseView *view, const PushViewOptions &opts) {
  qCritical() << "push view";

  if (auto view = topView()) {
    if (auto panel = view->actionPanel()) {
      disconnect(panel);
      panel->close();
    }

    m_topBar->destroyCompleter();
  }

  m_stateStack.emplace_back(ViewState{.sender = view});
  m_topBar->setBackButtonVisiblity(m_stateStack.size() > 1);
  applyViewState(m_stateStack.back());
  emit pushViewRequested(view, opts);
  if (view->supportsSearch()) { m_topBar->input->setFocus(); }
}

void UIController::handleCurrentActionButtonClick() { executeDefaultAction(); }

void UIController::executeAction(AbstractAction *action) {
  if (action->isPrimary()) {
    if (m_topBar->m_completer->isVisible()) {
      for (int i = 0; i != m_topBar->m_completer->m_args.size(); ++i) {
        auto &arg = m_topBar->m_completer->m_args.at(i);
        auto input = m_topBar->m_completer->m_inputs.at(i);

        qCritical() << "required" << arg.required << input->text();

        if (arg.required && input->text().isEmpty()) {
          input->setFocus();
          return;
        }
      }
    }
  }

  if (auto view = topView()) {
    if (auto panel = view->actionPanel()) {
      action->execute();
      if (action->isSubmenu()) {
        if (auto submenu = action->createSubmenu()) {
          panel->pushView(submenu);
          panel->show();
          return;
        }
      }
      panel->close();
    }
  }
}

void UIController::executeDefaultAction() {
  if (auto view = topView()) {
    if (auto panel = view->actionPanel()) {
      if (auto action = panel->primaryAction()) {
        executeAction(action);
      } else {
        panel->show();
      }
    }
  }
}

void UIController::handleActionButtonClick() {
  if (auto view = topView()) {
    if (auto panel = view->actionPanel()) { panel->show(); }
  }
}

void UIController::popView() {
  if (m_stateStack.size() == 1) return;

  if (auto view = topView()) {
    if (auto accessory = view->searchBarAccessory()) {
      m_topBar->clearAccessoryWidget();
      accessory->deleteLater();
    }
  }

  if (!m_stateStack.empty()) { m_stateStack.pop_back(); }

  qCritical() << "Pop view";
  m_topBar->setBackButtonVisiblity(m_stateStack.size() > 1);

  if (!m_stateStack.empty()) {
    auto &state = m_stateStack.back();

    applyViewState(state);
  }

  emit popViewRequested();
}

void UIController::handleCompleterArgumentsChanged(const std::vector<std::pair<QString, QString>> &values) {
  m_stateStack.back().completer.values = values;

  if (auto view = topView()) { view->argumentValuesChanged(values); }
}

void UIController::applyViewState(const ViewState &state) {
  qCritical() << "restore navigation" << state.navigation.title;
  m_statusBar->setNavigationTitle(state.navigation.title);
  m_statusBar->setNavigationIcon(state.navigation.icon);

  m_topBar->input->setText(state.text);
  m_topBar->input->setPlaceholderText(state.placeholderText);
  m_topBar->input->setVisible(state.sender->supportsSearch());
  m_topBar->setVisible(state.sender->needsGlobalTopBar());
  m_statusBar->setVisible(state.sender->needsGlobalStatusBar());

  if (auto data = state.completer.data) {
    m_topBar->activateCompleter(*data, state.completer.values);
  } else {
    m_topBar->destroyCompleter();
  }

  m_topBar->input->setFocus();
  m_topBar->input->selectAll();

  if (auto accessory = state.sender->searchBarAccessory()) { m_topBar->setAccessoryWidget(accessory); }

  if (auto panel = state.sender->actionPanel()) {
    connect(panel, &ActionPanelV2Widget::openChanged, this,
            [this](bool value) { m_statusBar->setActionButtonHighlight(value); });
    connect(panel, &ActionPanelV2Widget::actionsChanged, this,
            [this]() { setActionPanel(topView()->actionPanel()); });

    setActionPanel(panel);
  } else {
    clearActionPanel();
  }

  if (auto accessory = state.searchAccessory) { m_topBar->setAccessoryWidget(accessory); }
}

void UIController::setActionPanelWidget(BaseView *sender, ActionPanelV2Widget *panel) {
  updateViewState(sender, [&](ViewState &state) { state.actionPanel = panel; });

  if (topView() == sender) {
    connect(panel, &ActionPanelV2Widget::openChanged, this,
            [this](bool value) { m_statusBar->setActionButtonHighlight(value); });
  }
}
