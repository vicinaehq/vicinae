#pragma once

#include <algorithm>
#include <ranges>
#include <set>
#include <utility>
#include "common/enumerate.hpp"
#include "navigation-controller.hpp"
#include "notes-search.hpp"
#include "notes-tags.hpp"
#include "service-registry.hpp"
#include "services/notes/notes-front-matter.hpp"
#include "services/notes/notes-store.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/quick/completion-model.hpp"
#include "ui/views/mono-list-view-host.hpp"
#include "ui/windows/notes-window-controller.hpp"

class NotesViewHost : public MonoListViewHost<Notes::Note> {
  Q_OBJECT
  Q_PROPERTY(CompletionModel *categoryFilterModel READ categoryFilterModel CONSTANT)
  Q_PROPERTY(int currentCategoryFilter READ currentCategoryFilter NOTIFY currentCategoryFilterChanged)

public:
  Q_INVOKABLE void setCategoryFilter(int index) {
    const bool valid = index >= 1 && std::cmp_less_equal(index, m_tags.size());
    auto filter = valid ? std::optional(m_tags[index - 1]) : std::nullopt;
    if (filter == m_tagFilter) return;
    m_tagFilter = std::move(filter);
    emit currentCategoryFilterChanged();
    applyTagFilter();
  }

signals:
  void currentCategoryFilterChanged();

public:
  QUrl qmlSearchAccessoryUrl() const override { return qml::componentUrl(u"CategoryFilterAccessory"); }

  CompletionModel *categoryFilterModel() { return &m_tagFilterModel; }
  int currentCategoryFilter() const {
    if (!m_tagFilter) return 0;
    return static_cast<int>(std::ranges::find(m_tags, *m_tagFilter) - m_tags.begin()) + 1;
  }

  void onMount() override {
    setSearchPlaceholderText(tr("Search notes…"));
    m_emptyTitle = tr("No notes found");
    m_emptyDescription = tr("Create a note to start writing.");
    m_emptyIcon = ImageUrl{ImageURL::builtin(BuiltinIcon::Feather).setBackgroundTint(SemanticColor::Yellow)};
    rebuildTagFilter();
    connect(context()->services->notes(), &Notes::Store::changed, this, [this] {
      if (context()->navigation->isWindowOpened()) reload();
    });
    connect(context()->navigation.get(), &NavigationController::windowVisiblityChanged, this,
            [this](bool visible) {
              if (visible) reload();
            });
    reload();
  }

  void onReactivated() override {
    ListViewHost::onReactivated();
    reload();
  }

  QString displayId(const ItemType &note) const override { return QString::fromStdString(note.id); }
  QString displayTitle(const ItemType &note) const override {
    return note.title.empty() ? tr("Untitled Note") : QString::fromStdString(note.title);
  }
  QString displaySubtitle(const ItemType &) const override { return {}; }
  std::optional<ImageURL> displayIcon(const ItemType &) const override {
    return ImageURL::builtin(BuiltinIcon::Feather).setBackgroundTint(SemanticColor::Yellow);
  }
  AccessoryList displayAccessories(const ItemType &note) const override {
    AccessoryList accessories;
    accessories.reserve(MAX_VISIBLE_TAGS + 1);
    for (const auto &tag : note.tags | std::views::take(MAX_VISIBLE_TAGS))
      accessories.emplace_back(ListAccessory{
          .text = QString::fromStdString(tag), .color = notes_tags::color(tag), .fillBackground = true});
    if (note.tags.size() > MAX_VISIBLE_TAGS)
      accessories.emplace_back(
          ListAccessory{.text = QStringLiteral("+%1").arg(note.tags.size() - MAX_VISIBLE_TAGS)});
    return accessories;
  }
  bool hasDetailPane() const override { return true; }
  std::optional<ListItemDetail> displayDetail(const ItemType &note) const override {
    return ListItemDetail{.markdown = QString::fromUtf8(Notes::body(note.markdown)),
                          .baseUrl = QUrl::fromLocalFile(QString::fromStdString(note.path))};
  }

