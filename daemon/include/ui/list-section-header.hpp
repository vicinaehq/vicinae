#pragma once
#include <qboxlayout.h>
#include "ui/omni-list-item-widget.hpp"

class OmniListSectionHeader : public OmniListItemWidget {
public:
  OmniListSectionHeader(const QString &title, const QString &subtitle, size_t count);
};
