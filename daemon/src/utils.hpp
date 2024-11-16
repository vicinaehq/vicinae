#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unistd.h>

struct SplitIterator {
  size_t idx = 0;
  std::string_view data;
  std::string_view charset;

  inline bool isSep(unsigned char c) {
    return charset.find(c) != std::string::npos;
  }

  std::optional<std::string_view> next() {
    size_t tmp = idx;
    std::string_view view;

    while (tmp < data.size()) {
      if (isSep(data.at(tmp))) {
        view = {data.begin() + idx, data.begin() + tmp};
        while (isSep(data.at(tmp)))
          ++tmp;
        break;
      } else {
        ++tmp;
      }
    }

    idx = tmp;

    if (view.empty())
      return std::nullopt;

    return view;
  }

  SplitIterator(std::string_view data, std::string_view charset)
      : data(data), charset(charset) {}
};
