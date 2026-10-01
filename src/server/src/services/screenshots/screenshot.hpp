#pragma once

#include <QDateTime>
#include <QSize>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <vector>
#include "fuzzy/fuzzy-searchable.hpp"

struct Screenshot {
  enum class Kind { Image, Recording };

  std::filesystem::path path;
  QDateTime createdAt;
  QSize size;
  Kind kind = Kind::Image;
  std::optional<double> durationSeconds;
};

struct ScreenshotResult {
  std::vector<Screenshot> items;
  std::optional<QString> error;

  const Screenshot *latestImage() const {
    const auto item = std::ranges::find(items, Screenshot::Kind::Image, &Screenshot::kind);
    return item == items.end() ? nullptr : &*item;
  }
};

template <> struct fuzzy::FuzzySearchable<Screenshot> {
  static fuzzy::Match score(const Screenshot &item, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted(
        {{item.path.filename().string(), 1.0}, {item.createdAt.toString(Qt::ISODate).toStdString(), 0.8}},
        query);
  }
};
