#include "emoji-grid-model.hpp"
#include "builtin_icon.hpp"
#include "clipboard-actions.hpp"
#include "edit-keywords-view-host.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/views/base-view.hpp"
#include "utils.hpp"
#include "view-utils.hpp"
#include <algorithm>
#include <utility>

namespace {

class VisitEmojiActionWrapper : public ProxyAction {
  std::string_view m_emoji;

public:
  VisitEmojiActionWrapper(std::string_view emoji, AbstractAction *action)
      : ProxyAction(action), m_emoji(emoji) {}
  void executeAfter(ApplicationContext *ctx) override {
    ctx->services->glyphService()->registerVisit(m_emoji);
  }
};

class PinEmojiAction : public AbstractAction {
  std::string_view m_emoji;

public:
  PinEmojiAction(std::string_view emoji) : m_emoji(emoji) {}
  QString title() const override { return "Pin emoji"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("pin"); }
  void execute(ApplicationContext *ctx) override { ctx->services->glyphService()->pin(m_emoji); }
};

class UnpinEmojiAction : public AbstractAction {
  std::string_view m_emoji;

public:
  UnpinEmojiAction(std::string_view emoji) : m_emoji(emoji) {}
  QString title() const override { return "Unpin emoji"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("pin-disabled"); }
  void execute(ApplicationContext *ctx) override { ctx->services->glyphService()->unpin(m_emoji); }
};

class ResetEmojiRankingAction : public AbstractAction {
  std::string_view m_emoji;

public:
  ResetEmojiRankingAction(std::string_view emoji) : m_emoji(emoji) {}
  QString title() const override { return "Reset ranking"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("arrow-counter-clockwise"); }
  void execute(ApplicationContext *ctx) override { ctx->services->glyphService()->resetRanking(m_emoji); }
};

class ChangeEmojiSkinToneAction : public AbstractAction {
  std::string_view m_emoji;
  emoji::SkinToneInfo m_toneInfo;
  QString m_emojiIcon;

public:
  ChangeEmojiSkinToneAction(std::string_view emoji, emoji::SkinToneInfo toneInfo, QString icon)
      : m_emoji(emoji), m_toneInfo(toneInfo), m_emojiIcon(std::move(icon)) {}
  QString title() const override {
    return QStringLiteral("%1 skin tone").arg(qStringFromStdView(m_toneInfo.displayName));
  }
  std::optional<ImageURL> icon() const override { return ImageURL::emoji(m_emojiIcon); }
  void execute(ApplicationContext *ctx) override {
    ctx->services->glyphService()->setSkinTone(m_emoji, m_toneInfo.tone);
  }
};

class ResetEmojiSkinToneAction : public AbstractAction {
  std::string_view m_emoji;
  QString m_emojiIcon;

public:
  ResetEmojiSkinToneAction(std::string_view emoji, QString icon)
      : m_emoji(emoji), m_emojiIcon(std::move(icon)) {}
  QString title() const override { return "Reset to preference"; }
  std::optional<ImageURL> icon() const override { return ImageURL::emoji(m_emojiIcon); }
  void execute(ApplicationContext *ctx) override { ctx->services->glyphService()->resetSkinTone(m_emoji); }
};

class EditEmojiKeywordsAction : public AbstractAction {
public:
  void execute(ApplicationContext *ctx) override {
    auto glyphService = ctx->services->glyphService();
    auto view = new EditKeywordsViewHost(
        [glyphService, emoji = m_emoji]() {
          return QString::fromStdString(glyphService->mapMetadata(emoji).keyword);
        },
        [glyphService, emoji = m_emoji](const QString &kw) {
          return glyphService->setKeywords(emoji, kw.toStdString());
        },
        "Additional keywords that will be used to index this emoji");
    ctx->navigation->pushView(view);
    ctx->navigation->setNavigationTitle(title());
  }

  std::optional<ImageURL> icon() const override { return BuiltinIcon::Text; }

