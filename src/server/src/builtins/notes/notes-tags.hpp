#pragma once

#include <QCoreApplication>
#include <algorithm>
#include <array>
#include <cstdint>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include "common/context.hpp"
#include "service-registry.hpp"
#include "services/notes/notes-front-matter.hpp"
#include "services/notes/notes-store.hpp"
#include "services/toast/toast-service.hpp"
#include "theme/colors.hpp"
#include "ui/action-panel/action-list-view.hpp"
#include "ui/action-panel/action-panel-model.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/action-panel/action.hpp"

namespace notes_tags {

constexpr std::array COLORS = {SemanticColor::Blue,   SemanticColor::Green, SemanticColor::Magenta,
                               SemanticColor::Orange, SemanticColor::Red,   SemanticColor::Yellow,
                               SemanticColor::Cyan};

constexpr std::uint32_t FNV_OFFSET = 2166136261U;
constexpr std::uint32_t FNV_PRIME = 16777619U;

// FNV-1a rather than std::hash, so that a tag has the same color on every build and platform.
constexpr SemanticColor color(std::string_view tag) {
  auto hash = FNV_OFFSET;
  for (const unsigned char c : tag) {
    const std::uint32_t lower = c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c;
    hash = (hash ^ lower) * FNV_PRIME;
  }
  return COLORS[hash % COLORS.size()];
}

} // namespace notes_tags

/**
 * Lists every known tag and toggles them on a note. Typing a name that is not a tag yet offers to
 * create it.
 */
class NoteTagsPanel : public ActionListView {
  Q_DECLARE_TR_FUNCTIONS(NoteTagsPanel)

public:
  NoteTagsPanel(ApplicationContext &ctx, std::string noteId, std::vector<std::string> applied,
                const std::vector<std::string> &known, QObject *parent)
      : ActionListView(parent), m_ctx(ctx), m_noteId(std::move(noteId)), m_tags(std::move(applied)),
        m_applied(m_tags.begin(), m_tags.end()) {
    m_tags.reserve(m_tags.size() + known.size());
    for (const auto &tag : known)
      if (!m_applied.contains(tag)) m_tags.emplace_back(tag);
    connect(model(), &ActionPanelModel::filterChanged, this, [this](const QString &text) {
      m_filter = text.trimmed().toStdString();
      rebuild();
    });
    rebuild();
  }

  QVariantMap componentProps() override {
    auto props = ActionListView::componentProps();
    props[QStringLiteral("emptyText")] = tr("Type to create a tag");
    props[QStringLiteral("filterPlaceholder")] = tr("Filter or create a tag…");
    return props;
  }

protected:
  // Tags apply in place, so that several can be changed without reopening the panel.
  void activate(AbstractAction *action) override { action->execute(&m_ctx); }

private:
  void rebuild() {
    auto state = std::make_unique<ActionPanelState>();
    auto *section = state->createSection();
    for (const auto &tag : m_tags) {
      const auto icon = m_applied.contains(tag) ? BuiltinIcon::CheckCircle : BuiltinIcon::Circle;
      section->addAction(new StaticAction(QString::fromStdString(tag),
                                          ImageURL::builtin(icon).setFill(notes_tags::color(tag)),
                                          [this, tag] { toggle(tag); }));
    }
    if (Notes::isValidTag(m_filter) && !std::ranges::contains(m_tags, m_filter))
      state->createSection()->addAction(
          new StaticAction(tr("Create tag “%1”").arg(QString::fromStdString(m_filter)),
                           ImageURL::builtin(BuiltinIcon::Plus), [this, tag = m_filter] { toggle(tag); }));
    adoptState(std::move(state));
  }

  void toggle(const std::string &tag) {
    if (!m_applied.erase(tag)) m_applied.emplace(tag);
    if (!std::ranges::contains(m_tags, tag)) m_tags.emplace_back(tag);
    auto applied =
        m_tags | std::views::filter([this](const std::string &known) { return m_applied.contains(known); }) |
        std::ranges::to<std::vector>();
    m_ctx.services->notes()
        ->setTags(m_noteId, std::move(applied))
        .then(this, [this](const Notes::Result<Notes::Note> &result) {
          if (!result)
            m_ctx.services->toastService()->failure(
                tr("Could not change tags. %1").arg(QString::fromStdString(result.error())));
        });
    rebuild();
  }

  ApplicationContext &m_ctx;
  std::string m_noteId;
  std::vector<std::string> m_tags;
  std::set<std::string> m_applied;
  std::string m_filter;
};

class EditNoteTagsAction : public SubmenuAction {
  Q_DECLARE_TR_FUNCTIONS(EditNoteTagsAction)

public:
  EditNoteTagsAction(const Notes::Note &note, std::vector<std::string> known)
      : SubmenuAction(tr("Edit Tags…"), ImageURL::builtin(BuiltinIcon::Tag)), m_noteId(note.id),
        m_applied(note.tags), m_known(std::move(known)) {}

  ActionPanelView *createView(ApplicationContext *ctx, QObject *parent) override {
    return new NoteTagsPanel(*ctx, m_noteId, m_applied, m_known, parent);
  }

private:
  std::string m_noteId;
  std::vector<std::string> m_applied;
  std::vector<std::string> m_known;
};
