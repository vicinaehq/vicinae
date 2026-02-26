#pragma once
#include "extend/action-model.hpp"
#include "navigation-controller.hpp"
#include <QJsonArray>
#include <QJsonObject>
#include <expected>
#include <functional>
#include <memory>
#include <unordered_map>

namespace ExtensionActionPanelBuilder {

using NotifyFn = std::function<void(const QString &handler, const QJsonArray &args)>;
using SubmitFn = std::function<std::expected<QJsonObject, QString>()>;
using SubmenuCache = std::unordered_map<QString, ActionPannelSubmenuPtr>;

/// Build an ActionPanelState from an extension ActionPannelModel.
/// @param model        The action panel model from the extension.
/// @param notify       Callback to send notifications to the extension.
/// @param submenuCache Pointer to a mutable cache of submenu models (must outlive the returned panel).
/// @param preset       Shortcut preset to apply.
/// @param submit       Optional submit callback for form actions.
std::unique_ptr<ActionPanelState>
build(const ActionPannelModel &model, const NotifyFn& notify, SubmenuCache *submenuCache,
      ActionPanelState::ShortcutPreset preset = ActionPanelState::ShortcutPreset::None,
      const SubmitFn& submit = nullptr);

/// Build an ActionPanelState for a submenu (data path, for QML).
std::unique_ptr<ActionPanelState> buildSubmenuState(const ActionPannelSubmenuPtr &submenu, const NotifyFn& notify,
                                                    SubmenuCache *submenuCache, const SubmitFn& submit = nullptr);

} // namespace ExtensionActionPanelBuilder
