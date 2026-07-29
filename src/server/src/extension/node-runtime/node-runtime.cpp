#include "extension/node-runtime/node-runtime.hpp"
#include <filesystem>
#include <fstream>
#include <system_error>
#include <QCryptographicHash>
#include <QFile>
#include <QProcess>
#include <QScopeGuard>
#include <QThreadPool>
#include <qlogging.h>
#include <qstandardpaths.h>
#include "environment.hpp"
#include "internal/http-client.hpp"
#include "generated/version.h"
#include "vicinae.hpp"
#ifdef Q_OS_WIN
#include "internal/zip/unzip.hpp"
#endif

namespace fs = std::filesystem;

NodeRuntime::NodeRuntime(QObject *parent) : QObject(parent) {
  // via signal: install() may fail on a worker thread, keep the reset on this one
  connect(this, &NodeRuntime::installFailed, this, [this]() { m_downloadStarted = false; });
}

#ifndef Q_OS_WIN
// exec node through a hard link so the process shows up as "vicinae-ext-runtime"
// instead of a bare "node" in system monitors
fs::path NodeRuntime::tagged(const fs::path &node) {
  std::error_code ec;
  const fs::path tagged = node.parent_path() / "vicinae-ext-runtime";

  if (fs::equivalent(tagged, node, ec)) return tagged;

  fs::remove(tagged, ec);
  fs::create_hard_link(node, tagged, ec);

  if (ec) {
    qWarning() << "Failed to create tagged node runtime link:" << ec.message().c_str() << "- falling back to"
               << node.c_str();
    return node;
  }

  return tagged;
}
#endif

fs::path NodeRuntime::managedExecutable() const {
  const fs::path dir = Omnicast::dataDir() / "node" / VICINAE_NODE_RUNTIME_VERSION;
#ifdef Q_OS_WIN
  return dir / "node.exe";
#else
  return dir / "node";
#endif
}

std::optional<fs::path> NodeRuntime::executable() {
  if (auto bin = Environment::nodeBinaryOverride()) {
    std::error_code ec;

    if (fs::is_regular_file(*bin, ec)) { return bin; }

    if (auto path = QStandardPaths::findExecutable(QString::fromStdString(bin->string())); !path.isEmpty()) {
      return path.toStdString();
    }

    // we do not fallback if we got an explicit override
    return {};
  }

#if VICINAE_NODE_RUNTIME_DOWNLOAD
  const fs::path managed = managedExecutable();
  if (std::error_code ec; fs::is_regular_file(managed, ec)) {
#ifdef Q_OS_WIN
    return managed;
#else
    return tagged(managed);
#endif
  }

  if (!m_downloadStarted) download();
  return {};
#else
  if (auto path = QStandardPaths::findExecutable("node"); !path.isEmpty()) { return path.toStdString(); }
  return {};
#endif
}

void NodeRuntime::download() {
  m_downloadStarted = true;

  std::error_code ec;
  fs::create_directories(Omnicast::cacheDir(), ec);
#ifdef Q_OS_WIN
  const fs::path archive = Omnicast::cacheDir() / "node.zip";
#else
  const fs::path archive = Omnicast::cacheDir() / "node.tar.gz";
#endif

  qInfo() << "Downloading node runtime from" << VICINAE_NODE_RUNTIME_URL;

  auto *dl = http::Client().download(VICINAE_NODE_RUNTIME_URL, archive);

  connect(dl, &http::Download::finished, this, [this](const fs::path &path) {
    QThreadPool::globalInstance()->start([this, path]() { install(path); });
  });
  connect(dl, &http::Download::failed, this, [this](const QString &error) {
    qCritical() << "Failed to download node runtime, extensions will not work:" << error;
    emit installFailed(error);
  });
}

void NodeRuntime::install(const fs::path &archive) {
  std::error_code ec;
  auto removeArchive = qScopeGuard([&]() { fs::remove(archive, ec); });

  QFile file(archive);
  QCryptographicHash hash(QCryptographicHash::Sha256);

  qInfo() << "Verifying node runtime archive integrity (sha256)" << archive.c_str();

  if (!file.open(QIODevice::ReadOnly) || !hash.addData(&file) ||
      hash.result().toHex() != VICINAE_NODE_RUNTIME_SHA256) {
    qCritical() << "Node runtime archive failed checksum verification, extensions will not work";
    emit installFailed("checksum verification failed");
    return;
  }

  qInfo() << "Node runtime archive integrity verified";

  const fs::path dest = Omnicast::dataDir() / "node" / VICINAE_NODE_RUNTIME_VERSION;

  fs::remove_all(Omnicast::dataDir() / "node", ec);
  fs::create_directories(dest, ec);

  if (!extract(archive, dest)) {
    emit installFailed("extraction failed");
    return;
  }

  qInfo() << "Installed node runtime" << VICINAE_NODE_RUNTIME_VERSION;
  emit installed();
}

bool NodeRuntime::extract(const fs::path &archive, const fs::path &dest) {
#ifdef Q_OS_WIN
  Unzipper unzip(archive);
  if (!unzip) {
    qCritical() << "Failed to open node runtime archive" << archive.c_str();
    return false;
  }

  for (auto &entry : unzip.listFiles()) {
    if (entry.path().filename() != "node.exe") continue;
    const std::string data = entry.readAll();
    std::ofstream ofs(dest / "node.exe", std::ios::binary);
    ofs.write(data.data(), static_cast<std::streamsize>(data.size()));
    return ofs.good();
  }

  qCritical() << "node.exe not found in runtime archive";
  return false;
#else
  qInfo() << "Extracting" << VICINAE_NODE_RUNTIME_TAR_MEMBER << "to" << dest.c_str();

  QProcess tar;
  tar.setProgram("/usr/bin/tar");
  tar.setArguments(
      {"-xzf", archive.c_str(), "-C", dest.c_str(), "--strip-components=2", VICINAE_NODE_RUNTIME_TAR_MEMBER});
  tar.start();

  if (!tar.waitForFinished(60000) || tar.exitStatus() != QProcess::NormalExit || tar.exitCode() != 0) {
    qCritical() << "Failed to extract node runtime, extensions will not work:" << tar.readAllStandardError();
    return false;
  }

  return true;
#endif
}
