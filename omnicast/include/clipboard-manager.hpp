#pragma once
#include "proto.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <netinet/in.h>
#include <qlogging.h>
#include <qobject.h>
#include <qprocess.h>
#include <unistd.h>
#include <QDebug>

struct ClipboardDataOffer {
  std::string mimeType;
  std::filesystem::path path;
};

struct ClipboardSelection {
  std::vector<ClipboardDataOffer> offers;
};

class AbstractClipboardManager : public QObject {
  Q_OBJECT

public:
  AbstractClipboardManager() {}

  virtual bool start() = 0;
  virtual bool isAlive() const = 0;

signals:
  void saveSelection(const ClipboardSelection &selection);
};

class WlrClipboardManager : public AbstractClipboardManager {
  std::vector<uint8_t> _message;
  uint32_t _messageLength = 0;
  Proto::Marshaler _marshaler;
  QProcess *process = nullptr;

  bool isAlive() const override { return process->isOpen(); }

  void handleMessage(const std::string &message, const Proto::Variant &data) {
    if (message == "selection") {
      Proto::Array offers = data.asArray();
      ClipboardSelection selection;

      selection.offers.reserve(offers.size());

      for (const auto &offer : offers) {
        auto dict = offer.asDict();
        auto filePath = dict["file_path"].asString();
        auto mimeType = dict["mime_type"].asString();

        selection.offers.push_back({mimeType, filePath});

        qDebug() << "WlrClipboardManager got" << mimeType;
      }

      emit saveSelection(selection);
    }
  }

  void handleRead() {
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

  void handleExit(int code, QProcess::ExitStatus status) {}

public:
  bool start() override {
    process = new QProcess;

    connect(process, &QProcess::readyReadStandardOutput, this, &WlrClipboardManager::handleRead);
    connect(process, &QProcess::finished, this, &WlrClipboardManager::handleExit);
    process->start("omni-wlr-clip", {});

    qDebug() << "start WlrClipboardManager";

    return process;
  }
};
