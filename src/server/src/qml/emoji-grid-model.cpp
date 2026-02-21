#include "emoji-grid-model.hpp"
#include "clipboard-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/views/base-view.hpp"
#include "utils.hpp"
#include "view-utils.hpp"
#include <algorithm>

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

} // namespace

EmojiGridModel::EmojiGridModel(QObject *parent) : CommandGridModel(parent) {}

void EmojiGridModel::initialize(ApplicationContext *ctx) {
  CommandGridModel::initialize(ctx);
  m_emojiService = ctx->services->emojiService();
  m_grouped = m_emojiService->grouped();

  // Read skin tone preference from the command
  auto *nav = ctx->navigation.get();
  if (auto *state = nav->topState(); state && state->sender) {
    if (auto *cmd = state->sender->command()) {
      auto skinToneId = cmd->preferenceValues().value("skinTone").toString().toStdString();
      if (auto it = std::ranges::find_if(
              emoji::skinTones(), [&](const emoji::SkinToneInfo &info) { return info.id == skinToneId; });
          it != emoji::skinTones().end()) {
        m_skinTone = it->tone;
      }
    }
  }

  regenerateMetaSections();

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
  std::vector<SectionInfo> secs;

  if (m_displayMode == DisplayMode::Root) {
    secs.push_back({QStringLiteral("Pinned"), static_cast<int>(m_pinned.size())});
    secs.push_back({QStringLiteral("Recently used"), static_cast<int>(m_recent.size())});
    for (const auto &[groupName, emojis] : m_grouped) {
      secs.push_back(
          {QString::fromUtf8(groupName.data(), groupName.size()), static_cast<int>(emojis.size())});
    }
  } else {
    secs.push_back({QStringLiteral("Results"), static_cast<int>(m_searchResults.size())});
  }

  setSections(secs);
}

void EmojiGridModel::setFilter(const QString &text) {
  m_displayMode = text.isEmpty() ? DisplayMode::Root : DisplayMode::Search;

  if (!text.isEmpty()) { m_searchResults = m_emojiService->search(text.toStdString()); }

  setSelectFirstOnReset(true);
  rebuildSections();
  setSelectFirstOnReset(false);
  selectFirst();
}

const EmojiData *EmojiGridModel::emojiAt(int section, int item) const {
  if (section < 0 || section >= static_cast<int>(sections().size())) return nullptr;
  if (item < 0 || item >= sections()[section].count) return nullptr;

  if (m_displayMode == DisplayMode::Root) {
    if (section == 0) return m_pinned[item];
    if (section == 1) return m_recent[item];
    return m_grouped[section - 2].second[item];
  }

  return m_searchResults[item].data;
}

QString EmojiGridModel::emojiIcon(int section, int item) const {
  auto *data = emojiAt(section, item);
  if (!data) return {};

  if (data->skinToneSupport && m_skinTone) {
    auto toned = emoji::applySkinTone(data->emoji, m_skinTone.value());
    return qml::imageSourceFor(ImageURL::emoji(toned.c_str()));
  }
  return qml::imageSourceFor(ImageURL::emoji(qStringFromStdView(data->emoji)));
}

QString EmojiGridModel::emojiName(int section, int item) const {
  auto *data = emojiAt(section, item);
  if (!data) return {};
  return qStringFromStdView(data->name);
}

QString EmojiGridModel::cellTooltip(int section, int item) const { return emojiName(section, item); }

void EmojiGridModel::onItemSelected(int section, int item) { updateNavigationTitle(); }

void EmojiGridModel::onSelectionCleared() {
  CommandGridModel::onSelectionCleared();
  updateNavigationTitle();
}

void EmojiGridModel::updateNavigationTitle() {
  auto name = emojiName(selectedSection(), selectedItem());
  ctx()->navigation->setNavigationTitle(name.isEmpty() ? QStringLiteral("Search Emojis")
                                                       : QStringLiteral("Search Emojis - %1").arg(name));
}

std::unique_ptr<ActionPanelState> EmojiGridModel::createActionPanel(int section, int item) const {
  auto *data = emojiAt(section, item);
  if (!data) return nullptr;

  auto metadata = m_emojiService->mapMetadata(data->emoji);
  QString copiedEmoji = data->skinToneSupport && m_skinTone
                            ? emoji::applySkinTone(data->emoji, m_skinTone.value()).c_str()
                            : QString::fromUtf8(data->emoji);

  auto wm = ctx()->services->windowManager();
  auto panel = std::make_unique<ListActionPanelState>();
  auto *copyEmoji = new CopyToClipboardAction(Clipboard::Text(copiedEmoji), "Copy emoji");
  auto *copyName = new CopyToClipboardAction(
      Clipboard::Text(QString::fromUtf8(data->name.data(), data->name.size())), "Copy emoji name");
  auto *copyGroup = new CopyToClipboardAction(
      Clipboard::Text(QString::fromUtf8(data->group.data(), data->group.size())), "Copy emoji group");
  auto *resetRanking = new ResetEmojiRankingAction(data->emoji);

  auto *mainSection = panel->createSection();

  QString defaultAction;
  if (auto *state = ctx()->navigation->topState(); state && state->sender) {
    if (auto *cmd = state->sender->command())
      defaultAction = cmd->preferenceValues().value("defaultAction").toString();
  }

  if (wm->canPaste()) {
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
