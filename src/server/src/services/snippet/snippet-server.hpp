#pragma once
#include <QProcess>
#include <qapplication.h>
#include <qlogging.h>
#include <QMimeData>
#include <QObject>
#include <QClipboard>
#include "common/common.hpp"
#include "snippet/types.hpp"

class SnippetServer : public QObject {
  Q_OBJECT

signals:
  void keywordTriggered(std::string trigger) const;
  void serverStopped();

public:
  SnippetServer() {
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &SnippetServer::handleOutput);
    connect(&m_process, &QProcess::readyReadStandardError, this, &SnippetServer::handleError);
    connect(&m_process, &QProcess::started, this, &SnippetServer::handleStarted);
    connect(&m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError) {
      qCritical() << "snippet server process error occured" << m_process.errorString();
    });

    m_server.route<snippet::ipc::TriggerSnippet>([this](const snippet::ipc::TriggerSnippet::Request &snip) {
      emit keywordTriggered(snip.trigger);
      return snippet::ipc::TriggerSnippet::Response();
    });
  }

  void start() {
    const auto path = vicinae::findHelperProgram("vicinae-snippet-server");

    if (!path) {
      qWarning() << "could not find vicinae-snippet-server helper binary, snippet expansion will not work";
      return;
    }

    m_process.setProgram(path->c_str());
    m_process.start();
    if (!m_process.waitForStarted()) { qCritical() << "Failed to start snippet server" << m_process.error(); }
  }

  void stop() { m_process.close(); }

  void registerSnippet(snippet::ipc::CreateSnippet::Request payload) {
    request(m_client.request<snippet::ipc::CreateSnippet>(payload));
  }

  void injectClipboardText(std::string_view trigger, QString text, bool terminal = false) {
    auto clip = QApplication::clipboard();
    auto expansionData = new QMimeData;

    expansionData->setData("text/plain;charset=utf-8", text.toUtf8());
    expansionData->setData("vicinae/concealed", "1");
    clip->setMimeData(expansionData);

    request(m_client.request<snippet::ipc::InjectClipboardExpansion>(
        {.trigger = std::string{trigger}, .terminal = terminal}));
  }

  void unregisterSnippet(std::string_view keyword) {
    request(m_client.request<snippet::ipc::RemoveSnippet>({.trigger = std::string{keyword}}));
  }

  bool isRunning() const { return m_process.state() == QProcess::ProcessState::Running; }

protected:
  void handleOutput() {
    const auto buf = m_process.readAllStandardOutput().toStdString();

    m_data.append(buf);

    for (;;) {
      if (m_size == 0 && m_data.size() >= sizeof(m_size)) {
        m_size = *reinterpret_cast<decltype(m_size) *>(m_data.data());
        m_data.erase(m_data.begin(),
                     m_data.begin() + sizeof(m_size)); // we need a better way than this, this is slow
      }

      if (m_size && m_size <= m_data.size()) {
        std::string_view view(m_data);
        handleMessage(view.substr(0, m_size));
        m_data = view.substr(m_size);
        m_size = 0;
        continue;
      }

      break;
    }
  }

  void handleMessage(std::string_view message) {
    qDebug() << "got message from snippet server of size" << message.size();

    using Req = decltype(m_server)::SchemaType::Request;
    using Res = decltype(m_client)::Schema::Response;

    std::variant<Req, Res> v;

    if (const auto error = glz::read_json(v, message)) {
      std::println(std::cerr, "Failed to parse message: {}", glz::format_error(error));
      return;
    }

    if (auto it = std::get_if<Req>(&v)) {
      const auto res = m_server.call(*it);

      if (!res) {
        std::println(std::cerr, "Request failed: {}", res.error());
        return;
      }

      if (!it->id) return;

      std::string data = std::move(res).value();
      uint32_t size = data.size();

      m_process.write(reinterpret_cast<const char *>(&size), sizeof(size));
      m_process.write(data.data(), data.size());

      return;
    }

    if (auto it = std::get_if<Res>(&v)) {
      auto _ = m_client.call(*it);
      return;
    }
  }

  void handleError() {
    for (const auto &line : m_process.readAllStandardError().split('\n')) {
      qDebug() << "[SNIPPET-SERVER]" << line;
    }
  }

  void request(std::string_view message) {
    uint32_t size = message.size();
    m_process.write(reinterpret_cast<const char *>(&size), sizeof(size));
    m_process.write(message.data(), message.size());
  }

  void handleStarted() {}

private:
  QProcess m_process;
  ipc::RpcClient<snippet::ipc::ClientSchema> m_client;
  ipc::RpcServer<snippet::ipc::ServerSchema> m_server;

  uint32_t m_size = 0;
  std::string m_data;
};
