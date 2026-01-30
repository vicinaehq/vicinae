#pragma once
#include "snippet-server.hpp"
#include "snippet-db.hpp"

class SnippetService : public QObject {
public:
  SnippetService(std::filesystem::path path) : m_db(path) {}

  bool start() {
    m_server.start();
    return true;
  }

  /**
   * The snippet server will not be running if it is explicitly disabled or
   * if it doesn't have the required permisions to do its job.
   */
  bool isServerRunning() { return m_server.isRunning(); }

  SnippetServer *server() { return &m_server; }

private:
  SnippetServer m_server;
  SnippetDatabase m_db;
};
