#pragma once

#include <QDateTime>
#include <QLocale>
#include "builtins/ai/quick-ai-view-host.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/ai/conversation-store.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/views/mono-list-view-host.hpp"

template <> struct fuzzy::FuzzySearchable<AI::ConversationInfo> {
  static fuzzy::Match score(const AI::ConversationInfo &conversation, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{conversation.title, 1.0}}, query);
  }
};

class ConversationHistoryViewHost : public MonoListViewHost<AI::ConversationInfo> {
  Q_DECLARE_TR_FUNCTIONS(ConversationHistoryViewHost)

public:
  void onMount() override {
    setSearchPlaceholderText(tr("Filter conversations..."));
    reload();
  }

  void onReactivated() override {
    ListViewHost::onReactivated();
    reload();
  }

  QString displayId(const ItemType &item) const override { return QString::fromStdString(item.id); }
  QString displayTitle(const ItemType &item) const override { return QString::fromStdString(item.title); }
  QString displaySubtitle(const ItemType &item) const override {
    return QLocale().toString(QDateTime::fromMSecsSinceEpoch(item.updatedAt), QLocale::ShortFormat);
  }
  std::optional<ImageURL> displayIcon(const ItemType &) const override {
    return ImageURL::builtin(BuiltinIcon::Stars);
  }
  AccessoryList displayAccessories(const ItemType &) const override { return {}; }

  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &item) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto *section = panel->createSection();
    section->addAction(new StaticAction(tr("Open Conversation"), ImageURL::builtin(BuiltinIcon::ArrowRight),
                                        [id = item.id](ApplicationContext *ctx) {
                                          QuickAIViewHost::openConversation(
                                              ctx->navigation->topState()->sender, id);
                                        }));

    auto *remove = new StaticAction(
        tr("Delete Conversation"), ImageURL::builtin(BuiltinIcon::Trash),
        [id = item.id](ApplicationContext *ctx) {
          auto *store = ctx->services->conversations();
          store->remove(id).then(ctx->services->toastService(), [ctx](const AI::Result<void> &result) {
            if (!result) ctx->services->toastService()->failure(tr("Could not delete conversation."));
          });
        });
    remove->setStyle(AbstractAction::Style::Danger);
    remove->setShortcut(Keybind::RemoveAction);
    section->addAction(remove);
    return panel;
  }

private:
  void reload() {
    auto *store = context()->services->conversations();
    if (!m_connected) {
      connect(store, &AI::ConversationStore::changed, this, [this] {
        const auto *state = context()->navigation->topState();
        if (state && state->sender == this) reload();
      });
      m_connected = true;
    }
    const auto generation = ++m_generation;
    setLoading(true);
    store->list().then(this, [this, generation](AI::Result<std::vector<AI::ConversationInfo>> result) {
      if (generation != m_generation) return;
      setLoading(false);
      if (result)
        setItems(std::move(*result));
      else
        context()->services->toastService()->failure(tr("Could not load conversations."));
    });
  }

  bool m_connected = false;
  std::uint64_t m_generation = 0;
};
