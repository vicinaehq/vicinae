#include "services/global-shortcuts/input-server-global-shortcut-backend.hpp"
#include "services/global-shortcuts/xkb-keysym.hpp"
#include "services/input-server/linux-input-server.hpp"

namespace {

// Neutral modifier bitmask shared with the input server (see figura/snippet.fig).
// Not Qt's numbering, not xkb's: the helper resolves these against the live keymap.
enum ShortcutModifier : uint32_t {
  Ctrl = 1u << 0,
  Alt = 1u << 1,
  Shift = 1u << 2,
  Super = 1u << 3,
};

uint32_t modifiersFor(Qt::KeyboardModifiers mods) {
  uint32_t out = 0;
  if (mods & Qt::ControlModifier) { out |= Ctrl; }
  if (mods & Qt::AltModifier) { out |= Alt; }
  if (mods & Qt::ShiftModifier) { out |= Shift; }
  if (mods & Qt::MetaModifier) { out |= Super; }
  return out;
}

std::optional<snippet_gen::GlobalShortcutBinding> bindingFor(const QString &id,
                                                             const Keyboard::Shortcut &shortcut) {
  auto keysym = global_shortcuts::xkbKeysymForQtKey(shortcut.key());
  if (!keysym) { return std::nullopt; }

  return snippet_gen::GlobalShortcutBinding{
      .id = id.toStdString(),
      .keysym = *keysym,
      .modifiers = modifiersFor(shortcut.mods()),
  };
}

} // namespace

InputServerGlobalShortcutBackend::InputServerGlobalShortcutBackend(LinuxInputServer &inputServer)
    : m_inputServer(inputServer) {
  connect(&m_inputServer, &LinuxInputServer::globalShortcutTriggered, this,
          [this](std::string id, quint64 timestamp) {
            emit shortcutActivated(QString::fromStdString(id), timestamp);
          });
  connect(&m_inputServer, &LinuxInputServer::serverReady, this,
          &InputServerGlobalShortcutBackend::onServerReady);
}

bool InputServerGlobalShortcutBackend::isActivatable() const { return true; }

bool InputServerGlobalShortcutBackend::start() {
  if (m_inputServer.isRunning()) { emit ready(); }
  return true;
}

void InputServerGlobalShortcutBackend::bindShortcut(const GlobalShortcutRequest &request) {
  GlobalShortcutInfo info;
  info.id = request.id;
  info.trigger = request.preferredTrigger;
  if (request.preferredTrigger) { info.triggerDisplay = request.preferredTrigger->toDisplayString(); }

  std::optional<snippet_gen::GlobalShortcutBinding> binding;
  if (request.preferredTrigger) { binding = bindingFor(request.id, *request.preferredTrigger); }

  if (!binding) {
    info.status = GlobalShortcutStatus::Failed;
    info.error = "Unsupported shortcut";
    m_shortcuts[request.id] = std::move(info);
    emit shortcutsChanged();
    return;
  }

  info.status = GlobalShortcutStatus::Bound;
  m_shortcuts[request.id] = std::move(info);
  m_inputServer.registerGlobalShortcut(*binding);
  emit shortcutsChanged();
}

void InputServerGlobalShortcutBackend::unbindShortcut(const QString &id) {
  if (m_shortcuts.erase(id) == 0) { return; }
  m_inputServer.unregisterGlobalShortcut(id.toStdString());
  emit shortcutsChanged();
}

std::optional<GlobalShortcutInfo> InputServerGlobalShortcutBackend::shortcut(const QString &id) const {
  if (auto it = m_shortcuts.find(id); it != m_shortcuts.end()) { return it->second; }
  return std::nullopt;
}

void InputServerGlobalShortcutBackend::onServerReady() {
  for (const auto &[id, info] : m_shortcuts) {
    if (info.status != GlobalShortcutStatus::Bound || !info.trigger) { continue; }
    if (auto binding = bindingFor(id, *info.trigger)) { m_inputServer.registerGlobalShortcut(*binding); }
  }
  emit ready();
}
