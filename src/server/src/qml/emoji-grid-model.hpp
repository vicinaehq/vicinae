#pragma once
#include "emoji/emoji.hpp"
#include "command-grid-model.hpp"
#include "services/emoji-service/emoji-service.hpp"

class EmojiGridModel : public CommandGridModel {
  Q_OBJECT

public:
  explicit EmojiGridModel(QObject *parent = nullptr);

  void initialize(ApplicationContext *ctx) override;
  void setFilter(const QString &text) override;
  QString searchPlaceholder() const override { return QStringLiteral("Search for emojis..."); }
  QUrl qmlComponentUrl() const override { return QUrl(QStringLiteral("qrc:/Vicinae/EmojiGridView.qml")); }

  Q_INVOKABLE QString emojiIcon(int section, int item) const;
  Q_INVOKABLE QString emojiName(int section, int item) const;
  QString cellTooltip(int section, int item) const override;

protected:
  std::unique_ptr<ActionPanelState> createActionPanel(int section, int item) const override;

private:
  enum class DisplayMode { Root, Search };

  void regenerateMetaSections();
  void rebuildSections();
  const EmojiData *emojiAt(int section, int item) const;

  EmojiService *m_emojiService = nullptr;
  std::optional<emoji::SkinTone> m_skinTone;

  DisplayMode m_displayMode = DisplayMode::Root;
  std::vector<const EmojiData *> m_pinned;
  std::vector<const EmojiData *> m_recent;
  EmojiService::GroupedEmojis m_grouped;
  std::vector<Scored<const EmojiData *>> m_searchResultsStorage;
  std::span<Scored<const EmojiData *>> m_searchResults;
};
