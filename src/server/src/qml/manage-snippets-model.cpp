#include "manage-snippets-model.hpp"
#include "builtin_icon.hpp"
#include "keyboard/keybind.hpp"
#include "snippet-form-view-host.hpp"
#include "service-registry.hpp"
#include "services/snippet/snippet-copy.hpp"
#include "services/snippet/snippet-service.hpp"
#include "services/toast/toast-service.hpp"

QString ManageSnippetsSection::displayTitle(const snippet::SerializedSnippet &item) const {
  return QString::fromStdString(item.name);
}

QString ManageSnippetsSection::displayIconSource(const snippet::SerializedSnippet &item) const {
  const auto visitor =
      overloads{[this](const snippet::TextSnippet &) { return imageSourceFor(BuiltinIcon::TextInput); },
                [this](const auto &) { return imageSourceFor(BuiltinIcon::BlankDocument); }};
  return std::visit(visitor, item.data);
}

QVariantList ManageSnippetsSection::displayAccessories(const snippet::SerializedSnippet &item) const {
  if (item.expansion) return qml::textAccessory(QString::fromStdString(item.expansion->keyword));
  return {};
}

std::unique_ptr<ActionPanelState>
ManageSnippetsSection::buildActionPanel(const snippet::SerializedSnippet &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();

  auto copy =
      new StaticAction(tr("Copy to clipboard"), BuiltinIcon::CopyClipboard, [item](ApplicationContext *ctx) {
        auto clipman = ctx->services->clipman();
        if (SnippetCopy::copyToClipboard(item, ctx->navigation->completionValues(), *clipman)) {
          ctx->navigation->showHud(tr("Copied to clipboard"));
        } else {
          ctx->services->toastService()->failure(tr("Failed to copy to clipboard"));
        }
      });

  auto edit = new StaticAction(tr("Edit snippet"), BuiltinIcon::Pencil, [item](ApplicationContext *ctx) {
    ctx->navigation->pushView(new SnippetFormViewHost(item, SnippetFormViewHost::Mode::Edit));
  });

  auto duplicate =
      new StaticAction(tr("Duplicate snippet"), BuiltinIcon::Duplicate, [item](ApplicationContext *ctx) {
        ctx->navigation->pushView(new SnippetFormViewHost(item, SnippetFormViewHost::Mode::Duplicate));
      });

  auto remove = new StaticAction(tr("Remove snippet"), BuiltinIcon::Trash, [item](ApplicationContext *ctx) {
    if (const auto result = ctx->services->snippetService()->removeSnippet(item.id); !result) {
      ctx->services->toastService()->failure(tr("Failed to remove snippet"));
    }
  });

  edit->setShortcut(Keybind::EditAction);
  duplicate->setShortcut(Keybind::DuplicateAction);
  remove->setShortcut(Keybind::RemoveAction);

  section->addAction(copy);
  section->addAction(edit);
  section->addAction(duplicate);
  section->addAction(remove);

  return panel;
}
