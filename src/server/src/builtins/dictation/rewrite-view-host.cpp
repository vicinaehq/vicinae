#include "rewrite-view-host.hpp"
#include "actions/clipboard-actions.hpp"
#include "builtins/dictation/dictation.hpp"
#include "builtins/dictation/style-form-view-host.hpp"
#include "common/context.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/dictation/dictation-service.hpp"
#include "theme/colors.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/action-panel/action.hpp"

void DictationRewriteViewHost::onMount() {
  setSearchPlaceholderText(tr("Search styles..."));
  m_emptyTitle = tr("No styles yet");
  m_emptyDescription = tr("Create a style to try it on this dictation.");
  m_emptyIcon = ImageUrl{Dictation::STYLE_ICON};

  const auto reload = [this]() {
    const auto styles = context()->services->dictation()->styles()->entries();
    setItems({styles.begin(), styles.end()});
    if (styles.empty()) setActions(emptyActionPanel());
  };
  connect(context()->services->dictation()->styles(), &DictationStyles::changed, this, reload);
  reload();
}

ImageURL DictationRewriteViewHost::initialNavigationIcon() const { return Dictation::STYLE_ICON; }

std::optional<ImageURL> DictationRewriteViewHost::displayIcon(const ItemType &) const {
  return Dictation::STYLE_ICON;
}

AccessoryList DictationRewriteViewHost::displayAccessories(const ItemType &style) const {
  const auto it = m_runs.find(style.id);
  if (it == m_runs.end()) return {};
  const auto &run = it->second;
  if (run.rewrite->running()) return {ListAccessory{.text = tr("Rewriting…")}};
  if (run.error) return {ListAccessory{.text = tr("Failed"), .color = SemanticColor::Red}};
  return {ListAccessory{.text = tr("Done"), .color = SemanticColor::Green}};
}

std::optional<ListItemDetail> DictationRewriteViewHost::displayDetail(const ItemType &style) const {
  const auto it = m_runs.find(style.id);
  const auto *run = it == m_runs.end() ? nullptr : &it->second;
  const bool rewritten = run && run->result;

  QVariantList meta;
  meta.append(QVariantMap{
      {QStringLiteral("label"), tr("Showing")},
      {QStringLiteral("value"), rewritten ? tr("Rewritten text") : tr("Original text")},
  });
  meta.append(QVariantMap{
      {QStringLiteral("label"), tr("Model")},
      {QStringLiteral("value"),
       Dictation::styleModelName(*context()->services->ai(), style).value_or(tr("Unavailable"))},
  });
  if (run && run->error) {
    meta.append(QVariantMap{{QStringLiteral("label"), tr("Error")}, {QStringLiteral("value"), *run->error}});
  }

  return ListItemDetail{.metadata = std::move(meta),
                        .markdown = Dictation::escapeMarkdown(rewritten ? *run->result : m_text)};
}

std::unique_ptr<ActionPanelState> DictationRewriteViewHost::buildActionPanel(const ItemType &style) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *main = panel->createSection();
  auto *self = const_cast<DictationRewriteViewHost *>(this);
  const auto it = m_runs.find(style.id);

  if (it != m_runs.end() && it->second.rewrite->running()) return panel;

  const auto addRewrite = [&](const QString &title) {
    main->addAction(new StaticAction(title, ImageURL::builtin(BuiltinIcon::Wand),
                                     [self, style]() { self->start(style); }));
  };

  if (it != m_runs.end() && it->second.result) {
    const auto text = Clipboard::Text{*it->second.result};
    main->addAction(new PasteToFocusedWindowAction(text));
    main->addAction(new CopyToClipboardAction(text, tr("Copy")));
    addRewrite(tr("Rewrite Again"));
  } else {
    addRewrite(tr("Rewrite"));
  }

  return panel;
}

std::unique_ptr<ActionPanelState> DictationRewriteViewHost::emptyActionPanel() {
  auto panel = std::make_unique<ListActionPanelState>();
  panel->createSection()->addAction(
      new StaticAction(tr("Create Style"), ImageURL::builtin(BuiltinIcon::Plus), [](ApplicationContext *ctx) {
        ctx->navigation->pushView(new DictationStyleFormViewHost);
      }));
  return panel;
}

void DictationRewriteViewHost::beforePop() {
  MonoListViewHost::beforePop();
  for (const auto &[id, run] : m_runs)
    run.rewrite->abort();
}

void DictationRewriteViewHost::start(const ItemType &style) {
  auto &run = m_runs[style.id];
  if (!run.rewrite) {
    run.rewrite = new DictationRewrite(context()->services->dictation()->ai(), this);
    connect(run.rewrite, &DictationRewrite::finished, this, [this, &run](const QString &text) {
      run.result = text;
      refresh();
    });
    connect(run.rewrite, &DictationRewrite::failed, this, [this, &run](const QString &reason) {
      run.error = reason;
      refresh();
    });
  }
  run.result.reset();
  run.error.reset();
  run.rewrite->start(style, m_text);
  refresh();
}

void DictationRewriteViewHost::refresh() {
  notifyChanged();
  refreshDetail();
  model()->refreshActionPanel();
}
