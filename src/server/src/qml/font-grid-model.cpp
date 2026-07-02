#include "font-grid-model.hpp"
#include "clipboard-actions.hpp"
#include "common/context.hpp"
#include "config/config.hpp"
#include "font-demo-view-host.hpp"
#include "fuzzy/fzf.hpp"
#include "keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"
#include "view-utils.hpp"

namespace {

class SetAppFont : public AbstractAction {
  QString m_family;

  void execute(ApplicationContext *ctx) override {
    ctx->services->config()->mergeWithUser(
        {.font = config::Partial<config::FontConfig>{.normal = {.family = m_family.toStdString()}}});
  }

public:
  SetAppFont(const QString &family)
      : AbstractAction("Set as vicinae font", ImageURL::builtin("text")), m_family(family) {}
};

class PreviewFontAction : public AbstractAction {
  QString m_family;
  FontCategory m_category;

  void execute(ApplicationContext *ctx) override {
    ctx->navigation->pushView(new FontDemoViewHost(m_family, m_category));
  }

public:
  PreviewFontAction(const QString &family, FontCategory category)
      : AbstractAction("Preview font", ImageURL::builtin("eye")), m_family(family), m_category(category) {}
};

std::unique_ptr<ActionPanelState> buildFontActionPanel(const FontFamily *family) {
  if (!family) return nullptr;

  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();
  auto copyFamily = new CopyToClipboardAction(Clipboard::Text(family->name), "Copy font family");
  auto preview = new PreviewFontAction(family->family, family->primary);

  copyFamily->setShortcut(Keybind::CopyAction);
  preview->setPrimary(true);
  section->addAction(preview);
  section->addAction(copyFamily);
  section->addAction(new SetAppFont(family->family));

  return panel;
}

} // namespace

void FontGridSource::setBucket(QString name, std::vector<const FontFamily *> families) {
  m_name = std::move(name);
  m_families = std::move(families);
  m_search = false;
}

void FontGridSource::setResults(QString name, std::span<Scored<const FontFamily *>> results) {
  m_name = std::move(name);
  m_results = results;
  m_search = true;
}

std::unique_ptr<ActionPanelState> FontGridSource::actionPanel(int i) const {
  return buildFontActionPanel(familyAt(i));
}

FontGridModel::FontGridModel(QObject *parent) : SectionGridModel(parent) {}

void FontGridModel::initialize() {
  m_fontService = ServiceRegistry::instance()->fontService();
  connect(this, &SectionGridModel::selectionChanged, this, &FontGridModel::updateNavigationTitle);
  setColumns(6);
  setAspectRatio(1.0);
  buildFilterOptions();
  rebuildRoot();
  setFilter(QString());
}

void FontGridModel::updateNavigationTitle() {
  const FontFamily *family = familyAt(selectedSection(), selectedItem());
  const QString name = family ? family->name : QString{};
  const QString command = scope().appContext()->navigation->activeCommand()->name();
  scope().setNavigationTitle(name.isEmpty() ? name : QStringLiteral("%1 - %2").arg(command).arg(name));
}

void FontGridModel::buildFilterOptions() {
  FontCategoryMask present = 0;
  for (const auto &family : m_fontService->fontFamilies()) {
    present |= family.categories;
  }

  m_filterCategories.clear();
  m_categoryNames.clear();
  for (const FontCategory category : FontService::orderedCategories()) {
    if (present & categoryBit(category)) {
      m_filterCategories.push_back(category);
      m_categoryNames.push_back(FontService::categoryName(category));
    }
  }
}

void FontGridModel::rebuildRoot() {
  std::vector<const FontFamily *> members;
  for (const auto &family : m_fontService->fontFamilies()) {
    if (!m_categoryFilter || family.has(*m_categoryFilter)) members.push_back(&family);
  }

  const int n = static_cast<int>(members.size());
  const QString title =
      m_categoryFilter ? QStringLiteral("%1 (%2)").arg(FontService::categoryName(*m_categoryFilter)).arg(n)
                       : QStringLiteral("All Fonts (%1)").arg(n);
  m_rootSource.setBucket(title, std::move(members));
}

void FontGridModel::setFilter(const QString &text) {
  if (text.isEmpty()) {
    m_mode = Mode::Root;
  } else {
    m_mode = Mode::Search;
    const auto &all = m_fontService->fontFamilies();
    auto results = m_scorer.score(std::span<const FontFamily>(all), text.toStdString(),
                                  [this](const FontFamily &f, std::string_view query) {
                                    if (m_categoryFilter && !f.has(*m_categoryFilter)) return 0;
                                    return fzf::threadLocalMatcher().fuzzy_match_v2_score_query(
                                        f.name.toStdString(), query);
                                  });
    m_searchSource.setResults(QStringLiteral("Results (%1)").arg(results.size()), results);
  }
  applyReset();
}

void FontGridModel::setCategoryFilter(std::optional<int> index) {
  if (index && *index >= 0 && *index < static_cast<int>(m_filterCategories.size()))
    m_categoryFilter = m_filterCategories[*index];
  else
    m_categoryFilter = std::nullopt;

  rebuildRoot();
  if (m_mode == Mode::Root) applyReset();
}

void FontGridModel::applyReset() {
  setSelectFirstOnReset(true);
  rebuildSections();
  setSelectFirstOnReset(false);
  selectFirst();
}

void FontGridModel::rebuildSections() {
  clearSources();
  addSource(m_mode == Mode::Search ? &m_searchSource : &m_rootSource);
  rebuild();
}

const FontFamily *FontGridModel::familyAt(int section, int item) const {
  int sourceIdx = 0, itemIdx = 0;
  if (!resolveSelection(section, item, sourceIdx, itemIdx)) return nullptr;
  auto *src = dynamic_cast<FontGridSource *>(sources()[sourceIdx]);
  return src ? src->familyAt(itemIdx) : nullptr;
}

QString FontGridModel::cellTitle(int section, int item) const {
  const FontFamily *family = familyAt(section, item);
  return family ? family->name : QString{};
}

QString FontGridModel::cellTooltip(int section, int item) const {
  const FontFamily *family = familyAt(section, item);
  return family ? family->name : QString{};
}

QString FontGridModel::fontIcon(int section, int item) const {
  const FontFamily *family = familyAt(section, item);
  if (!family) return {};

  if (family->glyph.isEmpty())
    return qml::imageSourceFor(
        ImageURL::fontPreview(QString(), QStringLiteral("?")).setFill(SemanticColor::Foreground));

  auto url = ImageURL::fontPreview(family->family, family->glyph);
  if (!family->color) url.setFill(SemanticColor::Foreground);
  return qml::imageSourceFor(url);
}
