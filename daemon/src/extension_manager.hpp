#pragma once
#include <QDebug>
#include <QJsonArray>
#include <QString>
#include <QUuid>
#include <qdir.h>
#include <qhash.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qprocess.h>
#include <qstringview.h>
#include <qtypes.h>
#include <quuid.h>
#include <unistd.h>

struct Message {
  QString id;
  QString type;
  QJsonObject data;

  static Message create(const QString &type, QJsonObject data) {
    auto id = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);

    return {id, type, data};
  }

  static Message createReply(const Message &lhs, QJsonObject data) {
    return {lhs.id, lhs.type, data};
  }
};

struct Extension {
  struct Command {
    QString name;
    QString title;
    QString subtitle;
    QString description;
    QString mode;
    QString extensionId;
  };

  QString sessionId;
  QList<Extension::Command> commands;
};

struct LoadedCommand {
  QString sessionId;
  struct {
    QString name;
  } command;
};

static const char *exec =
    "/home/aurelle/prog/perso/omnicast-sdk/extension-manager/dist/index.js";

class ExtensionManager : public QObject {
  Q_OBJECT

  enum MessageType { REQUEST, RESPONSE, EVENT };

  QHash<MessageType, QString> messageTypeToString = {
      {MessageType::REQUEST, "request"},
      {MessageType::RESPONSE, "response"},
      {MessageType::EVENT, "event"},
  };

  QHash<QString, MessageType> stringToMessageType = {
      {"request", MessageType::REQUEST},
      {"response", MessageType::RESPONSE},
      {"event", MessageType::EVENT},
  };

  struct Messenger {
    enum Type { MAIN, MANAGER, EXTENSION };

    QString id;
    Type type;
  };

  QHash<Messenger::Type, QString> messengerTypeToString = {
      {Messenger::Type::MAIN, "main"},
      {Messenger::Type::MANAGER, "manager"},
      {Messenger::Type::EXTENSION, "extension"},
  };

