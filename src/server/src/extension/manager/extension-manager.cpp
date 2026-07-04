#include "extension/manager/extension-manager.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <filesystem>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include <qstandardpaths.h>
#include <qstringview.h>
#include <qtypes.h>
#include <string>
#include <system_error>
#include <utility>
#include "environment.hpp"
#include "http-client.hpp"
#include "pid-file/pid-file.hpp"
#include "generated/version.h"
#include <QCryptographicHash>
#include <QScopeGuard>
#include <QThreadPool>
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
      uint32_t const length = ntohl(*reinterpret_cast<uint32_t *>(_message.data.data()));
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
}

bool ExtensionManager::isRunning() const { return m_process.state() == QProcess::ProcessState::Running; }

std::optional<fs::path> ExtensionManager::nodeExecutable() {
  if (auto bin = Environment::nodeBinaryOverride()) {
    std::error_code ec;

    if (fs::is_regular_file(*bin, ec)) { return bin; }

    if (auto path = QStandardPaths::findExecutable(bin->c_str()); !path.isEmpty()) {
      return path.toStdString();
    }

    // we do not fallback if we got an explicit override
    return {};
  }

#if VICINAE_NODE_RUNTIME_DOWNLOAD
  // self-distributed builds always run the pinned managed runtime so the node
  // version does not depend on what the host has installed
  const fs::path managed = Omnicast::dataDir() / "node" / VICINAE_NODE_RUNTIME_VERSION / "node";
  if (std::error_code ec; fs::is_regular_file(managed, ec)) { return managed; }
#else
  if (auto path = QStandardPaths::findExecutable("node"); !path.isEmpty()) { return path.toStdString(); }

#ifdef Q_OS_MACOS
  // GUI apps get a minimal PATH; probe the usual homes directly
  for (const char *path : {"/opt/homebrew/bin/node", "/usr/local/bin/node"}) {
    if (std::error_code ec; fs::is_regular_file(path, ec)) { return fs::path(path); }
  }
#endif
#endif

  return {};
}

#if VICINAE_NODE_RUNTIME_DOWNLOAD
void ExtensionManager::downloadNodeRuntime() {
  m_nodeDownloadStarted = true;

  std::error_code ec;
  fs::create_directories(Omnicast::cacheDir(), ec);
  const fs::path archive = Omnicast::cacheDir() / "node.tar.gz";

  qInfo() << "Downloading node runtime from" << VICINAE_NODE_RUNTIME_URL;

  auto *download = http::Client().download(VICINAE_NODE_RUNTIME_URL, archive);

  connect(download, &http::Download::finished, this, [this](const fs::path &path) {
    QThreadPool::globalInstance()->start([this, path]() { installNodeRuntime(path); });
  });
  connect(download, &http::Download::failed, this, [](const QString &error) {
    qCritical() << "Failed to download node runtime, extensions will not work:" << error;
  });
}

void ExtensionManager::installNodeRuntime(const std::filesystem::path &archive) {
  std::error_code ec;
  auto removeArchive = qScopeGuard([&]() { fs::remove(archive, ec); });

  QFile file(archive.c_str());
  QCryptographicHash hash(QCryptographicHash::Sha256);

  qInfo() << "Verifying node runtime archive integrity (sha256)" << archive.c_str();

  if (!file.open(QIODevice::ReadOnly) || !hash.addData(&file) ||
      hash.result().toHex() != VICINAE_NODE_RUNTIME_SHA256) {
    qCritical() << "Node runtime archive failed checksum verification, extensions will not work";
    return;
  }

  qInfo() << "Node runtime archive integrity verified";

  const fs::path dest = Omnicast::dataDir() / "node" / VICINAE_NODE_RUNTIME_VERSION;

  fs::remove_all(Omnicast::dataDir() / "node", ec);
  fs::create_directories(dest, ec);

  qInfo() << "Extracting" << VICINAE_NODE_RUNTIME_TAR_MEMBER << "to" << dest.c_str();

  QProcess tar;

  tar.setProgram("/usr/bin/tar");
  tar.setArguments(
      {"-xzf", archive.c_str(), "-C", dest.c_str(), "--strip-components=2", VICINAE_NODE_RUNTIME_TAR_MEMBER});
  tar.start();

  if (!tar.waitForFinished(60000) || tar.exitStatus() != QProcess::NormalExit || tar.exitCode() != 0) {
    qCritical() << "Failed to extract node runtime, extensions will not work:" << tar.readAllStandardError();
    return;
  }

  qInfo() << "Installed node runtime" << VICINAE_NODE_RUNTIME_VERSION;

  QMetaObject::invokeMethod(this, [this]() { start(); }, Qt::QueuedConnection);
}
#endif

bool ExtensionManager::stop() {
  m_process.terminate();
  return m_process.waitForFinished();
}

bool ExtensionManager::start() {
#ifndef HAS_TYPESCRIPT_EXTENSIONS
  qCritical() << "Cannot start extension manager as extension support was disabled at compile time";
  return false;
#endif

  int const maxWaitForStart = 5000;

  if (m_process.state() == QProcess::Running) { m_process.close(); }

  auto node = nodeExecutable();

  if (!node) {
#if VICINAE_NODE_RUNTIME_DOWNLOAD
    if (!m_nodeDownloadStarted) {
      // start() is called again once the runtime is installed
      downloadNodeRuntime();
      return true;
    }
#endif
    qCritical() << "Unable to find a suitable node executable. TypeScript extensions will not work.";
    return false;
  }

  fs::path const managerPath = Omnicast::runtimeDir() / "extension-manager.js";

  QFile::remove(managerPath);
  QFile::copy(":bin/extension-manager", managerPath.c_str());
  PidFile pidFile("extension-manager");

  if (pidFile.exists() && pidFile.kill()) { qInfo() << "Killed existing extension manager instance"; }

  m_process.start(node->c_str(), {managerPath.c_str()});

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
