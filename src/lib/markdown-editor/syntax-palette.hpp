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
  QColor keyword = QColor("#8eb7e8");
  QColor controlFlow = QColor("#c6a0db");
  QColor function = QColor("#e6c384");
  QColor type = QColor("#7fc5c0");
  QColor string = QColor("#b2c98f");
  QColor number = QColor("#e9aa80");
  QColor constant = QColor("#c4a7e7");
  QColor attribute = QColor("#9abfdd");
  QColor preprocessor = QColor("#d99ca7");
  QColor special = QColor("#e6c384");
  QColor error = QColor("#ef8b82");
  QColor comment = QColor("#868b98");
  QColor heading = QColor("#e6c384");
  QColor emphasis = QColor("#d99ca7");
  QColor link = QColor("#7fc5c0");
  QColor linkTarget = QColor("#96a9c8");
  QColor inlineCode = QColor("#e9aa80");
  QColor marker = QColor("#8eb7e8");

  bool operator==(const Palette &) const = default;

  QColor readable(const QColor &color) const {
    const auto minimum = std::min(4.5, contrast(foreground, background));
    const auto opaque = mix(background, color, color.alphaF());
    auto result = opaque;
    for (int step = 1; step <= 100 && contrast(result, background) < minimum; ++step)
      result = mix(opaque, foreground, step / 100.0);
    return result;
  }

  StyleMap styles() const {
    StyleMap map;
    map.fill(foreground);
    auto set = [&](TextStyle style, const QColor &color) { map[style] = readable(color); };
    set(TextStyle::Keyword, keyword);
    set(TextStyle::ControlFlow, controlFlow);
    set(TextStyle::Import, controlFlow);
    set(TextStyle::Function, function);
    set(TextStyle::BuiltIn, function);
    set(TextStyle::Extension, type);
    set(TextStyle::DataType, type);
    set(TextStyle::Attribute, attribute);
    set(TextStyle::Preprocessor, preprocessor);
    set(TextStyle::Constant, constant);
    set(TextStyle::Information, type);
    set(TextStyle::Others, special);
    set(TextStyle::SpecialChar, special);
    for (auto style :
         {TextStyle::Char, TextStyle::String, TextStyle::VerbatimString, TextStyle::SpecialString})
      set(style, string);
    for (auto style : {TextStyle::DecVal, TextStyle::BaseN, TextStyle::Float})
      set(style, number);
    set(TextStyle::Warning, number);
    for (auto style : {TextStyle::Alert, TextStyle::Error})
      set(style, error);
    for (auto style : {TextStyle::Comment, TextStyle::Documentation, TextStyle::Annotation,
                       TextStyle::CommentVar, TextStyle::RegionMarker})
      set(style, comment);
    return map;
  }
};

} // namespace Vicinae::Syntax
