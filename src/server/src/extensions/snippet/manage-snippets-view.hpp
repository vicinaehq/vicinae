#pragma once
#include <iterator>
#include <qapplication.h>
#include <ranges>
#include "builtin_icon.hpp"
#include "services/snippet/snippet-copy.hpp"
#include "common/context.hpp"
#include "common/types.hpp"
#include "extend/metadata-model.hpp"
#include "navigation-controller.hpp"
#include "placeholder.hpp"
#include "services/snippet/snippet-db.hpp"
#include "services/snippet/snippet-expander.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/detail/detail-widget.hpp"
#include "ui/image/url.hpp"
#include "ui/text-file-viewer/text-file-viewer.hpp"
#include "ui/views/typed-list-view.hpp"
#include "services/snippet/snippet-service.hpp"
#include "extensions/snippet/create-snippet-view.hpp"

class ManageSnippetsView : public FilteredTypedListView<snippet::SerializedSnippet> {
public:
  ImageURL getIcon(const snippet::SerializedSnippet &item) const {
    const auto visitor =
        overloads{[](const snippet::TextSnippet &text) { return BuiltinIcon::TextInput; },
                  [](const auto &other) { return BuiltinIcon::BlankDocument; }};

    return std::visit(visitor, item.data);
  }

  FilteredItemData mapFilteredData(const snippet::SerializedSnippet &item) const override {
    auto alias = item.expansion.transform([](auto &&e) { return e.keyword; });
    return {.id = item.name, .title = item.name, .icon = getIcon(item), .alias = alias};
  }

  std::unique_ptr<CompleterData>
  createCompleter(const snippet::SerializedSnippet &item) const override {
    const auto visitor =
        overloads{[&](const snippet::TextSnippet &text) -> std::unique_ptr<CompleterData> {
                    const auto str = PlaceholderString::parseSnippetText(text.text.c_str());
                    const auto args = str.arguments();
                    if (args.empty()) return nullptr;

                    CompleterData data;

                    data.iconUrl = getIcon(item);

                    const auto completerArgs =
                        args | std::views::transform([](auto &&arg) {
                          return CommandArgument{.name = arg.name,
                                                 .type = CommandArgument::Type::Text,
                                                 .placeholder = arg.name,
                                                 .required = arg.defaultValue.isEmpty()};
                        });

                    std::ranges::copy(completerArgs, std::back_inserter(data.arguments));

                    return std::make_unique<CompleterData>(data);
                  },
                  [](auto &&other) -> std::unique_ptr<CompleterData> { return nullptr; }};

    return std::visit(visitor, item.data);
  }

  QWidget *generateDetail(const snippet::SerializedSnippet &item) const override {
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

    const auto visitor = overloads{
        [this](const snippet::TextSnippet &snippet) {
          const auto result = m_expander.expand(snippet.text.c_str(), {});
          const auto expanded = result.parts | std::views::transform([](auto &&r) { return r.text; }) |
                                std::views::join | std::ranges::to<QString>();
          return expanded.toUtf8();
        },
        [](const snippet::FileSnippet &file) { return QByteArray::fromStdString(file.file); }};

    QByteArray buf = std::visit(visitor, item.data);

    viewer->load(buf);

    return detail;
  }

  std::unique_ptr<ActionPanelState>
  createActionPanel(const snippet::SerializedSnippet &item) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto section = panel->createSection();
    auto toast = context()->services->toastService();

    const auto copy =
        new StaticAction("Copy to clipboard", BuiltinIcon::CopyClipboard, [item](ApplicationContext *ctx) {
          auto clip = QApplication::clipboard();
          SnippetCopy::copyToClipboard(item, ctx->navigation->completionValues(), clip);
          ctx->navigation->showHud("Copied to clipboard");
        });

    const auto edit = new StaticAction("Edit snippet", BuiltinIcon::Pencil, [item](ApplicationContext *ctx) {
      ctx->navigation->pushView(new EditSnippetView(item));
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

    section->addAction(copy);
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

    connect(context()->navigation.get(), &NavigationController::completionValuesChanged, this,
            [this](const ArgumentValues &values) {
              const auto snippet = currentItem();

              if (!snippet) return;

              if (const auto text = std::get_if<snippet::TextSnippet>(&currentItem()->data)) {
                const auto result = m_expander.expand(text->text.c_str(), values);
                const auto expanded = result.parts | std::views::transform([](auto &&r) { return r.text; }) |
                                      std::views::join | std::ranges::to<QString>();

                if (const auto d = detail()) {
                  TextFileViewer *viewer =
                      static_cast<TextFileViewer *>(static_cast<DetailWidget *>(d)->content());
                  viewer->load(expanded.toUtf8());
                }
              }
            });

    connect(service, &SnippetService::snippetsChanged, this, [this]() {
      setDataSet(generateData());
      refreshCurrent();
    });

    setSearchPlaceholderText("Search by snippet name, contents or keyword...");

    return generateData();
  }

private:
  SnippetExpander m_expander;
};