  QString title() const override { return "Edit keyword"; }

  EditEmojiKeywordsAction(std::string_view emoji) : m_emoji(emoji) {}

private:
  std::string_view m_emoji;
};

std::unique_ptr<ActionPanelState> buildEmojiActionPanel(const glyph::Item *data,
                                                        std::optional<emoji::SkinTone> skinTone,
                                                        const ViewScope &scope) {
  if (!data) return nullptr;

  auto metadata = scope.services()->glyphService()->mapMetadata(data->character);
  auto const defaultTone = skinTone.value_or(emoji::SkinTone::Default);
  auto const tone = metadata.tone.value_or(defaultTone);

  QString const copiedEmoji = data->skinnable ? emoji::applySkinTone(data->character, tone).c_str()
                                              : QString::fromUtf8(data->character);
  QString const defaultToneEmoji = data->skinnable
                                       ? emoji::applySkinTone(data->character, defaultTone).c_str()
                                       : QString::fromUtf8(data->character);

  auto pasteService = scope.services()->pasteService();
  auto panel = std::make_unique<ListActionPanelState>();
  auto *copyEmoji = new CopyToClipboardAction(Clipboard::Text(copiedEmoji), "Copy emoji");
  auto *copyName = new CopyToClipboardAction(
      Clipboard::Text(QString::fromUtf8(data->name.data(), data->name.size())), "Copy emoji name");
  auto const categoryLabel = glyph::categoryLabel(data->category);
  auto *copyGroup = new CopyToClipboardAction(
      Clipboard::Text(QString::fromUtf8(categoryLabel.data(), categoryLabel.size())), "Copy category");
  auto *resetRanking = new ResetEmojiRankingAction(data->character);
  auto editKeyword = new EditEmojiKeywordsAction(data->character);

  editKeyword->setShortcut(Keybind::EditAction);

  auto *mainSection = panel->createSection();

  QString defaultAction;
  if (auto *state = scope.topState(); state && state->sender) {
    if (auto *cmd = state->sender->command())
      defaultAction = cmd->preferenceValues().value("defaultAction").toString();
  }

  if (pasteService->supportsPaste()) {
    auto *paste = new PasteToFocusedWindowAction(Clipboard::Text(copiedEmoji));
    if (defaultAction == "paste") {
      mainSection->addAction(new VisitEmojiActionWrapper(data->character, paste));
      mainSection->addAction(new VisitEmojiActionWrapper(data->character, copyEmoji));
    } else {
      mainSection->addAction(new VisitEmojiActionWrapper(data->character, copyEmoji));
      mainSection->addAction(new VisitEmojiActionWrapper(data->character, paste));
    }
  } else {
    mainSection->addAction(new VisitEmojiActionWrapper(data->character, copyEmoji));
  }

  mainSection->addAction(copyName);
  mainSection->addAction(copyGroup);
  mainSection->addAction(editKeyword);
  mainSection->addAction(resetRanking);

  if (metadata.pinnedAt) {
    mainSection->addAction(new UnpinEmojiAction(data->character));
  } else {
    mainSection->addAction(new PinEmojiAction(data->character));
  }

  if (data->skinnable) {
    auto *toneSection = panel->createSection("Skin tones");

    if (tone != defaultTone)
      toneSection->addAction(new ResetEmojiSkinToneAction(data->character, defaultToneEmoji));

    for (auto const toneInfo : emoji::skinTones()) {
      if (toneInfo.tone == tone || toneInfo.tone == defaultTone) continue;

      QString tonedEmoji = emoji::applySkinTone(data->character, toneInfo.tone).c_str();
      toneSection->addAction(new ChangeEmojiSkinToneAction(data->character, toneInfo, tonedEmoji));
    }
  }

  return panel;
}

} // namespace

// --- EmojiGridSource ---

