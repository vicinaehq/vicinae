#include "services/clipboard/wlr-clipboard-server.hpp"
#include "build/wlr-clip/proto/wlr-clipboard.pb.h"
#include "services/clipboard/clipboard-server.hpp"
#include <algorithm>
#include <netinet/in.h>
#include <qlogging.h>
#include <qprocess.h>
#include <qdebug.h>

bool WlrClipboardServer::isAlive() const { return process->isOpen(); }

bool WlrClipboardServer::isActivatable() const {
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  auto namedVars = {
      env.value("XDG_CURRENT_DESKTOP").toLower(),
      env.value("XDG_SESSION_DESKTOP").toLower(),
      env.value("WAYLAND_COMPOSITOR").toLower(),
  };

  auto nameFlags = {"wlroots", "sway", "river", "wayfire", "hyprland"};

  return std::ranges::any_of(namedVars, [&](auto &&var) {
    return std::ranges::any_of(nameFlags, [&](auto &&flag) { return var.contains(flag); });
  });
}

void WlrClipboardServer::handleMessage(const proto::ext::wlrclip::Selection &sel) {
  ClipboardSelection cs;

  cs.offers.reserve(sel.offers().size());

  for (const auto &offer : sel.offers()) {
    cs.offers.push_back({offer.mime_type().c_str(), offer.file_path().c_str()});
  }

  qDebug() << "selection emitted for" << sel.offers().size();

  emit selection(cs);
}

void WlrClipboardServer::handleExit(int code, QProcess::ExitStatus status) {}

bool WlrClipboardServer::start() {
  process = new QProcess;

  connect(process, &QProcess::readyReadStandardOutput, this, &WlrClipboardServer::handleRead);
  connect(process, &QProcess::finished, this, &WlrClipboardServer::handleExit);
  process->start("omni-wlr-clip", {});

  qDebug() << "start WlrClipboardManager";

  return process;
}

void WlrClipboardServer::handleRead() {
  auto array = process->readAllStandardOutput();
  auto _buf = array.constData();

  qDebug() << "reading" << array.size();

  _message.insert(_message.end(), _buf, _buf + array.size());

  if (_messageLength == 0 && _message.size() > sizeof(uint32_t)) {
    _messageLength = ntohl(*reinterpret_cast<uint32_t *>(_message.data()));
    qDebug() << "message of length" << _messageLength;
    _message.erase(_message.begin(), _message.begin() + sizeof(uint32_t));
  }

  if (_message.size() >= _messageLength) {
    std::string data(_message.begin(), _message.begin() + _messageLength);
    proto::ext::wlrclip::Selection selection;

    if (!selection.ParseFromString(data)) {
      qCritical() << "Failed to parse selection";
    } else {
      handleMessage(selection);
    }

    _message.erase(_message.begin(), _message.begin() + _messageLength);
    _messageLength = 0;
  }
}

WlrClipboardServer::WlrClipboardServer() : AbstractClipboardServer(WlrootsDataControlClipboardServer) {}
