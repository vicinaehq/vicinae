#include "dummy-clipboard-server.hpp"
#include <qdebug.h>

bool DummyClipboardServer::isActivatable() const { return true; }

bool DummyClipboardServer::start() {
  qDebug() << "Started dummy clipboard server! This clipboard server does nothing and thus clipboard related "
              "operations will not work. Help contribute clipboard support for your environment: "
              "https://docs.vicinae.com/contrib/clipboard";
  return true;
};

QString DummyClipboardServer::id() const { return "dummy"; }

bool DummyClipboardServer::isAlive() const { return false; }

DummyClipboardServer::DummyClipboardServer() {}
