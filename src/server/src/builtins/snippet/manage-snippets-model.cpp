#include <algorithm>
#include "builtins/snippet/manage-snippets-model.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "keyboard/keybind.hpp"
#include "builtins/snippet/snippet-form-view-host.hpp"
#include "service-registry.hpp"
#include "services/snippet/snippet-copy.hpp"
#include "services/snippet/snippet-service.hpp"
#include "services/toast/toast-service.hpp"

QString ManageSnippetsSection::displayTitle(const snippet::SerializedSnippet &item) const {
  return QString::fromStdString(item.name);
}

std::optional<ImageURL> ManageSnippetsSection::displayIcon(const snippet::SerializedSnippet &item) const {
  const auto visitor = overloads{
      [](const snippet::TextSnippet &) { return ImageURL(BuiltinIcon::TextInput); },
      [](const auto &) { return ImageURL(BuiltinIcon::BlankDocument); },
  };
  return std::visit(visitor, item.data);
}

AccessoryList ManageSnippetsSection::displayAccessories(const snippet::SerializedSnippet &item) const {
  if (item.expansion) return {{.text = QString::fromStdString(item.expansion->keyword)}};
  return {};
}

std::unique_ptr<ActionPanelState>
ManageSnippetsSection::buildActionPanel(const snippet::SerializedSnippet &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();

  auto copy =
      new StaticAction(tr("Copy to clipboard"), BuiltinIcon::CopyClipboard, [item](ApplicationContext *ctx) {
        auto clipman = ctx->services->clipman();
        if (SnippetCopy::copyToClipboard(item, ctx->navigation->completionValues(), *ctx->services->appDb(),
                                         *clipman)) {
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

  if (m_query.empty()) {
    const auto pos = std::ranges::find_if(m_items, [&](const auto &entry) { return entry.id == item.id; });

    if (pos != m_items.end()) {
      if (pos != m_items.begin()) {
        auto moveUp = new StaticAction(tr("Move up"), BuiltinIcon::ArrowUp, [item](ApplicationContext *ctx) {
          if (const auto result = ctx->services->snippetService()->moveSnippetUp(item.id); !result) {
            ctx->services->toastService()->failure(tr("Failed to move snippet"));
          }
        });
        moveUp->setShortcut(Keybind::MoveUpAction);
        section->addAction(moveUp);
      }

      if (std::next(pos) != m_items.end()) {
        auto moveDown =
            new StaticAction(tr("Move down"), BuiltinIcon::ArrowDown, [item](ApplicationContext *ctx) {
              if (const auto result = ctx->services->snippetService()->moveSnippetDown(item.id); !result) {
                ctx->services->toastService()->failure(tr("Failed to move snippet"));
              }
            });
        moveDown->setShortcut(Keybind::MoveDownAction);
        section->addAction(moveDown);
      }
    }
  }

  section->addAction(remove);

  return panel;
}
