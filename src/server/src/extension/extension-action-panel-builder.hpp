#pragma once
#include "extend/action-model.hpp"
#include "navigation-controller.hpp"
#include <QJsonArray>
#include <QJsonObject>
#include <expected>
#include <functional>
#include <memory>

namespace ExtensionActionPanelBuilder {

using NotifyFn = std::function<void(const QString &handler, const QJsonArray &args)>;
using SubmitFn = std::function<std::expected<QJsonObject, QString>()>;

std::unique_ptr<ActionPanelState>
build(const ActionPannelModel &model, const NotifyFn &notify,
      ActionPanelState::ShortcutPreset preset = ActionPanelState::ShortcutPreset::None,
      const SubmitFn &submit = nullptr);

std::unique_ptr<ActionPanelState> buildSubmenuState(const ActionPannelSubmenuPtr &submenu,
                                                    const NotifyFn &notify, const SubmitFn &submit = nullptr);

} // namespace ExtensionActionPanelBuilder
