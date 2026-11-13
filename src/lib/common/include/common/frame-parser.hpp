#pragma once
#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <string_view>

namespace vicinae {

class FrameParser {
public:
  using Handler = std::function<void(std::string_view message)>;

  void setHandler(Handler fn) { m_fn = std::move(fn); }

  void feed(std::string_view chunk) {
    m_data.append(chunk);

    for (;;) {
      if (m_size == 0 && m_data.size() >= sizeof(m_size)) {
        std::memcpy(&m_size, m_data.data(), sizeof(m_size));
        m_data.erase(0, sizeof(m_size));
      }

      if (m_size && m_size <= m_data.size()) {
        if (m_fn) m_fn(std::string_view(m_data).substr(0, m_size));
        m_data.erase(0, m_size);
        m_size = 0;
        continue;
      }

      break;
    }
  }

private:
  std::string m_data;
  std::uint32_t m_size = 0;
  Handler m_fn;
};

} // namespace vicinae
