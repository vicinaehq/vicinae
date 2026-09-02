#pragma once
#include "builtin_icon.hpp"
#include "common/context.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "fuzzy-section.hpp"
#include "list-view-host.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-runtime/app-runtime.hpp"
#include "services/app-service/app-service.hpp"
#include "services/menu-bar/macos-menu-bar.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include "ui/action-pannel/action.hpp"
#include <QCoreApplication>
#include <algorithm>

template <> struct fuzzy::FuzzySearchable<MenuBar::Entry> {
  static fuzzy::Match score(const MenuBar::Entry &entry, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{entry.title.toStdString(), 1.0}, {entry.path.join(' ').toStdString(), 0.5}},
                                query);
  }
};

class MenuBarMenuSection : public FuzzySection<MenuBar::Entry> {
  Q_DECLARE_TR_FUNCTIONS(MenuBarMenuSection)

public:
  using ItemType = MenuBar::Entry;

  // Section mode lists one top-level menu under its header while the query is empty;
  // Flat mode holds every entry and takes over, headerless and globally ranked, while searching
  enum class Mode { Section, Flat };

  MenuBarMenuSection(Mode mode, QString title, std::optional<ImageURL> icon)
      : m_mode(mode), m_title(std::move(title)), m_icon(std::move(icon)) {}

  QString sectionName() const override {
    if (m_mode == Mode::Flat) return tr("Results ({count} items)");
    return m_title;
  }

  int count() const override {
    const bool searching = !m_query.empty();
    if ((m_mode == Mode::Flat) != searching) return 0;
    return FuzzySection::count();
  }

  QString displayTitle(const ItemType &entry) const override { return entry.title; }
  QString displaySubtitle(const ItemType &entry) const override {
    if (m_mode == Mode::Flat) return entry.path.join(" › ");
    return entry.path.mid(1).join(" › ");
  }
  QString displayId(const ItemType &entry) const override { return entry.path.join('/') + '/' + entry.title; }

  std::optional<ImageURL> displayIcon(const ItemType &) const override {
    if (m_icon) return m_icon;
    return ImageURL::builtin(BuiltinIcon::AppWindowList);
  }

  QVariantList displayShortcut(const ItemType &entry) const override {
    return entry.shortcut.toDisplayTokens();
  }

  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &entry) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto section = panel->createSection();
    section->addAction(new StaticAction(
        tr("Run Menu Item"), ImageURL::builtin(BuiltinIcon::Bolt), [entry](ApplicationContext *ctx) {
          ctx->navigation->closeWindow({.popToRootType = PopToRootType::Immediate});
          MenuBar::pressItem(entry);
        }));
    section->addAction(new StaticAction(tr("Open in Menu Bar"), ImageURL::builtin(BuiltinIcon::AppWindowList),
                                        [entry](ApplicationContext *ctx) {
                                          ctx->navigation->closeWindow(
                                              {.popToRootType = PopToRootType::Immediate});
                                          MenuBar::revealInMenuBar(entry);
                                        }));
    return panel;
  }

private:
  Mode m_mode;
  QString m_title;
  std::optional<ImageURL> m_icon;
};

class MenuBarSearchViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override {
    BaseView::initialize();
    initModel();
    setSearchPlaceholderText(tr("Filter by menu item title..."));
    connect(context()->services->appRuntime(), &AppRuntime::frontmostAppChanged, this, [this] { rescan(); });
  }

  void loadInitialData() override { rescan(); }

private:
  void rescan() {
    auto target = MenuBar::frontmostTarget();
    if (target.pid == m_pid) return;
    m_pid = target.pid;

    setLoading(true);
    MenuBar::scan(std::move(target)).then(this, [this](MenuBar::Snapshot snapshot) {
      if (snapshot.pid != m_pid) return;
      applySnapshot(std::move(snapshot));
    });
  }

  void applySnapshot(MenuBar::Snapshot snapshot) {
    std::optional<ImageURL> icon;
    if (auto app = context()->services->appDb()->findById(snapshot.bundleId)) icon = app->iconUrl();

    model()->clearSources();
    m_sections.clear();

    std::vector<std::pair<QString, std::vector<MenuBar::Entry>>> groups;
    for (const auto &entry : snapshot.entries) {
      QString key = entry.path.isEmpty() ? snapshot.appName : entry.path.front();
      auto it = std::ranges::find_if(groups, [&](const auto &g) { return g.first == key; });
      if (it == groups.end()) it = groups.insert(groups.end(), {key, {}});
      it->second.push_back(entry);
    }

    for (auto &[title, entries] : groups) {
      auto section = std::make_unique<MenuBarMenuSection>(MenuBarMenuSection::Mode::Section, title, icon);
      model()->addSource(section.get());
      section->setItems(std::move(entries));
      m_sections.push_back(std::move(section));
    }

    auto flat = std::make_unique<MenuBarMenuSection>(MenuBarMenuSection::Mode::Flat, QString(), icon);
    model()->addSource(flat.get());
    flat->setItems(std::move(snapshot.entries));
    m_sections.push_back(std::move(flat));

    model()->setFilter(searchText());
    setLoading(false);
  }

  int m_pid = -1;
  std::vector<std::unique_ptr<MenuBarMenuSection>> m_sections;
};
