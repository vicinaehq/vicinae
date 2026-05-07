#pragma once
#include "common/types.hpp"
#include "http-client.hpp"
#include <QObject>
#include <QTimer>
#include <filesystem>
#include <string>
#include <vector>

namespace config {
class Manager;
}

class TelemetryService : public QObject, NonCopyable {
  struct State {
    std::string userId;
    std::optional<std::uint64_t> systemInfoLastSentAt;
  };

  struct ScreenInfo {
    struct {
      int width;
      int height;
    } resolution;
    double scale = 1;
  };

  struct ForgetRequest {
    std::string userId;
  };

  struct ForgetResponse {
    std::string message;
  };

  struct SystemInfoRequest {
    std::string userId;
    std::vector<std::string> desktops;
    std::string vicinaeVersion;
    std::string qtVersion;
    std::string displayProtocol;
    std::string architecture;
    std::string operatingSystem;
    std::string buildProvenance;
    std::string locale;
    std::vector<ScreenInfo> screens;
    std::string chassisType;
    std::string kernelVersion;
    std::string productId;
    std::string productVersion;
  };

  struct SystemInfoResponse {
    bool ok = true;
  };

public:
  explicit TelemetryService(config::Manager &config);

  void setEnabled(bool enabled);
  void trySendSystemInfo();

  /**
   * Request that all data linked to the current vicinae user id is fully anonymized (strips userId from
   * records)
   */
  QFuture<bool> forget();

private:
  // productId == distribution on linux
  std::string determineProductId() const;

  static std::string toLower(const std::string &s);
  std::string generateUserId();
  void sendSystemInfo();
  void saveState();
  void loadState();

  config::Manager &m_config;
  State m_state;
  http::Client m_client;
  QTimer m_timer;
  std::filesystem::path m_statePath;
  std::string m_buf;
};
