#include "snippet/snippet.hpp"
#include <cctype>
#include <cstdint>
#include <cstring>
#include <glaze/json/read.hpp>
#include <linux/input-event-codes.h>
#include <expected>
#include <linux/input.h>
#include <libudev.h>
#include <string_view>
#include <xkbcommon/xkbcommon.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <algorithm>
#include <unordered_map>

static constexpr size_t MAX_BUFFER_SIZE = 32;
static constexpr uint32_t MAX_MESSAGE_SIZE = 64 * 1024;
static constexpr const char *VIRTUAL_KB_NAME = "vicinae-snippet-virtual-keyboard";

static bool isWordSeparator(char c) {
  return std::isspace(static_cast<unsigned char>(c)) || std::ispunct(static_cast<unsigned char>(c));
}

/**
 * Parse message as data gets in, and call the provided callback once a full message
 * has been parsed.
 */
class Frame {
public:
  using Handler = std::function<void(std::string_view message)>;

  void setHandler(Handler fn) { m_fn = std::move(fn); }

  bool readPart(int fd) {
    const int rc = read(fd, m_buf.data(), m_buf.size());

    if (rc == -1) {
      std::cerr << "Failed to read fd " << fd << ": " << strerror(errno) << '\n';
      return false;
    }

    if (rc == 0) { return false; }

    data.append(std::string_view(m_buf.data(), rc));

    for (;;) {
      if (size == 0 && data.size() >= sizeof(size)) {
        size = *reinterpret_cast<decltype(size) *>(data.data());
        data.erase(data.begin(), data.begin() + sizeof(size));
        if (size > MAX_MESSAGE_SIZE) { return false; }
      }

      if (size && size <= data.size()) {
        const std::string_view view(data);
        if (m_fn) m_fn(view.substr(0, size));
        data = view.substr(size);
        size = 0;
        continue;
      }

      break;
    }

    return true;
  }

private:
  std::array<char, 8096> m_buf;
  std::string data;
  uint32_t size = 0;
  Handler m_fn;
};

namespace snippet {

struct Input {
  int fd = -1;
  std::string name;
  epoll_event ev;
};

SnippetService::SnippetService(snippet_gen::RpcTransport &transport)
    : snippet_gen::AbstractSnippet(transport), m_udev(udev_new()),
      m_xkb(xkb_context_new(XKB_CONTEXT_NO_FLAGS)),
      m_keymap(xkb_keymap_new_from_names(m_xkb, nullptr, XKB_KEYMAP_COMPILE_NO_FLAGS)),
      m_kbState(xkb_state_new(m_keymap)) {
  m_text.reserve(MAX_BUFFER_SIZE);
}

std::vector<std::string> SnippetService::enumerateKeyboards() {
  udev_enumerate *enumerate = udev_enumerate_new(m_udev);
  std::vector<std::string> devNames;

  udev_enumerate_add_match_subsystem(enumerate, "input");
  udev_enumerate_add_match_property(enumerate, "ID_INPUT_KEYBOARD", "1");
  udev_enumerate_scan_devices(enumerate);

  udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
  udev_list_entry *entry;

  udev_list_entry_foreach(entry, devices) {
    const char *path = udev_list_entry_get_name(entry);
    struct udev_device *dev = udev_device_new_from_syspath(m_udev, path);
    const char *node = udev_device_get_devnode(dev);

    if (node) { devNames.emplace_back(node); }

    udev_device_unref(dev);
  }

  udev_enumerate_unref(enumerate);

  return devNames;
}

std::expected<void, std::string> SnippetService::setKeymap(snippet_gen::LayoutInfo info) {
  setLayout(info);
  return {};
}

std::expected<snippet_gen::CreateSnippetResponse, std::string>
SnippetService::createSnippet(snippet_gen::CreateSnippetRequest req) {
  std::cerr << "Created new snippet with trigger " << req.trigger << '\n';
  m_snippets.push_back(Snippet{.trigger = req.trigger, .mode = req.mode});
  std::ranges::sort(m_snippets, [](auto &&a, auto &&b) { return a.trigger.size() > b.trigger.size(); });
  return snippet_gen::CreateSnippetResponse{};
}

std::expected<snippet_gen::RemoveSnippetResponse, std::string>
SnippetService::removeSnippet(snippet_gen::RemoveSnippetRequest req) {
  std::erase_if(m_snippets, [&](auto &&s) { return s.trigger == req.trigger; });
  return snippet_gen::RemoveSnippetResponse{};
}

std::expected<void, std::string> SnippetService::resetContext() {
  m_text.clear();
  return {};
}

void SnippetService::setLayout(const LayoutInfo &info) {
  const auto toPtr = [](const std::optional<std::string> &str) {
    return str.transform([](auto &&str) { return str.c_str(); }).value_or(nullptr);
  };
  const xkb_rule_names rules{.rules = toPtr(info.rules),
                             .model = toPtr(info.model),
                             .layout = info.layout.c_str(),
                             .variant = toPtr(info.variant),
                             .options = toPtr(info.options)};
  auto keymap = xkb_keymap_new_from_names(m_xkb, &rules, XKB_KEYMAP_COMPILE_NO_FLAGS);
  auto state = xkb_state_new(keymap);

  xkb_state_unref(m_kbState);
  xkb_keymap_unref(m_keymap);
  m_keymap = keymap;
  m_kbState = state;
  std::cerr << "changed keyboard layout to " << rules.layout << '\n';
}

void SnippetService::listen(snippet_gen::Server &rpcServer) {
  const std::error_code ec;

  std::unordered_map<int, Input> inputs;

  int epollfd = epoll_create1(0);

  if (epollfd == -1) {
    std::cerr << "Failed to create epollfd: " << strerror(errno) << '\n';
    return;
  }

  epoll_event ev;

  ev.data.fd = STDIN_FILENO;
  ev.events = EPOLLIN;

  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1) {
    std::cerr << "Failed to add stdin to epoll: " << strerror(errno) << '\n';
    exit(1);
  }

