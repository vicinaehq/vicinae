#include "extension-action-panel-builder.hpp"
#include "common-actions.hpp"
#include "qml/shortcut-form-view-host.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"
#include <qjsonobject.h>

namespace ExtensionActionPanelBuilder {

static AbstractAction *createActionFromModel(const ActionModel &model, const NotifyFn &notify,
                                             const SubmitFn &submit);
static AbstractAction *createSubmenuAction(const ActionPannelSubmenuPtr &submenuModel, const NotifyFn &notify,
                                           const SubmitFn &submit);

static AbstractAction *createActionFromModel(const ActionModel &model, const NotifyFn &notify,
                                             const SubmitFn &submit) {
  if (model.type == "create-quicklink") {
    auto quicklinkObj = model.quicklink;
    QString const name = quicklinkObj.value("name").toString();
    QString const link = quicklinkObj.value("link").toString();
    QString const application = quicklinkObj.value("application").toString();
    QString icon;

    if (quicklinkObj.contains("icon")) {
      auto iconValue = quicklinkObj.value("icon");
      if (iconValue.isString()) { icon = iconValue.toString(); }
    }

    auto view = new ShortcutFormViewHost();
    view->setPrefilledValues(link, name, application, icon);
    ImageURL actionIcon;

    if (model.icon) {
      actionIcon = ImageURL(*model.icon);
    } else {
      actionIcon = ImageURL::builtin("link");
    }

    auto action = new PushViewAction(model.title, view, actionIcon);
    if (model.stableId) { action->setId(*model.stableId); }
    return action;
  }

  auto action = new StaticAction(model.title, model.icon, [notify, model, submit]() {
    notify(model.onAction, {});

    if (auto handler = model.onSubmit) {
      if (submit) {
        auto res = submit();
        if (res.has_value()) {
          notify(*handler, QJsonArray{res.value()});
        } else {
          qCritical() << "Failed to submit action" << res.error();
        }
      }
    }
  });

  if (model.stableId) { action->setId(*model.stableId); }
  return action;
}

static AbstractAction *createSubmenuAction(const ActionPannelSubmenuPtr &submenuModel, const NotifyFn &notify,
                                           const SubmitFn &submit) {
  if (!submenuModel) return nullptr;

  std::optional<ImageURL> icon;
  if (submenuModel->icon) { icon = ImageURL(*submenuModel->icon); }

  std::function<void()> onOpen = nullptr;
  if (!submenuModel->onOpen.isEmpty()) {
    onOpen = [notify, handler = submenuModel->onOpen]() {
      if (!handler.isEmpty()) { notify(handler, {}); }
    };
  }

  auto action = new SubmenuAction(submenuModel->title, icon, onOpen);
  if (submenuModel->stableId) { action->setId(*submenuModel->stableId); }
  if (submenuModel->shortcut) { action->addShortcut(submenuModel->shortcut.value()); }
  if (!submenuModel->onSearchTextChange.isEmpty()) {
    action->setOnSearchTextChangeHandler(submenuModel->onSearchTextChange);
  }

  auto stateFactory = [notify, submenuModel, submit]() -> std::unique_ptr<ActionPanelState> {
    return buildSubmenuState(submenuModel, notify, submit);
  };
  action->setSubmenuStateFactory(stateFactory);

  return action;
}

std::unique_ptr<ActionPanelState> buildSubmenuState(const ActionPannelSubmenuPtr &submenuModel,
                                                    const NotifyFn &notify, const SubmitFn &submit) {
  if (!submenuModel) return nullptr;

  auto state = std::make_unique<ActionPanelState>();
  state->setTitle(submenuModel->title);
  if (submenuModel->stableId) { state->setId(*submenuModel->stableId); }

  ActionPanelSectionState *outsideSection = nullptr;

  for (const auto &child : submenuModel->children) {
    if (auto sectionPtr = std::get_if<ActionPannelSectionPtr>(&child)) {
      auto section = sectionPtr ? *sectionPtr : nullptr;
      if (!section) continue;
      outsideSection = nullptr;

      auto sec = state->createSection(section->title);

      for (const auto &item : section->items) {
        if (auto actionModel = std::get_if<ActionModel>(&item)) {
          auto action = createActionFromModel(*actionModel, notify, submit);
          if (actionModel->shortcut) { action->addShortcut(actionModel->shortcut.value()); }
          sec->addAction(action);
        } else if (auto submenuPtr = std::get_if<ActionPannelSubmenuPtr>(&item)) {
          auto action = createSubmenuAction(*submenuPtr, notify, submit);
          if (action) { sec->addAction(action); }
        }
      }
    } else if (auto actionModel = std::get_if<ActionModel>(&child)) {
      if (!outsideSection) { outsideSection = state->createSection(); }
      auto action = createActionFromModel(*actionModel, notify, submit);
      if (actionModel->shortcut) { action->addShortcut(actionModel->shortcut.value()); }
      outsideSection->addAction(action);
    } else if (auto submenuPtr = std::get_if<ActionPannelSubmenuPtr>(&child)) {
      if (!outsideSection) { outsideSection = state->createSection(); }
      auto action = createSubmenuAction(*submenuPtr, notify, submit);
      if (action) { outsideSection->addAction(action); }
    }
  }

  return state;
}

std::unique_ptr<ActionPanelState> build(const ActionPannelModel &model, const NotifyFn &notify,
                                        ActionPanelState::ShortcutPreset preset, const SubmitFn &submit) {
  auto panel = std::make_unique<ActionPanelState>();
  panel->setDirty(model.dirty);
  panel->setTitle(model.title);
  panel->setShortcutPreset(preset);
  if (model.stableId) { panel->setId(*model.stableId); }

  size_t idx = 0;
  ActionPanelSectionState *outsideSection = nullptr;

  for (const auto &item : model.children) {
    if (auto sectionPtr = std::get_if<ActionPannelSectionPtr>(&item)) {
      auto section = sectionPtr ? *sectionPtr : nullptr;
      if (!section) continue;
      outsideSection = nullptr;

      auto sec = panel->createSection(section->title);

      for (const auto &sectionItem : section->items) {
        if (auto actionModel = std::get_if<ActionModel>(&sectionItem)) {
          auto action = createActionFromModel(*actionModel, notify, submit);

          if (idx == 0) { action->setPrimary(true); }
          if (actionModel->shortcut) { action->addShortcut(actionModel->shortcut.value()); }

          sec->addAction(action);
          ++idx;
        } else if (auto submenuPtr = std::get_if<ActionPannelSubmenuPtr>(&sectionItem)) {
          auto action = createSubmenuAction(*submenuPtr, notify, submit);

          if (!action) continue;
          if (idx == 0) { action->setPrimary(true); }

          sec->addAction(action);
          ++idx;
        }
      }
    }

    if (auto actionModel = std::get_if<ActionModel>(&item)) {
      if (!outsideSection) { outsideSection = panel->createSection(); }

      auto action = createActionFromModel(*actionModel, notify, submit);
      if (idx == 0) { action->setPrimary(true); }
      if (actionModel->shortcut) { action->addShortcut(actionModel->shortcut.value()); }

      outsideSection->addAction(action);
      ++idx;
    }

    if (auto submenuPtr = std::get_if<ActionPannelSubmenuPtr>(&item)) {
      if (!outsideSection) { outsideSection = panel->createSection(); }

      auto action = createSubmenuAction(*submenuPtr, notify, submit);
      if (!action) continue;
      if (idx == 0) { action->setPrimary(true); }

      outsideSection->addAction(action);
      ++idx;
    }
  }

  return panel;
}

} // namespace ExtensionActionPanelBuilder
