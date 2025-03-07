#pragma once

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <cstring>
#include <fcntl.h>
#include "proto.hpp"
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <variant>

struct CommandError {
  std::string message;
};

class CommandResponse {
  std::variant<Proto::Variant, CommandError> _value;

public:
  operator bool() { return isOk(); }
  bool isOk() { return std::holds_alternative<Proto::Variant>(_value); }
  Proto::Variant value() { return isOk() ? std::get<Proto::Variant>(_value) : Proto::Variant(); }

  CommandResponse(const Proto::Variant &variant) : _value(variant) {}
  CommandResponse(const CommandError &error) : _value(error) {}
};

class CommandClient {
  int _fd;
  char _buf[8096];

public:
  CommandClient() : _fd(-1) {}
  ~CommandClient() { close(_fd); }

  bool connect(const std::string &localPath) {
    _fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (_fd < 0) {
      std::cout << "Failed to create socket:" << strerror(errno);
      return false;
    }

    // Setup address structure
    struct sockaddr_un serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, localPath.c_str(), sizeof(serverAddr.sun_path) - 1);

    if (::connect(_fd, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
      close(_fd);
      return false;
    }

    return true;
  }

  bool connect() {
    std::vector<std::filesystem::path> dirs;

    if (auto p = getenv("XDG_RUNTIME_DIR")) { dirs.push_back(p); }

    dirs.push_back("/tmp");

    for (const auto &dir : dirs) {
      auto localPath = dir / "omnicast.sock";

      if (connect(localPath)) { return true; }
    }

    return false;
  }

  static CommandResponse oneshot(const std::string &type, const Proto::Variant &data = {}) {
    CommandClient client;

    if (!client.connect()) {
      throw std::runtime_error("Failed to connect to omnicast daemon. Is omnicast running?");
    }

    return client.request(type, data);
  }

  CommandResponse request(const std::string &type, const Proto::Variant &data = {}) {
    Proto::Array message({type, data});
    Proto::Marshaler marshaler;

    {
      std::vector<uint8_t> frame;
      auto packet = marshaler.marshal(message);
      uint32_t length = packet.size();

      frame.push_back((length >> 24) & 0xFF);
      frame.push_back((length >> 16) & 0xFF);
      frame.push_back((length >> 8) & 0xFF);
      frame.push_back(length & 0xFF);
      frame.insert(frame.end(), packet.begin(), packet.end());

      if (send(_fd, frame.data(), frame.size(), 0) < 0) { throw std::runtime_error("Failed to send"); }
    }

    std::vector<uint8_t> packet;
    int rc = 0;

    while ((rc = recv(_fd, _buf, sizeof(_buf), 0)) > 0) {
      packet.insert(packet.end(), _buf, _buf + rc);
    }

    if (packet.size() < sizeof(uint32_t)) { throw std::runtime_error("packet too small"); }

    uint32_t length = ntohl(*reinterpret_cast<uint32_t *>(packet.data()));

    // std::cout << "answer of length" << length << std::endl;
    auto result = marshaler.unmarshal<Proto::Array>({packet.begin() + sizeof(uint32_t), packet.end()});

    if (auto err = std::get_if<Proto::Marshaler::Error>(&result)) {
      throw std::runtime_error("failed to unmarshal: " + err->message);
    }

    auto &arr = std::get<Proto::Array>(result);

    if (arr.empty()) { throw std::runtime_error("array empty"); }

    if (!arr.at(0).isInt()) { throw std::runtime_error("expected int as first item"); }

    auto status = arr.at(0).asInt();

    if (status != 0) { return CommandError{arr.at(1).asString()}; }

    return arr.at(1);
  }
};
