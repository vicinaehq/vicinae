#include "wlr-clipboard-server.hpp"
#include "pid-file/pid-file.hpp"
#include "proto/wlr-clipboard.pb.h"
#include "services/clipboard/clipboard-server.hpp"
#include <QtCore>
#include <QApplication>
#include <netinet/in.h>
#include <qlogging.h>
#include <qprocess.h>
#include <qdebug.h>
#include <qresource.h>
#include <qstringview.h>
#include "lib/wayland/globals.hpp"

bool WlrClipboardServer::isAlive() const { return m_process.isOpen(); }

bool WlrClipboardServer::isActivatable() const { return Wayland::Globals::wlrDataControlManager(); }

void WlrClipboardServer::handleMessage(const proto::ext::wlrclip::Selection &sel) {
  ClipboardSelection cs;

  cs.offers.reserve(sel.offers().size());

  for (const auto &offer : sel.offers()) {
    cs.offers.push_back({offer.mime_type().c_str(), QByteArray::fromStdString(offer.data())});
  }

  emit selectionAdded(cs);
}

void WlrClipboardServer::handleExit(int code, QProcess::ExitStatus status) {
  if (status == QProcess::ExitStatus::CrashExit) {
    qCritical() << "wlr-clipboard process exited with status code" << status;
  }
}

QString WlrClipboardServer::id() const { return "wlr-clipboard"; }

int WlrClipboardServer::activationPriority() const { return 1; }

bool WlrClipboardServer::stop() {
  m_process.terminate();
  return true;
}

bool WlrClipboardServer::start() {
  PidFile pidFile(ENTRYPOINT);
  int maxWaitForStart = 5000;
  std::error_code ec;

  if (pidFile.exists() && pidFile.kill()) { qInfo() << "Killed existing wlr-clip instance"; }

  m_process.start("/proc/self/exe", {ENTRYPOINT});

  if (!m_process.waitForStarted(maxWaitForStart)) {
    qCritical() << "Failed to start wlr-clipboard process" << m_process.errorString();
    return false;
  }

  pidFile.write(m_process.processId());

  return m_process.state() == QProcess::ProcessState::Running;
}

void WlrClipboardServer::handleReadError() { QTextStream(stderr) << m_process.readAllStandardError(); }

void WlrClipboardServer::handleRead() {
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

WlrClipboardServer::WlrClipboardServer() {
  connect(&m_process, &QProcess::readyReadStandardOutput, this, &WlrClipboardServer::handleRead);
  connect(&m_process, &QProcess::readyReadStandardError, this, &WlrClipboardServer::handleReadError);
  connect(&m_process, &QProcess::finished, this, &WlrClipboardServer::handleExit);
}
