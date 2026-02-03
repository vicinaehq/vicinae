#include <cassert>
#include <cstdint>
#include <cstring>
#include <glaze/json.hpp>
#include "poll.h"
#include "vicinae-ipc/ipc.hpp"
#include "vicinae-ipc/client.hpp"
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <iostream>
#include <glaze/glaze.hpp>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <variant>

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

using ExtensionRpcSchema = ipc::RpcSchema<ipc::FocusTab, ipc::BrowserInit>;

static void sendExtensionCommand(int fd, std::string_view message) {
  uint32_t size = message.size();
  write(fd, reinterpret_cast<char *>(&size), sizeof(size));
  write(fd, message.data(), message.size());
}

static int entrypoint() {
  if (isatty(STDIN_FILENO)) {
    std::println(
        std::cerr,
        "This executable is meant to be started by the vicinae browser extension using native host messaging "
        "(https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/Native_messaging). Do not "
        "start manually.");
    return 1;
  }

  auto vicinaeClient = ipc::Client<ipc::BrowserExtensionSchema>::make();

  if (!vicinaeClient) {
    std::println(std::cerr, "Failed to create vicinae client");
    return false;
  }

  if (auto result = vicinaeClient->connect(); !result) {
    std::println(std::cerr, "Failed to connect: {}", result.error());
    return 1;
  }

  ipc::RpcClient<ExtensionRpcSchema> extensionClient;

  ipc::RpcServer<ipc::RpcSchema<ipc::FocusTab, ipc::CloseBrowserTab>>
      vicinaeReceiver; // listen to vicinae requests
  ipc::RpcServer<ipc::RpcSchema<ipc::BrowserInit, ipc::BrowserTabsChanged>>
      extensionReceiver; // listen to browser extension requests

  vicinaeReceiver.route<ipc::FocusTab>([](const ipc::FocusTab::Request &req) {
    ipc::RpcClient<decltype(vicinaeReceiver)::SchemaType> client;
    sendExtensionCommand(STDOUT_FILENO, client.notify<ipc::FocusTab>(req));
    return ipc::FocusTab::Response();
  });

  vicinaeReceiver.route<ipc::CloseBrowserTab>([](const ipc::CloseBrowserTab::Request &req) {
    ipc::RpcClient<decltype(vicinaeReceiver)::SchemaType> client;
    std::println(std::cerr, "close tab event gotten");
    sendExtensionCommand(STDOUT_FILENO, client.notify<ipc::CloseBrowserTab>(req));
    return ipc::CloseBrowserTab::Response();
  });

  extensionReceiver.route<ipc::BrowserInit>(
      [&](const ipc::BrowserInit::Request &init) { return vicinaeClient->request<ipc::BrowserInit>(init); });

  extensionReceiver.route<ipc::BrowserTabsChanged>(
      [&](const ipc::BrowserTabsChanged::Request &req)
          -> std::expected<ipc::BrowserTabsChanged::Response, std::string> {
        // pass tab changes to vicinae
        if (const auto result = vicinaeClient->notify<ipc::BrowserTabsChanged>(req); !result) {
          return std::unexpected(result.error());
        }

        return ipc::BrowserTabsChanged::Response();
      });

  Frame extensionFrame;

  extensionFrame.setHandler([&](std::string_view message) {
    using Req = decltype(extensionReceiver)::SchemaType::Request;
    using Res = decltype(extensionClient)::Schema::Response;

    std::variant<Req, Res> msg;

    std::println(std::cerr, "got message from extension frame: {}", message);

    if (const auto error = glz::read_json(msg, message)) {
      std::println(std::cerr, "\"{}\"", message);
      std::println(std::cerr, "Failed to parse json: {}", glz::format_error(error));
      return;
    }

    if (Req *req = std::get_if<Req>(&msg)) {
      const auto callRes = extensionReceiver.call(*req);

      if (!callRes) {
        std::println(std::cerr, "Failed to call extension rpc: {}", callRes.error());
        return;
      }

      std::println(std::cerr, "response => {}", callRes.value());

      if (req->id) { sendExtensionCommand(STDOUT_FILENO, callRes.value()); }
    } else if (auto res = std::get_if<Res>(&msg)) {
      if (const auto result = extensionClient.call(*res); !result) {
        std::println(std::cerr, "Failed to process extension response");
      }
    }

    std::println(std::cerr, "Processing message: {}", message);
  });

  Frame vicinaeFrame;

  vicinaeFrame.setHandler([&](std::string_view message) {
    assert(!message.empty());
    decltype(vicinaeReceiver)::SchemaType::Request req;

    std::println(std::cerr, "got message from vicinae frame: {}", message);

    if (const auto error = glz::read_json(req, message)) {
      std::println(std::cerr, "\"{}\"", message);
      std::println(std::cerr, "Failed to parse json: {}", glz::format_error(error));
      return;
    }

    const auto callRes = vicinaeReceiver.call(req);

    if (!callRes) {
      std::println(std::cerr, "Failed to call extension rpc: {}", callRes.error());
      return;
    }

    std::println(std::cerr, "Response: {}", callRes.value());
  });

  auto fds = std::array{pollfd(STDIN_FILENO, POLLIN), pollfd(vicinaeClient->handle(), POLLIN)};

  assert(fds.size() == 2);
  std::println(std::cerr, "Listening for messages...");

  while (true) {
    int ready = poll(fds.data(), fds.size(), -1);

    if (ready == -1) {
      std::println(std::cerr, "poll() failed: {} (exiting)", strerror(errno));
      break;
    }

    // read extension stuff
    if (fds[0].revents & POLLIN) {
      if (!extensionFrame.readPart(fds[0].fd)) {
        std::println(std::cerr, "extension input closed, exiting...");
        break;
      }
    }

    // take vicinae requests/response
    if (fds[1].revents & POLLIN) {
      if (!vicinaeFrame.readPart(fds[1].fd)) {
        std::println(std::cerr, "The connection to the vicinae dameon broke, exiting...");
        break;
      }
    }
  }

  std::println(std::cerr, "Native host is exiting.");

  return 0;
}

int main(int ac, char **av) { entrypoint(); }
