#include "extension-action-panel-builder.hpp"
#include "extension-action-list-view.hpp"
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

    auto qTitle = QString::fromStdString(model.title);
    auto action = new PushViewAction(qTitle, view, actionIcon);
    if (model.stableId) { action->setId(QString::fromStdString(*model.stableId)); }
    return action;
  }

  auto qTitle = QString::fromStdString(model.title);
  auto action = new StaticAction(qTitle, model.icon, [notify, model, submit]() {
    notify(QString::fromStdString(model.onAction), {});

    if (auto handler = model.onSubmit) {
      if (submit) {
        auto res = submit();
        if (res.has_value()) {
          notify(QString::fromStdString(*handler), QJsonArray{res.value()});
        } else {
          qCritical() << "Failed to submit action" << res.error();
        }
      }
    }
  });

  if (model.stableId) { action->setId(QString::fromStdString(*model.stableId)); }
  return action;
}

static AbstractAction *createSubmenuAction(const ActionPannelSubmenuPtr &submenuModel, const NotifyFn &notify,
                                           const SubmitFn &submit) {
  if (!submenuModel) return nullptr;
  return new ExtensionSubmenuAction(submenuModel, notify, submit);
}

std::unique_ptr<ActionPanelState> buildSubmenuState(const ActionPannelSubmenuPtr &submenuModel,
                                                    const NotifyFn &notify, const SubmitFn &submit) {
  if (!submenuModel) return nullptr;

  auto state = std::make_unique<ActionPanelState>();
  state->setTitle(QString::fromStdString(submenuModel->title));
  if (submenuModel->stableId) { state->setId(QString::fromStdString(*submenuModel->stableId)); }

  ActionPanelSectionState *outsideSection = nullptr;

  for (const auto &child : submenuModel->children) {
    if (auto sectionPtr = std::get_if<ActionPannelSectionPtr>(&child)) {
      auto section = sectionPtr ? *sectionPtr : nullptr;
      if (!section) continue;
      outsideSection = nullptr;

      auto sec = state->createSection(QString::fromStdString(section->title));

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
  panel->setTitle(QString::fromStdString(model.title));
  panel->setShortcutPreset(preset);
  if (model.stableId) { panel->setId(QString::fromStdString(*model.stableId)); }

  size_t idx = 0;
  ActionPanelSectionState *outsideSection = nullptr;

  for (const auto &item : model.children) {
    if (auto sectionPtr = std::get_if<ActionPannelSectionPtr>(&item)) {
      auto section = sectionPtr ? *sectionPtr : nullptr;
      if (!section) continue;
      outsideSection = nullptr;

      auto sec = panel->createSection(QString::fromStdString(section->title));

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
