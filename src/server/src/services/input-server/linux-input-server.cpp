#include "linux-input-server.hpp"
#include <cstdint>
#include <QTimer>
#include <qtenvironmentvariables.h>

// InputServerBus

void InputServerBus::send(std::string_view data) {
  uint32_t size = data.size();
  m_device->write(reinterpret_cast<const char *>(&size), sizeof(size));
  m_device->write(data.data(), data.size());
}

void InputServerBus::readyRead() {
  while (m_device->bytesAvailable() > 0) {
    auto read = m_device->readAll();
    m_message.data.append(read);

    while (std::cmp_greater_equal(m_message.data.size(), sizeof(uint32_t))) {
      uint32_t const length = *reinterpret_cast<uint32_t *>(m_message.data.data());
      bool const isComplete = m_message.data.size() - sizeof(uint32_t) >= length;

      if (!isComplete) break;

      auto packet = m_message.data.sliced(sizeof(uint32_t), length);
      emit messageReceived(packet);
      m_message.data = m_message.data.sliced(sizeof(uint32_t) + length);
    }
  }
}

InputServerBus::InputServerBus(QIODevice *device) : m_device(device) {
  connect(device, &QIODevice::readyRead, this, &InputServerBus::readyRead);
}

// LinuxInputServer

LinuxInputServer::LinuxInputServer() : m_bus(&m_process), m_rpc(m_bus), m_client(m_rpc) {
  connect(&m_process, &QProcess::readyReadStandardError, this, &LinuxInputServer::handleError);
  connect(&m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError) {
    qCritical() << "input server process error occured" << m_process.errorString();
  });

  connect(&m_process, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus status) {
    if (status == QProcess::CrashExit || exitCode != 0) {
      qWarning() << "input server crashed with code" << exitCode;
      handleCrash();
    } else {
      qInfo() << "input server exited with code" << exitCode;
    }
  });

  connect(
      m_client.snippet(), &snippet_gen::SnippetService::triggerSnippet, this,
      [this](const snippet_gen::TriggerSnippetPayload &payload) { emit keywordTriggered(payload.trigger); });

  connect(m_client.snippet(), &snippet_gen::SnippetService::undoSnippet, this,
          [this](const snippet_gen::UndoSnippetPayload &payload) { emit undoTriggered(payload.trigger); });

  connect(&m_bus, &InputServerBus::messageReceived, this, [this](const QByteArray &msg) {
    std::string_view view{msg.constData(), static_cast<size_t>(msg.size())};
    if (auto res = m_client.route(view); !res) {
      qWarning() << "Failed to route input server message" << res.error();
    }
  });
}

void LinuxInputServer::setEnabled(bool value) {
  if (m_enabled.has_value() && m_enabled.value() == value) return;

  m_enabled = value;

  if (*m_enabled) {
    start();
  } else {
    m_crashCount = 0;
    m_process.close();
  }
}

void LinuxInputServer::start() {
  std::optional<std::filesystem::path> path;

  {
    // trick used by mostly NixOS since this binary has special security implications (needs a setcap to
    // enable input injection/monitoring at the kernel level).
    // Other installations just find the binary at the standard $prefix/libexec/vicinae/vicinae-input-server
    // location, and the binary is setcap'd as part of the post install.
    if (auto const binOverride = qEnvironmentVariable("VICINAE_INPUT_SERVER_BIN"); !binOverride.isEmpty()) {
      path = binOverride.toStdString();
    } else {
      path = vicinae::findHelperProgram("vicinae-input-server");
    }
  }

  if (!path) {
    qWarning() << "could not find vicinae-input-server helper binary, snippet expansion will not work";
    return;
  }

  m_process.setProgram(path->c_str());
  m_process.start();
  if (!m_process.waitForStarted()) {
    qCritical() << "Failed to start input server" << m_process.error();
    return;
  }

  m_client.snippet()->getCapabilities().then(
      [this](std::expected<snippet_gen::KeyboardCapabilities, std::string> result) {
        if (result) { m_supportsInjection = result->injection; }
        scheduleStabilityReset();
        emit serverReady();
      });
}

void LinuxInputServer::handleCrash() {
  if (!m_enabled.value_or(false)) return;

  ++m_crashCount;

  if (m_crashCount > MAX_RESTART_ATTEMPTS) {
    qCritical() << "input server crashed" << m_crashCount << "times, giving up on restart";
    return;
  }

  const int delay = BASE_RESTART_DELAY_MS * (1 << (m_crashCount - 1));
  qWarning() << "input server crashed, restarting in" << delay << "ms"
             << "(attempt" << m_crashCount << "/" << MAX_RESTART_ATTEMPTS << ")";

  QTimer::singleShot(delay, this, [this]() { start(); });
}

void LinuxInputServer::scheduleStabilityReset() {
  QTimer::singleShot(STABILITY_THRESHOLD_MS, this, [this]() {
    if (m_process.state() == QProcess::ProcessState::Running) { m_crashCount = 0; }
  });
}

void LinuxInputServer::registerSnippet(snippet_gen::CreateSnippetRequest payload) {
  if (!isRunning()) return;
  m_client.snippet()->createSnippet(payload);
}

void LinuxInputServer::unregisterSnippet(std::string_view keyword) {
  if (!isRunning()) return;
  m_client.snippet()->removeSnippet({.trigger = std::string{keyword}});
}

void LinuxInputServer::setKeymap(snippet_gen::LayoutInfo info) {
  if (!isRunning()) return;
  m_client.snippet()->setKeymap(info);
}

void LinuxInputServer::resetContext() {
  if (!isRunning()) return;
  m_client.snippet()->resetContext();
}

void LinuxInputServer::injectExpand(unsigned charsToDelete, unsigned prePasteDelayUs, bool terminal,
                                    unsigned cursorLeftMoves) {
  if (!isRunning()) return;
  m_client.snippet()->injectExpand({.charsToDelete = charsToDelete,
                                    .prePasteDelayUs = prePasteDelayUs,
                                    .terminal = terminal,
                                    .cursorLeftMoves = cursorLeftMoves});
}

void LinuxInputServer::injectUndo(unsigned backspaceCount, const std::string &trigger) {
  if (!isRunning()) return;
  m_client.snippet()->injectUndo({.backspaceCount = backspaceCount, .triggerText = trigger});
}

void LinuxInputServer::injectPaste(bool terminal) {
  if (!isRunning()) return;
  m_client.snippet()->injectPaste({.terminal = terminal});
}

void LinuxInputServer::setKeyDelay(int us) {
  if (!isRunning()) return;
  m_client.snippet()->setKeyDelay(us);
}

void LinuxInputServer::handleError() {
  for (const auto &line : m_process.readAllStandardError().split('\n')) {
    qDebug() << "[INPUT-SERVER]" << line;
  }
}
