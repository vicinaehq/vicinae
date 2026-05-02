#include "emoji-grid-model.hpp"
#include "clipboard-actions.hpp"
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
    ctx->services->emojiService()->registerVisit(m_emoji);
  }
};

class PinEmojiAction : public AbstractAction {
  std::string_view m_emoji;

public:
  PinEmojiAction(std::string_view emoji) : m_emoji(emoji) {}
  QString title() const override { return "Pin emoji"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("pin"); }
  void execute(ApplicationContext *ctx) override { ctx->services->emojiService()->pin(m_emoji); }
};

class UnpinEmojiAction : public AbstractAction {
  std::string_view m_emoji;

public:
  UnpinEmojiAction(std::string_view emoji) : m_emoji(emoji) {}
  QString title() const override { return "Unpin emoji"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("pin-disabled"); }
  void execute(ApplicationContext *ctx) override { ctx->services->emojiService()->unpin(m_emoji); }
};

class ResetEmojiRankingAction : public AbstractAction {
  std::string_view m_emoji;

public:
  ResetEmojiRankingAction(std::string_view emoji) : m_emoji(emoji) {}
  QString title() const override { return "Reset ranking"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("arrow-counter-clockwise"); }
  void execute(ApplicationContext *ctx) override { ctx->services->emojiService()->resetRanking(m_emoji); }
};

std::unique_ptr<ActionPanelState> buildEmojiActionPanel(const EmojiData *data,
                                                        std::optional<emoji::SkinTone> skinTone,
                                                        const ViewScope &scope) {
  if (!data) return nullptr;

  auto metadata = scope.services()->emojiService()->mapMetadata(data->emoji);
  QString const copiedEmoji = data->skinToneSupport && skinTone
                                  ? emoji::applySkinTone(data->emoji, *skinTone).c_str()
                                  : QString::fromUtf8(data->emoji);

  auto pasteService = scope.services()->pasteService();
  auto panel = std::make_unique<ListActionPanelState>();
  auto *copyEmoji = new CopyToClipboardAction(Clipboard::Text(copiedEmoji), "Copy emoji");
  auto *copyName = new CopyToClipboardAction(
      Clipboard::Text(QString::fromUtf8(data->name.data(), data->name.size())), "Copy emoji name");
  auto *copyGroup = new CopyToClipboardAction(
      Clipboard::Text(QString::fromUtf8(data->group.data(), data->group.size())), "Copy emoji group");
  auto *resetRanking = new ResetEmojiRankingAction(data->emoji);

  auto *mainSection = panel->createSection();

  QString defaultAction;
  if (auto *state = scope.topState(); state && state->sender) {
    if (auto *cmd = state->sender->command())
      defaultAction = cmd->preferenceValues().value("defaultAction").toString();
  }

  if (pasteService->supportsPaste()) {
    auto *paste = new PasteToFocusedWindowAction(Clipboard::Text(copiedEmoji));
    if (defaultAction == "paste") {
      mainSection->addAction(new VisitEmojiActionWrapper(data->emoji, paste));
      mainSection->addAction(new VisitEmojiActionWrapper(data->emoji, copyEmoji));
    } else {
      mainSection->addAction(new VisitEmojiActionWrapper(data->emoji, copyEmoji));
      mainSection->addAction(new VisitEmojiActionWrapper(data->emoji, paste));
    }
  } else {
    mainSection->addAction(new VisitEmojiActionWrapper(data->emoji, copyEmoji));
  }

  mainSection->addAction(copyName);
  mainSection->addAction(copyGroup);
  mainSection->addAction(resetRanking);

  if (metadata.pinnedAt) {
    mainSection->addAction(new UnpinEmojiAction(data->emoji));
  } else {
    mainSection->addAction(new PinEmojiAction(data->emoji));
  }

  return panel;
}

QString emojiIconStr(const EmojiData *data, std::optional<emoji::SkinTone> skinTone) {
  if (!data) return {};
  if (data->skinToneSupport && skinTone) {
    auto toned = emoji::applySkinTone(data->emoji, *skinTone);
    return qml::imageSourceFor(ImageURL::emoji(toned.c_str()));
  }
  return qml::imageSourceFor(ImageURL::emoji(qStringFromStdView(data->emoji)));
}

QString emojiNameStr(const EmojiData *data) {
  if (!data) return {};
  return qStringFromStdView(data->name);
}

} // namespace

// --- EmojiGridSource ---

void EmojiGridSource::setEmojis(const QString &name, std::span<const EmojiData *const> emojis) {
  m_name = name;
  m_emojis.assign(emojis.begin(), emojis.end());
  notifyChanged();
}

