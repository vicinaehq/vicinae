#pragma once
#include <cstdint>
#include <qapplication.h>
#include <qclipboard.h>
#include <qlogging.h>
#include <qmimedata.h>
#include <qobject.h>
#include <qprocess.h>
#include <variant>
#include <QDebug>
#include "snippet/snippet.hpp"
#include "vicinae-ipc/ipc.hpp"
#include <glaze/glaze.hpp>

class SnippetService : public QObject {
public:
  SnippetService() {
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &SnippetService::handleOutput);
    connect(&m_process, &QProcess::readyReadStandardError, this, &SnippetService::handleError);
    connect(&m_process, &QProcess::started, this, &SnippetService::handleStarted);

    m_server.route<snippet::ipc::TriggerSnippet>([this](const snippet::ipc::TriggerSnippet::Request &snip) {
      auto clip = QApplication::clipboard();
      auto expansionData = new QMimeData;

      expansionData->setData("text/plain;charset=utf-8", "there");
      expansionData->setData("vicinae/concealed", "1");

      clip->setMimeData(expansionData);

      qDebug() << "snippet needs expanding" << snip.trigger.c_str();

      request(m_client.request<snippet::ipc::InjectClipboardExpansion>({.trigger = snip.trigger}));

      return snippet::ipc::TriggerSnippet::Response();
    });
  }

  void start() {
    m_process.setProgram("/proc/self/exe");
    m_process.setArguments({"snippet-server"});
    m_process.start();
  }

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

    std::variant<Req> v;

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

    /*
if (auto it = std::get_if<Res>(&v)) {
  auto _ = m_client.call(*it);
  return;
}
    */
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

  void handleStarted() {
    request(m_client.request<snippet::ipc::CreateSnippet>({.trigger = ":vibin:"}));
    request(m_client.request<snippet::ipc::CreateSnippet>({.trigger = ":ez:"}));
    request(m_client.request<snippet::ipc::CreateSnippet>({.trigger = "thre"}));
  }

private:
  QProcess m_process;
  ipc::RpcClient<snippet::ipc::ClientSchema> m_client;
  ipc::RpcServer<snippet::ipc::ServerSchema> m_server;

  uint32_t m_size = 0;
  std::string m_data;
};
