#pragma once
#include "font-service.hpp"
#include "fuzzy-scorer.hpp"
#include "fuzzy/scored.hpp"
#include "grid-source.hpp"
#include "section-grid-model.hpp"
#include <QString>
#include <QStringList>
#include <optional>
#include <span>
#include <vector>

class FontGridSource : public GridSource {
public:
  void setBucket(QString name, std::vector<const FontFamily *> families);
  void setResults(QString name, std::span<Scored<const FontFamily *>> results);

  QString sectionName() const override { return m_name; }
  int count() const override {
    return m_search ? static_cast<int>(m_results.size()) : static_cast<int>(m_families.size());
  }

  const FontFamily *familyAt(int i) const {
    if (i < 0 || i >= count()) return nullptr;
    return m_search ? m_results[i].data : m_families[i];
  }

  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  QString m_name;
  bool m_search = false;
  std::vector<const FontFamily *> m_families;
  std::span<Scored<const FontFamily *>> m_results;
};

class FontGridModel : public SectionGridModel {
  Q_OBJECT

public:
  explicit FontGridModel(QObject *parent = nullptr);

  void initialize();
  void setFilter(const QString &text);
  void setCategoryFilter(std::optional<int> index);
  const QStringList &categoryNames() const { return m_categoryNames; }
  QString searchPlaceholder() const { return QStringLiteral("Search fonts..."); }

  Q_INVOKABLE QString cellTitle(int section, int item) const;
  Q_INVOKABLE QString fontIcon(int section, int item) const;
  Q_INVOKABLE QString cellTooltip(int section, int item) const;

private:
  enum class Mode : std::uint8_t { Root, Search };

  void buildFilterOptions();
  void rebuildRoot();
  void rebuildSections();
  void applyReset();
  void updateNavigationTitle();
  const FontFamily *familyAt(int section, int item) const;

  FontService *m_fontService = nullptr;
  Mode m_mode = Mode::Root;
  std::vector<FontCategory> m_filterCategories;
  QStringList m_categoryNames;
  std::optional<FontCategory> m_categoryFilter;
  FontGridSource m_rootSource;
  FontGridSource m_searchSource;
  FuzzyScorer<FontFamily> m_scorer;
};
