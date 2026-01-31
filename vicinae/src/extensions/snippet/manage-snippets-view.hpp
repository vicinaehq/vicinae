#include "clipboard-actions.hpp"
#include "common/context.hpp"
#include "extend/metadata-model.hpp"
#include "navigation-controller.hpp"
#include "services/snippet/snippet-db.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/detail/detail-widget.hpp"
#include "ui/text-file-viewer/text-file-viewer.hpp"
#include "ui/views/typed-list-view.hpp"
#include "services/snippet/snippet-service.hpp"
#include "extensions/snippet/create-snippet-view.hpp"
#include <qaccessible_base.h>
#include <type_traits>

class ManageSnippetsView : public FilteredTypedListView<SnippetDatabase::SerializedSnippet> {
public:
  FilteredItemData mapFilteredData(const SnippetDatabase::SerializedSnippet &item) const override {
    return {.id = item.name, .title = item.name, .icon = BuiltinIcon::BlankDocument};
  }

  QWidget *generateDetail(const SnippetDatabase::SerializedSnippet &item) const override {
    auto detail = new DetailWidget;
    auto viewer = new TextFileViewer;
    std::vector<MetadataItem> metadata = {
        MetadataLabel{.text = "Text", .title = "Type"},
    };

    const auto createdAt = QDateTime::fromSecsSinceEpoch(item.createdAt).toString();

    metadata.emplace_back(MetadataLabel{.text = createdAt, .title = "Created at"});

    if (item.updatedAt) {
      const auto updatedAt = QDateTime::fromSecsSinceEpoch(item.updatedAt.value()).toString();
      metadata.emplace_back(MetadataLabel{.text = updatedAt, .title = "Updated at"});
    }

    if (item.expansion) {
      metadata.emplace_back(MetadataLabel{.text = item.expansion->keyword.c_str(), .title = "Keyword"});
    }

    detail->setContent(viewer);
    detail->setMetadata(metadata);
    std::visit(
        [viewer](const auto &d) {
          if constexpr (std::is_same_v<std::decay_t<decltype(d)>, SnippetDatabase::TextSnippet>)
            viewer->load(QByteArray::fromStdString(d.text));
          else
            viewer->load(QByteArray::fromStdString(d.file));
        },
        item.data);

    return detail;
  }

  std::unique_ptr<ActionPanelState>
  createActionPanel(const SnippetDatabase::SerializedSnippet &item) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto section = panel->createSection();
    auto toast = context()->services->toastService();

    const auto edit = new StaticAction("Edit snippet", BuiltinIcon::Pencil, [item](ApplicationContext *ctx) {
      ctx->navigation->pushView(new UpdateSnippetView(item));
    });
    const auto duplicate =
        new StaticAction("Duplicate snippet", BuiltinIcon::Pencil, [item](ApplicationContext *ctx) {
          ctx->navigation->pushView(new DuplicateSnippetView(item));
        });
    const auto remove =
        new StaticAction("Remove snippet", BuiltinIcon::Trash, [item, toast](ApplicationContext *ctx) {
          if (const auto result = ctx->services->snippetService()->removeSnippet(item.id); !result) {
            toast->failure("Failed to remove snippet");
          }
        });

    edit->setShortcut(Keybind::EditAction);
    duplicate->setShortcut(Keybind::DuplicateAction);
    remove->setShortcut(Keybind::RemoveAction);

    section->addAction(new CopyToClipboardAction(Clipboard::Text()));
    section->addAction(edit);
    section->addAction(duplicate);
    section->addAction(remove);

    return panel;
  }

  std::vector<Section> generateData() {
    return {Section("Snippets ({count})", context()->services->snippetService()->database()->snippets())};
  }

  DataSet initializeDataSet() override {
    auto service = context()->services->snippetService();

    connect(service, &SnippetService::snippetsChanged, this, [this]() {
      setDataSet(generateData());
      refreshCurrent();
    });

    setSearchPlaceholderText("Search by snippet name, contents or keyword...");

    return generateData();
  }
};
