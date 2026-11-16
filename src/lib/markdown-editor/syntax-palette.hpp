#pragma once

#include <KSyntaxHighlighting/Theme>
#include <QColor>
#include <algorithm>
#include <array>
#include <cmath>

namespace Vicinae::Syntax {

using TextStyle = KSyntaxHighlighting::Theme::TextStyle;
using StyleMap = std::array<QColor, static_cast<int>(TextStyle::Others) + 1>;

inline QColor mix(const QColor &from, const QColor &to, double amount) {
  return QColor::fromRgbF(std::lerp(from.redF(), to.redF(), amount),
                          std::lerp(from.greenF(), to.greenF(), amount),
                          std::lerp(from.blueF(), to.blueF(), amount));
}

inline double contrast(const QColor &first, const QColor &second) {
  auto luminance = [](const QColor &color) {
    auto linear = [](double channel) {
      return channel <= 0.04045 ? channel / 12.92 : std::pow((channel + 0.055) / 1.055, 2.4);
    };
    return 0.2126 * linear(color.redF()) + 0.7152 * linear(color.greenF()) + 0.0722 * linear(color.blueF());
  };
  const auto a = luminance(first);
  const auto b = luminance(second);
  return (std::max(a, b) + 0.05) / (std::min(a, b) + 0.05);
}

struct Palette {
  QColor background = QColor("#202124");
  QColor foreground = QColor("#d4d4d4");
  QColor muted = QColor("#92969c");
  QColor accent = QColor("#8ab4f8");
  QColor blue = QColor("#8ab4f8");
  QColor purple = QColor("#c5a5df");
  QColor cyan = QColor("#8fc9cf");
  QColor green = QColor("#a8c799");
  QColor orange = QColor("#d3b18d");
  QColor red = QColor("#d99797");

  bool operator==(const Palette &) const = default;

  QColor readable(const QColor &color) const {
    const auto minimum = std::min(4.5, contrast(foreground, background));
    auto result = color;
    for (int step = 1; step <= 10 && contrast(result, background) < minimum; ++step)
      result = mix(color, foreground, step / 10.0);
    return result;
  }

  QColor restrained(const QColor &color) const {
    return readable(mix(mix(foreground, color, 0.85), background, 0.04));
  }

  StyleMap styles() const {
    StyleMap map;
    map.fill(foreground);
    auto set = [&](TextStyle style, const QColor &color) { map[style] = restrained(color); };
    for (auto style : {TextStyle::Keyword, TextStyle::ControlFlow, TextStyle::Import, TextStyle::Attribute})
      set(style, purple);
    for (auto style : {TextStyle::Function, TextStyle::Information, TextStyle::Others})
      set(style, blue);
    for (auto style : {TextStyle::BuiltIn, TextStyle::Extension, TextStyle::DataType})
      set(style, cyan);
    for (auto style :
         {TextStyle::Char, TextStyle::String, TextStyle::VerbatimString, TextStyle::SpecialString})
      set(style, green);
    for (auto style : {TextStyle::Preprocessor, TextStyle::SpecialChar, TextStyle::DecVal, TextStyle::BaseN,
                       TextStyle::Float, TextStyle::Constant, TextStyle::Warning})
      set(style, orange);
    for (auto style : {TextStyle::Alert, TextStyle::Error})
      set(style, red);
    for (auto style : {TextStyle::Comment, TextStyle::Documentation, TextStyle::Annotation,
                       TextStyle::CommentVar, TextStyle::RegionMarker})
      map[style] = readable(muted);
    return map;
  }
};

} // namespace Vicinae::Syntax
