#pragma once
#include "ui/views/form-view.hpp"
#include "ui/views/grid-view.hpp"
#include "clipboard-actions.hpp"
#include "common-actions.hpp"
#include "common.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "omni-icon.hpp"
#include "services/emoji-service/emoji.hpp"
#include "services/toast/toast-service.hpp"
#include "timer.hpp"
#include "services/emoji-service/emoji-service.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/form.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/omni-grid/omni-grid.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/toast/toast.hpp"
#include "utils/utils.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <memory>
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <unistd.h>

class EditEmojiKeywordsView : public FormView {
  FormWidget *m_form = new FormWidget;
  BaseInput *m_keywords = new BaseInput;
  std::string_view m_emoji;

  void onActivate() override { m_form->focusFirst(); }

  void handleSubmit() {
    auto emojiService = context()->services->emojiService();
    auto toast = context()->services->toastService();

    if (emojiService->setCustomKeywords(m_emoji, m_keywords->text())) {
      toast->setToast("Keywords edited", ToastPriority::Success);
      popSelf();
    } else {
      toast->setToast("Failed to edit keywords", ToastPriority::Danger);
    }
  }

  void initialize() override {
    /*
auto emojiService = ServiceRegistry::instance()->emojiService();
auto panel = new ActionPanelStaticListView;
auto submit = new StaticAction("Edit keywords", BuiltinOmniIconUrl("text"), [this]() { handleSubmit(); });
auto metadata = emojiService->mapMetadata(m_emoji);

m_keywords->setText(metadata.keywords);
submit->setPrimary(true);
submit->setShortcut({.key = "return", .modifiers = {"shift"}});
panel->addAction(submit);
m_actionPannelV2->setView(panel);
  */
  }

public:
  EditEmojiKeywordsView(std::string_view emoji) : m_emoji(emoji) {
    auto inputField = new FormField();

    inputField->setWidget(m_keywords);
    inputField->setName("Keywords");

    m_form->addField(inputField);
    setupUI(m_form);
  }
};

class ResetEmojiRankingAction : public AbstractAction {
  std::string_view m_emoji;

public:
  void execute(ApplicationContext *ctx) override {
    auto emojiService = ctx->services->emojiService();
    auto toast = ctx->services->toastService();

    if (emojiService->resetRanking(m_emoji)) {
      toast->setToast("Ranking successfuly reset");
    } else {
      toast->setToast("Failed to reset ranking", ToastPriority::Danger);
    }
  }
  QString title() const override { return "Reset ranking"; }
  OmniIconUrl icon() const override { return BuiltinOmniIconUrl("arrow-counter-clockwise"); }

  ResetEmojiRankingAction(std::string_view emoji) : m_emoji(emoji) {}
};

class PasteEmojiAction : public PasteToFocusedWindowAction {
  std::string_view m_emoji;

public:
  void execute(ApplicationContext *ctx) override {
    auto emojiService = ctx->services->emojiService();
    PasteToFocusedWindowAction::execute(ctx);

    emojiService->registerVisit(m_emoji);
  }

  PasteEmojiAction(std::string_view emoji)
      : PasteToFocusedWindowAction(Clipboard::Text(qStringFromStdView(emoji))), m_emoji(emoji) {}
};

class PinEmojiAction : public AbstractAction {
  std::string_view m_emoji;

public:
  void execute(ApplicationContext *ctx) override {
    auto toast = ctx->services->toastService();

    if (ctx->services->emojiService()->pin(m_emoji)) {
      toast->setToast("Emoji pinned");
    } else {
      toast->setToast("Failed to pin emoji", ToastPriority::Danger);
    }
  }

  OmniIconUrl icon() const override { return BuiltinOmniIconUrl("pin"); }
  QString title() const override { return "Pin emoji"; };

  PinEmojiAction(std::string_view emoji) : m_emoji(emoji) {}
};

class EditEmojiKeywordsAction : public AbstractAction {
  std::string_view m_emoji;

  void execute(ApplicationContext *ctx) override {
    PushViewAction pushAction(title(), new EditEmojiKeywordsView(m_emoji), icon());

    pushAction.execute(ctx);
  }

  QString title() const override { return "Edit custom keywords"; }
  OmniIconUrl icon() const override { return BuiltinOmniIconUrl("text"); }

public:
  EditEmojiKeywordsAction(std::string_view emoji) : m_emoji(emoji) {}
};

class UnpinEmojiAction : public AbstractAction {
  std::string_view m_emoji;

public:
  void execute(ApplicationContext *ctx) override {
    auto toast = ctx->services->toastService();
    auto emojiService = ctx->services->emojiService();

    if (emojiService->unpin(m_emoji)) {
      toast->setToast("Emoji unpinned");
    } else {
      toast->setToast("Failed to unpin emoji", ToastPriority::Danger);
    }
  }

  OmniIconUrl icon() const override { return BuiltinOmniIconUrl("pin-disabled"); }
  QString title() const override { return "Unpin emoji"; };

  UnpinEmojiAction(std::string_view emoji) : m_emoji(emoji) {}
};

class EmojiGridItem : public OmniGrid::AbstractGridItem, public GridView::Actionnable {
public:
  const EmojiData &info;
  bool m_pinned = false;

  QString tooltip() const override { return qStringFromStdView(info.name); }

  QWidget *centerWidget() const override {
    auto icon = new Omnimg::ImageWidget();
    OmniIconUrl url;

    url.setType(OmniIconType::Emoji);
    url.setName(QString::fromStdString(std::string(info.emoji)));
    icon->setUrl(url);
    icon->setContentsMargins(10, 10, 10, 10);

    return icon;
  }

  bool centerWidgetRecyclable() const override { return true; }

