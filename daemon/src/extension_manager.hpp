#pragma once
#include <QDebug>
#include <QJsonArray>
#include <QString>
#include <QUuid>
#include <qdir.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qlogging.h>
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

static const char *exec =
    "/home/aurelle/prog/perso/omnicast-sdk/extension-manager/dist/index.js";

class ExtensionManager : public QObject {
  Q_OBJECT

  QProcess process;
  QLocalServer ipc;
  QString socketPath;
  QLocalSocket *managerSocket = nullptr;
  QList<Extension> loadedExtensions;

  Message parsePacket(QByteArray &buf) {
    QDataStream dataStream(&buf, QIODevice::ReadOnly);
    QByteArray data;

    dataStream >> data;

    auto json = QJsonDocument::fromJson(data);

    qDebug() << "type=" << json["type"].toString();

    auto id = json["id"].toString();
    auto type = json["type"].toString();
    auto mdat = json["data"].toObject();

    qDebug() << "Extension ID" << json["extensionId"];
    qDebug() << "Command name" << json["commandName"];

    return {id, type, mdat};
  }

  void writePacket(const QString &message) {
    QByteArray data;
    QDataStream dataStream(&data, QIODevice::WriteOnly);

    dataStream << message.toUtf8();

    managerSocket->write(data);
    managerSocket->waitForBytesWritten();
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

  void reply(const Message &message, QJsonObject data) {
    writeMessage(Message::createReply(message, data));
  }

  const QList<Extension> &extensions() { return loadedExtensions; }

  void activateCommand(const QString &extensionId, const QString &cmdName) {
    QJsonObject data;

    data["extensionId"] = extensionId;
    data["commandName"] = cmdName;

    writeMessage(Message::create("activate-command", data));
  }

  void deactivateCommand(const QString &extensionId, const QString &cmdName) {
    QJsonObject data;

    data["extensionId"] = extensionId;
    data["commandName"] = cmdName;

    writeMessage(Message::create("deactivate-command", data));
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
  void extensionMessage(const Message &msg);

private slots:
  void newConnection() {
    qDebug() << "new connection";
    managerSocket = ipc.nextPendingConnection();
    writeMessage(Message::create("ping", {}));

    managerSocket->waitForReadyRead();
    qDebug() << "ready read";
    auto buf = managerSocket->readAll();

    qDebug() << "Got back a packet of size" << buf.size() << buf;
    auto msg = parsePacket(buf);

    if (msg.type != "pong") {
      qDebug() << "pong expected, got " << msg.type;
    }

    connect(managerSocket, &QLocalSocket::readyRead, this,
            &ExtensionManager::readLocalSocket);

    writeMessage(Message::create("list-extensions", {}));
  }

  void readLocalSocket() {
    auto buf = managerSocket->readAll();
    auto msg = parsePacket(buf);

    qDebug() << "got message of type " << msg.type;

    if (msg.type == "list-extensions") {
      parseListExtensionData(msg.data);
    } else {
      emit extensionMessage(msg);
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
