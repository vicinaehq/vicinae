#pragma once

#include <QString>
#include <cstddef>

inline QString formatDMenuOutput(const QString &text, std::size_t originalIndex, bool outputIndex) {
  return outputIndex ? QString::number(static_cast<qulonglong>(originalIndex)) : text;
}
