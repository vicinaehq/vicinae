#pragma once
#include "everything-client.hpp"

struct _everything3_client_s;

// Everything 1.5 (SDK 3) transport: named pipe through the vendored voidtools client library.
class EverythingPipeClient : public EverythingClient {
public:
  static std::unique_ptr<EverythingPipeClient> connect(const std::wstring &instanceName);

  explicit EverythingPipeClient(_everything3_client_s *client);
  ~EverythingPipeClient() override;

  bool isConnected() override;
  std::optional<std::vector<WinFileCandidate>> search(const EverythingSearch &search) override;

private:
  _everything3_client_s *m_client;
};
