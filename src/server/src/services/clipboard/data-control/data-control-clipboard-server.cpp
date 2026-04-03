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
#include <glaze/base64/base64.hpp>
#include "data-control-clipboard-server.hpp"
#include "common/common.hpp"
#include "wayland/globals.hpp"

static constexpr const char *HELPER_PROGRAM = "vicinae-data-control-server";

// DataControlBus

void DataControlBus::send(std::string_view) {
  // data-control-server is one-way, we never send to it
}

void DataControlBus::readyRead() {
  while (m_device->bytesAvailable() > 0) {
    auto read = m_device->readAll();
    m_message.data.append(read);

    while (std::cmp_greater_equal(m_message.data.size(), sizeof(uint32_t))) {
      uint32_t const length = ntohl(*reinterpret_cast<uint32_t *>(m_message.data.data()));
      bool const isComplete = m_message.data.size() - sizeof(uint32_t) >= length;

      if (!isComplete) break;

      auto packet = m_message.data.sliced(sizeof(uint32_t), length);

      emit messageReceived(packet);

      m_message.data = m_message.data.sliced(sizeof(uint32_t) + length);
    }
  }
}

DataControlBus::DataControlBus(QIODevice *device) : m_device(device) {
  connect(device, &QIODevice::readyRead, this, &DataControlBus::readyRead);
}

// DataControlClipboardServer

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

bool DataControlClipboardServer::stop() {
  m_process.terminate();
  return m_process.waitForFinished();
}

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

DataControlClipboardServer::DataControlClipboardServer() : m_bus(&m_process), m_rpc(m_bus), m_client(m_rpc) {
  connect(&m_process, &QProcess::readyReadStandardError, this, &DataControlClipboardServer::handleReadError);
  connect(&m_process, &QProcess::finished, this, &DataControlClipboardServer::handleExit);

  connect(m_client.clipboard(), &wlrclip::ClipboardService::selectionAdded, this,
          [this](const wlrclip::ClipboardSelection &sel) {
            ClipboardSelection cs;
            cs.offers.reserve(sel.offers.size());

            for (const auto &offer : sel.offers) {
              cs.offers.push_back({
                  QString::fromStdString(offer.mime_type),
                  QByteArray::fromStdString(glz::read_base64(offer.data)),
              });
            }

            emit selectionAdded(cs);
          });

  connect(&m_bus, &DataControlBus::messageReceived, this, [this](const QByteArray &msg) {
    std::string_view view{msg.constData(), static_cast<size_t>(msg.size())};
    if (auto res = m_client.route(view); !res) {
      qWarning() << "Failed to route clipboard message" << res.error();
    }
  });
}
