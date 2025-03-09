#pragma once

#include "theme.hpp"
#include <qlabel.h>

class TextLabel : public QLabel {
public:
  TextLabel(const QString &s = "");
};
