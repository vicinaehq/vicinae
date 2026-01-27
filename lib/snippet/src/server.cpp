#include "snippet/snippet.hpp"
#include "vicinae-ipc/ipc.hpp"
#include <cstdint>
#include <cstring>
#include <glaze/json/read.hpp>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <xkbcommon/xkbcommon.h>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>

namespace fs = std::filesystem;

#ifdef NDEBUG
#define READ_SIZE 8096
#else
#define READ_SIZE 1
#endif

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
  std::array<char, READ_SIZE> m_buf;
  std::string data;
  uint32_t size = 0;
  Handler m_fn;
};

namespace snippet {

struct Input {
  int fd = -1;
  std::string name;
};

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

  for (const auto &entry : fs::directory_iterator("/dev/input", ec)) {
    if (entry.is_other()) {
      const auto &pathStr = entry.path().string();
      if (entry.path().filename().string().starts_with("event")) {
        int fd = open(entry.path().c_str(), O_RDONLY);

        if (fd == -1) {
          std::println(std::cerr, "Failed to open {}: {}", entry.path().c_str(), strerror(errno));
          continue;
        }

        ev.events = EPOLLIN;
        ev.data.fd = fd;

        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1) {
          std::println(std::cerr, "Failed to add fd {} for {} to epoll: {}", fd, entry.path().c_str(),
                       strerror(errno));
          close(fd);
          continue;
        }

        inputs[fd] = {.fd = fd, .name = entry.path()};
      }
    }
  }

  for (const auto &[fd, data] : inputs) {
    std::println(std::cerr, "Input ready: {} (fd={})", data.name, data.fd);
  }

  constexpr const int MAX_EVENTS = 256;
  std::array<epoll_event, MAX_EVENTS> events;
  xkb_context *ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

  xkb_rule_names rules{.rules = nullptr,   // defaults to "evdev"
                       .model = nullptr,   // defaults to "pc105"
                       .layout = "us",     // or "fr", "de", etc.
                       .variant = nullptr, // e.g. "azerty" for French
                       .options = nullptr};

  struct xkb_keymap *keymap = xkb_keymap_new_from_names(ctx, &rules, XKB_KEYMAP_COMPILE_NO_FLAGS);
  struct xkb_state *state = xkb_state_new(keymap);

  Frame ipcFrame;

  ::ipc::RpcServer<snippet::ipc::ClientSchema> server; // vicinae -> snippet requests
  ::ipc::RpcClient<snippet::ipc::ServerSchema> client; // snippet -> vicinae requests/notifications

  server.route<snippet::ipc::CreateSnippet>([](const snippet::ipc::CreateSnippet::Request &req) {
    std::println(std::cerr, "Created new snippet with trigger {}", req.trigger);
    return snippet::ipc::CreateSnippet::Response();
  });

  server.route<snippet::ipc::RemoveSnippet>([](const snippet::ipc::RemoveSnippet::Request &req) {
    return snippet::ipc::RemoveSnippet::Response();
  });

  ipcFrame.setHandler([&](std::string_view message) {
    using Req = decltype(server)::SchemaType::Request;
    using Res = decltype(client)::Schema::Response;

    std::variant<Req, Res> v;
    if (const auto error = glz::read_json(v, message)) {
      std::println(std::cerr, "Failed to parse message: {}", glz::format_error(error));
      return;
    }

    if (auto it = std::get_if<Req>(&v)) {
      const auto res = server.call(*it);

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
      auto _ = client.call(*it);
      return;
    }
  });

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

      if (ev.type != EV_KEY) {
        // std::println(std::cerr, "Reading key input {} (fd={}) code={}", it->second.name, fd, ev.code);
        continue;
      }

      xkb_keycode_t keycode = ev.code + 8;
      std::array<char, 32> key;
      int len = xkb_state_key_get_utf8(state, keycode, key.data(), key.size());
      std::string_view keyStr{key.data(), static_cast<size_t>(len)};

      std::println(std::cerr, "len={}", len);

      if (ev.value == 0) {
        xkb_state_update_key(state, keycode, XKB_KEY_DOWN);
      } else if (ev.value == 1) {
        xkb_state_update_key(state, keycode, XKB_KEY_UP);
      }

      std::println(std::cerr, "key={} ({})", keyStr, ev.value == 0 ? "down" : "up");
    }
  }
}

}; // namespace snippet
