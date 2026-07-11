#include "extension/manager/extension-manager.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <filesystem>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include <qstringview.h>
#include <qtypes.h>
#include <string>
#include <system_error>
#include <utility>
#include "pid-file/pid-file.hpp"
#include "generated/version.h"
#include "rang/rang.hpp"
#include "vicinae.hpp"

namespace fs = std::filesystem;

void Bus::send(std::string_view data) {
  sendMessage(QByteArray{data.data(), static_cast<qsizetype>(data.size())});
}

void Bus::sendMessage(const QByteArray &data) {
  QByteArray message;
  QDataStream dataStream(&message, QIODevice::WriteOnly);

  dataStream << data;

  device->write(message);
  device->waitForBytesWritten(1000);
}

void Bus::readyRead() {
  while (device->bytesAvailable() > 0) {
    auto read = device->readAll();

    _message.data.append(read);

    while (std::cmp_greater_equal(_message.data.size(), sizeof(uint32_t))) {
      const auto *p = reinterpret_cast<const unsigned char *>(_message.data.data());
      uint32_t const length =
          (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | uint32_t(p[3]);
      bool const isComplete = _message.data.size() - sizeof(uint32_t) >= length;

      if (!isComplete) break;

      auto packet = _message.data.sliced(sizeof(uint32_t), length);

      emit messageReceived(packet);

      _message.data = _message.data.sliced(sizeof(uint32_t) + length);
    }
  }
}

Bus::Bus(QIODevice *socket) : device(socket) {
  connect(socket, &QIODevice::readyRead, this, &Bus::readyRead);
}

// Extension Manager

ExtensionManager::ExtensionManager() : m_bus(&m_process), m_rpc(m_bus), m_client(m_rpc) {
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  env.insert("VICINAE_VERSION", VICINAE_GIT_TAG);
  env.insert("VICINAE_COMMIT", VICINAE_GIT_COMMIT_HASH);
  m_process.setProcessEnvironment(env);

  connect(&m_process, &QProcess::readyReadStandardError, this, &ExtensionManager::readError);
  connect(&m_process, &QProcess::finished, this, &ExtensionManager::finished);
  connect(&m_process, &QProcess::started, this, &ExtensionManager::processStarted);
  connect(m_client.manager(), &manager::ManagerService::extensionMessage, this,
          &ExtensionManager::extensionMessageReceived);
  connect(m_client.manager(), &manager::ManagerService::extensionCrash, this,
          &ExtensionManager::extensionCrashed);

  connect(&m_bus, &Bus::messageReceived, this, [this](const QByteArray &msg) {
    std::string_view view{msg.constData(), static_cast<size_t>(msg.size())};
    if (auto res = m_client.route(view); !res) { qWarning() << "Failed to route message" << res.error(); }
  });

  connect(&m_node, &NodeRuntime::installed, this, [this]() { start(); });
}

bool ExtensionManager::isRunning() const { return m_process.state() == QProcess::ProcessState::Running; }

bool ExtensionManager::stop() {
  m_process.terminate();
  if (m_process.waitForFinished(2000)) return true;
  m_process.kill();
  return m_process.waitForFinished(2000);
}

bool ExtensionManager::start() {
#ifndef HAS_TYPESCRIPT_EXTENSIONS
  qCritical() << "Cannot start extension manager as extension support was disabled at compile time";
  return false;
#endif

  int const maxWaitForStart = 5000;

  if (m_process.state() == QProcess::Running) { m_process.close(); }

  auto node = m_node.executable();

  if (!node) {
    // executable() kicks off an async download on managed builds; installed() re-drives start()
    if (m_node.provisioning()) return true;
    qCritical() << "Unable to find a suitable node executable. TypeScript extensions will not work.";
    return false;
  }

  fs::path const managerPath = Omnicast::runtimeDir() / "extension-manager.js";
  QString const managerPathStr = QString::fromStdString(managerPath.string());

  // A live node process holds the bundle open on Windows, blocking the overwrite; kill it first.
  PidFile pidFile("extension-manager");
  if (pidFile.exists() && pidFile.kill()) { qInfo() << "Killed existing extension manager instance"; }

  QFile managerFile(managerPath);
  if (managerFile.exists()) {
    // The previous copy inherited the read-only bit of the Qt resource, which blocks removal on Windows.
    managerFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    if (!managerFile.remove()) {
      qWarning() << "Failed to remove stale extension manager bundle:" << managerFile.errorString();
    }
  }
  if (!QFile::copy(":bin/extension-manager", managerPathStr)) {
    qCritical() << "Failed to deploy extension manager bundle to" << managerPath.c_str();
    return false;
  }
  QFile::setPermissions(managerPathStr, QFileDevice::ReadOwner | QFileDevice::WriteOwner);

  m_process.start(QString::fromStdString(node->string()), {managerPathStr});

  if (!m_process.waitForStarted(maxWaitForStart)) {
    qCritical() << "Failed to start extension manager" << m_process.errorString();
    return false;
  }

  pidFile.write(m_process.processId());
  qInfo() << "Started extension manager" << managerPath.c_str() << "with" << node->c_str();

  return true;
}

void ExtensionManager::addDevelopmentSession(const QString &id) { m_developmentSessions.insert(id); }

void ExtensionManager::removeDevelopmentSession(const QString &id) { m_developmentSessions.erase(id); }

bool ExtensionManager::hasDevelopmentSession(const QString &id) const {
  return m_developmentSessions.contains(id);
}

void ExtensionManager::processStarted() { emit started(); }

void ExtensionManager::finished(int exitCode, QProcess::ExitStatus status) {
  qCritical() << "Extension manager crashed. Extensions will not work" << m_process.errorString();
}

void ExtensionManager::readError() {
  auto buf = m_process.readAllStandardError();
  auto ts = QDateTime::currentDateTime().toString("yyyy-MM-dd'T'hh:mm:ss");

  for (const auto &line : buf.trimmed().split('\n')) {
    std::cout << "[" << rang::fg::magenta << "E" << rang::fg::reset << "] " << rang::fg::gray
              << ts.toStdString() << " " << line.toStdString() << "\n";
  }
}
