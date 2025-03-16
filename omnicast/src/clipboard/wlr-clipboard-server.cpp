#include "clipboard/wlr-clipboard-server.hpp"
#include "clipboard/clipboard-server.hpp"
#include "proto.hpp"
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

  for (const auto &var : namedVars) {
    for (const auto &flag : nameFlags) {
      if (var.contains(flag)) { return true; }
    }
  }

  return false;
}

void WlrClipboardServer::handleMessage(const std::string &message, const Proto::Variant &data) {
  if (message == "selection") {
    Proto::Array offers = data.asArray();
    ClipboardSelection cs;

    cs.offers.reserve(offers.size());

    for (const auto &offer : offers) {
      auto dict = offer.asDict();
      auto filePath = dict["file_path"].asString();
      auto mimeType = dict["mime_type"].asString();

      cs.offers.push_back({mimeType, filePath});

      qDebug() << "WlrClipboardManager got" << mimeType;
    }

    emit selection(cs);
  }
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
    _message.erase(_message.begin(), _message.begin() + sizeof(uint32_t));
  }

  if (_message.size() >= _messageLength) {
    std::vector<uint8_t> data(_message.begin(), _message.begin() + _messageLength);
    auto result = _marshaler.unmarshal<Proto::Array>(data);

    _message.erase(_message.begin(), _message.begin() + _messageLength);

    if (auto err = std::get_if<Proto::Marshaler::Error>(&result)) {
      qDebug() << "Failed to unmarshal message of size" << _messageLength << err->message;
      return;
    }

    auto arr = std::get<Proto::Array>(result);

    if (arr.size() != 2) {
      qDebug() << "Malformed input";
      return;
    }

    auto verb = arr[0].asString();
    auto content = arr[1];

    handleMessage(verb, content);
    _messageLength = 0;
  }
}

WlrClipboardServer::WlrClipboardServer() : AbstractClipboardServer(WlrootsDataControlClipboardServer) {}
