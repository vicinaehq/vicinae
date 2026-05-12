#pragma once
#include "fuzzy/scored.hpp"
#include "section-source.hpp"
#include <functional>
#include <string>
#include <string_view>
#include <vector>

class DMenuSection : public SectionSource {
public:
  void setRawEntries(std::vector<std::string_view> entries);
  void setSectionTemplate(std::string_view tpl) { m_sectionTemplate = tpl; }
  void setNoSection(bool v) { m_noSection = v; }
  void setNoQuickLook(bool v) { m_noQuickLook = v; }
  void setFilter(std::string_view query) override;

  QString sectionName() const override;
  int count() const override { return static_cast<int>(m_filtered.size()); }

  void setOnEntryChosen(std::function<void(const QString &)> cb) { m_onEntryChosen = std::move(cb); }
  void setOnFileHighlighted(std::function<void(std::string_view)> cb) { m_onFileHighlighted = std::move(cb); }

  void onSelected(int i) override;

protected:
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemIconSource(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  std::string_view entryAt(int i) const;
  QString expandSectionName(size_t count) const;
  void selectEntry(const QString &text) const;

  std::vector<std::string_view> m_entries;
  std::vector<Scored<std::string_view>> m_filtered;
  std::string_view m_sectionTemplate = "Entries ({count})";
  QString m_currentSearchText;
  bool m_noSection = false;
  bool m_noQuickLook = false;

  std::function<void(const QString &)> m_onEntryChosen;
  std::function<void(std::string_view)> m_onFileHighlighted;
};
