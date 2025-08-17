#include "services/clipboard/dummy-clipboard-server.hpp"
#include <qdebug.h>

bool DummyClipboardServer::isActivatable() const { return true; }

bool DummyClipboardServer::start() {
  qDebug() << "Started dummy clipboard server!";
  return true;
};

bool DummyClipboardServer::isAlive() const { return false; }

DummyClipboardServer::DummyClipboardServer() {}
