#include "file-indexer/io-pacer.hpp"
#include <algorithm>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>
#include <thread>

namespace file_indexer {

IoPacer::IoPacer(const char *psiPath, int checkpointsPerProbe)
    : m_psiPath(psiPath), m_checkpointsPerProbe(checkpointsPerProbe) {
  std::error_code ec;
  m_available = std::filesystem::exists(m_psiPath, ec);
}

std::optional<double> IoPacer::parseSomeAvg10(std::string_view content) {
  auto const some = content.find("some ");
  if (some == std::string_view::npos) return std::nullopt;

  static constexpr std::string_view KEY = "avg10=";
  auto const key = content.find(KEY, some);
  if (key == std::string_view::npos) return std::nullopt;

  auto const begin = content.data() + key + KEY.size();
  double value = 0;

  if (auto [_, ec] = std::from_chars(begin, content.data() + content.size(), value); ec != std::errc{}) {
    return std::nullopt;
  }

  return value;
}

void IoPacer::checkpoint() {
  if (!m_available) return;
  if (++m_counter < m_checkpointsPerProbe) return;
  m_counter = 0;

  auto const now = std::chrono::steady_clock::now();
  if (now - m_lastProbe < MIN_PROBE_INTERVAL) return;
  m_lastProbe = now;

  maybeBackOff();
}

void IoPacer::maybeBackOff() {
  std::ifstream ifs(m_psiPath);
  std::stringstream content;

  content << ifs.rdbuf();

  auto const pressure = parseSomeAvg10(content.view());
  if (!pressure || *pressure < SOME_AVG10_THRESHOLD) return;

  double const ratio = std::min(*pressure / 100.0, 1.0);
  std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::milliseconds>(MAX_BACKOFF * ratio));
}

} // namespace file_indexer
