#pragma once
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iosfwd>

namespace cli {

class FileTailer {
public:
  explicit FileTailer(std::filesystem::path path);

  bool isOpen() const { return m_stream.is_open(); }

  std::streamoff drain(std::ostream &out);
  void follow(std::ostream &out);

private:
  static constexpr auto POLL_INTERVAL = std::chrono::milliseconds(200);

  void reopen(std::ostream &out);

  std::filesystem::path m_path;
  std::ifstream m_stream;
  std::streamoff m_offset = 0;
};

} // namespace cli
