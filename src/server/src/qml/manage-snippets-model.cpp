#include "manage-snippets-model.hpp"
#include "builtin_icon.hpp"
#include "keyboard/keybind.hpp"
#include "snippet-form-view-host.hpp"
#include "service-registry.hpp"
#include "services/snippet/snippet-copy.hpp"
#include "services/snippet/snippet-service.hpp"
#include "services/toast/toast-service.hpp"
#include <QGuiApplication>

QString ManageSnippetsModel::displayTitle(const snippet::SerializedSnippet &item) const {
  return QString::fromStdString(item.name);
}

QString ManageSnippetsModel::displayIconSource(const snippet::SerializedSnippet &item) const {
  const auto visitor =
      overloads{[this](const snippet::TextSnippet &) { return imageSourceFor(BuiltinIcon::TextInput); },
                [this](const auto &) { return imageSourceFor(BuiltinIcon::BlankDocument); }};
  return std::visit(visitor, item.data);
}

QString ManageSnippetsModel::displayAccessory(const snippet::SerializedSnippet &item) const {
  if (item.expansion) return QString::fromStdString(item.expansion->keyword);
  return {};
}

std::unique_ptr<ActionPanelState>
ManageSnippetsModel::buildActionPanel(const snippet::SerializedSnippet &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();

  auto copy =
      new StaticAction("Copy to clipboard", BuiltinIcon::CopyClipboard, [item](ApplicationContext *ctx) {
        auto clip = QGuiApplication::clipboard();
        SnippetCopy::copyToClipboard(item, ctx->navigation->completionValues(), clip);
        ctx->navigation->showHud("Copied to clipboard");
      });

  auto edit = new StaticAction("Edit snippet", BuiltinIcon::Pencil, [item](ApplicationContext *ctx) {
    ctx->navigation->pushView(new SnippetFormViewHost(item, SnippetFormViewHost::Mode::Edit));
  });

  auto duplicate =
      new StaticAction("Duplicate snippet", BuiltinIcon::Duplicate, [item](ApplicationContext *ctx) {
        ctx->navigation->pushView(new SnippetFormViewHost(item, SnippetFormViewHost::Mode::Duplicate));
      });

  auto remove = new StaticAction("Remove snippet", BuiltinIcon::Trash, [item](ApplicationContext *ctx) {
    if (const auto result = ctx->services->snippetService()->removeSnippet(item.id); !result) {
      ctx->services->toastService()->failure("Failed to remove snippet");
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

void ManageSnippetsModel::itemSelected(const snippet::SerializedSnippet &item) {
  emit snippetSelected(item);
}

QString ManageSnippetsModel::sectionLabel() const {
  return QStringLiteral("Snippets ({count})");
}
