#include "services/clipboard/wlr-clipboard-server.hpp"
#include "build/wlr-clip/proto/wlr-clipboard.pb.h"
#include "services/clipboard/clipboard-server.hpp"
#include "vicinae.hpp"
#include <QtCore>
#include <algorithm>
#include <filesystem>
#include <netinet/in.h>
#include <qlogging.h>
#include <qprocess.h>
#include <qdebug.h>
#include <qresource.h>
#include <qstringview.h>

bool WlrClipboardServer::isAlive() const { return process->isOpen(); }

bool WlrClipboardServer::isActivatable() const { return QApplication::platformName() == "wayland"; }

void WlrClipboardServer::handleMessage(const proto::ext::wlrclip::Selection &sel) {
  ClipboardSelection cs;

  cs.offers.reserve(sel.offers().size());

  for (const auto &offer : sel.offers()) {
    cs.offers.push_back({offer.mime_type().c_str(), QByteArray::fromStdString(offer.data())});
  }

  emit selection(cs);
}

void WlrClipboardServer::handleExit(int code, QProcess::ExitStatus status) {}

bool WlrClipboardServer::start() {
  process = new QProcess;
  QFile bin(":bin/wlr-clip");

  if (!bin.exists()) {
    qWarning() << ":bin/wlr-clip resource could not found, can't start clipboard server";
    return false;
  }

  bin.open(QIODevice::ReadOnly);

  std::filesystem::path target = Omnicast::runtimeDir() / "wlr-clip";
  std::filesystem::remove(target);

  bin.copy(target);

#ifdef Q_OS_UNIX
  QFile::setPermissions(target, QFile::ExeOwner | QFile::ReadOwner | QFile::WriteOwner);
#endif

  connect(process, &QProcess::readyReadStandardOutput, this, &WlrClipboardServer::handleRead);
  connect(process, &QProcess::readyReadStandardError, this, &WlrClipboardServer::handleReadError);
  connect(process, &QProcess::finished, this, &WlrClipboardServer::handleExit);

  process->start(target.c_str(), {});

  if (!process->waitForStarted(2000)) {
    qCritical() << "Failed to start wlr-clip" << target.c_str() << process->errorString();
    return false;
  }

  return process;
}

void WlrClipboardServer::handleReadError() { QTextStream(stderr) << process->readAllStandardError(); }

void WlrClipboardServer::handleRead() {
  auto array = process->readAllStandardOutput();
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

WlrClipboardServer::WlrClipboardServer() : AbstractClipboardServer(WlrootsDataControlClipboardServer) {}
