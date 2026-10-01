#include "services/global-shortcuts/global-shortcut-service.hpp"
#include <algorithm>
#include <utility>
#include "config/config.hpp"
#include "services/app-runtime/app-runtime.hpp"

GlobalShortcutHandle::GlobalShortcutHandle(GlobalShortcutService *service, QString id, std::uint64_t serial)
    : m_service(service), m_id(std::move(id)), m_serial(serial) {}

GlobalShortcutHandle::GlobalShortcutHandle(GlobalShortcutHandle &&other) noexcept
    : m_service(std::exchange(other.m_service, nullptr)), m_id(std::exchange(other.m_id, {})),
      m_serial(std::exchange(other.m_serial, 0)) {}

GlobalShortcutHandle &GlobalShortcutHandle::operator=(GlobalShortcutHandle &&other) noexcept {
  if (this != &other) {
    reset();
    m_service = std::exchange(other.m_service, nullptr);
    m_id = std::exchange(other.m_id, {});
    m_serial = std::exchange(other.m_serial, 0);
  }
  return *this;
}

GlobalShortcutHandle::~GlobalShortcutHandle() { reset(); }

void GlobalShortcutHandle::reset() {
  if (m_service) { m_service->release(m_id, m_serial); }
  m_service = nullptr;
  m_id.clear();
  m_serial = 0;
}

GlobalShortcutService::GlobalShortcutService(config::Manager &config, AppRuntime &appRuntime,
                                             std::unique_ptr<AbstractGlobalShortcutBackend> backend)
    : m_config(config), m_appRuntime(appRuntime), m_backend(std::move(backend)) {
  connect(m_backend.get(), &AbstractGlobalShortcutBackend::shortcutActivated, this,
          [this](const QString &id, quint64) { onActivated(id); });
  connect(m_backend.get(), &AbstractGlobalShortcutBackend::shortcutReleased, this,
          [this](const QString &id, quint64) { onReleased(id); });
  // `ready` may be re-emitted after a backend reset, in which case every binding is replayed
  connect(m_backend.get(), &AbstractGlobalShortcutBackend::ready, this, [this] {
    m_applied.clear();
    releaseHeld();
    reconcile();
  });
  connect(&m_config, &config::Manager::configChanged, this, &GlobalShortcutService::updateInhibition);
  connect(&m_appRuntime, &AppRuntime::frontmostAppChanged, this, &GlobalShortcutService::updateInhibition);

  m_inhibited = computeInhibited();
  m_backend->start();
}

GlobalShortcutHandle GlobalShortcutService::bind(const QString &id, Binding binding) {
  if (!binding.trigger.isValid()) {
    qWarning() << "Refusing to bind global shortcut" << id << "with an invalid trigger";
    return {};
  }

  const auto serial = m_nextSerial++;
  m_bindings.insert_or_assign(id, Entry{.binding = std::move(binding), .serial = serial});
  reconcile();
  return {this, id, serial};
}

void GlobalShortcutService::release(const QString &id, std::uint64_t serial) {
  auto it = m_bindings.find(id);
  if (it == m_bindings.end() || it->second.serial != serial) { return; }
  m_bindings.erase(it);
  reconcile();
}

void GlobalShortcutService::setCapturing(bool capturing) {
  if (m_capturing == capturing) { return; }
  m_capturing = capturing;
  m_backend->setCapturing(capturing);

  if (capturing) {
    unbindAll();
  } else {
    reconcile();
  }
}

void GlobalShortcutService::unbindAll() {
  m_backend->unbindAll();
  m_applied.clear();
  releaseHeld();
}

void GlobalShortcutService::releaseHeld() {
  std::vector<QString> held;
  held.reserve(m_bindings.size());
  for (const auto &[id, entry] : m_bindings) {
    if (entry.held) { held.emplace_back(id); }
  }
  for (const auto &id : held) {
    onReleased(id);
  }
}

void GlobalShortcutService::reconcile() {
  if (m_capturing || m_inhibited) { return; }
  if (!isSupported()) { return; }

  for (auto it = m_applied.begin(); it != m_applied.end();) {
    auto desired = m_bindings.find(it->first);
    if (desired == m_bindings.end() || desired->second.binding.trigger != it->second) {
      m_backend->unbindShortcut(it->first);
      it = m_applied.erase(it);
    } else {
      ++it;
    }
  }

  for (const auto &[id, entry] : m_bindings) {
    if (m_applied.contains(id)) { continue; }

    const auto &binding = entry.binding;
    auto bound =
        m_backend->bindShortcut({.id = id, .trigger = binding.trigger, .description = binding.description});
    m_applied.emplace(id, binding.trigger);

    if (!bound) {
      qWarning() << "Failed to bind global shortcut" << id << "(" << binding.trigger.toString()
                 << "):" << bound.error();
    }
  }
}

std::optional<QString> GlobalShortcutService::probeBind(const Keyboard::Shortcut &shortcut) {
  if (!isSupported() || !m_capturing) { return std::nullopt; }

  const QString probeId = QStringLiteral("@probe");
  auto bound =
      m_backend->bindShortcut({.id = probeId, .trigger = shortcut, .description = QStringLiteral("Vicinae")});
  m_backend->unbindShortcut(probeId);

  if (!bound) { return bound.error(); }
  return std::nullopt;
}

void GlobalShortcutService::updateInhibition() {
  const bool inhibited = computeInhibited();
  if (inhibited == m_inhibited) { return; }

  m_inhibited = inhibited;
  if (m_capturing) { return; }

  if (inhibited) {
    unbindAll();
  } else {
    reconcile();
  }
}

bool GlobalShortcutService::computeInhibited() const {
  const auto &apps = m_config.value().globalShortcuts.inhibitApps;
  if (apps.empty()) { return false; }

  const auto app = m_appRuntime.frontmostApp();
  if (!app) { return false; }

  const std::string id = app->id().toStdString();
  return std::ranges::contains(apps, id);
}

void GlobalShortcutService::onActivated(const QString &id) {
  auto it = m_bindings.find(id);
  if (it == m_bindings.end()) { return; }

  auto &entry = it->second;
  if (entry.binding.onReleased) {
    if (entry.held) { return; }
    entry.held = true;
  }

  if (auto handler = entry.binding.onActivated) { handler(); }
}

void GlobalShortcutService::onReleased(const QString &id) {
  auto it = m_bindings.find(id);
  if (it == m_bindings.end() || !it->second.held) { return; }

  it->second.held = false;
  if (auto handler = it->second.binding.onReleased) { handler(); }
}

std::optional<QString> GlobalShortcutService::findConflict(const Keyboard::Shortcut &shortcut,
                                                           const QString &excludeId) const {
  if (!isSupported()) { return std::nullopt; }

  for (const auto &[id, entry] : m_bindings) {
    if (id == excludeId || entry.binding.trigger != shortcut) { continue; }
    return entry.binding.description.isEmpty() ? id : entry.binding.description;
  }

  return std::nullopt;
}