  void recycleCenterWidget(QWidget *widget) const override {
    auto icon = static_cast<Omnimg::ImageWidget *>(widget);
    OmniIconUrl url;

    url.setType(OmniIconType::Emoji);
    url.setName(QString::fromStdString(std::string(info.emoji)));
    icon->setUrl(url);
    icon->setContentsMargins(10, 10, 10, 10);
  }

  QString generateId() const override { return QString::fromUtf8(info.emoji.data(), info.emoji.size()); }

  /*
  ActionPanelView *actionPanel() const override {
    auto panel = new ActionPanelStaticListView;
    auto paste = new PasteEmojiAction(info.emoji);
    auto copyName = new CopyToClipboardAction(
        Clipboard::Text(QString::fromUtf8(info.name.data(), info.name.size())), "Copy emoji name");
    auto copyGroup = new CopyToClipboardAction(
        Clipboard::Text(QString::fromUtf8(info.group.data(), info.group.size())), "Copy emoji group");
    auto editKeywords = new EditEmojiKeywordsAction(info.emoji);
    auto resetRanking = new ResetEmojiRankingAction(info.emoji);

    paste->setPrimary(true);
    panel->addAction(paste);
    panel->addAction(copyName);
    panel->addAction(copyGroup);
    panel->addAction(editKeywords);
    panel->addAction(resetRanking);

    if (m_pinned) {
      panel->addAction(new UnpinEmojiAction(info.emoji));
    } else {
      panel->addAction(new PinEmojiAction(info.emoji));
    }

    return panel;
  }
  */

  QString navigationTitle() const override { return qStringFromStdView(info.name); }

  EmojiGridItem(const EmojiData &info, bool pinned = false) : info(info), m_pinned(pinned) {}
};

class PinnedEmojiGridItem : public EmojiGridItem {
  QString generateId() const override { return QString("pinned.%1").arg(EmojiGridItem::generateId()); }
  using EmojiGridItem::EmojiGridItem;
};

class RecentlyUsedEmojiGridItem : public EmojiGridItem {
  QString generateId() const override { return QString("recent.%1").arg(EmojiGridItem::generateId()); }
  using EmojiGridItem::EmojiGridItem;
};

class EmojiView : public GridView {
public:
  void initialize() override {
    setSearchPlaceholderText("Search for emojis...");
    textChanged(searchText());
  }

  void resetList() {
    if (isVisible() && context()->navigation->searchText().isEmpty()) {
      generateRootList(OmniList::SelectionPolicy::PreserveSelection);
    }
  }

  void handlePinned(std::string_view emoji) { resetList(); }

  void handleUnpinned(std::string_view emoji) { resetList(); }

  void handleVisited(std::string_view emoji) { resetList(); }

  void generateRootList(OmniList::SelectionPolicy selectionPolicy = OmniList::SelectFirst) {
    m_grid->updateModel(
        [&]() {
          auto makeEmojiItem = [](auto &&item) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
            return std::make_unique<EmojiGridItem>(*item);
          };
          auto emojiService = ServiceRegistry::instance()->emojiService();
          Timer timer;
          auto visited = emojiService->getVisited();
          std::unordered_map<std::string_view, EmojiWithMetadata> metadataMap;

          for (const auto &item : visited) {
            metadataMap.insert({item.data->emoji, item});
          }

          timer.time("Get visited");

          size_t i = 0;

          auto &pinnedSection = m_grid->addSection("Pinned");

          pinnedSection.setColumns(8);
          pinnedSection.setSpacing(10);

          for (; i < visited.size() && visited[i].pinnedAt; ++i) {
            pinnedSection.addItem(std::make_unique<PinnedEmojiGridItem>(*visited[i].data, true));
          }

          auto &recentSection = m_grid->addSection("Recently used");

          recentSection.setColumns(8);
          recentSection.setSpacing(10);

          for (; i < visited.size(); ++i) {
            if (visited[i].visitCount > 0) {
              recentSection.addItem(std::make_unique<RecentlyUsedEmojiGridItem>(*visited[i].data, false));
            }
          }

          for (const auto &[group, emojis] : emojiService->grouped()) {
            auto &section = m_grid->addSection(QString::fromStdString(std::string(group)));

            section.setColumns(8);
            section.setSpacing(10);

            for (const auto &item : emojis) {
              if (auto it = metadataMap.find(item->emoji); it != metadataMap.end()) {
                section.addItem(
                    std::make_unique<EmojiGridItem>(*it->second.data, it->second.pinnedAt.has_value()));
              } else {
                section.addItem(std::make_unique<EmojiGridItem>(*item, false));
              }
            }
          }
        },
        selectionPolicy);
  }

  void generateFilteredList(const QString &s) {
    auto emojiService = ServiceRegistry::instance()->emojiService();
    Timer timer;
    auto matches = emojiService->mapMetadata(emojiService->search(s.toStdString()));
    timer.time("trie search");

    m_grid->updateModel([&]() {
      if (matches.empty()) { return; }

      auto &results = m_grid->addSection("Results");

      results.setColumns(8);
      results.setSpacing(10);

      for (const auto &match : matches) {
        results.addItem(std::make_unique<EmojiGridItem>(*match.data, match.pinnedAt.has_value()));
      }
    });
  }

  QString rootNavigationTitle() const override { return "Search Emojis"; }

  EmojiView() {
    auto emojiService = ServiceRegistry::instance()->emojiService();

    connect(emojiService, &EmojiService::pinned, this, &EmojiView::handlePinned);
    connect(emojiService, &EmojiService::unpinned, this, &EmojiView::handleUnpinned);
    connect(emojiService, &EmojiService::visited, this, &EmojiView::handleVisited);
  }

  void textChanged(const QString &s) override {
    if (s.isEmpty()) return generateRootList();

    return generateFilteredList(s);
  }
};
