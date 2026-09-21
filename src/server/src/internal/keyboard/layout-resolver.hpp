#pragma once
#include <memory>
#include <optional>
#include <qnamespace.h>
#include <qtypes.h>

namespace Keyboard {

// Characters a physical key types at its unshifted and shifted levels, as Qt keys.
struct KeyLevels {
  std::optional<Qt::Key> base;
  std::optional<Qt::Key> shifted;
};

class LayoutResolver {
public:
  virtual ~LayoutResolver() = default;
  virtual KeyLevels levels(Qt::Key key, quint32 scanCode) = 0;
};

std::unique_ptr<LayoutResolver> createLayoutResolver();

} // namespace Keyboard
