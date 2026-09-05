#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "services/files-service/windows/win-file-candidates.hpp"

struct EverythingSearch {
  std::wstring text;
  bool regex = false;
  int maxResults = 0;
};

class EverythingClient {
public:
  virtual ~EverythingClient() = default;
  virtual bool isConnected() = 0;
  virtual std::optional<std::vector<WinFileCandidate>> search(const EverythingSearch &search) = 0;
};

std::unique_ptr<EverythingClient> connectEverythingClient(const std::wstring &instanceName);