void EmojiGridSource::setEmojis(const QString &name, std::span<const glyph::Item *const> emojis) {
  m_name = name;
  m_emojis.assign(emojis.begin(), emojis.end());
  notifyChanged();
}

const glyph::Item *EmojiGridSource::emojiAt(int i) const {
  if (i < 0 || std::cmp_greater_equal(i, m_emojis.size())) return nullptr;
  return m_emojis[i];
}

std::unique_ptr<ActionPanelState> EmojiGridSource::actionPanel(int i) const {
  return buildEmojiActionPanel(emojiAt(i), m_skinTone, scope());
}

// --- SearchEmojiGridSource ---

void SearchEmojiGridSource::setResults(std::span<Scored<const glyph::Item *>> results) {
  m_results = results;
  notifyChanged();
}

const glyph::Item *SearchEmojiGridSource::emojiAt(int i) const {
  if (i < 0 || std::cmp_greater_equal(i, m_results.size())) return nullptr;
  return m_results[i].data;
}

std::unique_ptr<ActionPanelState> SearchEmojiGridSource::actionPanel(int i) const {
  return buildEmojiActionPanel(emojiAt(i), m_skinTone, scope());
}

// --- EmojiGridModel ---

EmojiGridModel::EmojiGridModel(QObject *parent) : SectionGridModel(parent) {}

void EmojiGridModel::initialize() {
  m_glyphService = scope().services()->glyphService();
  m_sections = glyph::sections();

  if (auto *state = scope().topState(); state && state->sender) {
    if (auto *cmd = state->sender->command()) {
      auto skinToneId = cmd->preferenceValues().value("skinTone").toString().toStdString();
      if (auto it = std::ranges::find_if(
              emoji::skinTones(), [&](const emoji::SkinToneInfo &info) { return info.id == skinToneId; });
          it != emoji::skinTones().end()) {
        m_skinTone = it->tone;
      }
    }
  }

  m_pinnedSource.setSkinTone(m_skinTone);
  m_recentSource.setSkinTone(m_skinTone);
  m_searchSource.setSkinTone(m_skinTone);

  connect(this, &SectionGridModel::selectionChanged, this, &EmojiGridModel::updateNavigationTitle);

  refreshMetadataCache();

  regenerateMetaSections();
  rebuildSections();

  connect(m_glyphService, &GlyphService::pinned, this, [this](auto) {
    regenerateMetaSections();
    setSelectFirstOnReset(true);
    rebuildSections();
    setSelectFirstOnReset(false);
    selectFirst();
  });
  connect(m_glyphService, &GlyphService::unpinned, this, [this](auto) {
    regenerateMetaSections();
    setSelectFirstOnReset(true);
    rebuildSections();
    setSelectFirstOnReset(false);
    selectFirst();
  });
  connect(m_glyphService, &GlyphService::visited, this, [this](auto) {
    regenerateMetaSections();
    rebuildSections();
  });
  connect(m_glyphService, &GlyphService::skintoneChanged, this, [this](auto) {
    int const section = selectedSection();
    int const item = selectedItem();

    refreshMetadataCache();

    setSelectFirstOnReset(true);
    rebuildSections();
    setSelectFirstOnReset(false);

    if (section >= 0 && item >= 0) {
      select(section, item);
    } else {
      selectFirst();
    }
  });
}

void EmojiGridModel::refreshMetadataCache() {
  m_metadataCache.clear();

  auto rows = m_glyphService->getVisited();
  m_metadataCache.reserve(rows.size());

  for (auto &row : rows) {
    m_metadataCache.emplace(row.data, std::move(row));
  }
}

void EmojiGridModel::regenerateMetaSections() {
  m_pinned.clear();
  m_recent.clear();

  for (const auto &visited : m_glyphService->getVisited()) {
    if (visited.pinnedAt) {
      m_pinned.emplace_back(visited.data);
    } else {
      m_recent.emplace_back(visited.data);
    }
  }
}

