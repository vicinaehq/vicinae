#pragma once

template <typename T> struct Scored {
  T data;
  int score = 0;

  bool operator<(const Scored<T> &rhs) const { return score < rhs.score; }
  bool operator>(const Scored<T> &rhs) const { return score > rhs.score; }
  bool operator==(const Scored<T> &rhs) const { return score == rhs.score; }
};
