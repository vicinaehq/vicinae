#pragma once
#include "abstract-snippet-server.hpp"

class LinuxInputServer;

class LinuxSnippetServer : public AbstractSnippetServer {
  Q_OBJECT

public:
  explicit LinuxSnippetServer(LinuxInputServer &inputServer);

  void registerSnippet(snippet_gen::CreateSnippetRequest payload) override;
  void unregisterSnippet(std::string_view keyword) override;
  void setKeymap(snippet_gen::LayoutInfo info) override;
  void resetContext() override;

  void injectExpand(unsigned charsToDelete, unsigned prePasteDelayUs, bool terminal,
                    unsigned cursorLeftMoves) override;
  void injectUndo(unsigned backspaceCount, const std::string &trigger) override;
  void setKeyDelay(int us) override;
  bool supportsKeyInjection() const override;

  bool isRunning() const override;

private:
  LinuxInputServer &m_inputServer;
};
