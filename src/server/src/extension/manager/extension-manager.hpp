#pragma once
#include <QDebug>
#include <QJsonArray>
#include <QString>
#include <QUuid>
#include <QtCore>
#include <cstdint>
#include "common.hpp"
#include "proto/common.pb.h"
#include "proto/extension.pb.h"
#include "proto/ipc.pb.h"
#include "proto/manager.pb.h"
#include <netinet/in.h>
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
#include <unistd.h>
#include "common/types.hpp"

class ManagerRequest : public QObject {
  Q_OBJECT

signals:
  void finished(proto::ext::manager::ResponseData data);
};

class Bus : public QObject {
  Q_OBJECT

  struct MessageBuffer {
    QByteArray data;
    uint32_t length;
  };

  std::unordered_map<std::string, ManagerRequest *> m_pendingManagerRequests;
  MessageBuffer _message = {.length = 0};

  QIODevice *device = nullptr;
  void sendMessage(const QByteArray &data);
  void handleMessage(const proto::ext::IpcMessage &message);
  void readyRead();

public:
  ManagerRequest *requestManager(proto::ext::manager::RequestData *req);
  bool respondToExtension(const QString &sessionId, const QString &requestId,
                          proto::ext::extension::Response *data);
  void emitExtensionEvent(proto::ext::QualifiedExtensionEvent *event);
  void ping();

  Bus(QIODevice *socket);

signals:
  void managerResponse(const proto::ext::ManagerResponse &res);
  void extensionRequest(const proto::ext::QualifiedExtensionRequest &req);
  void extensionEvent(const proto::ext::QualifiedExtensionEvent &event);
};

class ExtensionRequest : NonCopyable {
  proto::ext::QualifiedExtensionRequest m_request;
  Bus &m_bus;
  bool m_responded = false;

public:
  ExtensionRequest(Bus &bus, const proto::ext::QualifiedExtensionRequest &req) : m_bus(bus), m_request(req) {}

  ~ExtensionRequest() {
    if (!m_responded) { respondWithError("Unhandled request"); }
  }

  QString requestId() const { return QString::fromStdString(m_request.request().request_id()); }
  QString sessionId() const { return QString::fromStdString(m_request.session_id()); }
  const proto::ext::extension::RequestData &requestData() const { return m_request.request().data(); }

  void respond(proto::ext::extension::Response *data) {
    if (m_responded) {
      qCritical() << "Request" << requestId() << "already responded";
      return;
    }

    data->set_request_id(requestId().toStdString());
    m_bus.respondToExtension(sessionId(), requestId(), data);
    m_responded = true;
  }

  void respondWithError(const QString &errorText) {
    auto error = new proto::ext::common::ErrorResponse;
    auto res = new proto::ext::extension::Response;

    error->set_error_text(errorText.toStdString());
    res->set_allocated_error(error);
    respond(res);
  }
};

class ExtensionEvent {
  proto::ext::QualifiedExtensionEvent m_event;

public:
  QString sessionId() const { return QString::fromStdString(m_event.session_id()); };
  const proto::ext::extension::Event *data() const { return &m_event.event(); }

  ExtensionEvent(const proto::ext::QualifiedExtensionEvent &event) : m_event(event) {}
};

struct PendingManagerRequestInfo {
  QString sessionId;
};

class ExtensionManager : public QObject {
  Q_OBJECT

signals:
  void managerResponse(const proto::ext::ManagerResponse &res);
  void extensionRequest(ExtensionRequest *req);
  void extensionEvent(const ExtensionEvent &event);
  void started() const;

public:
  ExtensionManager();

  /**
   * Return the node executable used to spawn the extension manager.
   * We first look for the first `vicinae-node` binary in PATH and then do the same for `node`.
   * `vicinae-node` is used by script installations and have a specific name to ensure we do not
   * create conflict with existing node installations.
   * If we are running from an appimage, the path to the internal distribution of node is returned.
   */
  std::optional<std::filesystem::path> nodeExecutable();

  ManagerRequest *requestManager(proto::ext::manager::RequestData *req);
  bool respondToExtension(const QString &requestId, proto::ext::extension::ResponseData *data);
  void emitExtensionEvent(proto::ext::QualifiedExtensionEvent *event);
  void emitGenericExtensionEvent(const QString &sessionId, const QString &handlerId, const QJsonArray &args);

  void addDevelopmentSession(const QString &id);
  void removeDevelopmentSession(const QString &id);
  bool hasDevelopmentSession(const QString &id) const;

  void processStarted();
  static QJsonObject serializeLaunchProps(const LaunchProps &props);

  bool isRunning() const;
  bool start();
  bool stop();

  void loadCommand(const QString &extensionId, const QString &cmd, const QJsonObject &preferenceValues = {},
                   const LaunchProps &launchProps = {});

  void unloadCommand(const QString &sessionId);
  void handleManagerResponse(const QString &action, QJsonObject &data);
  void finished(int exitCode, QProcess::ExitStatus status);
  void readError();

private:
  QProcess m_process;
  Bus m_bus;
  std::unordered_set<QString> m_developmentSessions;
};
