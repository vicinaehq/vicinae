#pragma once
#include "services/clipboard/data-control/data-control-clipboard-server.hpp"
#include "wayland/globals.hpp"

class WlrClipboardServer : public DataControlClipboardServer {
public:
  static constexpr const char *ENTRYPOINT = "wlr-clip";

  WlrClipboardServer() : DataControlClipboardServer(ENTRYPOINT) {}

  bool isActivatable() const override { return Wayland::Globals::wlrDataControlManager(); }
};
