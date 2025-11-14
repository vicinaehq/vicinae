#include "extension/manager/extension-manager.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <filesystem>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include <qstandardpaths.h>
#include <qstringview.h>
#include <string>
#include <system_error>
#include <unordered_map>
#include "environment.hpp"
#include "pid-file/pid-file.hpp"
#include "proto/extension.pb.h"
#include "proto/manager.pb.h"
#include "version.h"
#include "vicinae.hpp"

namespace fs = std::filesystem;

void Bus::sendMessage(const QByteArray &data) {
  QByteArray message;
  QDataStream dataStream(&message, QIODevice::WriteOnly);

  dataStream << data;

  device->write(message);
  device->waitForBytesWritten(1000);
}

void Bus::handleMessage(const proto::ext::IpcMessage &msg) {
  if (msg.has_extension_request()) { emit extensionRequest(msg.extension_request()); }
  if (msg.has_extension_event()) { emit extensionEvent(msg.extension_event()); }
  if (msg.has_manager_response()) {
    auto &response = msg.manager_response();

    if (auto it = m_pendingManagerRequests.find(response.request_id());
        it != m_pendingManagerRequests.end()) {
      m_pendingManagerRequests.erase(it);
      emit it->second->finished(response.value());
    } else {
      qWarning() << "Got response but no matching request id" << response.request_id().c_str();
    }

    // emit managerResponse(msg.manager_response());
  }
}

void Bus::readyRead() {
  while (device->bytesAvailable() > 0) {
    auto read = device->readAll();

    _message.data.append(read);

    while (_message.data.size() >= sizeof(uint32_t)) {
      uint32_t length = ntohl(*reinterpret_cast<uint32_t *>(_message.data.data()));
      bool isComplete = _message.data.size() - sizeof(uint32_t) >= length;

      if (!isComplete) break;

      auto packet = _message.data.sliced(sizeof(uint32_t), length);
      std::string stringData = packet.toStdString();

      proto::ext::IpcMessage msg;

      msg.ParseFromString(stringData);

      handleMessage(msg);

      // m_parseMessageTask.setFuture(task);
      _message.data = _message.data.sliced(sizeof(uint32_t) + length);
    }
  }
}

ManagerRequest *Bus::requestManager(proto::ext::manager::RequestData *req) {
  std::string data;
  proto::ext::IpcMessage message;
  auto request = new proto::ext::ManagerRequest;
  auto id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();

  request->set_request_id(id);
  request->set_allocated_payload(req);

  message.set_allocated_manager_request(request);
  message.SerializeToString(&data);
  sendMessage(QByteArray::fromStdString(data));

  auto handle = new ManagerRequest;

  m_pendingManagerRequests.insert({id, handle});

  return handle;
}

void Bus::emitExtensionEvent(proto::ext::QualifiedExtensionEvent *event) {
  std::string data;
  proto::ext::IpcMessage message;

  message.set_allocated_extension_event(event);
  message.SerializeToString(&data);
  sendMessage(QByteArray::fromStdString(data));
}

bool Bus::respondToExtension(const QString &sessionId, const QString &requestId,
                             proto::ext::extension::Response *response) {
  proto::ext::IpcMessage message;
  std::string serialized;

  auto qualifiedResponse = new proto::ext::QualifiedExtensionResponse;

  // TODO: get session id from request
  qualifiedResponse->set_session_id(sessionId.toStdString());
  qualifiedResponse->set_allocated_response(response);

  message.set_allocated_extension_response(qualifiedResponse);
  message.SerializeToString(&serialized);

  sendMessage(QByteArray::fromStdString(serialized));

  return true;
}

void Bus::ping() {}

Bus::Bus(QIODevice *socket) : device(socket) {
  connect(socket, &QIODevice::readyRead, this, &Bus::readyRead);
  // connect(&m_parseMessageTask, &QFutureWatcher<FullMessage>::finished, this, &Bus::handleMessage);
}

// Extension Manager

ExtensionManager::ExtensionManager() : m_bus(&m_process) {
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  env.insert("VICINAE_VERSION", VICINAE_GIT_TAG);
  env.insert("VICINAE_COMMIT", VICINAE_GIT_COMMIT_HASH);
  m_process.setProcessEnvironment(env);

  connect(&m_process, &QProcess::readyReadStandardError, this, &ExtensionManager::readError);
  connect(&m_process, &QProcess::finished, this, &ExtensionManager::finished);
  connect(&m_process, &QProcess::started, this, &ExtensionManager::processStarted);

  /// TODO: implement
  // connect(&bus, &Bus::managerResponse, this, &ExtensionManager::handleManagerResponse);
  connect(&m_bus, &Bus::extensionEvent, this,
          [this](const proto::ext::QualifiedExtensionEvent &proto) { emit extensionEvent(proto); });

  connect(&m_bus, &Bus::extensionRequest, this, [this](const proto::ext::QualifiedExtensionRequest &req) {
    auto request = new ExtensionRequest(m_bus, req);
    emit extensionRequest(request);
  });
}

