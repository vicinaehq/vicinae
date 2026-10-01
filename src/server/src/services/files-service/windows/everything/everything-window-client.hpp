#pragma once
#include "everything-client.hpp"

class EverythingWindowClient : public EverythingClient {
public:
  static std::unique_ptr<EverythingWindowClient> connect();

  bool isConnected() override;
  std::optional<std::vector<WinFileCandidate>> search(const EverythingSearch &search) override;
};
