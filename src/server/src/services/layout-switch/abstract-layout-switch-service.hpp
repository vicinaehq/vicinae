#pragma once
#include <vector>
#include <QObject>
#include <QString>

struct InputLayout {
  QString id;   // platform-native identifier, stored as-is in config
  QString name; // localized display name
};

/**
 * Switches the active keyboard layout while the launcher window is shown and restores the previous
 * one on hide. The base class is the inert fallback for platforms without an implementation.
 */
class AbstractLayoutSwitchService : public QObject {
  Q_OBJECT

public:
  using QObject::QObject;

  virtual bool isSupported() const { return false; }
  virtual std::vector<InputLayout> availableLayouts() const { return {}; }

  /// Remember the active layout and switch to `layoutId`.
  virtual void activate(const QString & /*layoutId*/) {}

  /// Switch back to the remembered layout, unless the user changed layouts in the meantime.
  virtual void restore() {}
};