bool ExtensionManager::isRunning() const { return m_process.state() == QProcess::ProcessState::Running; }

std::optional<fs::path> ExtensionManager::nodeExecutable() {
  static const std::array<const char *, 2> candidates = {"vicinae-node", "node"};

  if (auto bin = Environment::nodeBinaryOverride()) {
    std::error_code ec;

    if (fs::is_regular_file(*bin, ec)) { return bin; }

    if (auto path = QStandardPaths::findExecutable(bin->c_str()); !path.isEmpty()) {
      return path.toStdString();
    }

    // we do not fallback if we got an explicit override
    return {};
  }

  if (auto appDir = Environment::appImageDir()) {
    std::error_code ec;
    fs::path path = *appDir / "usr" / "bin" / "node";
    if (fs::is_regular_file(path, ec)) return path;
  }

  for (auto candidate : candidates) {
    if (auto path = QStandardPaths::findExecutable(candidate); !path.isEmpty()) { return path.toStdString(); }
  }

  return {};
}

bool ExtensionManager::stop() {
  m_process.terminate();
  return m_process.waitForFinished();
}

bool ExtensionManager::start() {
#ifndef HAS_TYPESCRIPT_EXTENSIONS
  qCritical() << "Cannot start extension manager as extension support was disabled at compile time";
  return false;
#endif

  int maxWaitForStart = 5000;

  if (m_process.state() == QProcess::Running) { m_process.close(); }

  auto node = nodeExecutable();

  if (!node) {
    qCritical() << "Unable to find a suitable node executable. TypeScript extensions will not work.";
    return false;
  }

  fs::path managerPath = Omnicast::runtimeDir() / "extension-manager.js";

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

ManagerRequest *ExtensionManager::requestManager(proto::ext::manager::RequestData *req) {
  return m_bus.requestManager(req);
}

void ExtensionManager::emitExtensionEvent(proto::ext::QualifiedExtensionEvent *event) {
  return m_bus.emitExtensionEvent(event);
}

void ExtensionManager::addDevelopmentSession(const QString &id) { m_developmentSessions.insert(id); }

void ExtensionManager::removeDevelopmentSession(const QString &id) { m_developmentSessions.erase(id); }

bool ExtensionManager::hasDevelopmentSession(const QString &id) const {
  return m_developmentSessions.contains(id);
}

void ExtensionManager::emitGenericExtensionEvent(const QString &sessionId, const QString &handlerId,
                                                 const QJsonArray &args) {
  auto qualified = new proto::ext::QualifiedExtensionEvent;
  auto event = new proto::ext::extension::Event;
  auto generic = new proto::ext::extension::GenericEventData;
  QJsonDocument document;

  document.setArray(args);
  qualified->set_session_id(sessionId.toStdString());
  event->set_id(handlerId.toStdString());
  event->set_allocated_generic(generic);
  generic->set_json(document.toJson(QJsonDocument::Compact).toStdString());
  qualified->set_allocated_event(event);
  emitExtensionEvent(qualified);
}

void ExtensionManager::processStarted() { emit started(); }

QJsonObject ExtensionManager::serializeLaunchProps(const LaunchProps &props) {
  QJsonObject obj;
  QJsonObject arguments;

  for (const auto &[k, v] : props.arguments) {
    arguments[k] = v;
  }

  obj["arguments"] = arguments;

  return obj;
}

void ExtensionManager::finished(int exitCode, QProcess::ExitStatus status) {
  qCritical() << "Extension manager crashed. Extensions will not work";
}

void ExtensionManager::readError() {
  auto buf = m_process.readAllStandardError();

  for (const auto &line : buf.trimmed().split('\n')) {
    qInfo() << "[EXTENSION]" << line;
  }
}

void ExtensionManager::unloadCommand(const QString &sessionId) {
  auto requestData = new proto::ext::manager::RequestData;
  auto unload = new proto::ext::manager::ManagerUnloadCommand;

  unload->set_session_id(sessionId.toStdString());
  requestData->set_allocated_unload(unload);
  requestManager(requestData);
}

void ExtensionManager::handleManagerResponse(const QString &action, QJsonObject &data) {}
