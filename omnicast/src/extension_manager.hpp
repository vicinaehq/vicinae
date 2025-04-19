#pragma once
#include <QDebug>
#include <QJsonArray>
#include <QString>
#include <QUuid>
#include <QtCore>
#include <cstdint>
#include "extension/extension.hpp"
#include "omni-command-db.hpp"
#include "omnicast.hpp"
#include <filesystem>
#include <netinet/in.h>
#include <qdebug.h>
#include <qdir.h>
#include <qhash.h>
#include <qimage.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qprocess.h>
#include <qstringview.h>
#include <qthread.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <quuid.h>
#include <unistd.h>

struct LoadedCommand {
  QString sessionId;
  struct {
    QString name;
  } command;
};

static const char *exec = "/home/aurelle/.local/share/omnicast/extensions/manager.js";

enum MessageType { REQUEST, RESPONSE, EVENT };

static QHash<MessageType, QString> messageTypeToString = {
    {MessageType::REQUEST, "request"},
    {MessageType::RESPONSE, "response"},
    {MessageType::EVENT, "event"},
};

struct Messenger {
  enum Type { MAIN, MANAGER, EXTENSION };

  QString id;
  Type type;
};

struct MessageEnvelope {
  QString id;
  MessageType type;
  Messenger sender;
  Messenger target;
  QString action;
};

struct FullMessage {
  MessageEnvelope envelope;
  QJsonObject data;
};

class DataDecoder : public QObject {
  Q_OBJECT

  QHash<QString, MessageType> stringToMessageType = {
      {"request", MessageType::REQUEST},
      {"response", MessageType::RESPONSE},
      {"event", MessageType::EVENT},
  };

  QHash<QString, Messenger::Type> stringToMessengerType = {
      {"main", Messenger::MAIN},
      {"manager", Messenger::MANAGER},
      {"extension", Messenger::EXTENSION},
  };

  Messenger parseMessenger(const QJsonObject &lhs) {
    Messenger messenger{.id = lhs["id"].toString(), .type = stringToMessengerType[lhs["type"].toString()]};

    return messenger;
  }

  MessageEnvelope parseEnvelope(const QJsonObject &lhs) {
    MessageEnvelope envelope;

    envelope.id = lhs["id"].toString();
    envelope.action = lhs["action"].toString();
    envelope.type = stringToMessageType[lhs["type"].toString()];
    envelope.target = parseMessenger(lhs["target"].toObject());
    envelope.sender = parseMessenger(lhs["sender"].toObject());

    return envelope;
  }

  FullMessage parseFullMessage(const QJsonObject &lhs) {
    auto envelope = parseEnvelope(lhs["envelope"].toObject());
    auto data = lhs["data"].toObject();

    return {envelope, data};
  }

public slots:
  void processData(const QByteArray &data) {
    qDebug() << "processing packet of size " << data.size();
    auto json = QJsonDocument::fromJson(data);

    // QTextStream(stdout) << json.toJson();
    auto msg = parseFullMessage(json.object());

    emit messageParsed(msg);
  }

signals:
  void messageParsed(FullMessage message);
};

class Bus : public QObject {
  Q_OBJECT

  struct MessageBuffer {
    QByteArray data;
    uint32_t length;
  };

  MessageBuffer _message = {.length = 0};

  QThread *workerThread = nullptr;
  DataDecoder *worker = nullptr;

  Messenger selfMessenger{.type = Messenger::MAIN};

  QHash<QString, MessageEnvelope> outgoingPendingRequests;
  QHash<QString, MessageEnvelope> incomingPendingRequests;

  QHash<Messenger::Type, QString> messengerTypeToString = {
      {Messenger::Type::MAIN, "main"},
      {Messenger::Type::MANAGER, "manager"},
      {Messenger::Type::EXTENSION, "extension"},
  };

  QHash<QString, Messenger::Type> stringToMessengerType = {
      {"main", Messenger::MAIN},
      {"manager", Messenger::MANAGER},
      {"extension", Messenger::EXTENSION},
  };

  QIODevice *device;

  QJsonObject serializeMessenger(const Messenger &lhs) {
    QJsonObject obj;

    obj["id"] = lhs.id;
    obj["type"] = messengerTypeToString[lhs.type];

    return obj;
  }

