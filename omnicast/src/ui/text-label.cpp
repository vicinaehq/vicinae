#include "ui/text-label.hpp"
#include <qlabel.h>
#include <qpalette.h>

TextLabel::TextLabel(const QString &s) : QLabel(s) { setProperty("subtext", true); }
