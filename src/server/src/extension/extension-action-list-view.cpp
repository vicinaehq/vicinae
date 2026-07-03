#include "extension-action-list-view.hpp"
#include "action-panel-model.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include <utility>

ExtensionActionListView::ExtensionActionListView(ExtensionActionPanelBuilder::NotifyFn notify,
                                                 const QString &onSearchTextChangeHandler, QObject *parent)
    : ActionListView(parent), m_notify(std::move(notify)),
      m_onSearchTextChangeHandler(onSearchTextChangeHandler) {
  if (!m_onSearchTextChangeHandler.isEmpty()) {
    connect(model(), &ActionPanelModel::filterChanged, this,
            [this](const QString &text) { m_notify(m_onSearchTextChangeHandler, {text}); });
  }
}

ExtensionSubmenuAction::ExtensionSubmenuAction(ActionPannelSubmenuPtr model,
                                               ExtensionActionPanelBuilder::NotifyFn notify,
                                               ExtensionActionPanelBuilder::SubmitFn submit)
    : ListSubmenuAction(QString::fromStdString(model->title),
                        model->icon ? std::optional(ImageURL(*model->icon)) : std::nullopt),
      m_model(std::move(model)), m_notify(std::move(notify)), m_submit(std::move(submit)) {
  if (m_model->stableId) { setId(QString::fromStdString(*m_model->stableId)); }
  if (m_model->shortcut) { addShortcut(*m_model->shortcut); }
}

void ExtensionSubmenuAction::onOpen() {
  if (!m_model->onOpen.empty()) { m_notify(QString::fromStdString(m_model->onOpen), {}); }
}

std::unique_ptr<ActionPanelState> ExtensionSubmenuAction::buildState() const {
  return ExtensionActionPanelBuilder::buildSubmenuState(m_model, m_notify, m_submit);
}

ActionPanelView *ExtensionSubmenuAction::createView(QObject *parent) {
  auto state = buildState();
  if (!state) return nullptr;

  auto *view =
      new ExtensionActionListView(m_notify, QString::fromStdString(m_model->onSearchTextChange), parent);
  view->adoptState(std::move(state));
  return view;
}
