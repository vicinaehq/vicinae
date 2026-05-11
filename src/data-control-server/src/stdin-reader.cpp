#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <glaze/glaze.hpp>
#include "stdin-reader.hpp"

bool StdinReader::readAndProcess() {
  char tmp[4096];
  const ssize_t n = read(STDIN_FILENO, tmp, sizeof(tmp));

  if (n <= 0) return false;

  m_buffer.insert(m_buffer.end(), tmp, tmp + n);

  using SizeType = uint32_t;
  constexpr size_t HEADER_SIZE = sizeof(SizeType);
  constexpr size_t TAG_SIZE = 1;

  while (m_buffer.size() >= HEADER_SIZE) {
    uint32_t frameLen = ntohl(*reinterpret_cast<SizeType *>(m_buffer.data()));

    if (m_buffer.size() - HEADER_SIZE < frameLen) break;
    if (frameLen < TAG_SIZE) {
      m_buffer.erase(m_buffer.begin(), m_buffer.begin() + HEADER_SIZE + frameLen);
      continue;
    }

    auto tag = static_cast<clipboard_proto::Command>(m_buffer[HEADER_SIZE]);
    std::string_view payload{m_buffer.data() + HEADER_SIZE + TAG_SIZE, frameLen - TAG_SIZE};

    if (tag == clipboard_proto::Command::SetClipboard) {
      clipboard_proto::Selection selection;
      if (auto err = glz::read_beve(selection, payload)) {
        std::cerr << "Failed to parse SetClipboard payload\n";
      } else {
        m_callback(std::move(selection));
      }
    }

    m_buffer.erase(m_buffer.begin(), m_buffer.begin() + HEADER_SIZE + frameLen);
  }

  return true;
}
