#pragma once
#include <cstdint>
#include <deque>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

class SearchHistory {
public:
  struct Entry {
    std::string q;
    std::uint64_t ts = 0;
  };

  explicit SearchHistory(const std::filesystem::path &path);

  void add(std::string_view q);
  std::optional<Entry> at(int offset) const;

private:
  struct Data {
    std::deque<Entry> entries;
  };

  void loadFromDisk();
  void saveToDisk();

  std::filesystem::path m_path;
  std::string m_buf;
  Data m_data;
};
