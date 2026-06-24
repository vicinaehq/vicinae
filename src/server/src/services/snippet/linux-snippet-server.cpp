#include "linux-snippet-server.hpp"
#include "services/input-server/linux-input-server.hpp"

LinuxSnippetServer::LinuxSnippetServer(LinuxInputServer &inputServer) : m_inputServer(inputServer) {
  connect(&m_inputServer, &LinuxInputServer::keywordTriggered, this, &LinuxSnippetServer::keywordTriggered);
  connect(&m_inputServer, &LinuxInputServer::undoTriggered, this, &LinuxSnippetServer::undoTriggered);
  connect(&m_inputServer, &LinuxInputServer::serverReady, this, &LinuxSnippetServer::ready);
}

void LinuxSnippetServer::registerSnippet(snippet_gen::CreateSnippetRequest payload) {
  m_inputServer.registerSnippet(std::move(payload));
}

void LinuxSnippetServer::unregisterSnippet(std::string_view keyword) {
  m_inputServer.unregisterSnippet(keyword);
}

void LinuxSnippetServer::setKeymap(snippet_gen::LayoutInfo info) { m_inputServer.setKeymap(std::move(info)); }

void LinuxSnippetServer::resetContext() { m_inputServer.resetContext(); }

void LinuxSnippetServer::injectExpand(unsigned charsToDelete, unsigned prePasteDelayUs, bool terminal,
                                      unsigned cursorLeftMoves) {
  m_inputServer.injectExpand(charsToDelete, prePasteDelayUs, terminal, cursorLeftMoves);
}

void LinuxSnippetServer::injectUndo(unsigned backspaceCount, const std::string &trigger) {
  m_inputServer.injectUndo(backspaceCount, trigger);
}

void LinuxSnippetServer::setKeyDelay(int us) { m_inputServer.setKeyDelay(us); }

bool LinuxSnippetServer::supportsKeyInjection() const { return m_inputServer.supportsKeyInjection(); }

bool LinuxSnippetServer::isRunning() const { return m_inputServer.isRunning(); }
