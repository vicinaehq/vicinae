#pragma once

#include <QString>
#include <QByteArray>
#include <cmark-gfm.h>
#include <cmark-gfm-extension_api.h>
#include "math-renderer.hpp"

namespace vicinae::document {

namespace markdown_math {
QByteArray normalizeDelimiters(const QByteArray &markdown);
cmark_syntax_extension *extension();
bool isMath(cmark_node *node);
bool isDisplay(cmark_node *node);
QString render(cmark_node *node, const QString &color, math::Resources &resources);
QString render(const QString &latex, const QString &source, bool display, const QString &color,
               math::Resources &resources);
} // namespace markdown_math

} // namespace vicinae::document
