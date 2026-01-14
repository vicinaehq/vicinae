#pragma once
#include <QObject>
#include "vicinae-ipc/ipc.hpp"

class BrowserExtensionService : public QObject {
  Q_OBJECT

signals:
  void tabFocusRequested(int id) const;
  void tabsChanged(std::span<const ipc::BrowserTabInfo> tabs) const;

public:
  BrowserExtensionService() = default;

  std::span<const ipc::BrowserTabInfo> tabs() const { return m_tabs; }
  void setTabs(std::vector<ipc::BrowserTabInfo> tabs) {
    m_tabs = tabs;
    emit tabsChanged(m_tabs);
  }

  void focusTab(int id) { emit tabFocusRequested(id); }

private:
  std::vector<ipc::BrowserTabInfo> m_tabs;
};
