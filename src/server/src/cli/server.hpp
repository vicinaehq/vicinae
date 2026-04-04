#pragma once
#include <filesystem>
#include <string>

struct ServerLaunchOptions {
  bool open = false;
  bool noExtensionRuntime = false;
  std::string config;
};

int startServer(const ServerLaunchOptions &opts);
