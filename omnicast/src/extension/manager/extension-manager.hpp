#pragma once
#include <QDebug>
#include <QJsonArray>
#include <QString>
#include <QUuid>
#include <QtCore>
#include <cstdint>
#include "common.hpp"
#include "extension/extension.hpp"
#include "omni-command-db.hpp"
#include <netinet/in.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfuturewatcher.h>
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

enum ExtensionMessageType { REQUEST, RESPONSE, EVENT };

struct Messenger {
  enum Type { MAIN, MANAGER, EXTENSION };

  QString id;
  Type type;
};

struct MessageEnvelope {
  QString id;
  ExtensionMessageType type;
  Messenger sender;
  Messenger target;
  QString action;
};

struct FullMessage {
  MessageEnvelope envelope;
  QJsonObject data;
};

class Bus : public QObject {
  Q_OBJECT

  struct MessageBuffer {
    QByteArray data;
    uint32_t length;
  };

  MessageBuffer _message = {.length = 0};
  QFutureWatcher<FullMessage> m_parseMessageTask;
  std::map<QString, MessageEnvelope> m_outgoingPendingRequests;
  std::map<QString, MessageEnvelope> m_incomingPendingRequests;
  Messenger selfMessenger{.type = Messenger::MAIN};

  QIODevice *device = nullptr;
  QJsonObject serializeMessenger(const Messenger &lhs);
  MessageEnvelope makeEnvelope(ExtensionMessageType type, const Messenger &target, const QString &action);
  void sendMessage(const MessageEnvelope &envelope, const QJsonObject &payload);
  void request(const Messenger &target, const QString &action, const QJsonObject &payload);
  void emitEvent(const Messenger &target, const QString &action, const QJsonObject &payload);
  void requestExtension(const QString &sessionId, const QString &action, const QJsonObject &payload);
  void handleMessage(FullMessage &message);
  void readyRead();

  static Messenger parseMessenger(const QJsonObject &lhs);
  static MessageEnvelope parseEnvelope(const QJsonObject &lhs);
  static FullMessage parseFullMessage(const QJsonObject &lhs);
  static FullMessage parseRawMessage(const QByteArray &data);

public:
  void requestManager(const QString &action, const QJsonObject &payload);
  bool respond(const QString &id, const QJsonObject &payload);
  void emitExtensionEvent(const QString &sessionId, const QString &action, const QJsonObject &payload);
  void ping();
  void loadCommand(const QString &extensionId, const QString &cmdName);
  void unloadCommand(const QString &sessionId);

  Bus(QIODevice *socket);

signals:
  void managerResponse(const QString &action, QJsonObject &data);
  void extensionRequest(const QString &sessionId, const QString &id, const QString &action,
                        QJsonObject &data);
  void extensionEvent(const QString &sessionId, const QString &action, QJsonObject &data);
};

class ExtensionManager : public QObject {
  Q_OBJECT

  QProcess process;
  Bus bus;
  std::vector<std::shared_ptr<Extension>> loadedExtensions;
  OmniCommandDatabase &commandDb;

public:
  ExtensionManager(OmniCommandDatabase &commandDb);

  const std::vector<std::shared_ptr<Extension>> &extensions() const;
  bool respond(const QString &id, const QJsonObject &payload);
  void emitExtensionEvent(const QString &sessionId, const QString &action, const QJsonObject &payload);
  void requestManager(const QString &action, const QJsonObject &payload);
  void processStarted();
  static QJsonObject serializeLaunchProps(const LaunchProps &props);

  bool start();

  void startDevelopmentSession(const QString &id);
  void refreshDevelopmentSession(const QString &id);
  void stopDevelopmentSession(const QString &id);

  void loadCommand(const QString &extensionId, const QString &cmd, const QJsonObject &preferenceValues = {},
                   const LaunchProps &launchProps = {});
  void unloadCommand(const QString &sessionId);
  void parseListExtensionData(QJsonObject &obj);
  void handleManagerResponse(const QString &action, QJsonObject &data);
  void finished(int exitCode, QProcess::ExitStatus status);
  void readError();

signals:
  void extensionEvent(const QString &sessionId, const QString &action, const QJsonObject &payload);
  void extensionRequest(const QString &sessionId, const QString &id, const QString &action,
                        const QJsonObject &payload);
  void managerResponse(const QString &action, QJsonObject &payload);
  void commandLoaded(const LoadedCommand &);
};
