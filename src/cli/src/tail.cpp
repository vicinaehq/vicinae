#include "tail.hpp"
#include <array>
#include <iostream>
#include <thread>
#include <utility>

namespace cli {

FileTailer::FileTailer(std::filesystem::path path)
    : m_path(std::move(path)), m_stream(m_path, std::ios::binary) {}

std::streamoff FileTailer::drain(std::ostream &out) {
  std::array<char, 8192> buf;
  std::streamoff written = 0;

  for (;;) {
    m_stream.read(buf.data(), buf.size());
    auto const count = m_stream.gcount();

    if (count <= 0) break;

    out.write(buf.data(), count);
    written += count;
  }

  m_stream.clear();
  out.flush();
  m_offset += written;

  return written;
}

void FileTailer::reopen(std::ostream &out) {
  m_stream.close();
  m_stream.clear();
  m_stream.open(m_path, std::ios::binary);
  m_offset = 0;

  if (m_stream) drain(out);
}

void FileTailer::follow(std::ostream &out) {
  for (;;) {
    std::this_thread::sleep_for(POLL_INTERVAL);

    std::error_code ec;
    auto const size = std::filesystem::file_size(m_path, ec);

    if (ec) continue;

    if (!m_stream.is_open() || std::cmp_less(size, m_offset)) {
      reopen(out);
      continue;
    }

    if (std::cmp_equal(size, m_offset)) continue;

    m_stream.clear();
    m_stream.seekg(m_offset);

    if (drain(out) == 0) reopen(out);
  }
}

} // namespace cli
