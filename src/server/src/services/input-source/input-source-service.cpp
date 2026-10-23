#include "services/input-source/input-source-service.hpp"
#include <QtGlobal>

#ifndef Q_OS_MACOS
class DefaultInputSourceService : public InputSourceService {
public:
  bool isSupported() const override { return false; }
  std::vector<InputSourceEntry> availableInputSources() const override { return {}; }
  QString currentInputSourceId() const override { return {}; }
  bool selectInputSource(const QString &) override { return false; }
  void onLauncherOpening() override {}
  void onLauncherClosing() override {}
};

InputSourceService *InputSourceService::instance() {
  static DefaultInputSourceService instance;
  return &instance;
}
#endif
