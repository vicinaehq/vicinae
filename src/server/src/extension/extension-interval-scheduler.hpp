#pragma once
#include <chrono>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include <QObject>
#include <QPointer>
#include <QTimer>
#include "common/context.hpp"

class ExtensionCommand;
class ExtensionCommandRuntime;

class ExtensionIntervalScheduler : public QObject {
  Q_OBJECT

  struct ScheduledEntry {
    std::string key;
    std::shared_ptr<ExtensionCommand> command;
    std::chrono::seconds interval;
    QTimer timer;

    // set while a run is in progress, the runtime deletes itself once it finishes
    QPointer<ExtensionCommandRuntime> runtime;
  };

public:
  explicit ExtensionIntervalScheduler(ApplicationContext &ctx);
  ~ExtensionIntervalScheduler() override;

  void rebuild();

private:
  ScheduledEntry *findEntry(const std::string &key);

  void tick(const std::string &key);
  void cancelRun(ScheduledEntry &entry);

  void loadState();
  void saveState();
  void recordExecution(const std::string &key);
  std::chrono::milliseconds computeInitialDelay(const std::string &key, std::chrono::seconds interval) const;

  static std::string makeKey(const ExtensionCommand &cmd);

  ApplicationContext &m_ctx;
  std::filesystem::path m_statePath;
  std::unordered_map<std::string, std::uint64_t> m_lastRun;
  std::vector<std::unique_ptr<ScheduledEntry>> m_entries;
  std::string m_buf;
};