  udev_monitor *mon = udev_monitor_new_from_netlink(m_udev, "udev");

  udev_monitor_filter_add_match_subsystem_devtype(mon, "input", nullptr);
  udev_monitor_enable_receiving(mon);

  const int udevFd = udev_monitor_get_fd(mon);

  if (udevFd == -1) {
    std::cerr << "Failed to get udev monitor fd: " << strerror(errno) << '\n';
    exit(1);
  }

  ev.data.fd = udevFd;
  ev.events = EPOLLIN;

  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1) {
    std::cerr << "Failed to add watch to epoll: " << strerror(errno) << '\n';
    exit(1);
  }

  const auto createInput = [&](const char *device) -> bool {
    int fd = open(device, O_RDONLY);

    if (fd == -1) {
      std::cerr << "Failed to open " << device << ": " << strerror(errno) << '\n';
      return false;
    }

    std::array<char, 256> name{};
    if (ioctl(fd, EVIOCGNAME(name.size()), name.data()) >= 0 &&
        std::string_view(name.data()) == VIRTUAL_KB_NAME) {
      close(fd);
      return false;
    }

    ev.events = EPOLLIN;
    ev.data.fd = fd;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1) {
      std::cerr << "Failed to add fd " << fd << " for " << device << " to epoll: " << strerror(errno) << '\n';
      close(fd);
      return false;
    }

    inputs[fd] = {.fd = fd, .name = device, .ev = ev};
    return true;
  };

  for (const auto &device : enumerateKeyboards()) {
    createInput(device.c_str());
  }

  for (const auto &[fd, data] : inputs) {
    std::cerr << "Input ready: " << data.name << " (fd=" << data.fd << ")\n";
  }

  constexpr const int MAX_EVENTS = 256;
  std::array<epoll_event, MAX_EVENTS> events;

  Frame ipcFrame;

  ipcFrame.setHandler([&](std::string_view message) { rpcServer.route(message); });

  static constexpr int MODIFIER_TIMEOUT_MS = 3000;

  for (;;) {
    const int timeout = m_pendingExpansion ? MODIFIER_TIMEOUT_MS : -1;
    const int nfds = epoll_wait(epollfd, events.data(), events.size(), timeout);

    if (nfds == -1) {
      std::cerr << "Failed to epoll_wait: " << strerror(errno) << '\n';
      exit(1);
    }

    if (nfds == 0 && m_pendingExpansion) {
      std::cerr << "Modifier release timeout, emitting anyway\n";
      flushPendingExpansion();
      continue;
    }

    for (int n = 0; n < nfds; ++n) {
      int fd = events[n].data.fd;

      if (fd == STDIN_FILENO) {
        if (!ipcFrame.readPart(STDIN_FILENO)) { exit(0); }
        continue;
      }

      // hot plug/unplug
      if (fd == udevFd) {
        udev_device *dev = udev_monitor_receive_device(mon);
        const std::string_view action = udev_device_get_action(dev);
        const char *node = udev_device_get_devnode(dev);

        if (!node) continue;

        const char *kb = udev_device_get_property_value(dev, "ID_INPUT_KEYBOARD");

        if (!kb || std::string_view{kb} != "1") continue;

        if (action == "add") {
          std::cerr << "CREATE ev: " << node << '\n';

          if (createInput(node)) {
            std::cerr << "Hot plugged device " << node << '\n';
          } else {
            std::cerr << "Failed to hot plug device " << node << '\n';
          }
        }

        else if (action == "remove") {
          std::cerr << "DELETE ev: " << node << '\n';

          if (auto it = std::ranges::find_if(inputs, [&](auto &&input) { return input.second.name == node; });
              it != inputs.end()) {
            epoll_ctl(epollfd, EPOLL_CTL_DEL, it->second.ev.data.fd, &it->second.ev);
            close(it->second.fd);
            std::cerr << "Removed device " << it->second.name << " (fd=" << it->second.fd << ")\n";
            inputs.erase(it);
          }
        }

        continue;
      }

      auto it = inputs.find(fd);

      if (it == inputs.end()) {
        std::cerr << "fd " << fd << " does not map to any known input, skipping...\n";
        continue;
      }

      input_event ev;
      std::array<char, 8> key;

      const auto rc = read(fd, &ev, sizeof(ev));

      if (rc <= 0) {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, it->second.ev.data.fd, &it->second.ev);
        close(it->second.fd);
        std::cerr << "Removed device " << it->second.name << " (fd=" << it->second.fd << ")\n";
        inputs.erase(it);
        continue;
      }

      if (static_cast<size_t>(rc) < sizeof(ev)) {
        std::cerr << "read of invalid size for event: expected " << sizeof(ev) << ", got " << rc << '\n';
        continue;
      }

      if (ev.type != EV_KEY) { continue; }

      const xkb_keycode_t keycode = ev.code + 8;

      if (ev.value == 0) {
        xkb_state_update_key(m_kbState, keycode, XKB_KEY_UP);
        if (m_pendingExpansion && !hasActiveModifiers()) { flushPendingExpansion(); }
      } else if (ev.value <= 2) {
        const int len = xkb_state_key_get_utf8(m_kbState, keycode, key.data(), key.size());
        const std::string_view keyStr{key.data(), static_cast<size_t>(len)};

        if (ev.value == 1) { xkb_state_update_key(m_kbState, keycode, XKB_KEY_DOWN); }

        if (m_undoTrigger && ev.value == 1) {
          if (ev.code == KEY_BACKSPACE) {
            std::cerr << "SNIPPET UNDO: " << *m_undoTrigger << '\n';
            emitundoSnippet({.trigger = *m_undoTrigger});
            m_undoTrigger.reset();
            continue;
          }
          m_undoTrigger.reset();
        }

        if (!keyStr.empty()) {
          if (ev.code == KEY_BACKSPACE) {
            if (!m_text.empty()) { m_text.erase(m_text.end() - 1); }
          } else if (std::isprint(keyStr.at(0))) {
            m_text.append(keyStr);
            if (m_text.size() > MAX_BUFFER_SIZE) { m_text.erase(0, m_text.size() - MAX_BUFFER_SIZE); }
          }
        }

        const bool wordSep = !keyStr.empty() && isWordSeparator(keyStr.at(0));

        for (const auto &snippet : m_snippets) {
          if (snippet.mode == snippet_gen::ExpansionMode::Keydown) {
            if (snippet.trigger.size() > m_text.size()) continue;
            if (m_text.ends_with(snippet.trigger)) {
              emitExpansion(snippet);
              break;
            }
          } else if (wordSep) {
            if (snippet.trigger.size() + 1 > m_text.size()) continue;
            if (std::string_view(m_text).substr(0, m_text.size() - 1).ends_with(snippet.trigger)) {
              emitExpansion(snippet);
              break;
            }
          }
        }
      }
    }
  }
}

