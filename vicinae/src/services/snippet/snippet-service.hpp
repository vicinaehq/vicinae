#pragma once
#include <cstdint>
#include <qlogging.h>
#include <qobject.h>
#include <qprocess.h>
#include <QDebug>
#include "snippet/snippet.hpp"
#include "vicinae-ipc/ipc.hpp"

class SnippetService : public QObject {
public:
  SnippetService() {
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &SnippetService::handleOutput);
    connect(&m_process, &QProcess::readyReadStandardError, this, &SnippetService::handleError);
    connect(&m_process, &QProcess::started, this, &SnippetService::handleStarted);
  }

  void start() {
    m_process.setProgram("/proc/self/exe");
    m_process.setArguments({"snippet-server"});
    m_process.start();
  }

protected:
  void handleOutput() { qDebug() << "got some output"; }

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
  }

private:
  QProcess m_process;
  ipc::RpcClient<snippet::ipc::ClientSchema> m_client;
};
