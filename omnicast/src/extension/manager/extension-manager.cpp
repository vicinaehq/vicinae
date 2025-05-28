#include "extension/manager/extension-manager.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <qfuturewatcher.h>
#include <unordered_map>

static const std::unordered_map<ExtensionMessageType, QString> messageTypeToString = {
    {ExtensionMessageType::REQUEST, "request"},
    {ExtensionMessageType::RESPONSE, "response"},
    {ExtensionMessageType::EVENT, "event"},
};

static const std::unordered_map<Messenger::Type, QString> messengerTypeToString = {
    {Messenger::Type::MAIN, "main"},
    {Messenger::Type::MANAGER, "manager"},
    {Messenger::Type::EXTENSION, "extension"},
};

static const std::unordered_map<QString, Messenger::Type> stringToMessengerType = {
    {"main", Messenger::MAIN},
    {"manager", Messenger::MANAGER},
    {"extension", Messenger::EXTENSION},
};

static const std::unordered_map<QString, ExtensionMessageType> stringToExtensionMessageType = {
    {"request", ExtensionMessageType::REQUEST},
    {"response", ExtensionMessageType::RESPONSE},
    {"event", ExtensionMessageType::EVENT},
};

FullMessage Bus::parseRawMessage(const QByteArray &data) {
  qDebug() << "processing packet of size " << data.size();
  auto json = QJsonDocument::fromJson(data);

  return parseFullMessage(json.object());
}

Messenger Bus::parseMessenger(const QJsonObject &lhs) {
  Messenger messenger;

  messenger.id = lhs.value("id").toString();

  if (auto it = stringToMessengerType.find(lhs["type"].toString()); it != stringToMessengerType.end()) {
    messenger.type = it->second;
  }

  return messenger;
}

MessageEnvelope Bus::parseEnvelope(const QJsonObject &lhs) {
  MessageEnvelope envelope;

  envelope.id = lhs["id"].toString();
  envelope.action = lhs["action"].toString();

  if (auto it = stringToExtensionMessageType.find(lhs["type"].toString());
      it != stringToExtensionMessageType.end()) {
    envelope.type = it->second;
  }

  envelope.target = parseMessenger(lhs["target"].toObject());
  envelope.sender = parseMessenger(lhs["sender"].toObject());

  return envelope;
}

FullMessage Bus::parseFullMessage(const QJsonObject &lhs) {
  auto envelope = parseEnvelope(lhs["envelope"].toObject());
  auto data = lhs["data"].toObject();

  return {envelope, data};
}

QJsonObject Bus::serializeMessenger(const Messenger &lhs) {
  QJsonObject obj;

  obj["id"] = lhs.id;

  if (auto it = messengerTypeToString.find(lhs.type); it != messengerTypeToString.end()) {
    obj["type"] = it->second;
  }

  return obj;
}

MessageEnvelope Bus::makeEnvelope(ExtensionMessageType type, const Messenger &target, const QString &action) {
  auto id = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
  MessageEnvelope envelope{
      .id = id,
      .type = type,
      .sender = selfMessenger,
      .target = target,
      .action = action,
  };

  return envelope;
}

void Bus::sendMessage(const MessageEnvelope &envelope, const QJsonObject &payload) {
  QJsonObject serializedEnvelope;

  serializedEnvelope["id"] = envelope.id;

  if (auto it = messageTypeToString.find(envelope.type); it != messageTypeToString.end()) {
    serializedEnvelope["type"] = it->second;
  }

  serializedEnvelope["sender"] = serializeMessenger(envelope.sender);
  serializedEnvelope["target"] = serializeMessenger(envelope.target);
  serializedEnvelope["action"] = envelope.action;

  QJsonObject message;

  message["envelope"] = serializedEnvelope;
  message["data"] = payload;

  QJsonDocument doc(message);

  QByteArray data;
  QDataStream dataStream(&data, QIODevice::WriteOnly);

  dataStream << doc.toJson();
  device->write(data);
  qDebug() << "waiting for write...";
  device->waitForBytesWritten(1000);
  qDebug() << "wrote message";
}

