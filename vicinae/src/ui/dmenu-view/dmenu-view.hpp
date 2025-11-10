#pragma once
#include "proto/daemon.pb.h"
#include "ui/omni-list/omni-list.hpp"
#include "ui/views/list-view.hpp"

namespace DMenu {
struct Payload {
  std::string raw;
  std::optional<std::string> navigationTitle;
  std::optional<std::string> placeholder;
  std::optional<std::string> sectionTitle;
  std::optional<std::string> query;
  bool noSection = false;
  bool noQuickLook = false;
  bool noMetadata = false;

  static Payload fromProto(const proto::ext::daemon::DmenuRequest &req);
  proto::ext::daemon::DmenuRequest toProto() const;
};

class View : public ListView {
  Q_OBJECT

signals:
  void selected(const QString &text) const;
  void aborted() const;

public:
  View(Payload data);

protected:
  void hideEvent(QHideEvent *event) override;
  QString expandSectionName(size_t count) const;

private:
  void itemSelected(const OmniList::AbstractVirtualItem *item) override;
  void emptied() override;
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
  Payload m_data;

  std::vector<std::string_view> m_lines;
  std::vector<std::pair<std::string_view, int>> m_scoredItems;
};

}; // namespace DMenu
