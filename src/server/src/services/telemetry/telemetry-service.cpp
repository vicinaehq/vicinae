#include "telemetry-service.hpp"
#include "config/config.hpp"
#include "version.h"
#include "vicinae.hpp"
#include "xdgpp/env/env.hpp"
#include <QGuiApplication>
#include <QLocale>
#include <QScreen>
#include <QSysInfo>
#include <QUuid>
#include <algorithm>
#include <cctype>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <ranges>

TelemetryService::TelemetryService(config::Manager &config) : m_config(config) {
  m_client.setBaseUrl(Omnicast::API_URL);
  m_statePath = Omnicast::stateDir() / "telemetry.json";
  loadState();
}

void TelemetryService::trySendSystemInfo() {
  if (!m_config.value().telemetry.systemInfo) return;

  static constexpr const std::uint64_t ONE_DAY_SECS = 86400;

  auto const now = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());
  bool const shouldSend = !m_state.systemInfoLastSentAt.has_value() ||
                          (now - m_state.systemInfoLastSentAt.value()) >= ONE_DAY_SECS;

  if (shouldSend) { sendSystemInfo(); }
}

std::string TelemetryService::toLower(const std::string &s) {
  std::string lower = s;
  std::ranges::transform(lower, lower.begin(), ::tolower);
  return lower;
}

void TelemetryService::sendSystemInfo() {
  SystemInfoRequest payload;

  payload.userId = m_state.userId;
  payload.architecture = toLower(QSysInfo::currentCpuArchitecture().toStdString());
  payload.buildProvenance = toLower(VICINAE_PROVENANCE);
  payload.vicinaeVersion = toLower(VICINAE_GIT_TAG);
  payload.desktops = xdgpp::currentDesktop() | std::views::transform([](auto &&s) { return toLower(s); }) |
                     std::ranges::to<std::vector>();
  payload.displayProtocol = QGuiApplication::platformName().toStdString();
  payload.locale = QLocale::system().name().toStdString();
  payload.screens =
      QGuiApplication::screens() | std::views::transform([](QScreen *screen) {
        return ScreenInfo{.resolution = {.width = screen->size().width(), .height = screen->size().height()},
                          .scale = screen->devicePixelRatio()};
      }) |
      std::ranges::to<std::vector>();
  payload.operatingSystem = QSysInfo::kernelType().toStdString();
  payload.kernelVersion = QSysInfo::kernelVersion().toStdString();
  payload.productId = QSysInfo::productType().toStdString();
  payload.productVersion = QSysInfo::productVersion().toStdString();

  std::string debugBuf;
  [[maybe_unused]] auto writeErr = glz::write_json(payload, debugBuf);
  qDebug().noquote() << "Sending system info:\n" << glz::prettify_json(debugBuf);

  m_client.post<SystemInfoResponse, SystemInfoRequest>("/telemetry/system-info", std::move(payload))
      .then([this](const http::Client::Result<SystemInfoResponse> &res) {
        if (!res) {
          qWarning() << "Failed to post system info" << res.error();
          return;
        }
        m_state.systemInfoLastSentAt = QDateTime::currentSecsSinceEpoch();
        saveState();
      });
}

std::string TelemetryService::generateUserId() {
  return QString("user-%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces)).toStdString();
}

void TelemetryService::saveState() {
  if (auto const error = glz::write_file_json(m_state, m_statePath.c_str(), m_buf)) {
    qWarning() << "Failed to write telemetry state file at" << m_statePath.c_str()
               << glz::format_error(error);
  }
}

void TelemetryService::loadState() {
  if (!std::filesystem::is_regular_file(m_statePath)) {
    m_state = State{.userId = generateUserId()};
    saveState();
    return;
  }

  if (auto const error = glz::read_file_json(m_state, m_statePath.c_str(), m_buf)) {
    qWarning() << "Failed to read telemetry state file at" << m_statePath.c_str() << glz::format_error(error);
  }
}
