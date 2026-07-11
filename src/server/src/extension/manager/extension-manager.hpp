#pragma once
#include <QDebug>
#include <QJsonArray>
#include <QString>
#include "generated/manager.hpp"
#include "extension/node-runtime/node-runtime.hpp"
#include <QUuid>
#include <QtCore>
#include <cstdint>
#include "common.hpp"
#include <qdebug.h>
#include <qdir.h>
#include <qfuturewatcher.h>
#include <qhash.h>
#include <qimage.h>
#include <qjsonarray.h>
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
#include <quuid.h>

class Bus : public QObject, public manager::AbstractTransport {
  Q_OBJECT

  struct MessageBuffer {
    QByteArray data;
    uint32_t length;
  };

  MessageBuffer _message = {.length = 0};

  QIODevice *device = nullptr;
  void sendMessage(const QByteArray &data);
  void readyRead();
  void send(std::string_view data) override;

public:
  Bus(QIODevice *socket);

signals:
  void messageReceived(const QByteArray &msg);
};

struct PendingManagerRequestInfo {
  QString sessionId;
};

class ExtensionManager : public QObject {
  Q_OBJECT

signals:
  void started() const;
  void extensionMessageReceived(const std::string &sessionId, std::string_view data) const;
  void extensionCrashed(const std::string &sessionId, const std::string &reason) const;

public:
  ExtensionManager();

  void addDevelopmentSession(const QString &id);
  void removeDevelopmentSession(const QString &id);
  bool hasDevelopmentSession(const QString &id) const;

  void processStarted();

  bool isRunning() const;
  bool start();
  bool stop();

  void loadCommand(const QString &extensionId, const QString &cmd, const QJsonObject &preferenceValues = {},
                   const LaunchProps &launchProps = {});

  manager::Client &client() { return m_client; }

  // void unloadCommand(const QString &sessionId);
  void handleManagerResponse(const QString &action, QJsonObject &data);
  void finished(int exitCode, QProcess::ExitStatus status);
  void readError();

private:
  QProcess m_process;
  Bus m_bus;
  manager::RpcTransport m_rpc;
  manager::Client m_client;
  NodeRuntime m_node;
  bool m_stopping = false;
  std::unordered_set<QString> m_developmentSessions;
};
