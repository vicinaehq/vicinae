#pragma once
#include "ui/omni-list/omni-list.hpp"
#include "ui/views/list-view.hpp"

class DMenuListView : public ListView {
  Q_OBJECT

signals:
  void selected(const QString &text) const;
  void aborted() const;

public:
  struct DmenuPayload {
    std::string raw;
    std::string navigationTitle;
    std::string placeholder;
    std::string sectionTitle;
    bool noSection;
  };

  DMenuListView(DmenuPayload data);

protected:
  void hideEvent(QHideEvent *event) override;

private:
  void itemSelected(const OmniList::AbstractVirtualItem *item) override;
  bool showBackButton() const override { return false; }
  bool onBackspace() override { return true; }
  QString initialNavigationTitle() const override;
  QString initialSearchPlaceholderText() const override;
  // Data initData() const override;
  void textChanged(const QString &text) override;
  void beforePop() override;
  void selectEntry(const QString &text);
  void initialize() override;

  bool m_selected = false;
  DmenuPayload m_data;

  std::vector<std::string_view> m_lines;
  std::vector<std::pair<std::string_view, int>> m_scoredItems;
};
