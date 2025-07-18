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
#include "protocols/extension.pb.h"
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

class Bus : public QObject {
  Q_OBJECT

  struct MessageBuffer {
    QByteArray data;
    uint32_t length;
  };

  MessageBuffer _message = {.length = 0};

  QIODevice *device = nullptr;
  void sendMessage(const QByteArray &data);
  void handleMessage(const protocols::extensions::IpcMessage &message);
  void readyRead();

public:
  void requestManager(protocols::extensions::ManagerRequestData *req);
  bool respondToExtension(const QString &requestId, protocols::extensions::ExtensionResponseData *data);
  void emitExtensionEvent(protocols::extensions::QualifiedExtensionEvent *event);
  void ping();
  void loadCommand(const QString &extensionId, const QString &cmdName);
  void unloadCommand(const QString &sessionId);

  Bus(QIODevice *socket);

signals:
  void managerResponse(const protocols::extensions::ManagerResponse &res);
  void extensionRequest(const protocols::extensions::QualifiedExtensionRequest &req);
  void extensionEvent(const protocols::extensions::QualifiedExtensionEvent &event);
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

  void requestManager(protocols::extensions::ManagerRequestData *req);
  bool respondToExtension(const QString &requestId, protocols::extensions::ExtensionResponseData *data);
  void emitExtensionEvent(protocols::extensions::QualifiedExtensionEvent *event);

  void processStarted();
  static QJsonObject serializeLaunchProps(const LaunchProps &props);

  bool start();

  void loadCommand(const QString &extensionId, const QString &cmd, const QJsonObject &preferenceValues = {},
                   const LaunchProps &launchProps = {});
  void unloadCommand(const QString &sessionId);
  void handleManagerResponse(const QString &action, QJsonObject &data);
  void finished(int exitCode, QProcess::ExitStatus status);
  void readError();

signals:
  void managerResponse(const protocols::extensions::ManagerResponse &res);
  void extensionRequest(const protocols::extensions::QualifiedExtensionRequest &req);
  void extensionEvent(const protocols::extensions::QualifiedExtensionEvent &event);

  void commandLoaded(const LoadedCommand &);
};