  MessageEnvelope makeEnvelope(MessageType type, const Messenger &target, const QString &action) {
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

  void sendMessage(const MessageEnvelope &envelope, const QJsonObject &payload) {
    QJsonObject serializedEnvelope;

    serializedEnvelope["id"] = envelope.id;
    serializedEnvelope["type"] = messageTypeToString[envelope.type];
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

  void request(const Messenger &target, const QString &action, const QJsonObject &payload) {
    auto envelope = makeEnvelope(MessageType::REQUEST, target, action);

    outgoingPendingRequests.insert(envelope.id, envelope);
    sendMessage(envelope, payload);
  }

  void emitEvent(const Messenger &target, const QString &action, const QJsonObject &payload) {
    auto envelope = makeEnvelope(MessageType::EVENT, target, action);

    sendMessage(envelope, payload);
  }

  void requestExtension(const QString &sessionId, const QString &action, const QJsonObject &payload) {
    Messenger target{.id = sessionId, .type = Messenger::Type::EXTENSION};

    request(target, action, payload);
  }

private slots:
  void handleMessage(FullMessage msg) {
    qDebug() << "[DEBUG] readyRead: got message of type" << msg.envelope.action;

    if (msg.envelope.type == MessageType::REQUEST) {
      incomingPendingRequests.insert(msg.envelope.id, msg.envelope);
    }

    if (msg.envelope.sender.type == Messenger::Type::MANAGER) {
      if (msg.envelope.type == MessageType::REQUEST) {
        qDebug() << "manager->main is not supported";
        return;
      }

      if (msg.envelope.type == MessageType::RESPONSE) {
        auto it = outgoingPendingRequests.find(msg.envelope.id);

        if (it == outgoingPendingRequests.end()) {
          qDebug() << "No matching request for manager response";
          return;
        }

        outgoingPendingRequests.remove(msg.envelope.id);
        emit managerResponse(msg.envelope.action, msg.data);
      }
    }

    if (msg.envelope.sender.type == Messenger::Type::EXTENSION) {
      if (msg.envelope.type == MessageType::EVENT) {
        emit extensionEvent(msg.envelope.sender.id, msg.envelope.action, msg.data);
      }
      if (msg.envelope.type == MessageType::REQUEST) {
        emit extensionRequest(msg.envelope.sender.id, msg.envelope.id, msg.envelope.action, msg.data);
      }
    }
  }

  void readyRead() {
    while (device->bytesAvailable() > 0) {
      auto read = device->readAll();

      _message.data.append(read);

      while (_message.data.size() >= sizeof(uint32_t)) {
        uint32_t length = ntohl(*reinterpret_cast<uint32_t *>(_message.data.data()));
        bool isComplete = _message.data.size() - sizeof(uint32_t) >= length;

        if (!isComplete) break;

        auto packet = _message.data.sliced(sizeof(uint32_t), length);

        emit worker->processData(packet);
        _message.data = _message.data.sliced(sizeof(uint32_t) + length);
      }
    }
  }

public:
signals:
  void managerResponse(const QString &action, QJsonObject &data);
  void extensionRequest(const QString &sessionId, const QString &id, const QString &action,
                        QJsonObject &data);
  void extensionEvent(const QString &sessionId, const QString &action, QJsonObject &data);

public:
  void requestManager(const QString &action, const QJsonObject &payload) {
    Messenger target{.type = Messenger::Type::MANAGER};

    request(target, action, payload);
  }

  bool respond(const QString &id, const QJsonObject &payload) {
    auto it = incomingPendingRequests.find(id);

    if (it == incomingPendingRequests.end()) {
      qDebug() << "no request with id " << id;
      return false;
    }

    auto envelope = *it;

    incomingPendingRequests.remove(id);
    envelope.target = envelope.sender;
    envelope.sender = selfMessenger;
    envelope.type = MessageType::RESPONSE;
    sendMessage(envelope, payload);

    return true;
  }

  void emitExtensionEvent(const QString &sessionId, const QString &action, const QJsonObject &payload) {
    Messenger target{.id = sessionId, .type = Messenger::Type::EXTENSION};
    auto envelope = makeEnvelope(MessageType::EVENT, target, action);

    sendMessage(envelope, payload);
  }

  void ping() { requestManager("ping", {}); }

  void loadCommand(const QString &extensionId, const QString &cmdName) {
    QJsonObject data;

    data["extensionId"] = extensionId;
    data["commandName"] = cmdName;

    requestManager("load-command", data);
  }

  void unloadCommand(const QString &sessionId) {
    QJsonObject data;

    data["sessionId"] = sessionId;

    requestManager("unload-command", data);
  }

  void flush() { emit device->readyRead(); }

  Bus(QIODevice *socket) : device(socket), workerThread(new QThread), worker(new DataDecoder) {
    connect(socket, &QIODevice::readyRead, this, &Bus::readyRead);
    connect(worker, &DataDecoder::messageParsed, this, &Bus::handleMessage);

    worker->moveToThread(workerThread);
    workerThread->start();
  }

  ~Bus() { device->deleteLater(); }
};

class ExtensionManager : public QObject {
  Q_OBJECT

  QProcess process;
  Bus bus;
  std::vector<std::shared_ptr<Extension>> loadedExtensions;
  OmniCommandDatabase &commandDb;

public:
  ExtensionManager(OmniCommandDatabase &commandDb) : commandDb(commandDb), bus(&process) {
    connect(&process, &QProcess::readyReadStandardError, this, &ExtensionManager::readError);
    connect(&process, &QProcess::finished, this, &ExtensionManager::finished);
    connect(&process, &QProcess::started, this, &ExtensionManager::processStarted);

    connect(&bus, &Bus::managerResponse, this, &ExtensionManager::handleManagerResponse);
    connect(&bus, &Bus::extensionEvent, this, &ExtensionManager::extensionEvent);
    connect(&bus, &Bus::extensionRequest, this, &ExtensionManager::extensionRequest);
  }

  const std::vector<std::shared_ptr<Extension>> &extensions() const { return loadedExtensions; }

  bool respond(const QString &id, const QJsonObject &payload) { return bus.respond(id, payload); }

  void emitExtensionEvent(const QString &sessionId, const QString &action, const QJsonObject &payload) {
    return bus.emitExtensionEvent(sessionId, action, payload);
  }

  void requestManager(const QString &action, const QJsonObject &payload) {
    return bus.requestManager(action, payload);
  }

  void processStarted() { bus.requestManager("list-extensions", {}); }

public slots:
  bool start() {
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

  void startDevelopmentSession(const QString &id) {
    QJsonObject data;

    data["id"] = id;
    requestManager("develop.start", data);
  }

  void refreshDevelopmentSession(const QString &id) {
    QJsonObject data;

    data["id"] = id;
    requestManager("develop.refresh", data);
  }

  void stopDevelopmentSession(const QString &id) {
    QJsonObject data;

    data["id"] = id;
    requestManager("develop.stop", data);
  }

  void loadCommand(const QString &extensionId, const QString &cmd, const QJsonObject &preferenceValues = {}) {
    QJsonObject payload;

    payload["extensionId"] = extensionId;
    payload["commandName"] = cmd;
    payload["preferenceValues"] = preferenceValues;

    bus.requestManager("load-command", payload);
  }

  void unloadCommand(const QString &sessionId) {
    QJsonObject payload;

    payload["sessionId"] = sessionId;

    bus.requestManager("unload-command", payload);
  }

  void flush() { bus.flush(); }

  void parseListExtensionData(QJsonObject &obj) {
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

  void handleManagerResponse(const QString &action, QJsonObject &data) {
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

signals:
  void extensionEvent(const QString &sessionId, const QString &action, const QJsonObject &payload);
  void extensionRequest(const QString &sessionId, const QString &id, const QString &action,
                        const QJsonObject &payload);
  void managerResponse(const QString &action, QJsonObject &payload);
  void commandLoaded(const LoadedCommand &);

private slots:
  void finished(int exitCode, QProcess::ExitStatus status) {
    qDebug() << "Extension manager exited prematurely. Extensions will not work";
  }

  void readError() {
    auto buf = process.readAllStandardError();

    QTextStream(stderr) << buf;
  }
};
