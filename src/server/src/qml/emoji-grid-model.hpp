#pragma once
#include "emoji/emoji.hpp"
#include "grid-source.hpp"
#include "section-grid-model.hpp"
#include "services/emoji-service/emoji-service.hpp"
#include <optional>
#include <span>

class EmojiGridSource : public GridSource {
public:
  void setEmojis(const QString &name, std::span<const EmojiData *const> emojis);

  void setSkinTone(std::optional<emoji::SkinTone> tone) { m_skinTone = tone; }

  QString sectionName() const override { return m_name; }
  int count() const override { return static_cast<int>(m_emojis.size()); }

  const EmojiData *emojiAt(int i) const;
  QString emojiIcon(int i) const;
  QString emojiName(int i) const;

  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  QString m_name;
  std::vector<const EmojiData *> m_emojis;
  std::optional<emoji::SkinTone> m_skinTone;
};

class SearchEmojiGridSource : public GridSource {
public:
  void setResults(std::span<Scored<const EmojiData *>> results);

  void setSkinTone(std::optional<emoji::SkinTone> tone) { m_skinTone = tone; }

  QString sectionName() const override { return QStringLiteral("Results"); }
  int count() const override { return static_cast<int>(m_results.size()); }

  const EmojiData *emojiAt(int i) const;
  QString emojiIcon(int i) const;
  QString emojiName(int i) const;

  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  std::span<Scored<const EmojiData *>> m_results;
  std::optional<emoji::SkinTone> m_skinTone;
};

class EmojiGridModel : public SectionGridModel {
  Q_OBJECT

public:
  explicit EmojiGridModel(QObject *parent = nullptr);

  void initialize();
  void setFilter(const QString &text);
  QString searchPlaceholder() const { return QStringLiteral("Search for emojis..."); }
  QUrl qmlComponentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/EmojiGridView.qml")); }

  Q_INVOKABLE QString emojiIcon(int section, int item) const;
  Q_INVOKABLE QString emojiName(int section, int item) const;
  Q_INVOKABLE QString cellTooltip(int section, int item) const;

private:
  enum class DisplayMode { Root, Search };

  void regenerateMetaSections();
  void rebuildSections();
  void updateNavigationTitle();

  EmojiService *m_emojiService = nullptr;
  std::optional<emoji::SkinTone> m_skinTone;

  DisplayMode m_displayMode = DisplayMode::Root;

  EmojiGridSource m_pinnedSource;
  EmojiGridSource m_recentSource;
  std::vector<EmojiGridSource> m_groupSources;
  SearchEmojiGridSource m_searchSource;

  std::vector<const EmojiData *> m_pinned;
  std::vector<const EmojiData *> m_recent;
  EmojiService::GroupedEmojis m_grouped;
  std::vector<Scored<const EmojiData *>> m_searchResultsStorage;
  std::span<Scored<const EmojiData *>> m_searchResults;
};
