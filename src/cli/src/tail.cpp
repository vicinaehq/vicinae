#include "tail.hpp"
#include <algorithm>
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

std::streamoff FileTailer::findLastLines(std::size_t lines) {
  std::error_code ec;
  auto const size = static_cast<std::streamoff>(std::filesystem::file_size(m_path, ec));

  if (ec || size == 0) return 0;
  if (lines == 0) return size;

  std::array<char, 8192> buf;
  std::streamoff pos = size;
  std::size_t found = 0;
  bool atEnd = true;

  while (pos > 0) {
    auto const chunk = std::min<std::streamoff>(pos, buf.size());

    pos -= chunk;
    m_stream.seekg(pos);
    m_stream.read(buf.data(), chunk);

    auto end = chunk;
    if (atEnd && buf[chunk - 1] == '\n') --end;
    atEnd = false;

    for (auto i = end; i-- > 0;) {
      if (buf[i] == '\n' && ++found == lines) return pos + i + 1;
    }
  }

  return 0;
}

void FileTailer::tail(std::size_t lines, std::ostream &out) {
  if (!m_stream.is_open()) return;

  m_offset = findLastLines(lines);
  m_stream.clear();
  m_stream.seekg(m_offset);
  drain(out);
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
