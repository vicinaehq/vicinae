#include "wlr-clipboard-server.hpp"
#include "proto/wlr-clipboard.pb.h"
#include "services/clipboard/clipboard-server.hpp"
#include "vicinae.hpp"
#include <QtCore>
#include <QApplication>
#include <filesystem>
#include <netinet/in.h>
#include <qlogging.h>
#include <qprocess.h>
#include <qdebug.h>
#include <qresource.h>
#include <qstringview.h>

bool WlrClipboardServer::isAlive() const { return process->isOpen(); }

bool WlrClipboardServer::isActivatable() const {
  // Check if we're on Wayland
  if (QApplication::platformName() != "wayland") { return false; }

  // Check if we're in a GNOME environment
  // GNOME doesn't support wlr-data-control protocol, so we need to fall back
  // to GnomeClipboardServer for clipboard functionality in GNOME environments
  const QString desktop = qgetenv("XDG_CURRENT_DESKTOP");
  const QString session = qgetenv("GDMSESSION");

  if (desktop.contains("GNOME", Qt::CaseInsensitive) || session.contains("gnome", Qt::CaseInsensitive)) {
    return false;
  }

  return true;
}

void WlrClipboardServer::handleMessage(const proto::ext::wlrclip::Selection &sel) {
  ClipboardSelection cs;

  cs.offers.reserve(sel.offers().size());

  for (const auto &offer : sel.offers()) {
    cs.offers.push_back({offer.mime_type().c_str(), QByteArray::fromStdString(offer.data())});
  }

  emit selectionAdded(cs);
}

void WlrClipboardServer::handleExit(int code, QProcess::ExitStatus status) {}

QString WlrClipboardServer::id() const { return "wlr-clipboard"; }

int WlrClipboardServer::activationPriority() const {
  // High priority for Wayland environments (but not GNOME)
  return 15;
}

bool WlrClipboardServer::start() {
  int maxWaitForStart = 5000;
  process = new QProcess;

  connect(process, &QProcess::readyReadStandardOutput, this, &WlrClipboardServer::handleRead);
  connect(process, &QProcess::readyReadStandardError, this, &WlrClipboardServer::handleReadError);
  connect(process, &QProcess::finished, this, &WlrClipboardServer::handleExit);

  process->start(WLR_CLIP_BIN, {});

  if (!process->waitForStarted(maxWaitForStart)) {
    qCritical() << "Failed to start:" << WLR_CLIP_BIN << process->errorString();
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

WlrClipboardServer::WlrClipboardServer() {}
