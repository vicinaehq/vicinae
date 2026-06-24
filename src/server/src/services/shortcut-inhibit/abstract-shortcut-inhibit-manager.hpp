#pragma once
#include <qobject.h>
#include <qwindow.h>

class AbstractShortcutInhibitManager : public QObject {
public:
  virtual ~AbstractShortcutInhibitManager() = default;

  virtual bool isSupported() const = 0;
  virtual bool inhibit(QWindow *win) = 0;
  virtual bool release(QWindow *win) = 0;
};

class DummyShortcutInhibitManager : public AbstractShortcutInhibitManager {
public:
  bool isSupported() const override { return false; }
  bool inhibit(QWindow *) override { return false; }
  bool release(QWindow *) override { return false; }
};
