#pragma once

#include <chrono>
#include <iostream>
class Timer {
  std::chrono::time_point<std::chrono::system_clock> _start;

public:
  void start() { _start = std::chrono::high_resolution_clock::now(); }

  void time(const std::string &name) const {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - _start).count();

    std::cout << name << " " << duration << "ms" << std::endl;
  }

  Timer() { start(); }
};
