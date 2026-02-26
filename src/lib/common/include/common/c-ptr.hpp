#pragma once
#include <cstdlib>
#include <memory>

struct CFree {
  void operator()(void *p) const noexcept {
    free(p); // NOLINT(cppcoreguidelines-no-malloc)
  }
};

template <typename T> using CPtr = std::unique_ptr<T, CFree>;
