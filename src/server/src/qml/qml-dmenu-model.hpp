#pragma once
#include "common/scored.hpp"
#include "qml-command-list-model.hpp"

class QmlDMenuModel : public QmlCommandListModel {
  Q_OBJECT

signals:
  void entryChosen(const QString &text);
  void fileHighlighted(std::string_view path);

public:
  using QmlCommandListModel::QmlCommandListModel;

  void setRawEntries(std::vector<std::string_view> entries);
  void setSectionTemplate(std::string_view tpl) { m_sectionTemplate = tpl; }
  void setNoSection(bool v) { m_noSection = v; }
  void setFilter(const QString &text) override;

protected:
  QString itemTitle(int section, int item) const override;
  QString itemIconSource(int section, int item) const override;
  std::unique_ptr<ActionPanelState> createActionPanel(int section, int item) const override;
  void onItemSelected(int section, int item) override;
  void onSelectionCleared() override;

private:
  std::string_view entryAt(int section, int item) const;
  void rebuildSections();
  QString expandSectionName(size_t count) const;
  void selectEntry(const QString &text) const;

  std::vector<std::string_view> m_entries;
  std::vector<Scored<std::string_view>> m_filtered;
  std::string_view m_sectionTemplate = "Entries ({count})";
  QString m_currentSearchText;
  bool m_noSection = false;
};
