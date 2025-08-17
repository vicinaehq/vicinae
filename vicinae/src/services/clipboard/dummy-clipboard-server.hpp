#include "services/clipboard/clipboard-server.hpp"

class DummyClipboardServer : public AbstractClipboardServer {
public:
  bool isActivatable() const override;
  bool start() override;
  bool isAlive() const override;
  QString id() const override;

  DummyClipboardServer();
};