void EmojiGridModel::rebuildSections() {
  clearSources();
  m_groupSources.clear();

  if (m_displayMode == DisplayMode::Root) {
    if (!m_categoryFilter) {
      m_pinnedSource.setEmojis(QStringLiteral("Pinned"), m_pinned);
      addSource(&m_pinnedSource);

      m_recentSource.setEmojis(QStringLiteral("Recently used"), m_recent);
      addSource(&m_recentSource);
    }

    m_groupSources.reserve(m_sections.size());
    for (const auto &section : m_sections) {
      if (m_categoryFilter && section.category != *m_categoryFilter) continue;
      auto &src = m_groupSources.emplace_back();
      src.setSkinTone(m_skinTone);
      std::vector<const glyph::Item *> items;
      items.reserve(section.members.size());
      for (const auto &item : section.members)
        items.push_back(&item);
      src.setEmojis(QString::fromUtf8(section.label.data(), section.label.size()), items);
      addSource(&src);
    }
  } else {
    m_searchSource.setResults(m_searchResults);
    addSource(&m_searchSource);
  }

  rebuild();
}

void EmojiGridModel::setFilter(const QString &text) {
  m_displayMode = text.isEmpty() ? DisplayMode::Root : DisplayMode::Search;

  if (!text.isEmpty()) {
    auto results = m_glyphService->search(text.toStdString());
    if (m_categoryFilter) {
      m_searchResultsStorage.clear();
      for (const auto &scored : results)
        if (scored.data->category == *m_categoryFilter) m_searchResultsStorage.push_back(scored);
      m_searchResults = m_searchResultsStorage;
    } else {
      m_searchResults = results;
    }
  }

  setSelectFirstOnReset(true);
  rebuildSections();
  setSelectFirstOnReset(false);
  selectFirst();
}

void EmojiGridModel::setCategoryFilter(std::optional<glyph::Category> category) {
  m_categoryFilter = category;

  setSelectFirstOnReset(true);
  rebuildSections();
  setSelectFirstOnReset(false);
  selectFirst();
}

const glyph::Item *EmojiGridModel::emojiAt(int section, int item) const {
  int sourceIdx, itemIdx;
  if (!resolveSelection(section, item, sourceIdx, itemIdx)) return nullptr;

  auto *source = sources()[sourceIdx];
  if (auto *emoji = dynamic_cast<EmojiGridSource *>(source)) return emoji->emojiAt(itemIdx);
  if (auto *search = dynamic_cast<SearchEmojiGridSource *>(source)) return search->emojiAt(itemIdx);
  return nullptr;
}

QString EmojiGridModel::emojiIcon(int section, int item) const {
  const auto *data = emojiAt(section, item);
  if (!data) return {};

  if (data->kind == glyph::Kind::Symbol)
    return qml::imageSourceFor(ImageURL::symbol(qStringFromStdView(data->character)));

  if (data->skinnable) {
    auto tone = m_skinTone;
    if (auto it = m_metadataCache.find(data); it != m_metadataCache.end() && it->second.tone) {
      tone = it->second.tone.value();
    }

    auto toned = emoji::applySkinTone(data->character, tone);
    return qml::imageSourceFor(ImageURL::emoji(toned.c_str()));
  }
  return qml::imageSourceFor(ImageURL::emoji(qStringFromStdView(data->character)));
}

QString EmojiGridModel::emojiName(int section, int item) const {
  const auto *data = emojiAt(section, item);
  return data ? qStringFromStdView(data->name) : QString{};
}

QString EmojiGridModel::cellTooltip(int section, int item) const { return emojiName(section, item); }

void EmojiGridModel::updateNavigationTitle() {
  auto name = emojiName(selectedSection(), selectedItem());
  scope().setNavigationTitle(name.isEmpty() ? QStringLiteral("Search Emojis")
                                            : QStringLiteral("Search Emojis - %1").arg(name));
}
