#include "base-view.hpp"
#include <qevent.h>
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

void UIController::pushView(BaseView *view, const PushViewOptions &opts) {
  qCritical() << "push view";
  m_stateStack.emplace_back(ViewState{.sender = view});
  clearActionPanel();
  m_topBar->input->setText("");
  m_topBar->input->setPlaceholderText("");
  m_topBar->setBackButtonVisiblity(m_stateStack.size() > 1);
  m_topBar->input->setVisible(view->supportsSearch());
  m_topBar->setVisible(view->needsGlobalTopBar());
  m_statusBar->setVisible(view->needsGlobalStatusBar());
  emit pushViewRequested(view, opts);
  if (view->supportsSearch()) { m_topBar->input->setFocus(); }
}

void UIController::popView() {
  if (m_stateStack.size() == 1) return;
  if (!m_stateStack.empty()) { m_stateStack.pop_back(); }

  qCritical() << "Pop view";
  m_topBar->setBackButtonVisiblity(m_stateStack.size() > 1);

  if (!m_stateStack.empty()) {
    auto &state = m_stateStack.back();

    applyViewState(state);
    m_topBar->input->setVisible(state.sender->supportsSearch());
    m_topBar->setVisible(state.sender->needsGlobalTopBar());
    m_statusBar->setVisible(state.sender->needsGlobalStatusBar());
    m_topBar->input->selectAll();

    qDebug() << "pop supports search" << state.sender->supportsSearch();
  }

  emit popViewRequested();
}
