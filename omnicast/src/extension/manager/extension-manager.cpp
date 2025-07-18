#include "extension/manager/extension-manager.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <absl/strings/internal/str_format/extension.h>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include <qstringview.h>
#include <string>
#include <unordered_map>
#include "protocols/extension.pb.h"

void Bus::sendMessage(const QByteArray &data) {
  QByteArray message;
  QDataStream dataStream(&message, QIODevice::WriteOnly);

  dataStream << data;

  device->write(message);
  // qDebug() << "waiting for write...";
  device->waitForBytesWritten(1000);
  // qDebug() << "wrote message";
}

void Bus::handleMessage(const protocols::extensions::IpcMessage &msg) {
  // qDebug() << "[DEBUG] readyRead: got message of type" << msg.envelope.action;

  if (msg.has_extension_request()) { emit extensionRequest(msg.extension_request()); }
  if (msg.has_extension_event()) { emit extensionEvent(msg.extension_event()); }
  if (msg.has_manager_response()) { emit managerResponse(msg.manager_response()); }
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

      protocols::extensions::IpcMessage msg;

      msg.ParseFromString(stringData);

      handleMessage(msg);

      // m_parseMessageTask.setFuture(task);
      _message.data = _message.data.sliced(sizeof(uint32_t) + length);
    }
  }
}

void Bus::requestManager(protocols::extensions::ManagerRequestData *req) {
  std::string data;
  protocols::extensions::IpcMessage message;
  auto request = new protocols::extensions::ManagerRequest;

  request->set_request_id(QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString());
  request->set_allocated_payload(req);

  message.set_allocated_manager_request(request);
  message.SerializeToString(&data);
  sendMessage(QByteArray::fromStdString(data));
}

void Bus::emitExtensionEvent(protocols::extensions::QualifiedExtensionEvent *event) {
  std::string data;
  protocols::extensions::IpcMessage message;

  message.set_allocated_extension_event(event);
  message.SerializeToString(&data);
  sendMessage(QByteArray::fromStdString(data));
}

bool Bus::respondToExtension(const QString &requestId, protocols::extensions::ExtensionResponseData *data) {
  protocols::extensions::IpcMessage message;
  std::string serialized;

  auto qualifiedResponse = new protocols::extensions::QualifiedExtensionResponse;
  auto response = new protocols::extensions::ExtensionResponse;

  response->set_request_id(requestId.toStdString());
  response->set_allocated_data(data);
  // TODO: get session id from request
  qualifiedResponse->set_session_id("");
  qualifiedResponse->set_allocated_response(response);

  message.set_allocated_extension_response(qualifiedResponse);
  message.SerializeToString(&serialized);

  sendMessage(QByteArray::fromStdString(serialized));

  return true;
}

void Bus::ping() {}

void Bus::loadCommand(const QString &extensionId, const QString &cmdName) {
  auto requestData = new protocols::extensions::ManagerRequestData;
  auto command = new protocols::extensions::ManagerLoadCommand;

  qDebug() << "bus loadCommand";

  command->set_entrypoint("/fake/entrypoint/lol");
  command->set_env(protocols::extensions::CommandEnv::Development);
  command->set_mode(protocols::extensions::CommandMode::View);
  requestData->set_allocated_load(command);
  requestManager(requestData);
}

void Bus::unloadCommand(const QString &sessionId) {
  auto requestData = new protocols::extensions::ManagerRequestData;
  auto unload = new protocols::extensions::ManagerUnloadCommand;

  unload->set_session_id(sessionId.toStdString());
  requestData->set_allocated_unload(unload);
  requestManager(requestData);
}

Bus::Bus(QIODevice *socket) : device(socket) {
  connect(socket, &QIODevice::readyRead, this, &Bus::readyRead);
  // connect(&m_parseMessageTask, &QFutureWatcher<FullMessage>::finished, this, &Bus::handleMessage);
}

// Extension Manager

ExtensionManager::ExtensionManager(OmniCommandDatabase &commandDb) : commandDb(commandDb), bus(&process) {
  connect(&process, &QProcess::readyReadStandardError, this, &ExtensionManager::readError);
  connect(&process, &QProcess::finished, this, &ExtensionManager::finished);
  connect(&process, &QProcess::started, this, &ExtensionManager::processStarted);

  /// TODO: implement
  // connect(&bus, &Bus::managerResponse, this, &ExtensionManager::handleManagerResponse);
  connect(&bus, &Bus::extensionEvent, this, &ExtensionManager::extensionEvent);
  connect(&bus, &Bus::extensionRequest, this, &ExtensionManager::extensionRequest);
}

bool ExtensionManager::start() {
  QFile file(":assets/extension-runtime.js");

  if (!file.exists()) {
    qCritical("Could not find bundled extension runtime. Cannot start extension_manager.");
    return false;
  }

  if (!file.open(QIODevice::ReadOnly)) {
    qCritical("Failed to open bundled extension-runtime for read");
    return false;
  }

  auto runtimeFile = new QTemporaryFile(this);

  if (!runtimeFile->open()) {
    qCritical("Failed to open temporary file to write extension runtime code");
    return false;
  }

  qDebug() << "Started extension runtime" << runtimeFile->fileName();

  runtimeFile->write(file.readAll());
  process.start("/bin/node", {runtimeFile->fileName()});

  return true;
}

const std::vector<std::shared_ptr<Extension>> &ExtensionManager::extensions() const {
  return loadedExtensions;
}

void ExtensionManager::requestManager(protocols::extensions::ManagerRequestData *req) {
  return bus.requestManager(req);
}

bool ExtensionManager::respondToExtension(const QString &requestId,
                                          protocols::extensions::ExtensionResponseData *data) {
  return bus.respondToExtension(requestId, data);
}

void ExtensionManager::emitExtensionEvent(protocols::extensions::QualifiedExtensionEvent *event) {
  return bus.emitExtensionEvent(event);
}

void ExtensionManager::processStarted() {}

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
  qDebug() << "Extension manager exited prematurely. Extensions will not work";
}

void ExtensionManager::readError() {
  auto buf = process.readAllStandardError();

  QTextStream(stderr) << buf;
}

void ExtensionManager::loadCommand(const QString &extensionId, const QString &cmd,
                                   const QJsonObject &preferenceValues, const LaunchProps &launchProps) {
  bus.loadCommand(extensionId, cmd);
}

void ExtensionManager::unloadCommand(const QString &sessionId) { bus.unloadCommand(sessionId); }

void ExtensionManager::handleManagerResponse(const QString &action, QJsonObject &data) {
  if (action == "load-command") {
    auto sessionId = data["sessionId"].toString();

    LoadedCommand cmd;

    cmd.sessionId = sessionId;
    cmd.command.name = data["command"].toObject()["name"].toString();

    emit commandLoaded(cmd);
  }
}