  QHash<QString, Messenger::Type> stringToMessengerType = {
      {"main", Messenger::Type::MAIN},
      {"manager", Messenger::Type::MANAGER},
      {"extension", Messenger::Type::EXTENSION},
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

  Messenger selfMessenger{.type = Messenger::MAIN};

  QHash<QString, MessageEnvelope> outgoingPendingRequests;
  QHash<QString, MessageEnvelope> incomingPendingRequests;

  QProcess process;
  QLocalServer ipc;
  QString socketPath;
  QLocalSocket *managerSocket = nullptr;
  QList<Extension> loadedExtensions;

  FullMessage parsePacket(QByteArray &buf) {
    QDataStream dataStream(&buf, QIODevice::ReadOnly);
    QByteArray data;

    dataStream >> data;

    auto json = QJsonDocument::fromJson(data);

    QTextStream(stdout) << json.toJson();

    return parseFullMessage(json.object());
  }

  QJsonObject serializeMessenger(const Messenger &lhs) {
    QJsonObject obj;

    obj["id"] = lhs.id;
    obj["type"] = messengerTypeToString[lhs.type];

    return obj;
  }

  Messenger parseMessenger(const QJsonObject &lhs) {
    Messenger messenger{.id = lhs["id"].toString(),
                        .type = stringToMessengerType[lhs["type"].toString()]};

    return messenger;
  }

  MessageEnvelope parseEnvelope(const QJsonObject &lhs) {
    MessageEnvelope envelope;

    qDebug() << "type=" << lhs["type"].toString();
    qDebug() << "i=" << stringToMessageType[lhs["type"].toString().toUtf8()];

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

  MessageEnvelope makeEnvelope(MessageType type, const Messenger &target,
                               const QString &action) {
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

  void sendMessage(const MessageEnvelope &envelope,
                   const QJsonObject &payload) {
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
    managerSocket->write(data);
    managerSocket->waitForBytesWritten();
  }

  void request(const Messenger &target, const QString &action,
               const QJsonObject &payload) {
    auto envelope = makeEnvelope(MessageType::REQUEST, target, action);

    outgoingPendingRequests.insert(envelope.id, envelope);
    sendMessage(envelope, payload);
  }

  void emitEvent(const Messenger &target, const QString &action,
                 const QJsonObject &payload) {
    auto envelope = makeEnvelope(MessageType::EVENT, target, action);

    sendMessage(envelope, payload);
  }

  void requestManager(const QString &action, const QJsonObject &payload) {
    Messenger target{.type = Messenger::Type::MANAGER};

    request(target, action, payload);
  }

  void requestExtension(const QString &sessionId, const QString &action,
                        const QJsonObject &payload) {
    Messenger target{.id = sessionId, .type = Messenger::Type::EXTENSION};

    request(target, action, payload);
  }

  void writeMessage(const Message &msg) {
    QJsonObject obj;

    obj["id"] = msg.id;
    obj["type"] = msg.type;
    obj["data"] = msg.data;

    QJsonDocument doc(obj);

    QByteArray data;
    QDataStream dataStream(&data, QIODevice::WriteOnly);

    dataStream << doc.toJson();
    managerSocket->write(data);
    managerSocket->waitForBytesWritten();
  }

  void parseListExtensionData(QJsonObject &obj) {
    QList<Extension> extensions;

    for (const auto &ext : obj["extensions"].toArray()) {
      auto extObj = ext.toObject();
      Extension extension{.sessionId = extObj["sessionId"].toString()};

      for (const auto &cmd : extObj["commands"].toArray()) {
        auto cmdObj = cmd.toObject();
        Extension::Command finalCmd{.name = cmdObj["name"].toString(),
                                    .title = cmdObj["title"].toString(),
                                    .subtitle = cmdObj["subtitle"].toString(),
                                    .description =
                                        cmdObj["description"].toString(),
                                    .mode = cmdObj["mode"].toString(),
                                    .extensionId = extension.sessionId};

        extension.commands.push_back(finalCmd);
      }

      extensions.push_back(extension);
    }

    loadedExtensions = extensions;
  }

public:
  ExtensionManager() : socketPath("/tmp/omnicast-extension-manager.sock") {}

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

  void emitExtensionEvent(const QString &sessionId, const QString &action,
                          const QJsonObject &payload) {
    Messenger target{.id = sessionId, .type = Messenger::Type::EXTENSION};

    request(target, action, payload);
  }

  void reply(const Message &message, QJsonObject data) {
    writeMessage(Message::createReply(message, data));
  }

  const QList<Extension> &extensions() { return loadedExtensions; }

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

  void sendCommand(const QString &type, QJsonObject obj) {
    writeMessage(Message::create(type, obj));
  }

  void start() {
    QFile file(socketPath);

    if (file.exists())
      file.remove();

    if (!ipc.listen(socketPath)) {
      qDebug() << "failed to listen on socket path" << socketPath;
      return;
    }

    auto environ = QProcess::systemEnvironment();

    environ << "OMNICAST_SERVER_URL=" + socketPath;

    process.setWorkingDirectory(
        "/home/aurelle/.local/share/omnicast/extensions");

    connect(&process, &QProcess::readyReadStandardOutput, this,
            &ExtensionManager::readOutput);
    connect(&process, &QProcess::readyReadStandardError, this,
            &ExtensionManager::readError);
    connect(&process, &QProcess::finished, this, &ExtensionManager::finished);
    connect(&ipc, &QLocalServer::newConnection, this,
            &ExtensionManager::newConnection);

    process.setEnvironment(environ);
    process.start("/bin/node", {"runtime.js"});
  }

signals:
  void extensionMessage(const QString &sessionId, const QString &action,
                        const QJsonObject &payload);
  void extensionEvent(const QString &sessionId, const QString &action,
                      const QJsonObject &payload);
  void extensionRequest(const QString &sessionId, const QString &id,
                        const QString &action, const QJsonObject &payload);

  void commandLoaded(const LoadedCommand &);

private slots:
  void newConnection() {
    qDebug() << "new connection";
    managerSocket = ipc.nextPendingConnection();
    requestManager("ping", {});

    managerSocket->waitForReadyRead();
    auto buf = managerSocket->readAll();

    qDebug() << "Got back a packet of size" << buf.size() << buf;
    auto msg = parsePacket(buf);

    if (msg.envelope.action != "pong") {
      qDebug() << "pong expected, got " << msg.envelope.action;
    }

    qDebug() << "received pong";

    connect(managerSocket, &QLocalSocket::readyRead, this,
            &ExtensionManager::readLocalSocket);

    requestManager("list-extensions", {});
  }

  void handleManagerResponse(FullMessage &msg) {
    QString &action = msg.envelope.action;

    if (action == "list-extensions") {
      return parseListExtensionData(msg.data);
    }

    if (action == "load-command") {
      auto sessionId = msg.data["sessionId"].toString();

      LoadedCommand cmd;

      cmd.sessionId = sessionId;
      cmd.command.name = msg.data["command"].toObject()["name"].toString();

      emit commandLoaded(cmd);
    }
  }

  void readLocalSocket() {
    while (managerSocket->bytesAvailable() > 0) {
      auto buf = managerSocket->readAll();
      auto msg = parsePacket(buf);

      qDebug() << "got message of type " << msg.envelope.id << msg.envelope.type
               << msg.envelope.action;

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
          handleManagerResponse(msg);
        }
      }

      if (msg.envelope.sender.type == Messenger::Type::EXTENSION) {
        if (msg.envelope.type == MessageType::EVENT) {
          emit extensionEvent(msg.envelope.sender.id, msg.envelope.action,
                              msg.data);
        }
        if (msg.envelope.type == MessageType::REQUEST) {
          emit extensionRequest(msg.envelope.sender.id, msg.envelope.id,
                                msg.envelope.action, msg.data);
        }
      }
    }
    // QTextStream(stdout) << QJsonDocument(msg.data).toJson();
  }

  void finished(int exitCode, QProcess::ExitStatus status) {
    qDebug()
        << "Extension manager exited prematurely. Extensions will not work";
  }

  void readOutput() {
    auto buf = process.readAllStandardOutput();

    QTextStream(stdout) << buf;
  }

  void readError() {
    auto buf = process.readAllStandardError();

    QTextStream(stderr) << buf;
  }
};
