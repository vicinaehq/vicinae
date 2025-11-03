#include "ext-clipboard-server.hpp"
#include "pid-file/pid-file.hpp"
#include "proto/wlr-clipboard.pb.h"
#include "services/clipboard/clipboard-server.hpp"
#include "wayland/globals.hpp"
#include <QtCore>
#include <QApplication>
#include <netinet/in.h>
#include <qlogging.h>
#include <qprocess.h>
#include <qdebug.h>
#include <qresource.h>
#include <qstringview.h>

bool ExtDataControlClipboardServer::isAlive() const { return m_process.isOpen(); }

bool ExtDataControlClipboardServer::isActivatable() const {
  return Wayland::Globals::dataControlDeviceManager();
}

void ExtDataControlClipboardServer::handleMessage(const proto::ext::wlrclip::Selection &sel) {
  ClipboardSelection cs;

  cs.offers.reserve(sel.offers().size());

  for (const auto &offer : sel.offers()) {
    cs.offers.push_back({offer.mime_type().c_str(), QByteArray::fromStdString(offer.data())});
  }

  emit selectionAdded(cs);
}

void ExtDataControlClipboardServer::handleExit(int code, QProcess::ExitStatus status) {
  if (status == QProcess::ExitStatus::CrashExit) {
    qCritical() << "ext-clipboard process exited with status code" << status;
  }
}

QString ExtDataControlClipboardServer::id() const { return "ext-clipboard"; }

int ExtDataControlClipboardServer::activationPriority() const { return 5; }

bool ExtDataControlClipboardServer::stop() {
  m_process.terminate();
  return true;
}

bool ExtDataControlClipboardServer::start() {
  PidFile pidFile(ENTRYPOINT);
  int maxWaitForStart = 5000;
  std::error_code ec;

  if (pidFile.exists() && pidFile.kill()) { qInfo() << "Killed existing ext-clip instance"; }

  m_process.start("/proc/self/exe", {ENTRYPOINT});

  if (!m_process.waitForStarted(maxWaitForStart)) {
    qCritical() << "Failed to start ext-clipboard process" << m_process.errorString();
    return false;
  }

  pidFile.write(m_process.processId());

  return m_process.state() == QProcess::ProcessState::Running;
}

void ExtDataControlClipboardServer::handleReadError() {
  QTextStream(stderr) << m_process.readAllStandardError();
}

void ExtDataControlClipboardServer::handleRead() {
  auto array = m_process.readAllStandardOutput();
  auto _buf = array.constData();

  _message.insert(_message.end(), _buf, _buf + array.size());

  if (_messageLength == 0 && _message.size() > sizeof(uint32_t)) {
    _messageLength = ntohl(*reinterpret_cast<uint32_t *>(_message.data()));
    _message.erase(_message.begin(), _message.begin() + sizeof(uint32_t));
  }

  if (_message.size() >= _messageLength) {
    std::string data(_message.begin(), _message.begin() + _messageLength);
    proto::ext::wlrclip::Selection selection;

    if (!selection.ParseFromString(data)) {
      qWarning() << "Failed to parse selection";
    } else {
      handleMessage(selection);
    }

    _message.erase(_message.begin(), _message.begin() + _messageLength);
    _messageLength = 0;
  }
}

ExtDataControlClipboardServer::ExtDataControlClipboardServer() {
  connect(&m_process, &QProcess::readyReadStandardOutput, this, &ExtDataControlClipboardServer::handleRead);
  connect(&m_process, &QProcess::readyReadStandardError, this,
          &ExtDataControlClipboardServer::handleReadError);
  connect(&m_process, &QProcess::finished, this, &ExtDataControlClipboardServer::handleExit);
}
