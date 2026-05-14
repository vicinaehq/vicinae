#include "extension-interval-scheduler.hpp"
#include <algorithm>
#include <format>
#include <QDateTime>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <qlogging.h>
#include "extension/extension-command.hpp"
#include "extension/extension-command-runtime.hpp"
#include "extension/manager/extension-manager.hpp"
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
    if (entry->running) forceUnload(*entry);
  }
}

ExtensionIntervalScheduler::ScheduledEntry *ExtensionIntervalScheduler::findEntry(const std::string &key) {
  auto it = std::ranges::find(m_entries, key, &ScheduledEntry::key);
  return it != m_entries.end() ? it->get() : nullptr;
}

void ExtensionIntervalScheduler::rebuild() {
  auto *root = m_ctx.services->rootItemManager();
  std::vector<std::string> seen;

  for (auto *ext : root->extensions()) {
    for (const auto &cmd : ext->repository()->commands()) {
      auto extCmd = std::dynamic_pointer_cast<ExtensionCommand>(cmd);
      if (!extCmd || !extCmd->interval()) continue;
      if (extCmd->mode() != CommandModeNoView) continue;

      auto key = makeKey(*extCmd);
      seen.emplace_back(key);

      if (auto *existing = findEntry(key)) {
        if (existing->interval != *extCmd->interval()) {
          existing->interval = *extCmd->interval();
          qInfo() << "[ExtensionIntervalScheduler] Updated interval for" << key << "to" << existing->interval.count()
                  << "s";
        }
        continue;
      }

      auto entry = std::make_unique<ScheduledEntry>();
      entry->key = key;
      entry->command = extCmd;
      entry->interval = *extCmd->interval();
      entry->timer.setSingleShot(true);
      entry->timeoutTimer.setSingleShot(true);

      auto delay = computeInitialDelay(key, entry->interval);

      qInfo() << "[ExtensionIntervalScheduler] Registered" << key << "| interval:" << entry->interval.count()
              << "s | initial delay:" << delay.count() << "ms";

      connect(&entry->timer, &QTimer::timeout, this, [this, k = key]() { tick(k); });
      connect(&entry->timeoutTimer, &QTimer::timeout, this, [this, k = key]() {
        if (auto *e = findEntry(k); e && e->running) forceUnload(*e);
      });

      entry->timer.setInterval(delay);
      entry->timer.start();

      m_entries.emplace_back(std::move(entry));
    }
  }

  std::erase_if(m_entries, [&](const std::unique_ptr<ScheduledEntry> &entry) {
    if (std::ranges::find(seen, entry->key) != seen.end()) return false;

    qInfo() << "[ExtensionIntervalScheduler] Removing schedule for" << entry->key;
    if (entry->running) forceUnload(*entry);
    return true;
  });
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

  if (entry->running) {
    qInfo() << "[ExtensionIntervalScheduler] Skipping" << key << "- previous instance still running";
    entry->timer.setInterval(intervalMs);
    entry->timer.start();
    return;
  }

  if (!m_ctx.services->extensionManager()->isRunning()) {
    qWarning() << "[ExtensionIntervalScheduler] Extension manager not running, deferring" << key;
    entry->timer.setInterval(intervalMs);
    entry->timer.start();
    return;
  }

  qInfo() << "[ExtensionIntervalScheduler] Executing" << key;

  entry->running = true;

  auto *runtime = new ExtensionCommandRuntime(entry->command);
  runtime->setHeadless(true);
  runtime->setContext(&m_ctx);

  entry->runtime = runtime;

  entry->timeoutTimer.setInterval(intervalMs);
  entry->timeoutTimer.start();

  connect(m_ctx.services->extensionManager(), &ExtensionManager::extensionCrashed, entry->runtime,
          [this, k = key](const std::string &sessionId, const std::string &reason) {
            Q_UNUSED(sessionId);
            auto *e = findEntry(k);
            if (!e || !e->running) return;
            qWarning() << "[ExtensionIntervalScheduler] Crash cleanup for" << k << "-" << reason;
            forceUnload(*e);
          });

  LaunchProps props;
  entry->runtime->load(props);

  recordExecution(key);

  entry->timer.setInterval(intervalMs);
  entry->timer.start();
}

void ExtensionIntervalScheduler::forceUnload(ScheduledEntry &entry) {
  if (!entry.running) return;

  qInfo() << "[ExtensionIntervalScheduler] Unloading" << entry.key;

  entry.runtime->unload();
  entry.runtime->deleteLater();
  entry.runtime = nullptr;
  entry.timeoutTimer.stop();
  entry.running = false;
}

std::chrono::milliseconds ExtensionIntervalScheduler::computeInitialDelay(const std::string &key,
                                                                 std::chrono::seconds interval) const {
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

  if (auto const error = glz::read_file_json(m_lastRun, m_statePath.c_str(), m_buf)) {
    qWarning() << "[ExtensionIntervalScheduler] Failed to read state from" << m_statePath.c_str()
               << glz::format_error(error);
  }
}

void ExtensionIntervalScheduler::saveState() {
  std::error_code ec;
  fs::create_directories(m_statePath.parent_path(), ec);

  if (auto const error = glz::write_file_json(m_lastRun, m_statePath.c_str(), m_buf)) {
    qWarning() << "[ExtensionIntervalScheduler] Failed to write state to" << m_statePath.c_str()
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
