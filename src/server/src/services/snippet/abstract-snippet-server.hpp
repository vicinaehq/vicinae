#pragma once
#include <QObject>
#include "generated/snippet-client.hpp"

class AbstractSnippetServer : public QObject {
  Q_OBJECT

signals:
  void keywordTriggered(std::string trigger) const;
  void undoTriggered(std::string trigger) const;
  void ready();

public:
  using QObject::QObject;
  ~AbstractSnippetServer() override = default;

  virtual void registerSnippet(snippet_gen::CreateSnippetRequest payload) = 0;
  virtual void unregisterSnippet(std::string_view keyword) = 0;
  virtual void setKeymap(snippet_gen::LayoutInfo info) = 0;
  virtual void resetContext() = 0;

  virtual void injectExpand(const std::string &text, unsigned charsToDelete, unsigned prePasteDelayUs,
                            bool terminal, unsigned cursorLeftMoves) = 0;
  virtual void injectUndo(unsigned backspaceCount, const std::string &trigger) = 0;
  virtual void setKeyDelay(int us) = 0;
  virtual bool supportsKeyInjection() const = 0;

  // whether expansion is delivered via the clipboard (paste). when false, the backend types the text
  // directly and the service skips clipboard staging/restore.
  virtual bool usesClipboard() const = 0;

  virtual bool isRunning() const = 0;
};
