#pragma once
#include "abstract-snippet-server.hpp"

class NullSnippetServer : public AbstractSnippetServer {
  Q_OBJECT

public:
  using AbstractSnippetServer::AbstractSnippetServer;

  void registerSnippet(snippet_gen::CreateSnippetRequest) override {}
  void unregisterSnippet(std::string_view) override {}
  void setKeymap(snippet_gen::LayoutInfo) override {}
  void resetContext() override {}

  void injectExpand(const std::string &, unsigned, unsigned, bool, unsigned) override {}
  void injectUndo(unsigned, const std::string &) override {}
  void setKeyDelay(int) override {}
  bool supportsKeyInjection() const override { return false; }
  bool usesClipboard() const override { return true; }

  bool isRunning() const override { return false; }
};
