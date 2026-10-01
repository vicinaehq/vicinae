#pragma once
#include <chrono>
#include <qdebug.h>

class Timer {
  using Clock = std::chrono::high_resolution_clock;
  using TP = std::chrono::time_point<Clock>;
  TP _start;

public:
  void start() { _start = std::chrono::high_resolution_clock::now(); }

  void time(const std::string &name) const {
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration_cast<std::chrono::microseconds>(end - _start).count();
    auto msDuration = duration / 1000;

    qDebug() << QString("TIMER => %1 %2ms").arg(name.c_str()).arg(msDuration);
  }

  Timer() { start(); }
};

namespace timer {
template <typename Func, typename Clock = std::chrono::high_resolution_clock>
std::chrono::nanoseconds time(Func fun) {
  auto start = Clock::now();
  fun();
  auto elapsedNs = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start);
  return elapsedNs;
}

}; // namespace timer
