#pragma once

#include <QFileInfo>
#include "fuzzy/fuzzy-searchable.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/notes/notes-store.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/views/mono-list-view-host.hpp"
#include "ui/windows/notes-window-controller.hpp"

template <> struct fuzzy::FuzzySearchable<Notes::Note> {
  static fuzzy::Match score(const Notes::Note &note, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{note.title, 1.0}, {note.markdown, 0.5}}, query);
  }
};

class NotesViewHost : public MonoListViewHost<Notes::Note> {
  Q_DECLARE_TR_FUNCTIONS(NotesViewHost)

public:
  void onMount() override {
    setSearchPlaceholderText(tr("Search notes…"));
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
  QString displaySubtitle(const ItemType &note) const override {
    return QFileInfo(QString::fromStdString(note.path)).absolutePath();
  }
  std::optional<ImageURL> displayIcon(const ItemType &) const override {
    return ImageURL::builtin(BuiltinIcon::BlankDocument);
  }
  AccessoryList displayAccessories(const ItemType &) const override { return {}; }
  bool hasDetailPane() const override { return true; }
  std::optional<ListItemDetail> displayDetail(const ItemType &note) const override {
    return ListItemDetail{{}, QString::fromStdString(note.markdown)};
  }

  std::unique_ptr<ActionPanelState> emptyActionPanel() override {
    auto panel = std::make_unique<ListActionPanelState>();
    panel->createSection()->addAction(createAction());
    return panel;
  }

  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &note) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto *section = panel->createSection();
    section->addAction(new StaticAction(tr("Open Note"), ImageURL::builtin(BuiltinIcon::Pencil),
                                        [id = note.id](ApplicationContext *ctx) {
                                          ctx->navigation->closeWindow();
                                          ctx->notes->openNote(QString::fromStdString(id));
                                        }));
    section->addAction(createAction());
    if (note.readOnly) return panel;
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
  StaticAction *createAction() const {
    auto *action = new StaticAction(tr("Create Note"), ImageURL::builtin(BuiltinIcon::Plus),
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
          if (result)
            setItems(std::move(*result));
          else
            context()->services->toastService()->failure(
                tr("Could not load notes. %1").arg(QString::fromStdString(result.error())));
        });
  }
  std::uint64_t m_generation = 0;
};
