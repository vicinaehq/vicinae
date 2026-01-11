#pragma once
#include "proto/daemon.pb.h"
#include <QObject>

struct BrowserTab {
  int id;
  std::string title;
  std::string url;
  int windowId;
  bool active;

  static BrowserTab fromProto(const proto::ext::daemon::BrowserTabInfo &info) {
    return BrowserTab(info.id(), info.title(), info.url(), info.window_id(), info.active());
  }
};

class BrowserExtensionService : public QObject {
  Q_OBJECT

signals:
  void tabFocusRequested(int id) const;
  void tabsChanged(std::span<const BrowserTab> tabs) const;

public:
  BrowserExtensionService() = default;

  std::span<const BrowserTab> tabs() const { return m_tabs; }
  void setTabs(std::vector<BrowserTab> tabs) {
    m_tabs = tabs;
    emit tabsChanged(m_tabs);
  }

  void focusTab(int id) { emit tabFocusRequested(id); }

private:
  std::vector<BrowserTab> m_tabs;
};
