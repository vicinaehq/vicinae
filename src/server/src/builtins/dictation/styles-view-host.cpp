#include "styles-view-host.hpp"
#include <algorithm>
#include "builtins/dictation/dictation.hpp"
#include "builtins/dictation/style-form-view-host.hpp"
#include "common/context.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"
#include "services/app-service/app-service.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/dictation/dictation-service.hpp"
#include "services/toast/toast-service.hpp"
#include "theme/colors.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/action-panel/action.hpp"
#include "ui/alert/alert.hpp"
#include "ui/settings/settings-controller.hpp"
#include "ui/views/intro-view-host.hpp"

namespace {

bool hasCompletionModels(const ApplicationContext *ctx) {
  return !ctx->services->ai()->listModels(AI::Capability::Completion).empty();
}

void pushProviderSetup(const ApplicationContext *ctx) {
  ctx->navigation->pushView(new IntroViewHost(
      DictationStylesViewHost::tr("Set up an AI provider"),
      DictationStylesViewHost::tr("Dictation styles need a language model to rewrite your text. Add an AI "
                                  "provider in the settings."),
      Dictation::STYLE_ICON, DictationStylesViewHost::tr("Open AI Settings"), [ctx]() {
        ctx->settings->openSubpage(QStringLiteral("ai"), QStringLiteral("providers"));
        ctx->navigation->closeWindow();
      }));
}

AbstractAction *createStyleAction(const QString &title) {
  auto *action = new StaticAction(title, ImageURL::builtin(BuiltinIcon::Plus), [](ApplicationContext *ctx) {
    ctx->navigation->pushView(new DictationStyleFormViewHost);
  });
  return action;
}

} // namespace

void Dictation::openStyles(const ApplicationContext *ctx) {
  if (!hasCompletionModels(ctx) && ctx->services->dictation()->styles()->entries().empty()) {
    pushProviderSetup(ctx);
    return;
  }
  ctx->navigation->pushView(new DictationStylesViewHost);
}

void Dictation::openStyleForm(const ApplicationContext *ctx) {
  if (!hasCompletionModels(ctx)) {
    pushProviderSetup(ctx);
    return;
  }
  ctx->navigation->pushView(new DictationStyleFormViewHost);
}

std::optional<QString> Dictation::styleModelName(AI::Service &ai, const DictationStyles::Style &style) {
  const auto ref = AI::ModelRef::fromString(style.model);
  if (!ref) return std::nullopt;
  const auto models = ai.listModels(AI::Capability::Completion);
  const auto it = std::ranges::find_if(models, [&](const AI::ProviderModel &model) {
    return model.ref.provider == ref->provider && model.ref.id == ref->id;
  });
  if (it == models.end()) return std::nullopt;
  return QString::fromStdString(it->name);
}

void DictationStylesViewHost::onMount() {
  setSearchPlaceholderText(tr("Search styles..."));
  m_emptyTitle = tr("No styles yet");
  m_emptyDescription = tr("Create a style to have AI rewrite what you dictate.");
  m_emptyIcon = ImageUrl{Dictation::STYLE_ICON};

  auto *services = context()->services;
  connect(services->dictation()->styles(), &DictationStyles::changed, this, [this]() { reload(); });
  connect(services->ai(), &AI::Service::modelsChanged, this, [this]() { reload(); });
  reload();
}

void DictationStylesViewHost::reload() {
  const auto styles = context()->services->dictation()->styles()->entries();
  setItems({styles.begin(), styles.end()});
  if (styles.empty()) setActions(emptyActionPanel());
}

std::optional<ImageURL> DictationStylesViewHost::displayIcon(const ItemType &) const {
  return Dictation::STYLE_ICON;
}

