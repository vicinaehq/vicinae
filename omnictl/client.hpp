#pragma once

#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include "proto.hpp"
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>

struct CommandError {
  std::string message;
};

class CommandClient {
  int _fd;
  char _buf[8096];

public:
  CommandClient() {}

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
      std::cout << "failed to connect";
      return false;
    }

    return true;
  }

  std::variant<Proto::Variant, CommandError> request(const std::string &type, const Proto::Array &params) {
    Proto::Array message({type, Proto::Boolean(true)});
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
      std::cout << "rc" << rc << std::endl;
      packet.insert(packet.end(), _buf, _buf + rc);
    }

    if (packet.size() < sizeof(uint32_t)) { throw std::runtime_error("packet too small"); }

    uint32_t length = ntohl(*reinterpret_cast<uint32_t *>(packet.data()));

    std::cout << "answer of length" << length << std::endl;
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
