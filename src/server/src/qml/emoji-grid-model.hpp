#pragma once
#include "glyph/emoji.hpp"
#include "grid-source.hpp"
#include "section-grid-model.hpp"
#include "services/glyph-service/glyph-service.hpp"
#include <optional>
#include <span>

class EmojiGridSource : public GridSource {
public:
  void setEmojis(const QString &name, std::span<const glyph::Item *const> emojis);

  void setSkinTone(std::optional<emoji::SkinTone> tone) { m_skinTone = tone; }

  QString sectionName() const override { return m_name; }
  int count() const override { return static_cast<int>(m_emojis.size()); }

  const glyph::Item *emojiAt(int i) const;

  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  QString m_name;
  std::vector<const glyph::Item *> m_emojis;
  std::optional<emoji::SkinTone> m_skinTone;
};

class SearchEmojiGridSource : public GridSource {
public:
  void setResults(std::span<Scored<const glyph::Item *>> results);

  void setSkinTone(std::optional<emoji::SkinTone> tone) { m_skinTone = tone; }

  QString sectionName() const override { return QStringLiteral("Results"); }
  int count() const override { return static_cast<int>(m_results.size()); }

  const glyph::Item *emojiAt(int i) const;

  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  std::span<Scored<const glyph::Item *>> m_results;
  std::optional<emoji::SkinTone> m_skinTone;
};

class EmojiGridModel : public SectionGridModel {
  Q_OBJECT

public:
  explicit EmojiGridModel(QObject *parent = nullptr);

  void initialize();
  void setFilter(const QString &text);
  void setCategoryFilter(std::optional<glyph::Category> category);
  QString searchPlaceholder() const { return QStringLiteral("Search for emojis and symbols..."); }
  QUrl qmlComponentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/EmojiGridView.qml")); }

  Q_INVOKABLE QString emojiIcon(int section, int item) const;
  Q_INVOKABLE QString emojiName(int section, int item) const;
  Q_INVOKABLE QString cellTooltip(int section, int item) const;

private:
  enum class DisplayMode { Root, Search };

  const glyph::Item *emojiAt(int section, int item) const;
  void refreshMetadataCache();
  void regenerateMetaSections();
  void rebuildSections();
  void updateNavigationTitle();

  GlyphService *m_glyphService = nullptr;
  emoji::SkinTone m_skinTone = emoji::SkinTone::Default;

  DisplayMode m_displayMode = DisplayMode::Root;
  std::optional<glyph::Category> m_categoryFilter;

  EmojiGridSource m_pinnedSource;
  EmojiGridSource m_recentSource;
  std::vector<EmojiGridSource> m_groupSources;
  SearchEmojiGridSource m_searchSource;

  std::vector<const glyph::Item *> m_pinned;
  std::vector<const glyph::Item *> m_recent;
  std::span<const glyph::Section> m_sections;
  std::unordered_map<const glyph::Item *, GlyphMetadata, GlyphItemHash> m_metadataCache;
  std::vector<Scored<const glyph::Item *>> m_searchResultsStorage;
  std::span<Scored<const glyph::Item *>> m_searchResults;
};
