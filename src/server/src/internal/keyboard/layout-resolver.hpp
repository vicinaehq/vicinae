#pragma once
#include <memory>
#include <qnamespace.h>
#include <qtypes.h>

namespace Keyboard {

class LayoutResolver {
public:
  virtual ~LayoutResolver() = default;
  virtual Qt::Key unshift(Qt::Key key, quint32 scanCode) = 0;
};

std::unique_ptr<LayoutResolver> createLayoutResolver();

} // namespace Keyboard