const EmojiData *EmojiGridSource::emojiAt(int i) const {
  if (i < 0 || std::cmp_greater_equal(i, m_emojis.size())) return nullptr;
  return m_emojis[i];
}

QString EmojiGridSource::emojiIcon(int i) const { return emojiIconStr(emojiAt(i), m_skinTone); }
QString EmojiGridSource::emojiName(int i) const { return emojiNameStr(emojiAt(i)); }

std::unique_ptr<ActionPanelState> EmojiGridSource::actionPanel(int i) const {
  return buildEmojiActionPanel(emojiAt(i), m_skinTone, scope());
}

// --- SearchEmojiGridSource ---

void SearchEmojiGridSource::setResults(std::span<Scored<const EmojiData *>> results) {
  m_results = results;
  notifyChanged();
}

const EmojiData *SearchEmojiGridSource::emojiAt(int i) const {
  if (i < 0 || std::cmp_greater_equal(i, m_results.size())) return nullptr;
  return m_results[i].data;
}

QString SearchEmojiGridSource::emojiIcon(int i) const { return emojiIconStr(emojiAt(i), m_skinTone); }
QString SearchEmojiGridSource::emojiName(int i) const { return emojiNameStr(emojiAt(i)); }

std::unique_ptr<ActionPanelState> SearchEmojiGridSource::actionPanel(int i) const {
  return buildEmojiActionPanel(emojiAt(i), m_skinTone, scope());
}

// --- EmojiGridModel ---

EmojiGridModel::EmojiGridModel(QObject *parent) : SectionGridModel(parent) {}

void EmojiGridModel::initialize() {
  m_emojiService = scope().services()->emojiService();
  m_grouped = m_emojiService->grouped();

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

  regenerateMetaSections();
  rebuildSections();

  connect(m_emojiService, &EmojiService::pinned, this, [this](auto) {
    regenerateMetaSections();
    setSelectFirstOnReset(true);
    rebuildSections();
    setSelectFirstOnReset(false);
    selectFirst();
  });
  connect(m_emojiService, &EmojiService::unpinned, this, [this](auto) {
    regenerateMetaSections();
    setSelectFirstOnReset(true);
    rebuildSections();
    setSelectFirstOnReset(false);
    selectFirst();
  });
  connect(m_emojiService, &EmojiService::visited, this, [this](auto) {
    regenerateMetaSections();
    rebuildSections();
  });
}

void EmojiGridModel::regenerateMetaSections() {
  m_pinned.clear();
  m_recent.clear();

  for (const auto &visited : m_emojiService->getVisited()) {
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
    m_pinnedSource.setEmojis(QStringLiteral("Pinned"), m_pinned);
    addSource(&m_pinnedSource);

    m_recentSource.setEmojis(QStringLiteral("Recently used"), m_recent);
    addSource(&m_recentSource);

    m_groupSources.reserve(m_grouped.size());
    for (const auto &[groupName, emojis] : m_grouped) {
      auto &src = m_groupSources.emplace_back();
      src.setSkinTone(m_skinTone);
      src.setEmojis(QString::fromUtf8(groupName.data(), groupName.size()), emojis);
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

  if (!text.isEmpty()) { m_searchResults = m_emojiService->search(text.toStdString()); }

  setSelectFirstOnReset(true);
  rebuildSections();
  setSelectFirstOnReset(false);
  selectFirst();
}

QString EmojiGridModel::emojiIcon(int section, int item) const {
  int sourceIdx, itemIdx;
  if (!resolveSelection(section, item, sourceIdx, itemIdx)) return {};

  auto *source = sources()[sourceIdx];
  if (auto *emoji = dynamic_cast<EmojiGridSource *>(source)) return emoji->emojiIcon(itemIdx);
  if (auto *search = dynamic_cast<SearchEmojiGridSource *>(source)) return search->emojiIcon(itemIdx);
  return {};
}

QString EmojiGridModel::emojiName(int section, int item) const {
  int sourceIdx, itemIdx;
  if (!resolveSelection(section, item, sourceIdx, itemIdx)) return {};

  auto *source = sources()[sourceIdx];
  if (auto *emoji = dynamic_cast<EmojiGridSource *>(source)) return emoji->emojiName(itemIdx);
  if (auto *search = dynamic_cast<SearchEmojiGridSource *>(source)) return search->emojiName(itemIdx);
  return {};
}

QString EmojiGridModel::cellTooltip(int section, int item) const { return emojiName(section, item); }

void EmojiGridModel::updateNavigationTitle() {
  auto name = emojiName(selectedSection(), selectedItem());
  scope().setNavigationTitle(name.isEmpty() ? QStringLiteral("Search Emojis")
                                            : QStringLiteral("Search Emojis - %1").arg(name));
}
