#pragma once
#include "layout-resolver.hpp"

namespace Keyboard {

class WindowsLayoutResolver : public LayoutResolver {
public:
  Qt::Key unshift(Qt::Key key, quint32 scanCode) override;
};

} // namespace Keyboard
