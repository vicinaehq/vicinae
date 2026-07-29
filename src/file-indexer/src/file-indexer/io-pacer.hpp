#pragma once
#include <chrono>
#include <optional>
#include <string>
#include <string_view>

namespace file_indexer {

// Cooperative IO throttle: checkpoint() sleeps proportionally to /proc/pressure/io.
// No-op on kernels without CONFIG_PSI.
class IoPacer {
public:
  static constexpr int CHECKPOINTS_PER_PROBE = 256;

  IoPacer(const char *psiPath = "/proc/pressure/io", int checkpointsPerProbe = CHECKPOINTS_PER_PROBE);

  void checkpoint();

  static std::optional<double> parseSomeAvg10(std::string_view content);

private:
  static constexpr std::chrono::milliseconds MIN_PROBE_INTERVAL{250};

  static constexpr double SOME_AVG10_THRESHOLD = 20.0; // percent
  static constexpr std::chrono::milliseconds MAX_BACKOFF{500};

  std::string m_psiPath;
  int m_checkpointsPerProbe;
  bool m_available = false;
  int m_counter = 0;
  std::chrono::steady_clock::time_point m_lastProbe{};

  void maybeBackOff();
};

} // namespace file_indexer
