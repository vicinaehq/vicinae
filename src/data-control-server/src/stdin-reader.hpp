#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include "clipboard-protocol.hpp"

class StdinReader {
public:
  using SetClipboardCallback = std::function<void(clipboard_proto::Selection)>;

  explicit StdinReader(SetClipboardCallback cb) : m_callback(std::move(cb)) {}

  bool readAndProcess();

private:
  SetClipboardCallback m_callback;
  std::vector<char> m_buffer;
};
