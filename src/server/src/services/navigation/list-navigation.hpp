#pragma once
#include "service-registry.hpp"
#include "config/config.hpp"

// Centralizes selection stepping semantics for every list-like surface
// (root list, grids, action panel, completions). Wrap-around at the ends
// is opt-in through the `wrap_navigation` config toggle; the default is
// to clamp at the first and last selectable item.
class ListNavigation {
public:
  static bool wrapEnabled() { return ServiceRegistry::instance()->config()->value().wrapNavigation; }

  template <typename IsSelectable>
  static int nextIndex(int from, int direction, int count, IsSelectable &&isSelectable) {
    if (count <= 0) return from;

    const bool wrap = wrapEnabled();
    int idx = from;

    for (int steps = 0; steps < count; ++steps) {
      idx += direction;

      if (idx < 0) {
        if (!wrap) return from;
        idx = count - 1;
      } else if (idx >= count) {
        if (!wrap) return from;
        idx = 0;
      }

      if (isSelectable(idx)) return idx;
    }

    return from;
  }
};
