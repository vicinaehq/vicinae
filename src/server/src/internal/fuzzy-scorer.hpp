#pragma once

#include "fuzzy/scored.hpp"
#include <QtConcurrent>
#include <functional>
#include <qfuture.h>
#include <span>
#include <string_view>
#include <vector>

/**
 * A scorer meant to be reused across several searches, as it maintains its own backing store
 * in order to spare allocations.
 * Automatically splits the dataset in batches and dispatch the scoring work to multiple threads if the
 * dataset is huge.
 */
template <typename T> class FuzzyScorer {
public:
  using Scorer = std::function<int(const T &item, std::string_view query)>;
  static constexpr const size_t BATCH_SIZE = 500;
  using TScored = Scored<const T *>;

  std::span<Scored<const T *>> score(std::span<const T> items, std::string_view query,
                                     const Scorer &scorer) const {
    m_data.reserve(items.size());

    if (items.size() < BATCH_SIZE) { return scoreSync(items, query, scorer); }
    return scoreParallel(items, query, scorer);
  }

private:
  std::span<Scored<const T *>> scoreSync(std::span<const T> items, std::string_view query,
                                         const Scorer &scorer) const {
    m_data.clear();

    for (auto &item : items) {
      if (auto score = scorer(item, query)) { m_data.emplace_back(TScored(&item, score)); }
    }

    return m_data;
  }

  std::span<Scored<const T *>> scoreParallel(std::span<const T> items, std::string_view query,
                                             const Scorer &scorer) const {
    std::vector<QFuture<size_t>> futures;
    const auto batchCount = items.size() / BATCH_SIZE;

    m_data.resize(items.size());
    futures.resize(batchCount);

    for (size_t i = 0; i != batchCount; ++i) {
      const size_t start = i * BATCH_SIZE;
      const size_t end = std::min(start + BATCH_SIZE, items.size());

      futures[i] = QtConcurrent::run([this, start, end, scorer, query, items]() {
        size_t zeroCount = 0;

        for (auto i = start; i != end; ++i) {
          const auto &item = items[i];

          if (auto score = scorer(item, query)) {
            m_data[i].data = &item;
            m_data[i].score = score;
          } else {
            m_data[i].score = 0;
            zeroCount += 1;
          }
        }

        return zeroCount;
      });
    }

    auto future = QtFuture::whenAll(futures.begin(), futures.end());
    future.waitForFinished(); // all captures are safe, since we block here
    size_t zeroCount = 0;

    for (const auto &future : futures) {
      zeroCount += future.result();
    }

    std::ranges::sort(m_data, std::greater{});

    // all zeros are on the right, we can just cut them off from view
    return {m_data.data(), m_data.size() - zeroCount};
  }

  mutable std::vector<Scored<const T *>> m_data;
};
