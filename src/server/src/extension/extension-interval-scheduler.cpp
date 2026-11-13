#include "extension/extension-interval-scheduler.hpp"
#include <algorithm>
#include <format>
#include <unordered_set>
#include <QDateTime>
#include <QPointer>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <qlogging.h>
#include "extension/extension-command.hpp"
#include "extension/extension-command-runtime.hpp"
#include "extension/headless-command-runner.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "service-registry.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "vicinae.hpp"

namespace fs = std::filesystem;

ExtensionIntervalScheduler::ExtensionIntervalScheduler(ApplicationContext &ctx) : m_ctx(ctx) {
  m_statePath = Omnicast::stateDir() / "extension-intervals.json";
  loadState();

  connect(ctx.services->extensionRegistry(), &ExtensionRegistry::extensionsChanged, this,
          &ExtensionIntervalScheduler::rebuild);
}

ExtensionIntervalScheduler::~ExtensionIntervalScheduler() {
  for (auto &entry : m_entries) {
    cancelRun(*entry);
  }
}

ExtensionIntervalScheduler::ScheduledEntry *ExtensionIntervalScheduler::findEntry(const std::string &key) {
  auto it = std::ranges::find(m_entries, key, &ScheduledEntry::key);
  return it != m_entries.end() ? it->get() : nullptr;
}

void ExtensionIntervalScheduler::rebuild() {
  auto *root = m_ctx.services->rootItemManager();
  std::unordered_set<std::string> seen;

  for (auto *ext : root->extensions()) {
    for (const auto &cmd : ext->repository()->commands()) {
      auto extCmd = std::dynamic_pointer_cast<ExtensionCommand>(cmd);
      if (!extCmd || !extCmd->interval()) continue;
      if (extCmd->mode() != CommandModeNoView) continue;

      auto key = makeKey(*extCmd);
      seen.insert(key);

      if (auto *existing = findEntry(key)) {
        if (existing->interval != *extCmd->interval()) {
          existing->interval = *extCmd->interval();
          qInfo() << "[ExtensionIntervalScheduler] Updated interval for" << key << "to"
                  << existing->interval.count() << "s";
        }
        continue;
      }

      auto entry = std::make_unique<ScheduledEntry>();
      entry->key = key;
      entry->command = extCmd;
      entry->interval = *extCmd->interval();
      entry->timer.setSingleShot(true);

      auto delay = computeInitialDelay(key, entry->interval);

      qInfo() << "[ExtensionIntervalScheduler] Registered" << key << "| interval:" << entry->interval.count()
              << "s | initial delay:" << delay.count() << "ms";

      connect(&entry->timer, &QTimer::timeout, this, [this, k = key]() { tick(k); });

      entry->timer.setInterval(delay);
      entry->timer.start();

      m_entries.emplace_back(std::move(entry));
    }
  }

  // cancelling calls back into findEntry, so it can't happen while erasing
  for (auto &entry : m_entries) {
    if (seen.contains(entry->key)) continue;
    qInfo() << "[ExtensionIntervalScheduler] Removing schedule for" << entry->key;
    cancelRun(*entry);
  }

  std::erase_if(m_entries,
                [&](const std::unique_ptr<ScheduledEntry> &entry) { return !seen.contains(entry->key); });
}

void ExtensionIntervalScheduler::tick(const std::string &key) {
  auto *entry = findEntry(key);
  if (!entry) return;

  auto intervalMs = std::chrono::duration_cast<std::chrono::milliseconds>(entry->interval);

  if (!m_ctx.services->rootItemManager()->itemMetadata(entry->command->uniqueId()).enabled) {
    entry->timer.setInterval(intervalMs);
    entry->timer.start();
    return;
  }

  if (entry->runtime) {
    qInfo() << "[ExtensionIntervalScheduler] Skipping" << key << "- previous instance still running";
    entry->timer.setInterval(intervalMs);
    entry->timer.start();
    return;
  }

  const auto onFinished = [self = QPointer(this), k = key](const ExtensionCommandRuntime::RunResult &result) {
    if (!result) qWarning() << "[ExtensionIntervalScheduler]" << k << "failed:" << result.error();
    if (!self) return;
    if (auto *e = self->findEntry(k)) e->runtime = nullptr;
  };

  auto runtime = m_ctx.services->headlessCommandRunner()->run(entry->command, onFinished);
  if (!runtime) {
    qWarning() << "[ExtensionIntervalScheduler] Deferring" << key << "-" << runtime.error();
    entry->timer.setInterval(intervalMs);
    entry->timer.start();
    return;
  }

  qInfo() << "[ExtensionIntervalScheduler] Executing" << key;
  entry->runtime = *runtime;

  recordExecution(key);

  entry->timer.setInterval(intervalMs);
  entry->timer.start();
}

void ExtensionIntervalScheduler::cancelRun(ScheduledEntry &entry) {
  if (!entry.runtime) return;
  qInfo() << "[ExtensionIntervalScheduler] Cancelling" << entry.key;
  entry.runtime->cancel("The schedule was removed.");
}

std::chrono::milliseconds
ExtensionIntervalScheduler::computeInitialDelay(const std::string &key, std::chrono::seconds interval) const {
  auto it = m_lastRun.find(key);
  if (it == m_lastRun.end()) return std::chrono::milliseconds(0);

  auto now = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());
  auto elapsed = std::chrono::seconds(now - it->second);
  auto remaining = interval - elapsed;

  if (remaining <= std::chrono::seconds(0)) return std::chrono::milliseconds(0);

  return std::chrono::duration_cast<std::chrono::milliseconds>(remaining);
}

void ExtensionIntervalScheduler::loadState() {
  if (!fs::is_regular_file(m_statePath)) return;

  if (auto const error = glz::read_file_json(m_lastRun, m_statePath.string(), m_buf)) {
    qWarning() << "[ExtensionIntervalScheduler] Failed to read state from" << m_statePath.string()
               << glz::format_error(error);
  }
}

void ExtensionIntervalScheduler::saveState() {
  std::error_code ec;
  fs::create_directories(m_statePath.parent_path(), ec);

  if (auto const error = glz::write_file_json(m_lastRun, m_statePath.string(), m_buf)) {
    qWarning() << "[ExtensionIntervalScheduler] Failed to write state to" << m_statePath.string()
               << glz::format_error(error);
  }
}

void ExtensionIntervalScheduler::recordExecution(const std::string &key) {
  m_lastRun[key] = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());
  saveState();
}

std::string ExtensionIntervalScheduler::makeKey(const ExtensionCommand &cmd) {
  return std::format("{}:{}", cmd.extensionId().toStdString(), cmd.commandId().toStdString());
}
