#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "win-file-candidates.hpp"

// Text is passed to Everything untouched so its own search syntax applies.
struct EverythingSearch {
  std::wstring text;
  bool regex = false;
  int maxResults = 0;
};

// One Everything IPC transport. Results come back in Everything's own order (most recently changed
// first). `search` returns nullopt once the connection is gone.
class EverythingClient {
public:
  virtual ~EverythingClient() = default;
  virtual bool isConnected() = 0;
  virtual std::optional<std::vector<WinFileCandidate>> search(const EverythingSearch &search) = 0;
};

// Tries the Everything 1.5 named pipe first, then the Everything 1.4 IPC window.
std::unique_ptr<EverythingClient> connectEverythingClient(const std::wstring &instanceName);
