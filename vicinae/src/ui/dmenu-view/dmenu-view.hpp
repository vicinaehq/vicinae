#pragma once
#include "ui/views/list-view.hpp"

class DMenuListView : public SearchableListView {
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
  QString sectionName() const override;
  Data initData() const override;
  void beforePop() override;
  void selectEntry(const QString &text);

  bool m_selected = false;
  DmenuPayload m_data;
};
