#include "pid-file/pid-file.hpp"
#include "services/clipboard/clipboard-server.hpp"
#include <QtCore>
#include <QGuiApplication>
#include <cstdint>
#include <netinet/in.h>
#include <qlogging.h>
#include <qprocess.h>
#include <qdebug.h>
#include <qstringview.h>
#include <glaze/glaze.hpp>
#include "data-control-clipboard-server.hpp"
#include "common/common.hpp"
#include "wayland/globals.hpp"
#include "common/clipboard-protocol.hpp"

static constexpr const char *HELPER_PROGRAM = "vicinae-data-control-server";

bool DataControlClipboardServer::isAlive() const { return m_process.isOpen(); }

void DataControlClipboardServer::handleExit(int code, QProcess::ExitStatus status) {
  if (status == QProcess::ExitStatus::CrashExit) {
    qCritical() << "data-control-server process exited with status code" << code;
  }
}

QString DataControlClipboardServer::id() const { return "data-control"; }

int DataControlClipboardServer::activationPriority() const { return 1; }

bool DataControlClipboardServer::isActivatable() const {
  return Wayland::Globals::dataControlDeviceManager() || Wayland::Globals::wlrDataControlManager();
}

// Process must stay alive even when monitoring is off: it handles clipboard writes for snippets.
// Incoming selections are filtered by ClipboardService::saveSelection when monitoring is disabled.
bool DataControlClipboardServer::stop() { return true; }

bool DataControlClipboardServer::start() {
  PidFile pidFile(HELPER_PROGRAM);
  int const maxWaitForStart = 5000;
  std::error_code const ec;

  auto path = vicinae::findHelperProgram(HELPER_PROGRAM);
  if (!path) {
    qWarning() << "could not find" << HELPER_PROGRAM;
    return false;
  }

  if (pidFile.exists() && pidFile.kill()) { qInfo() << "Killed existing data-control-server instance"; }

  m_process.start(path->c_str(), {});

  if (!m_process.waitForStarted(maxWaitForStart)) {
    qCritical() << "Failed to start data-control-server process" << m_process.errorString();
    return false;
  }

  pidFile.write(m_process.processId());

  return m_process.state() == QProcess::ProcessState::Running;
}

void DataControlClipboardServer::handleReadError() {
  QTextStream(stderr) << m_process.readAllStandardError();
}

void DataControlClipboardServer::handleRead() {
  using SizeType = uint32_t;
  constexpr size_t TAG_SIZE = 1;

  while (m_process.bytesAvailable() > 0) {
    QByteArray data = m_process.readAllStandardOutput();
    m_message.reserve(data.size());
    m_message.insert(m_message.end(), data.begin(), data.end());

    while (m_message.size() >= sizeof(SizeType)) {
      uint32_t const length = ntohl(*reinterpret_cast<SizeType *>(m_message.data()));
      size_t const available = m_message.size() - sizeof(SizeType);

      if (available < length || length < TAG_SIZE) break;

      auto tag = static_cast<clipboard_proto::Command>(m_message[sizeof(SizeType)]);
      std::string_view payload{m_message.data() + sizeof(SizeType) + TAG_SIZE, length - TAG_SIZE};

      if (tag == clipboard_proto::Command::SelectionNotification) {
        clipboard_proto::Selection selection;

        if (auto err = glz::read_beve(selection, payload)) {
          qWarning() << "Failed to parse clipboard selection";
        } else {
          ClipboardSelection cs;
          cs.offers.reserve(selection.offers.size());

          for (const auto &offer : selection.offers) {
            cs.offers.push_back({
                QString::fromStdString(offer.mime_type),
                QByteArray(reinterpret_cast<const char *>(offer.data.data()), offer.data.size()),
            });
          }

          emit selectionAdded(cs);
        }
      } else {
        qWarning() << "Unknown command tag from data-control-server:" << static_cast<int>(tag);
      }

      m_message.erase(m_message.begin(), m_message.begin() + sizeof(SizeType) + length);
    }
  }
}

bool DataControlClipboardServer::setClipboardContent(QMimeData *data) {
  if (!QGuiApplication::focusWindow() && m_process.state() == QProcess::Running) {
    clipboard_proto::Selection selection;
    for (const auto &format : data->formats()) {
      QByteArray raw = data->data(format);
      selection.offers.push_back(
          {.mime_type = format.toStdString(), .data = std::vector<uint8_t>(raw.begin(), raw.end())});
    }

    std::string buf;
    if (auto err = glz::write_beve(selection, buf)) {
      qWarning() << "Failed to serialize clipboard write request";
      delete data;
      return false;
    }

    uint8_t tag = static_cast<uint8_t>(clipboard_proto::Command::SetClipboard);
    uint32_t netLen = htonl(static_cast<uint32_t>(buf.size() + 1));
    m_process.write(reinterpret_cast<const char *>(&netLen), sizeof(netLen));
    m_process.write(reinterpret_cast<const char *>(&tag), 1);
    m_process.write(buf.data(), buf.size());

    delete data;
    return true;
  }

  return AbstractClipboardServer::setClipboardContent(data);
}

DataControlClipboardServer::DataControlClipboardServer() {
  connect(&m_process, &QProcess::readyReadStandardOutput, this, &DataControlClipboardServer::handleRead);
  connect(&m_process, &QProcess::readyReadStandardError, this, &DataControlClipboardServer::handleReadError);
  connect(&m_process, &QProcess::finished, this, &DataControlClipboardServer::handleExit);
}