bool SnippetService::hasActiveModifiers() const {
  return xkb_state_mod_names_are_active(m_kbState, XKB_STATE_MODS_DEPRESSED, XKB_STATE_MATCH_ANY,
                                        XKB_MOD_NAME_SHIFT, XKB_MOD_NAME_CTRL, XKB_MOD_NAME_ALT,
                                        XKB_MOD_NAME_LOGO, nullptr) > 0;
}

void SnippetService::flushPendingExpansion() {
  if (!m_pendingExpansion) return;
  std::cerr << "SNIPPET EMITTING (mods released): " << m_pendingExpansion->trigger << '\n';
  emittriggerSnippet({.trigger = m_pendingExpansion->trigger});
  m_undoTrigger = m_pendingExpansion->trigger;
  m_pendingExpansion.reset();
}

void SnippetService::emitExpansion(const Snippet &snippet) {
  std::cerr << "SNIPPET TRIGGERED: " << snippet.trigger << '\n';
  m_text.clear();

  if (hasActiveModifiers()) {
    std::cerr << "Deferring expansion until modifiers are released\n";
    m_pendingExpansion = snippet;
    return;
  }

  emittriggerSnippet({.trigger = snippet.trigger});
  m_undoTrigger = snippet.trigger;
}

}; // namespace snippet
