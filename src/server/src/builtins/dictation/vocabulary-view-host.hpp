#pragma once
#include <memory>
#include <optional>
#include <string>
#include <QCoreApplication>
#include <QDateTime>
#include <QLocale>
#include <QString>
#include "actions/clipboard-actions.hpp"
#include "builtins/dictation/dictation.hpp"
#include "builtins/dictation/vocabulary-form-view-host.hpp"
#include "common/context.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/dictation/dictation-service.hpp"
#include "services/toast/toast-service.hpp"
#include "theme/colors.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/action-panel/action.hpp"
#include "ui/alert/alert.hpp"
#include "ui/views/mono-list-view-host.hpp"

template <> struct fuzzy::FuzzySearchable<DictationVocabulary::Entry> {
  static fuzzy::Match score(const DictationVocabulary::Entry &e, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{e.word, 1.0}}, query);
  }
};

class VocabularyViewHost : public MonoListViewHost<DictationVocabulary::Entry> {
  Q_DECLARE_TR_FUNCTIONS(VocabularyViewHost)

public:
  void onMount() override {
    setSearchPlaceholderText(tr("Search vocabulary..."));
    m_emptyTitle = tr("No words yet");
    m_emptyDescription = tr("Words you add here help dictation spell them right.");
    m_emptyIcon = ImageUrl{Dictation::VOCABULARY_ICON};

    auto *vocabulary = context()->services->dictation()->vocabulary();
    connect(vocabulary, &DictationVocabulary::changed, this, [this]() { reload(); });
    reload();
  }

  QString sectionName() const override { return tr("Words ({count})"); }
  QString displayTitle(const ItemType &e) const override { return QString::fromStdString(e.word); }
  QString displaySubtitle(const ItemType &) const override { return {}; }
  std::optional<ImageURL> displayIcon(const ItemType &) const override { return Dictation::VOCABULARY_ICON; }
  AccessoryList displayAccessories(const ItemType &e) const override {
    const auto added = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(e.createdAt));
    return {ListAccessory{.text = QLocale::system().toString(added, QLocale::ShortFormat),
                          .tooltip = tr("Added %1").arg(added.toString())}};
  }

  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &e) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto main = panel->createSection();
    const auto word = QString::fromStdString(e.word);

    main->addAction(new CopyToClipboardAction(Clipboard::Text{word}, tr("Copy")));

    auto add =
        new StaticAction(tr("Add Word"), ImageURL::builtin(BuiltinIcon::Plus), [](ApplicationContext *ctx) {
          ctx->navigation->pushView(new VocabularyFormViewHost);
        });
    add->setShortcut(Keybind::NewAction);
    main->addAction(add);

    auto remove = new StaticAction(
        tr("Remove Word"), ImageURL::builtin(BuiltinIcon::Trash), [word](ApplicationContext *ctx) {
          if (!ctx->services->dictation()->vocabulary()->remove(word.toStdString())) {
            ctx->services->toastService()->failure(tr("Failed to remove word"));
          }
        });
    remove->setStyle(AbstractAction::Style::Danger);
    remove->setShortcut(Keybind::RemoveAction);
    main->addAction(remove);

    auto removeAll = new StaticAction(tr("Remove All Words"), ImageURL::builtin(BuiltinIcon::Trash),
                                      [](ApplicationContext *ctx) {
                                        auto alert = new CallbackAlertWidget();
                                        alert->setTitle(tr("Remove all words?"));
                                        alert->setMessage(tr("Your whole vocabulary will be lost forever."));
                                        alert->setConfirmText(tr("Remove all"), SemanticColor::Red);
                                        alert->setConfirmCallback([ctx]() {
                                          ctx->services->dictation()->vocabulary()->clear();
                                          ctx->services->toastService()->success(tr("Vocabulary cleared"));
                                        });
                                        ctx->navigation->setDialog(alert);
                                      });
    removeAll->setStyle(AbstractAction::Style::Danger);
    removeAll->setShortcut(Keybind::DangerousRemoveAction);
    main->addAction(removeAll);

    return panel;
  }

private:
  void reload() {
    const auto entries = context()->services->dictation()->vocabulary()->entries();
    setItems({entries.rbegin(), entries.rend()});
  }
};