  std::unique_ptr<ActionPanelState> emptyActionPanel() override {
    auto panel = std::make_unique<ListActionPanelState>();
    panel->createSection()->addAction(createAction());
    return panel;
  }

  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &note) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto *section = panel->createSection();
    section->addAction(new StaticAction(tr("Open Note"), ImageURL::builtin(BuiltinIcon::Feather),
                                        [id = note.id](ApplicationContext *ctx) {
                                          ctx->navigation->closeWindow();
                                          ctx->notes->openNote(QString::fromStdString(id));
                                        }));
    section->addAction(createAction());
    if (note.readOnly) return panel;
    auto *tags = new EditNoteTagsAction(note, m_tags);
    tags->setShortcut(Keybind::EditAction);
    section->addAction(tags);
    auto *remove =
        new StaticAction(tr("Delete Note"), ImageURL::builtin(BuiltinIcon::Trash),
                         [id = note.id, title = displayTitle(note)](ApplicationContext *ctx) {
                           ctx->navigation->confirmAlert(
                               tr("Delete “%1”?").arg(title), tr("This note will be permanently deleted."),
                               [ctx, id] { ctx->notes->deleteNote(QString::fromStdString(id)); });
                         });
    remove->setStyle(AbstractAction::Style::Danger);
    remove->setShortcut(Keybind::RemoveAction);
    section->addAction(remove);
    return panel;
  }

private:
  static constexpr std::size_t MAX_VISIBLE_TAGS = 2;

  StaticAction *createAction() const {
    auto *action = new StaticAction(tr("Create Note"),
                                    ImageURL::builtin(BuiltinIcon::Feather)
                                        .setBackgroundTint(SemanticColor::Yellow)
                                        .setBadge(BuiltinIcon::Plus),
                                    [](ApplicationContext *ctx) {
                                      ctx->navigation->closeWindow();
                                      ctx->notes->newNote();
                                    });
    action->setShortcut(Keybind::NewAction);
    return action;
  }

  void reload() {
    const auto generation = ++m_generation;
    setLoading(true);
    context()->services->notes()->list().then(
        this, [this, generation](Notes::Result<std::vector<Notes::Note>> result) {
          if (generation != m_generation) return;
          setLoading(false);
          if (!result) {
            context()->services->toastService()->failure(
                tr("Could not load notes. %1").arg(QString::fromStdString(result.error())));
            return;
          }
          m_notes = std::move(*result);
          rebuildTagFilter();
          applyTagFilter();
        });
  }

  void rebuildTagFilter() {
    const auto tags =
        m_notes | std::views::transform(&Notes::Note::tags) | std::views::join | std::ranges::to<std::set>();
    m_tags.assign(tags.begin(), tags.end());
    if (m_tagFilter && !tags.contains(*m_tagFilter)) m_tagFilter.reset();

    QVariantList items;
    items.reserve(static_cast<qsizetype>(m_tags.size()) + 1);
    items.emplace_back(QVariantMap{
        {QStringLiteral("id"), QStringLiteral("0")},
        {QStringLiteral("displayName"), tr("All Notes")},
        {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::Feather))},
    });
    for (const auto &[index, tag] : vicinae::enumerate(m_tags)) {
      items.emplace_back(QVariantMap{
          {QStringLiteral("id"), QString::number(index + 1)},
          {QStringLiteral("displayName"), QString::fromStdString(tag)},
          {QStringLiteral("iconSource"),
           qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::Tag).setFill(notes_tags::color(tag)))},
      });
    }
    m_tagFilterModel.setItems(items);
    emit currentCategoryFilterChanged();
  }

  void applyTagFilter() {
    if (!m_tagFilter) {
      setItems(m_notes);
      return;
    }
    setItems(m_notes | std::views::filter([this](const Notes::Note &note) {
               return std::ranges::contains(note.tags, *m_tagFilter);
             }) |
             std::ranges::to<std::vector>());
  }

  CompletionModel m_tagFilterModel{this};
  std::vector<Notes::Note> m_notes;
  std::vector<std::string> m_tags;
  std::optional<std::string> m_tagFilter;
  std::uint64_t m_generation = 0;
};
