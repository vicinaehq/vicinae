#pragma once
#include "everything-client.hpp"

// Everything 1.4 (SDK 2) transport: WM_COPYDATA queries against the Everything IPC window.
class EverythingWindowClient : public EverythingClient {
public:
  static std::unique_ptr<EverythingWindowClient> connect();

  bool isConnected() override;
  std::optional<std::vector<WinFileCandidate>> search(const EverythingSearch &search) override;
};
