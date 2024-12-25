#include "ui/text-label.hpp"
#include <qlabel.h>

TextLabel::TextLabel(const QString &s) : QLabel(s) {
  setStyleSheet("QLabel { color: rgba(255, 255, "
                "255, 170); font-size: 12px; }");
}
