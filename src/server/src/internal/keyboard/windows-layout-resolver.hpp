#pragma once
#include "layout-resolver.hpp"

namespace Keyboard {

class WindowsLayoutResolver : public LayoutResolver {
public:
  KeyLevels levels(Qt::Key key, quint32 scanCode) override;
};

} // namespace Keyboard
