#pragma once
#include <QString>
#include <vector>

struct InputSourceEntry {
  QString id;
  QString name;
};

class InputSourceService {
public:
  virtual ~InputSourceService() = default;

  static InputSourceService *instance();

  virtual bool isSupported() const = 0;
  virtual std::vector<InputSourceEntry> availableInputSources() const = 0;
  virtual QString currentInputSourceId() const = 0;
  virtual bool selectInputSource(const QString &id) = 0;

  virtual void onLauncherOpening() = 0;
  virtual void onLauncherClosing() = 0;
};
