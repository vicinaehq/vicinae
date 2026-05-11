#include "snippet-server.hpp"
#include <cstdint>
#include <netinet/in.h>

// SnippetServerBus

void SnippetServerBus::send(std::string_view data) {
  uint32_t size = data.size();
  m_device->write(reinterpret_cast<const char *>(&size), sizeof(size));
  m_device->write(data.data(), data.size());
}

void SnippetServerBus::readyRead() {
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

SnippetServerBus::SnippetServerBus(QIODevice *device) : m_device(device) {
  connect(device, &QIODevice::readyRead, this, &SnippetServerBus::readyRead);
}

// SnippetServer

SnippetServer::SnippetServer() : m_bus(&m_process), m_rpc(m_bus), m_client(m_rpc) {
  connect(&m_process, &QProcess::readyReadStandardError, this, &SnippetServer::handleError);
  connect(&m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError) {
    qCritical() << "snippet server process error occured" << m_process.errorString();
  });

  connect(&m_process, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus status) {
    if (status == QProcess::CrashExit) {
      qWarning() << "snippet server crashed with code" << exitCode;
    } else {
      qInfo() << "snippet server exited with code" << exitCode;
    }
    emit serverStopped();
  });

  connect(
      m_client.snippet(), &snippet_gen::SnippetService::triggerSnippet, this,
      [this](const snippet_gen::TriggerSnippetPayload &payload) { emit keywordTriggered(payload.trigger); });

  connect(m_client.snippet(), &snippet_gen::SnippetService::undoSnippet, this,
          [this](const snippet_gen::UndoSnippetPayload &payload) { emit undoTriggered(payload.trigger); });

  connect(&m_bus, &SnippetServerBus::messageReceived, this, [this](const QByteArray &msg) {
    std::string_view view{msg.constData(), static_cast<size_t>(msg.size())};
    if (auto res = m_client.route(view); !res) {
      qWarning() << "Failed to route snippet message" << res.error();
    }
  });
}

void SnippetServer::start() {
  const auto path = vicinae::findHelperProgram("vicinae-input-server");

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
      });
}

void SnippetServer::registerSnippet(snippet_gen::CreateSnippetRequest payload) {
  if (!isRunning()) return;
  m_client.snippet()->createSnippet(payload);
}

void SnippetServer::unregisterSnippet(std::string_view keyword) {
  if (!isRunning()) return;
  m_client.snippet()->removeSnippet({.trigger = std::string{keyword}});
}

void SnippetServer::setKeymap(snippet_gen::LayoutInfo info) {
  if (!isRunning()) return;
  m_client.snippet()->setKeymap(info);
}

void SnippetServer::resetContext() {
  if (!isRunning()) return;
  m_client.snippet()->resetContext();
}

void SnippetServer::injectExpand(int charsToDelete, int prePasteDelayUs, bool terminal, int cursorLeftMoves) {
  if (!isRunning()) return;
  m_client.snippet()->injectExpand({.charsToDelete = charsToDelete,
                                    .prePasteDelayUs = prePasteDelayUs,
                                    .terminal = terminal,
                                    .cursorLeftMoves = cursorLeftMoves});
}

void SnippetServer::injectUndo(int backspaceCount, const std::string &trigger) {
  if (!isRunning()) return;
  m_client.snippet()->injectUndo({.backspaceCount = backspaceCount, .triggerText = trigger});
}

void SnippetServer::injectPaste(bool terminal) {
  if (!isRunning()) return;
  m_client.snippet()->injectPaste({.terminal = terminal});
}

void SnippetServer::cancelInjection() {
  if (!isRunning()) return;
  m_client.snippet()->cancelInjection();
}

void SnippetServer::setKeyDelay(int us) {
  if (!isRunning()) return;
  m_client.snippet()->setKeyDelay(us);
}

void SnippetServer::handleError() {
  for (const auto &line : m_process.readAllStandardError().split('\n')) {
    qDebug() << "[SNIPPET-SERVER]" << line;
  }
}
