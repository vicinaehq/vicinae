#include "extension-action-panel-builder.hpp"
#include "action-panel/action-panel.hpp"
#include "common-actions.hpp"
#include "create-quicklink-command.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"
#include <qjsonobject.h>

namespace ExtensionActionPanelBuilder {

namespace {

AbstractAction *createActionFromModel(const ActionModel &model, NotifyFn notify, SubmitFn submit) {
  if (model.type == "create-quicklink") {
    auto quicklinkObj = model.quicklink;
    QString name = quicklinkObj.value("name").toString();
    QString link = quicklinkObj.value("link").toString();
    QString application = quicklinkObj.value("application").toString();
    QString icon;

    if (quicklinkObj.contains("icon")) {
      auto iconValue = quicklinkObj.value("icon");
      if (iconValue.isString()) { icon = iconValue.toString(); }
    }

    auto view = new ShortcutFormView();
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

ActionPanelView *createSubmenuView(const ActionPannelSubmenuPtr &submenuModel, NotifyFn notify,
                                   SubmenuCache *submenuCache, SubmitFn submit);

AbstractAction *createSubmenuAction(const ActionPannelSubmenuPtr &submenuModel, NotifyFn notify,
                                    SubmenuCache *submenuCache, SubmitFn submit) {
  if (!submenuModel) return nullptr;

  if (submenuModel->stableId) { (*submenuCache)[*submenuModel->stableId] = submenuModel; }

  std::optional<ImageURL> icon;
  if (submenuModel->icon) { icon = ImageURL(*submenuModel->icon); }

  std::function<void()> onOpen = nullptr;
  if (!submenuModel->onOpen.isEmpty()) {
    onOpen = [notify, handler = submenuModel->onOpen]() {
      if (!handler.isEmpty()) { notify(handler, {}); }
    };
  }

  // Capture the pointer (not reference) so the lambda safely outlives this function
  QString stableId = submenuModel->stableId.value_or(QString());
  auto createSubmenuFn = [notify, submenuCache, submit, stableId]() -> ActionPanelView * {
    if (!stableId.isEmpty()) {
      auto it = submenuCache->find(stableId);
      if (it != submenuCache->end()) { return createSubmenuView(it->second, notify, submenuCache, submit); }
    }
    qWarning() << "Submenu model not found in map for stableId:" << stableId;
    return nullptr;
  };

  auto action = new SubmenuAction(submenuModel->title, icon, createSubmenuFn, onOpen);
  if (submenuModel->stableId) { action->setId(*submenuModel->stableId); }
  if (submenuModel->shortcut) { action->addShortcut(submenuModel->shortcut.value()); }

  return action;
}

ActionPanelView *createSubmenuView(const ActionPannelSubmenuPtr &submenuModel, NotifyFn notify,
                                   SubmenuCache *submenuCache, SubmitFn submit) {
  if (!submenuModel) return nullptr;
  auto panel = new ActionPanelStaticListView;
  panel->setTitle(submenuModel->title);
  if (submenuModel->stableId) { panel->setId(*submenuModel->stableId); }

  for (const auto &child : submenuModel->children) {
    if (auto sectionPtr = std::get_if<ActionPannelSectionPtr>(&child)) {
      auto section = sectionPtr ? *sectionPtr : nullptr;
      if (!section) continue;
      panel->addSection(section->title);

      for (const auto &item : section->items) {
        if (auto actionModel = std::get_if<ActionModel>(&item)) {
          auto action = createActionFromModel(*actionModel, notify, submit);
          if (actionModel->shortcut) { action->addShortcut(actionModel->shortcut.value()); }
          panel->addAction(action);
        } else if (auto submenuPtr = std::get_if<ActionPannelSubmenuPtr>(&item)) {
          auto action = createSubmenuAction(*submenuPtr, notify, submenuCache, submit);
          if (action) { panel->addAction(action); }
        }
      }
    } else if (auto actionModel = std::get_if<ActionModel>(&child)) {
      auto action = createActionFromModel(*actionModel, notify, submit);
      if (actionModel->shortcut) { action->addShortcut(actionModel->shortcut.value()); }
      panel->addAction(action);
    } else if (auto submenuPtr = std::get_if<ActionPannelSubmenuPtr>(&child)) {
      auto action = createSubmenuAction(*submenuPtr, notify, submenuCache, submit);
      if (action) { panel->addAction(action); }
    }
  }

  return panel;
}

void updateSubmenuCache(SubmenuCache *cache, const ActionPannelModel &model) {
  std::function<void(const ActionPannelSubmenuPtr &)> updateSubmenuModel =
      [&](const ActionPannelSubmenuPtr &submenu) {
        if (submenu && submenu->stableId) {
          (*cache)[*submenu->stableId] = submenu;
          for (const auto &child : submenu->children) {
            if (auto nestedSubmenuPtr = std::get_if<ActionPannelSubmenuPtr>(&child)) {
              updateSubmenuModel(*nestedSubmenuPtr);
            } else if (auto sectionPtr = std::get_if<ActionPannelSectionPtr>(&child)) {
              if (*sectionPtr) {
                for (const auto &sectionItem : (*sectionPtr)->items) {
                  if (auto submenuInSection = std::get_if<ActionPannelSubmenuPtr>(&sectionItem)) {
                    updateSubmenuModel(*submenuInSection);
                  }
                }
              }
            }
          }
        }
      };

  for (const auto &item : model.children) {
    if (auto submenuPtr = std::get_if<ActionPannelSubmenuPtr>(&item)) {
      updateSubmenuModel(*submenuPtr);
    } else if (auto sectionPtr = std::get_if<ActionPannelSectionPtr>(&item)) {
      for (const auto &sectionItem : (*sectionPtr)->items) {
        if (auto submenuPtr = std::get_if<ActionPannelSubmenuPtr>(&sectionItem)) {
          updateSubmenuModel(*submenuPtr);
        }
      }
    }
  }
}

} // namespace

std::unique_ptr<ActionPanelState> build(const ActionPannelModel &model, NotifyFn notify,
                                        SubmenuCache *submenuCache, ActionPanelState::ShortcutPreset preset,
                                        SubmitFn submit) {
  updateSubmenuCache(submenuCache, model);

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
          auto action = createSubmenuAction(*submenuPtr, notify, submenuCache, submit);

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

      auto action = createSubmenuAction(*submenuPtr, notify, submenuCache, submit);
      if (!action) continue;
      if (idx == 0) { action->setPrimary(true); }

      outsideSection->addAction(action);
      ++idx;
    }
  }

  return panel;
}

} // namespace ExtensionActionPanelBuilder
