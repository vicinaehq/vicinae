#pragma once

#include <QString>
#include <array>
#include <KSyntaxHighlighting/Theme>

namespace vicinae::document {

class DocumentStyle;

namespace syntax {

struct StyleInfo {
  QString color;
  bool bold = false;
  bool italic = false;
  bool operator==(const StyleInfo &) const = default;
};

using TextStyle = KSyntaxHighlighting::Theme::TextStyle;
using StyleMap = std::array<StyleInfo, static_cast<int>(TextStyle::Others) + 1>;

StyleMap buildStyleMap(const DocumentStyle &style);
QString highlight(const QString &code, const QString &language, const StyleMap &styles, bool isDark,
                  const QString &monoFamily);

} // namespace syntax

} // namespace vicinae::document
