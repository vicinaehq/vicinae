#pragma once
#include <vector>
#include <QString>
#include "command/argument.hpp"
#include "services/ai-commands/ai-command-store.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"
#include "utils/placeholder.hpp"

class AbstractAction;
struct ApplicationContext;

namespace AiCommands {

constexpr auto PROVIDER_ID = "ai-commands";
constexpr auto DEFAULT_ICON = BuiltinIcon::Stars;
constexpr auto COLOR = SemanticColor::Accent;

constexpr auto SELECTION_PLACEHOLDER = "selection";
constexpr auto CLIPBOARD_PLACEHOLDER = "clipboard";

ImageURL iconUrl(const AiCommandStore::Command &command);
QString outputLabel(AiCommandStore::Output output);
PlaceholderString parsePrompt(const AiCommandStore::Command &command);
ArgumentList arguments(const PlaceholderString &prompt);
std::vector<QString> completionValues(const ApplicationContext *ctx);
inline ImageURL defaultIconUrl() { return ImageURL::builtin(DEFAULT_ICON).setBackgroundTint(COLOR); }

/**
 * The form to create a command, or the AI setup screen when no model can run it.
 */
void openCreateForm(const ApplicationContext *ctx);

AbstractAction *runAction(const AiCommandStore::Command &command);
AbstractAction *editAction(const AiCommandStore::Command &command);
AbstractAction *duplicateAction(const AiCommandStore::Command &command);
AbstractAction *createAction();
AbstractAction *deleteAction(const AiCommandStore::Command &command);

} // namespace AiCommands
