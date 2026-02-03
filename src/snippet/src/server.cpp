#include "snippet/snippet.hpp"
#include "vicinae-ipc/ipc.hpp"
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <glaze/json/read.hpp>
#include <linux/input-event-codes.h>
#include <expected>
#include <linux/input.h>
#include <libudev.h>
#include <linux/uinput.h>
#include <string_view>
#include <xkbcommon/xkbcommon.h>
#include <fcntl.h>
#include <iostream>
#include <ostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>

static auto lastExpansionTime = std::chrono::steady_clock::now();

/**
 * Parse message as data gets in, and call the provided callback once a full message
 * has been parsed.
 */
class Frame {
public:
  using Handler = std::function<void(std::string_view message)>;

  void setHandler(Handler fn) { m_fn = fn; }

  bool readPart(int fd) {
    int rc = read(fd, m_buf.data(), m_buf.size());

    if (rc == -1) {
      std::println(std::cerr, "Failed to read fd {}: {}", fd, strerror(errno));
      return false;
    }

    if (rc == 0) { return false; }

    data.append(std::string_view(m_buf.data(), rc));

    for (;;) {
      if (size == 0 && data.size() >= sizeof(size)) {
        size = *reinterpret_cast<decltype(size) *>(data.data());
        data.erase(data.begin(),
                   data.begin() + sizeof(size)); // we need a better way than this, this is slow
      }

      if (size && size <= data.size()) {
        std::string_view view(data);
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

Server::Server() : m_udev(udev_new()), m_xkb(xkb_context_new(XKB_CONTEXT_NO_FLAGS)) {
  static constexpr const xkb_rule_names rules{.rules = nullptr,   // defaults to "evdev"
                                              .model = nullptr,   // defaults to "pc105"
                                              .layout = "us",     // or "fr", "de", etc.
                                              .variant = nullptr, // e.g. "azerty" for French
                                              .options = nullptr};

  m_keymap = xkb_keymap_new_from_names(m_xkb, &rules, XKB_KEYMAP_COMPILE_NO_FLAGS);
  m_kbState = xkb_state_new(m_keymap);
  setupIPC();
}

Server::~Server() { udev_unref(m_udev); }

std::vector<std::string> Server::enumerateKeyboards() {
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

void Server::setupIPC() {
  m_server.route<snippet::ipc::SetKeymap>([this](const snippet::ipc::SetKeymap::Request &req) {
    setLayout(req);
    return snippet::ipc::SetKeymap::Response();
  });

  m_server.route<snippet::ipc::CreateSnippet>([this](const snippet::ipc::CreateSnippet::Request &req) {
    std::println(std::cerr, "Created new snippet with trigger {}", req.trigger);
    m_snippetMap[req.trigger] = Snippet(req.trigger, req.mode);
    return snippet::ipc::CreateSnippet::Response();
  });

  m_server.route<snippet::ipc::RemoveSnippet>([this](const snippet::ipc::RemoveSnippet::Request &req) {
    m_snippetMap.erase(req.trigger);
    return snippet::ipc::RemoveSnippet::Response();
  });

  m_server.route<snippet::ipc::InjectClipboardExpansion>(
      [this](const snippet::ipc::InjectClipboardExpansion::Request &req)
          -> std::expected<snippet::ipc::InjectClipboardExpansion::Response, std::string> {
        const auto it = m_snippetMap.find(req.trigger);

        if (it == m_snippetMap.end()) {
          std::println(std::cerr, "Could not find snippet with trigger {}", req.trigger);
          return std::unexpected("No such snippet");
        }

        int mods = UInputKeyboard::Modifier::MOD_CTRL;
        std::println(std::cerr, "Received expansion request for snippet {} (took {}ms)", req.trigger,
                     std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                                           lastExpansionTime)
                         .count());

        if (req.terminal) mods |= UInputKeyboard::Modifier::MOD_SHIFT;

        m_kb.repeatKey(KEY_BACKSPACE, req.trigger.size());
        usleep(2000);
        m_kb.sendKey(KEY_V, mods);

        if (it->second.mode == ipc::ExpansionMode::Word) {
          usleep(2000);
          m_kb.sendKey(KEY_SPACE, 0);
        }

        return snippet::ipc::InjectClipboardExpansion::Response();
      });
}

void Server::setLayout(const LayoutInfo &info) {
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
  std::println(std::cerr, "changed keyboard layout to {}", rules.layout);
}

void Server::listen() {
  std::error_code ec;

  std::unordered_map<int, Input> inputs;

  int epollfd = epoll_create1(0);

  if (epollfd == -1) {
    std::println(std::cerr, "Failed to create epollfd: {}", strerror(errno));
    return;
  }

  epoll_event ev;

  ev.data.fd = STDIN_FILENO;
  ev.events = EPOLLIN;

  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1) {
    std::println(std::cerr, "Failed to add stdin to epoll: {}", strerror(errno));
    exit(1);
  }

  udev_monitor *mon = udev_monitor_new_from_netlink(m_udev, "udev");

  udev_monitor_filter_add_match_subsystem_devtype(mon, "input", nullptr);
  udev_monitor_enable_receiving(mon);

  int udevFd = udev_monitor_get_fd(mon);

  if (udevFd == -1) {
    std::println(std::cerr, "Failed to create inotify file descriptor", strerror(errno));
    exit(1);
  }

  ev.data.fd = udevFd;
  ev.events = EPOLLIN;

  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1) {
    std::println(std::cerr, "Failed to add watch to epoll: {}", strerror(errno));
    exit(1);
  }

  const auto createInput = [&](const char *device) -> bool {
    int fd = open(device, O_RDONLY);

    if (fd == -1) {
      std::println(std::cerr, "Failed to open {}: {}", device, strerror(errno));
      return false;
    }

    ev.events = EPOLLIN;
    ev.data.fd = fd;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1) {
      std::println(std::cerr, "Failed to add fd {} for {} to epoll: {}", fd, device, strerror(errno));
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
    std::println(std::cerr, "Input ready: {} (fd={})", data.name, data.fd);
  }

  constexpr const int MAX_EVENTS = 256;
  std::array<epoll_event, MAX_EVENTS> events;

  auto lastKeyTime = std::chrono::steady_clock::now();

  Frame ipcFrame;

  ipcFrame.setHandler([&](std::string_view message) {
    using Req = decltype(m_server)::SchemaType::Request;
    using Res = decltype(m_client)::Schema::Response;

    std::variant<Req, Res> v;
    if (const auto error = glz::read_json(v, message)) {
      std::println(std::cerr, "Failed to parse message: {}", glz::format_error(error));
      return;
    }

    if (auto it = std::get_if<Req>(&v)) {
      const auto res = m_server.call(*it);

      if (!res) {
        std::println(std::cerr, "Request failed: {}", res.error());
        return;
      }

      std::string data = std::move(res).value();
      uint32_t size = data.size();

      write(STDOUT_FILENO, reinterpret_cast<const char *>(&size), sizeof(size));
      write(STDOUT_FILENO, data.data(), data.size());

      return;
    }

    if (auto it = std::get_if<Res>(&v)) {
      auto _ = m_client.call(*it);
      return;
    }
  });

  setLayout({.layout = "us"});

  for (;;) {
    int nfds = epoll_wait(epollfd, events.data(), events.size(), -1);

    if (nfds == -1) {
      std::println(std::cerr, "Failed to epoll_wait: {}", strerror(errno));
      exit(1);
    }

    for (int n = 0; n < nfds; ++n) {
      int fd = events[n].data.fd;

      if (fd == STDIN_FILENO) {
        ipcFrame.readPart(STDIN_FILENO);
        continue;
      }

      // hot plug/unplug
      if (fd == udevFd) {
        udev_device *dev = udev_monitor_receive_device(mon);
        std::string_view action = udev_device_get_action(dev);
        const char *node = udev_device_get_devnode(dev);

        if (!node) continue;

        const char *kb = udev_device_get_property_value(dev, "ID_INPUT_KEYBOARD");

        if (!kb || std::string_view{kb} != "1") continue;

        if (action == "add") {
          std::println(std::cerr, "CREATE ev: {}", node);

          if (createInput(node)) {
            std::println(std::cerr, "Hot plugged device {}", node);
          } else {
            std::println(std::cerr, "Failed to hot plug device {}", node);
          }
        }

        else if (action == "remove") {
          std::println(std::cerr, "DELETE ev: {}", node);

          if (auto it = std::ranges::find_if(inputs, [&](auto &&input) { return input.second.name == node; });
              it != inputs.end()) {
            epoll_ctl(epollfd, EPOLL_CTL_DEL, it->second.ev.data.fd, &it->second.ev);
            close(it->second.fd);
            std::println(std::cerr, "Removed device {} (fd={})", it->second.name, it->second.fd);
            inputs.erase(it);
          }
        }

        continue;
      }

      auto it = inputs.find(fd);

      if (it == inputs.end()) {
        std::println(std::cerr, "fd {} does not map to any known input, skipping...", fd);
        continue;
      }

      int rc = 0;
      input_event ev;

      // TODO: handle this more gracefully to allow any read size and batch?
      if ((rc = read(fd, &ev, sizeof(ev))) < sizeof(ev)) {
        std::println(std::cerr, "read of invalid size for event: expected {}, got {}", sizeof(ev), rc);
        continue;
      }

      if (rc <= 0) {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, it->second.ev.data.fd, &it->second.ev);
        close(it->second.fd);
        std::println(std::cerr, "Removed device {} (fd={})", it->second.name, it->second.fd);
        inputs.erase(it);
        continue;
      }

      if (ev.type != EV_KEY) {
        // std::println(std::cerr, "Reading key input {} (fd={}) code={}", it->second.name, fd, ev.code);
        continue;
      }

      xkb_keycode_t keycode = ev.code + 8;

      if (ev.value == 0) {
        xkb_state_update_key(m_kbState, keycode, XKB_KEY_UP);
      } else if (ev.value <= 2) {
        const auto now = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastKeyTime).count();

        if (elapsed > 1000) { m_text.clear(); }

        lastKeyTime = now;

        std::array<char, 32> key;
        int len = xkb_state_key_get_utf8(m_kbState, keycode, key.data(), key.size());
        std::string_view keyStr{key.data(), static_cast<size_t>(len)};

        if (ev.value == 1) { xkb_state_update_key(m_kbState, keycode, XKB_KEY_DOWN); }

        if (!keyStr.empty()) {

          if (ev.code == KEY_BACKSPACE) {
            if (!m_text.empty()) { m_text.erase(m_text.end() - 1); }
          } else if (std::isprint(keyStr.at(0)) && !std::isspace(keyStr.at(0))) {
            m_text.append(keyStr);
          }
        }

        std::println(std::cerr, "text='{}'", m_text);

        const auto it = m_snippetMap.find(m_text);

        if (it != m_snippetMap.end()) {
          switch (it->second.mode) {
          case ipc::ExpansionMode::Keydown:
            emitExpansion(it->second);
            break;
          case ipc::ExpansionMode::Word:
            if (ev.code == KEY_SPACE || ev.code == KEY_ENTER) {
              m_kb.sendKey(KEY_BACKSPACE, 0);
              emitExpansion(it->second);
            }
            break;
          }
        }

        if (ev.code == KEY_SPACE) { m_text.clear(); }
      }
    }
  }
}

void Server::emitExpansion(const Snippet &snippet) {
  std::println(std::cerr, "SNIPPET EXPANDED: {}", snippet.trigger);
  lastExpansionTime = std::chrono::steady_clock::now();
  notify<ipc::TriggerSnippet>({.trigger = snippet.trigger});
  m_text.clear();
}

}; // namespace snippet