void Bus::request(const Messenger &target, const QString &action, const QJsonObject &payload) {
  auto envelope = makeEnvelope(ExtensionMessageType::REQUEST, target, action);

  m_outgoingPendingRequests.insert({envelope.id, envelope});
  sendMessage(envelope, payload);
}

void Bus::emitEvent(const Messenger &target, const QString &action, const QJsonObject &payload) {
  auto envelope = makeEnvelope(ExtensionMessageType::EVENT, target, action);

  sendMessage(envelope, payload);
}

void Bus::requestExtension(const QString &sessionId, const QString &action, const QJsonObject &payload) {
  Messenger target{.id = sessionId, .type = Messenger::Type::EXTENSION};

  request(target, action, payload);
}

void Bus::handleMessage() {
  FullMessage msg = m_parseMessageTask.result();

  qDebug() << "[DEBUG] readyRead: got message of type" << msg.envelope.action;

  if (msg.envelope.type == ExtensionMessageType::REQUEST) {
    m_incomingPendingRequests.insert({msg.envelope.id, msg.envelope});
  }

  if (msg.envelope.sender.type == Messenger::Type::MANAGER) {
    if (msg.envelope.type == ExtensionMessageType::REQUEST) {
      qDebug() << "manager->main is not supported";
      return;
    }

    if (msg.envelope.type == ExtensionMessageType::RESPONSE) {
      auto it = m_outgoingPendingRequests.find(msg.envelope.id);

      if (it == m_outgoingPendingRequests.end()) {
        qDebug() << "No matching request for manager response";
        return;
      }

      m_outgoingPendingRequests.erase(msg.envelope.id);
      emit managerResponse(msg.envelope.action, msg.data);
    }
  }

  if (msg.envelope.sender.type == Messenger::Type::EXTENSION) {
    if (msg.envelope.type == ExtensionMessageType::EVENT) {
      emit extensionEvent(msg.envelope.sender.id, msg.envelope.action, msg.data);
    }
    if (msg.envelope.type == ExtensionMessageType::REQUEST) {
      emit extensionRequest(msg.envelope.sender.id, msg.envelope.id, msg.envelope.action, msg.data);
    }
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

      // TODO: find a better way that does not block while still preserving order
      if (m_parseMessageTask.isRunning()) { m_parseMessageTask.waitForFinished(); }

      auto task = QtConcurrent::run([this, packet = std::move(packet)]() { return parseRawMessage(packet); });

      m_parseMessageTask.setFuture(task);
      _message.data = _message.data.sliced(sizeof(uint32_t) + length);
    }
  }
}

void Bus::requestManager(const QString &action, const QJsonObject &payload) {
  Messenger target{.type = Messenger::Type::MANAGER};

  request(target, action, payload);
}

bool Bus::respond(const QString &id, const QJsonObject &payload) {
  auto it = m_incomingPendingRequests.find(id);

  if (it == m_incomingPendingRequests.end()) {
    qDebug() << "no request with id " << id;
    return false;
  }

  auto envelope = it->second;

  m_incomingPendingRequests.erase(id);
  envelope.target = envelope.sender;
  envelope.sender = selfMessenger;
  envelope.type = ExtensionMessageType::RESPONSE;
  sendMessage(envelope, payload);

  return true;
}

void Bus::emitExtensionEvent(const QString &sessionId, const QString &action, const QJsonObject &payload) {
  Messenger target{.id = sessionId, .type = Messenger::Type::EXTENSION};
  auto envelope = makeEnvelope(ExtensionMessageType::EVENT, target, action);

  sendMessage(envelope, payload);
}

void Bus::ping() { requestManager("ping", {}); }

