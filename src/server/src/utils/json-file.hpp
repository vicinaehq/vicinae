#pragma once
#include <filesystem>
#include <string>
#include <system_error>
#include <utility>
#include <QDebug>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>

/**
 * A C++ value persisted as one JSON file. Loaded once on construction, written atomically on save()
 * so a crash mid-write never truncates the file. Unknown keys are ignored on read so older builds
 * keep loading files written by newer ones.
 */
template <typename T> class JsonFile {
public:
  explicit JsonFile(std::filesystem::path path) : m_path(std::move(path)) { load(); }

  T &data() { return m_data; }
  const T &data() const { return m_data; }
  const std::filesystem::path &path() const { return m_path; }

  bool save() {
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::create_directories(m_path.parent_path(), ec);

    const auto tmp = fs::path(m_path).concat(".tmp");
    if (auto error = glz::write_file_json(m_data, tmp.string(), m_buf)) {
      qWarning() << "Failed to write" << m_path.string().c_str() << glz::format_error(error);
      return false;
    }

    fs::rename(tmp, m_path, ec);
    if (ec) {
      qWarning() << "Failed to replace" << m_path.string().c_str() << ec.message().c_str();
      fs::remove(tmp, ec);
      return false;
    }
    return true;
  }

private:
  static constexpr auto READ_OPTS = glz::opts{.error_on_unknown_keys = false};

  void load() {
    if (!std::filesystem::exists(m_path)) return;

    if (auto error = glz::read_file_json<READ_OPTS>(m_data, m_path.string(), m_buf)) {
      qWarning() << "Failed to load" << m_path.string().c_str() << glz::format_error(error);
    }
  }

  std::filesystem::path m_path;
  std::string m_buf;
  T m_data{};
};
