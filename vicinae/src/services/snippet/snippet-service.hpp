#pragma once
#include "snippet-server.hpp"
#include "snippet-db.hpp"
#include <qtmetamacros.h>

class SnippetService : public QObject {
  Q_OBJECT

signals:
  void snippetAdded();
  void snippetUpdated();
  void snippetRemoved();
  void snippetsChanged(); // add/updated/remove

public:
  SnippetService(std::filesystem::path path) : m_db(path) {}

  bool start() {
    m_server.start();
    return true;
  }

  auto createSnippet(SnippetDatabase::SnippetPayload payload) {
    auto res = m_db.addSnippet(payload);
    if (res) {
      emit snippetAdded();
      emit snippetsChanged();
    }
    return res;
  }

  auto updateSnippet(std::string_view id, SnippetDatabase::SnippetPayload payload) {
    auto res = m_db.updateSnippet(id, payload);
    if (res) {
      emit snippetUpdated();
      emit snippetsChanged();
    }
    return res;
  }

  auto removeSnippet(std::string_view id) {
    auto res = m_db.removeSnippet(id);
    if (res) {
      emit snippetRemoved();
      emit snippetsChanged();
    }
    return res;
  }

  /**
   * The snippet server will not be running if it is explicitly disabled or
   * if it doesn't have the required permisions to do its job.
   */
  bool isServerRunning() { return m_server.isRunning(); }

  SnippetServer *server() { return &m_server; }
  SnippetDatabase *database() { return &m_db; }

private:
  SnippetServer m_server;
  SnippetDatabase m_db;
};