void Bus::loadCommand(const QString &extensionId, const QString &cmdName) {
  QJsonObject data;

  data["extensionId"] = extensionId;
  data["commandName"] = cmdName;

  requestManager("load-command", data);
}

void Bus::unloadCommand(const QString &sessionId) {
  QJsonObject data;

  data["sessionId"] = sessionId;

  requestManager("unload-command", data);
}

Bus::Bus(QIODevice *socket) : device(socket) {
  connect(socket, &QIODevice::readyRead, this, &Bus::readyRead);
  connect(&m_parseMessageTask, &QFutureWatcher<FullMessage>::finished, this, &Bus::handleMessage);
}

// Extension Manager

ExtensionManager::ExtensionManager(OmniCommandDatabase &commandDb) : commandDb(commandDb), bus(&process) {
  connect(&process, &QProcess::readyReadStandardError, this, &ExtensionManager::readError);
  connect(&process, &QProcess::finished, this, &ExtensionManager::finished);
  connect(&process, &QProcess::started, this, &ExtensionManager::processStarted);

  connect(&bus, &Bus::managerResponse, this, &ExtensionManager::handleManagerResponse);
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

bool ExtensionManager::respond(const QString &id, const QJsonObject &payload) {
  return bus.respond(id, payload);
}

void ExtensionManager::emitExtensionEvent(const QString &sessionId, const QString &action,
                                          const QJsonObject &payload) {
  return bus.emitExtensionEvent(sessionId, action, payload);
}

void ExtensionManager::requestManager(const QString &action, const QJsonObject &payload) {
  return bus.requestManager(action, payload);
}

void ExtensionManager::processStarted() { bus.requestManager("list-extensions", {}); }

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

void ExtensionManager::startDevelopmentSession(const QString &id) {
  QJsonObject data;

  data["id"] = id;
  requestManager("develop.start", data);
}

void ExtensionManager::refreshDevelopmentSession(const QString &id) {
  QJsonObject data;

  data["id"] = id;
  requestManager("develop.refresh", data);
}

void ExtensionManager::stopDevelopmentSession(const QString &id) {
  QJsonObject data;

  data["id"] = id;
  requestManager("develop.stop", data);
}

void ExtensionManager::loadCommand(const QString &extensionId, const QString &cmd,
                                   const QJsonObject &preferenceValues, const LaunchProps &launchProps) {
  QJsonObject payload;

  payload["extensionId"] = extensionId;
  payload["commandName"] = cmd;
  payload["preferenceValues"] = preferenceValues;
  payload["launchProps"] = serializeLaunchProps(launchProps);

  bus.requestManager("load-command", payload);
}

void ExtensionManager::unloadCommand(const QString &sessionId) {
  QJsonObject payload;

  payload["sessionId"] = sessionId;

  bus.requestManager("unload-command", payload);
}

void ExtensionManager::parseListExtensionData(QJsonObject &obj) {
  std::vector<std::shared_ptr<Extension>> extensions;
  auto extensionList = obj["extensions"].toArray();

  extensions.reserve(extensionList.size());

  qDebug() << "parse list extensions" << extensionList.size();

  for (const auto &ext : extensionList) {
    auto extension = std::make_shared<Extension>(Extension::fromObject((ext.toObject())));

    commandDb.registerRepository(extension);
    extensions.push_back(extension);
  }

  loadedExtensions = extensions;
}

void ExtensionManager::handleManagerResponse(const QString &action, QJsonObject &data) {
  if (action == "list-extensions") { return parseListExtensionData(data); }

  if (action == "develop.refresh") { bus.requestManager("list-extensions", {}); }

  if (action == "load-command") {
    auto sessionId = data["sessionId"].toString();

    LoadedCommand cmd;

    cmd.sessionId = sessionId;
    cmd.command.name = data["command"].toObject()["name"].toString();

    emit commandLoaded(cmd);
  }
}
