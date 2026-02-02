#pragma once
#include "services/clipboard/data-control/data-control-clipboard-server.hpp"
#include "wayland/globals.hpp"

class ExtDataControlClipboardServer : public DataControlClipboardServer {
public:
  static constexpr const char *ENTRYPOINT = "ext-clip";

  ExtDataControlClipboardServer() : DataControlClipboardServer(ENTRYPOINT) {}

  bool isActivatable() const override { return Wayland::Globals::dataControlDeviceManager(); }
};
