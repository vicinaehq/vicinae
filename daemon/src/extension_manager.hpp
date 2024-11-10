#pragma once
#include "config.hpp"
#include <QObject>
#include <ctime>
#include <iostream>
#include <jsoncpp/json/value.h>
#include <optional>
#include <qobject.h>
#include <qtmetamacros.h>
#include <string>
#include <sys/socket.h>
#include <vector>

enum ExtensionStatus {
  RUNNING,
  CRASHED,
};

struct Connection {
  time_t createdAt;
  int fd;
};

struct Extension {
  std::string name;
  std::vector<std::string> exec;
  time_t startedAt;
  struct {
    pid_t pid;
    int in;
    int out;
  } proc;
  std::optional<Connection> connection;
  ExtensionStatus status;
  std::string token;
};

class ExtensionManager : public QObject {
  Q_OBJECT

  std::vector<Extension> extensions;
  std::string activeId;

public:
  ExtensionManager(const std::vector<ConfigExtension> &confExts);

  void startServer();

  template <class T>
  void handler(std::string type, std::string id, T const &payload) {
    std::cout << type << ": " << payload << std::endl;
    for (const auto &ext : extensions) {
      if (ext.token != activeId)
        continue;

      Json::Value root;
      Json::Value data;

      data["type"] = type;
      data["handlerId"] = id;
      data["value"] = payload;

      root["type"] = "event";
      root["data"] = data;

      std::string jsonstring = root.toStyledString();

      uint32_t n = jsonstring.size();
      std::cout << "sending " << jsonstring << " bytes=" << n << std::endl;

      send(ext.connection->fd, &n, sizeof(n), 0);
      send(ext.connection->fd, jsonstring.data(), jsonstring.size(), 0);
    }
  }

signals:
  void render(Json::Value root);
};