AccessoryList DictationStylesViewHost::displayAccessories(const ItemType &style) const {
  auto *services = context()->services;
  AccessoryList accessories;
  accessories.reserve(style.apps.size() + 2);

  if (!Dictation::styleModelName(*services->ai(), style)) {
    accessories.emplace_back(ListAccessory{.text = tr("Model unavailable"), .color = SemanticColor::Red});
  }

  for (const auto &appId : style.apps) {
    const auto id = QString::fromStdString(appId);
    const auto app = services->appDb()->findById(id);
    accessories.emplace_back(ListAccessory{.tooltip = app ? app->displayName() : id,
                                           .icon = app ? std::optional(app->iconUrl()) : std::nullopt});
  }

  if (services->dictation()->styles()->isDefault(style.id)) {
    accessories.emplace_back(ListAccessory{.text = tr("Default"), .color = SemanticColor::Accent});
  } else if (style.apps.empty()) {
    accessories.emplace_back(ListAccessory{.text = tr("Not in use")});
  }

  return accessories;
}

std::optional<ListItemDetail> DictationStylesViewHost::displayDetail(const ItemType &style) const {
  auto *services = context()->services;
  QStringList apps;
  apps.reserve(static_cast<qsizetype>(style.apps.size()));
  for (const auto &appId : style.apps) {
    const auto id = QString::fromStdString(appId);
    const auto app = services->appDb()->findById(id);
    apps.append(app ? app->displayName() : id);
  }

  QVariantList meta;
  meta.append(QVariantMap{
      {QStringLiteral("label"), tr("Model")},
      {QStringLiteral("value"),
       Dictation::styleModelName(*services->ai(), style).value_or(tr("Unavailable"))},
  });
  if (!apps.isEmpty()) {
    meta.append(QVariantMap{
        {QStringLiteral("label"), tr("Applications")},
        {QStringLiteral("value"), apps.join(QStringLiteral(", "))},
    });
  }

  return ListItemDetail{.metadata = std::move(meta),
                        .markdown = Dictation::escapeMarkdown(QString::fromStdString(style.instructions))};
}

std::unique_ptr<ActionPanelState> DictationStylesViewHost::buildActionPanel(const ItemType &style) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *main = panel->createSection();

  auto *edit = new StaticAction(
      tr("Edit Style"), ImageURL::builtin(BuiltinIcon::Pencil),
      [style](ApplicationContext *ctx) { ctx->navigation->pushView(new DictationStyleFormViewHost(style)); });
  edit->setShortcut(Keybind::EditAction);
  main->addAction(edit);

  auto *create = createStyleAction(tr("Create Style"));
  create->setShortcut(Keybind::NewAction);
  main->addAction(create);

  const bool isDefault = context()->services->dictation()->styles()->isDefault(style.id);
  main->addAction(new StaticAction(
      isDefault ? tr("Stop Using as Default") : tr("Use as Default"), ImageURL::builtin(BuiltinIcon::Star),
      [id = style.id, isDefault](ApplicationContext *ctx) {
        ctx->services->dictation()->styles()->setDefault(isDefault ? std::nullopt : std::optional(id));
      }));

  auto *remove =
      new StaticAction(tr("Delete Style"), ImageURL::builtin(BuiltinIcon::Trash),
                       [id = style.id, name = QString::fromStdString(style.name)](ApplicationContext *ctx) {
                         auto *alert = new CallbackAlertWidget();
                         alert->setTitle(tr("Delete \"%1\"?").arg(name));
                         alert->setMessage(tr("Dictation will no longer use this style."));
                         alert->setConfirmText(tr("Delete"), SemanticColor::Red);
                         alert->setConfirmCallback([ctx, id]() {
                           if (!ctx->services->dictation()->styles()->remove(id)) {
                             ctx->services->toastService()->failure(tr("Failed to delete style"));
                           }
                         });
                         ctx->navigation->setDialog(alert);
                       });
  remove->setStyle(AbstractAction::Style::Danger);
  remove->setShortcut(Keybind::RemoveAction);
  main->addAction(remove);

  return panel;
}

std::unique_ptr<ActionPanelState> DictationStylesViewHost::emptyActionPanel() {
  auto panel = std::make_unique<ListActionPanelState>();
  panel->createSection()->addAction(createStyleAction(tr("Create Style")));
  return panel;
}
