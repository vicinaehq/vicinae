#pragma once
#include <QCoreApplication>
#include <QDateTime>
#include <QString>
#include <QVariantMap>
#include <memory>
#include "actions/clipboard-actions.hpp"
#include "builtins/dictation/dictation.hpp"
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

template <> struct fuzzy::FuzzySearchable<DictationHistory::Entry> {
  static fuzzy::Match score(const DictationHistory::Entry &e, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{e.text, 1.0}}, query);
  }
};

class DictationHistoryViewHost : public MonoListViewHost<DictationHistory::Entry> {
  Q_DECLARE_TR_FUNCTIONS(DictationHistoryViewHost)

public:
  void onMount() override {
    setSearchPlaceholderText(tr("Search dictations..."));
    m_emptyTitle = tr("No dictations yet");
    m_emptyDescription = tr("Everything you dictate shows up here.");
    m_emptyIcon = ImageUrl{Dictation::ICON};

    auto *history = context()->services->dictation()->history();
    connect(history, &DictationHistory::changed, this, [this]() { reload(); });
    reload();
  }

  QString displayId(const ItemType &e) const override { return QString::number(e.createdAt); }

  QString displayTitle(const ItemType &e) const override {
    constexpr auto MAX_TITLE_LENGTH = 80;
    auto title = QString::fromStdString(e.text).simplified();
    if (title.size() > MAX_TITLE_LENGTH) title = title.left(MAX_TITLE_LENGTH - 1) + QStringLiteral("…");
    return title;
  }

  QString displaySubtitle(const ItemType &) const override { return {}; }

  std::optional<ImageURL> displayIcon(const ItemType &) const override { return Dictation::ICON; }

  AccessoryList displayAccessories(const ItemType &) const override { return {}; }

  bool hasDetailPane() const override { return true; }

  std::optional<ListItemDetail> displayDetail(const ItemType &e) const override {
    QVariantList meta;
    meta.append(QVariantMap{
        {QStringLiteral("label"), tr("Dictated at")},
        {QStringLiteral("value"), QDateTime::fromSecsSinceEpoch(static_cast<qint64>(e.createdAt)).toString()},
    });
    meta.append(QVariantMap{
        {QStringLiteral("label"), tr("Duration")},
        {QStringLiteral("value"), formatDuration(e.durationMs)},
    });
    meta.append(QVariantMap{
        {QStringLiteral("label"), tr("Words")},
        {QStringLiteral("value"), QString::number(wordCount(e))},
    });
    meta.append(QVariantMap{
        {QStringLiteral("label"), tr("Language")},
        {QStringLiteral("value"), e.language ? QString::fromStdString(*e.language) : tr("Auto-detected")},
    });

    return ListItemDetail{.metadata = std::move(meta),
                          .markdown = escapeMarkdown(QString::fromStdString(e.text))};
  }

  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &e) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto main = panel->createSection();
    const auto text = Clipboard::Text{QString::fromStdString(e.text)};

    main->addAction(new CopyToClipboardAction(text, tr("Copy")));
    main->addAction(new PasteToFocusedWindowAction(text));

    auto remove = new StaticAction(
        tr("Remove Dictation"), ImageURL::builtin(BuiltinIcon::Trash), [e](ApplicationContext *ctx) {
          if (!ctx->services->dictation()->history()->remove(e)) {
            ctx->services->toastService()->failure(tr("Failed to remove dictation"));
          }
        });
    remove->setStyle(AbstractAction::Style::Danger);
    remove->setShortcut(Keybind::RemoveAction);
    main->addAction(remove);

    auto removeAll = new StaticAction(
        tr("Remove All Dictations"), ImageURL::builtin(BuiltinIcon::Trash), [](ApplicationContext *ctx) {
          auto alert = new CallbackAlertWidget();
          alert->setTitle(tr("Remove all dictations?"));
          alert->setMessage(tr("Your whole dictation history will be lost forever."));
          alert->setConfirmText(tr("Remove all"), SemanticColor::Red);
          alert->setConfirmCallback([ctx]() {
            ctx->services->dictation()->history()->clear();
            ctx->services->toastService()->success(tr("Dictation history cleared"));
          });
          ctx->navigation->setDialog(alert);
        });
    removeAll->setStyle(AbstractAction::Style::Danger);
    removeAll->setShortcut(Keybind::DangerousRemoveAction);
    main->addAction(removeAll);

    return panel;
  }

private:
  static int wordCount(const ItemType &e) {
    return static_cast<int>(
        QString::fromStdString(e.text).simplified().split(QLatin1Char(' '), Qt::SkipEmptyParts).size());
  }

  static QString formatDuration(std::uint64_t ms) {
    const auto secs = ms / 1000;
    return QStringLiteral("%1:%2")
        .arg(secs / 60, 2, 10, QLatin1Char('0'))
        .arg(secs % 60, 2, 10, QLatin1Char('0'));
  }

  // dictated text is shown verbatim, never interpreted as markup
  static QString escapeMarkdown(QString text) {
    static const auto SPECIAL = QStringLiteral("\\`*_{}[]()#+-.!<>|~");
    QString out;
    out.reserve(text.size() * 2);
    for (const auto ch : text) {
      if (SPECIAL.contains(ch)) out += QLatin1Char('\\');
      out += ch;
    }
    return out;
  }

  void reload() {
    const auto entries = context()->services->dictation()->history()->entries();
    setItems({entries.begin(), entries.end()});
  }
};
