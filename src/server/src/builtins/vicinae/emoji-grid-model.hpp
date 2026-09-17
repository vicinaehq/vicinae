#pragma once
#include <QtQml/qqmlregistration.h>
#include "ui/views/view-utils.hpp"
#include "glyph/emoji.hpp"
#include "ui/views/grid-source.hpp"
#include "ui/views/section-grid-model.hpp"
#include "services/glyph-service/glyph-service.hpp"
#include <QCoreApplication>
#include <optional>
#include <span>

using GlyphMetadataMap = std::unordered_map<const glyph::Item *, GlyphMetadata, GlyphItemHash>;

class EmojiSourceBase : public GridSource {
public:
  void setSkinTone(std::optional<emoji::SkinTone> tone) { m_skinTone = tone; }
  void setMetadata(const GlyphMetadataMap *metadata) { m_metadata = metadata; }

  virtual const glyph::Item *emojiAt(int i) const = 0;

  QString itemTooltip(int i) const override;
  std::optional<ImageURL> itemIcon(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  std::optional<emoji::SkinTone> m_skinTone;
  const GlyphMetadataMap *m_metadata = nullptr;
};

class EmojiGridSource : public EmojiSourceBase {
public:
  void setEmojis(const QString &name, std::span<const glyph::Item *const> emojis);

  QString sectionName() const override { return m_name; }
  int count() const override { return static_cast<int>(m_emojis.size()); }
  const glyph::Item *emojiAt(int i) const override;

private:
  QString m_name;
  std::vector<const glyph::Item *> m_emojis;
};

class SearchEmojiGridSource : public EmojiSourceBase {
public:
  void setResults(std::span<Scored<const glyph::Item *>> results);

  QString sectionName() const override {
    return QCoreApplication::translate("SearchEmojiGridSource", "Results (%1)").arg(m_results.size());
  }
  int count() const override { return static_cast<int>(m_results.size()); }
  const glyph::Item *emojiAt(int i) const override;

private:
  std::span<Scored<const glyph::Item *>> m_results;
};

class EmojiGridModel : public SectionGridModel {
  Q_OBJECT
  QML_NAMED_ELEMENT(EmojiGridModel)
  QML_UNCREATABLE("")

public:
  explicit EmojiGridModel(QObject *parent = nullptr);

  void initialize();
  void setFilter(const QString &text);
  void setCategoryFilter(std::optional<glyph::Category> category);
  QString searchPlaceholder() const { return tr("Search for emojis and symbols..."); }
  QUrl qmlComponentUrl() const { return qml::componentUrl(u"EmojiGridView"); }

private:
  enum class DisplayMode { Root, Search };

  const glyph::Item *emojiAt(int section, int item) const;
  QString emojiName(int section, int item) const;
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
  GlyphMetadataMap m_metadataCache;
  std::vector<Scored<const glyph::Item *>> m_searchResultsStorage;
  std::span<Scored<const glyph::Item *>> m_searchResults;
};
