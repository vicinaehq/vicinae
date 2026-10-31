#pragma once
#include "keyboard/keyboard.hpp"
#include <QFuture>
#include <QString>
#include <QStringList>
#include <memory>
#include <vector>

namespace MenuBar {

struct Entry {
  QString title;
  QStringList path;
  Keyboard::Shortcut shortcut;
  int pid = 0;
  // AXUIElementRef of the menu item, retained for the lifetime of the entry
  std::shared_ptr<const void> itemRef;
  // AXUIElementRefs of the submenu parents leading to the item, outermost first
  std::vector<std::shared_ptr<const void>> ancestorRefs;
};

struct Target {
  int pid = 0;
  QString appName;
  QString bundleId;
};

struct Snapshot {
  int pid = 0;
  QString appName;
  QString bundleId;
  std::vector<Entry> entries;
};

bool isSupported();
// Must be called from the main thread: NSWorkspace state is only reliable there
Target frontmostTarget();
QFuture<Snapshot> scan(Target target);
void pressItem(Entry entry);
void revealInMenuBar(Entry entry);

} // namespace MenuBar
