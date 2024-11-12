#include "extension_manager.hpp"
#include <QUuid>
#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <jsoncpp/json/reader.h>
#include <sys/socket.h>
#include <sys/un.h>

ExtensionManager::ExtensionManager(
    const std::vector<ConfigExtension> &confExts) {
  for (const auto &ext : confExts) {
    Extension extension;

    extension.name = ext.name;
    extension.startedAt = time(0);
    extension.status = ExtensionStatus::RUNNING;
    extension.token = QUuid::createUuid().toString().toStdString();
    extension.exec = ext.exec;

    extensions.push_back(extension);
  }
}

static char **vectorToArgv(const std::vector<std::string> &ss) {
  char **argv = new char *[ss.size() + 1];

  for (size_t i = 0; i != ss.size(); ++i)
    argv[i] = strdup(ss.at(i).c_str());

  argv[ss.size()] = 0;

  return argv;
}

void ExtensionManager::startServer() {
  fd_set readSet = {0}, writeSet = {0};
  int daemonSock = socket(AF_UNIX, SOCK_STREAM, 0);

  fcntl(daemonSock, F_SETFD, fcntl(daemonSock, F_GETFD) | O_NONBLOCK);
  FD_SET(daemonSock, &readSet);

  unlink("daemon.sock");
  struct sockaddr_un saddr = {AF_UNIX, "daemon.sock"};

  if (bind(daemonSock, (const struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
    perror("Failed to bind");
  }

  if (listen(daemonSock, 1024) == -1) {
    perror("Failed to listen");
  }

  for (auto &extension : extensions) {
    activeId = extension.token;
    int pipefd[2];

    if (pipe2(pipefd, 0) == -1) {
      perror("Failed to pipe()");
    }

    extension.proc.pid = fork();

    if (extension.proc.pid == 0) {
      close(pipefd[0]);
      auto argv = vectorToArgv(extension.exec);

      setenv("TOKEN", extension.token.c_str(), 1);
      setenv("ENDPOINT",
             (std::filesystem::current_path() / "daemon.sock").c_str(), 1);

      /*
  if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
    perror("Failed to dup2 stdout:");
  }
      */

      if (execvp(argv[0], argv) == -1) {
        perror("Failed to execvp extension:");
      }

      extension.status = ExtensionStatus::CRASHED;
    }

    close(pipefd[1]);
    extension.proc.in = pipefd[0];
    FD_SET(pipefd[0], &readSet);
  }

  char buf[102400];
  bool isRunning = true;

  std::vector<Connection> connections;
  std::map<int, Extension *> dataFdToExtension;

  while (isRunning) {
    fd_set readFds = readSet;
    fd_set writeFds = writeSet;

    // std::cout << "selecting..." << std::endl;
    if (select(1024, &readFds, nullptr, nullptr, nullptr) != -1) {
      // std::cout << "selected..." << std::endl;
      //  accept new connection on socket
      if (FD_ISSET(daemonSock, &readFds)) {
        int client = accept(daemonSock, nullptr, nullptr);

        if (client <= 0) {
          std::cout << "Failed to accept" << std::endl;
          continue;
        }

        // std::cout << "new connection" << std::endl;

        connections.push_back({.createdAt = time(0), .fd = client});
        FD_SET(client, &readSet);
      }

      for (const auto &conn : connections) {
        if (FD_ISSET(conn.fd, &readFds)) {
          // std::cout << conn.fd << " wants to read" << std::endl;
          uint32_t mlen = 0;
          int rc = read(conn.fd, &mlen, sizeof(mlen));

          if (rc < sizeof(mlen)) {
            // std::cout << "Failed to read message length!" << std::endl;
            continue;
          }

          // std::cout << "message of length=" << mlen << std::endl;

          rc = read(conn.fd, buf, mlen);

          if (rc < mlen) {
            // std::cout << "Failed to read message of mlen length!" <<
            // std::endl;
            continue;
          }

          buf[rc] = 0;

          Json::Reader reader;
          Json::Value value;

          reader.parse(buf, value);

          std::string type = value["type"].asString();
          Json::Value data = value["data"];

          if (type == "echo") {
            if (auto it = dataFdToExtension.find(conn.fd);
                it != dataFdToExtension.end()) {
              std::cout << "extension " << it->first << " says: " << "\""
                        << data["message"].asString() << "\"" << std::endl;
            }
          }

          if (type == "render") {
            if (auto it = dataFdToExtension.find(conn.fd);
                it != dataFdToExtension.end()) {
              // std::cout << "extension " << it->first << "renders." <<
              // std::endl;
              /*
  std::cout << "token=" << it->second->token << ", " << activeId
            << std::endl;
            */

              if (it->second->token == activeId) {
                std::cout << ">>>>>>>>>>>>>>>>> RENDER <<<<<<<<<<<<<<<"
                          << std::endl;
                emit render(data["root"]);
              }
            }
          }

          if (type == "register") {
            std::string token = data["token"].asString();

            for (auto &ext : extensions) {
              if (ext.token == token) {
                ext.connection = conn;
                dataFdToExtension[conn.fd] = &ext;
              }
            }

            std::cout << "token: " << token << std::endl;
          }
        }
      }
    }
  }
}
