#pragma once
#include <chrono>
#include <qdebug.h>

class Timer {
  std::chrono::time_point<std::chrono::high_resolution_clock> _start;

public:
  void start() { _start = std::chrono::high_resolution_clock::now(); }

  void time(const std::string &name) const {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - _start).count();

    qDebug() << QString("TIMER => %1 %2ms").arg(name.c_str()).arg(duration);
  }

  Timer() { start(); }
};
