#include "theme-file.hpp"
#include "common/types.hpp"
#include <expected>
#include "theme.hpp"
#include "theme/colors.hpp"
#include "theme/theme-parser.hpp"
#include "utils.hpp"
#include <filesystem>
#include <qfont.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qobjectdefs.h>

namespace fs = std::filesystem;

// clang-format on

void ThemeFile::setParent(const std::shared_ptr<ThemeFile> &file) { m_parent = file; }

const QString &ThemeFile::id() const { return m_data.id; }
const QString &ThemeFile::name() const { return m_data.name; }
const QString &ThemeFile::inherits() const { return m_data.inherits; }
ThemeVariant ThemeFile::variant() const { return m_data.variant; }
const QString &ThemeFile::description() const { return m_data.description; }
const ThemeFile::Icon &ThemeFile::icon() const { return m_data.icon; }
bool ThemeFile::isLight() const { return variant() == ThemeVariant::Light; }
bool ThemeFile::isDark() const { return variant() == ThemeVariant::Dark; }

QColor ThemeFile::resolve(SemanticColor color) const {
  if (auto it = m_data.semantics.find(color); it != m_data.semantics.end()) {
    auto visitor = overloads{[](const QColor &color) { return color; },
                             [&](const ColorRef &ref) {
                               if (ref.color == color) { return inherit(ref.color); }
                               auto color = resolve(ref.color);
                               if (ref.opacity) {
                                 auto bg = resolve(SemanticColor::Background);
                                 float const a = std::clamp(static_cast<float>(*ref.opacity), 0.0f, 1.0f);
                                 color = QColor(qRound(color.red() * a + bg.red() * (1.0f - a)),
                                                qRound(color.green() * a + bg.green() * (1.0f - a)),
                                                qRound(color.blue() * a + bg.blue() * (1.0f - a)));
                               }
                               if (ref.darker) color = color.darker(std::max(0, *ref.darker + 100));
                               if (ref.lighter) color = color.lighter(std::max(0, *ref.lighter + 100));
                               return color;
                             }};

    auto color = std::visit(visitor, it->second);
    return color;
  }
  return deriveSemantic(color);
}

QString ThemeFile::resolveAsString(SemanticColor color) const { return Utils::rgbaFromColor(resolve(color)); }

std::string ThemeFile::toToml() const { return ThemeSerializer().toToml(*this); }

ThemeFile ThemeFile::vicinaeDark() {
  InitData data;
  data.id = "vicinae-dark";
  data.name = "Vicinae Inkwell";
  data.variant = ThemeVariant::Dark;
  data.description = "Cool ink grounds, warm sand accents";
  data.semantics = {
      {SemanticColor::Background, QColor("#0f1014")},
      {SemanticColor::SecondaryBackground, QColor("#15161b")},
      {SemanticColor::ListItemSelectionBackground, QColor("#1c1d23")},
      {SemanticColor::Foreground, QColor("#e7e5e4")},
      {SemanticColor::BackgroundBorder, QColor("#2a241c")},
      {SemanticColor::Accent, QColor("#b8944e")},
      {SemanticColor::AccentForeground, QColor("#0f1014")},
      {SemanticColor::LinkDefault, QColor("#c9a76e")},

      {SemanticColor::Red, QColor("#B9543B")},
      {SemanticColor::Orange, QColor("#F0883E")},
      {SemanticColor::Yellow, QColor("#c9a76e")},
      {SemanticColor::Green, QColor("#3A9C61")},
      {SemanticColor::Cyan, QColor("#6a8a7c")},
      {SemanticColor::Blue, QColor("#2F6FED")},
      {SemanticColor::Magenta, QColor("#BC8CFF")},
      {SemanticColor::Purple, QColor("#BC8CFF")},
  };

  return ThemeFile(data);
}

ThemeFile ThemeFile::vicinaeLight() {
  InitData data;
  data.id = "vicinae-light";
  data.variant = ThemeVariant::Light;
  data.name = "Vicinae Sandstone";
  data.description = "Warm cream grounds, deepened sand accents";
  data.semantics = {
      {SemanticColor::Background, QColor("#faf8f4")},
      {SemanticColor::SecondaryBackground, QColor("#f0ece5")},
      {SemanticColor::ListItemSelectionBackground, QColor("#e6e1d8")},
      {SemanticColor::Foreground, QColor("#1c1917")},
      {SemanticColor::BackgroundBorder, QColor("#d5cfc3")},
      {SemanticColor::GridItemBackground, QColor("#ebe7df")},
      {SemanticColor::Accent, QColor("#8a6d35")},
      {SemanticColor::AccentForeground, QColor("#faf8f4")},
      {SemanticColor::LinkDefault, QColor("#7a6132")},

      {SemanticColor::Red, QColor("#b9543b")},
      {SemanticColor::Orange, QColor("#c97a30")},
      {SemanticColor::Yellow, QColor("#9a7b3f")},
      {SemanticColor::Green, QColor("#2d7a4d")},
      {SemanticColor::Cyan, QColor("#44635a")},
      {SemanticColor::Blue, QColor("#1F6FEB")},
      {SemanticColor::Magenta, QColor("#8B6EBF")},
      {SemanticColor::Purple, QColor("#8B6EBF")},
  };

  return ThemeFile(data);
}

QColor ThemeFile::inherit(SemanticColor color) const {
  return m_parent ? m_parent->resolve(color) : QColor();
}

QColor ThemeFile::deriveSemantic(SemanticColor color) const {
  switch (color) {
  case SemanticColor::LinkDefault:
    return resolve(SemanticColor::Blue);
  case SemanticColor::LinkVisited:
    return resolve(SemanticColor::Purple);

  case SemanticColor::LoadingBar:
    return resolve(SemanticColor::TextMuted);
  case SemanticColor::DynamicToastSpinner:
    return resolve(SemanticColor::Foreground);

  case SemanticColor::ListItemSelectionForeground:
    return resolve(SemanticColor::Foreground);

  case SemanticColor::ListItemSecondarySelectionForeground:
    return resolve(SemanticColor::ListItemSelectionForeground);
  case SemanticColor::ListItemSecondarySelectionBackground:
    return resolve(SemanticColor::ListItemSelectionBackground);
  case SemanticColor::ListItemSecondaryHoverForeground:
    return resolve(SemanticColor::ListItemSecondarySelectionForeground);
  case SemanticColor::ListItemSecondaryHoverBackground:
    return isDark() ? resolve(SemanticColor::ListItemSecondarySelectionBackground).darker(110)
                    : resolve(SemanticColor::ListItemSecondarySelectionBackground).lighter(110);

  case ButtonPrimaryBackground:
    return withAlphaF(resolve(SemanticColor::ListItemSelectionBackground), 0.7);
  case ButtonPrimaryForeground:
    return resolve(SemanticColor::Foreground);
  case ButtonPrimaryHoverBackground:
    return withAlphaF(resolve(SemanticColor::ButtonPrimaryBackground), 0.7);
  case ButtonPrimaryHoverForeground:
    return resolve(SemanticColor::ButtonPrimaryBackground);
  case SemanticColor::ButtonPrimaryFocusOutline:
    return resolve(SemanticColor::Accent);

  case SemanticColor::ScrollBarBackground:
    return withAlphaF(resolve(SemanticColor::Foreground), 0.25);
  case SemanticColor::SecondaryScrollBarBackground:
    return resolve(SemanticColor::ScrollBarBackground);

  case SemanticColor::PopoverBackground:
    return resolve(SemanticColor::SecondaryBackground);
  case SemanticColor::PopoverBorder:
    return resolve(SemanticColor::MainWindowBorder);

  case SemanticColor::TooltipBackground:
    return resolve(SemanticColor::PopoverBackground);
  case SemanticColor::TooltipBorder:
    return resolve(SemanticColor::PopoverBorder);
  case SemanticColor::TooltipForeground:
    return resolve(SemanticColor::Foreground);

  case SemanticColor::SecondaryBackgroundBorder:
    return resolve(SemanticColor::BackgroundBorder).lighter();
  case SemanticColor::ListItemSelectionBackground:
    return resolve(SemanticColor::SecondaryBackground);
  case SemanticColor::ListItemHoverBackground:
    return isDark() ? resolve(SemanticColor::ListItemSelectionBackground).darker(110)
                    : resolve(SemanticColor::ListItemSelectionBackground).lighter(110);
  case SemanticColor::ListItemHoverForegroud:
    return resolve(SemanticColor::Foreground);

  case SemanticColor::GridItemBackground:
    return resolve(SemanticColor::SecondaryBackground);
  case SemanticColor::GridItemSelectionOutline:
    return resolve(SemanticColor::Foreground);
  case SemanticColor::GridItemHoverOutline:
    return resolve(SemanticColor::GridItemSelectionOutline);

  case SemanticColor::AccentForeground:
    return QColor("#FFFFFF");

  case SemanticColor::TextSelectionBackground:
    return resolve(SemanticColor::Accent);
  case SemanticColor::TextSelectionForeground:
    return resolve(SemanticColor::AccentForeground);

  case SemanticColor::InputBorder:
    return resolve(SemanticColor::BackgroundBorder);
  case SemanticColor::InputBorderFocus:
    return resolve(SemanticColor::Accent);
  case SemanticColor::InputBorderError:
    return resolve(SemanticColor::Red);
  case SemanticColor::TextPlaceholder:
    return withAlphaF(resolve(SemanticColor::TextPrimary), 0.6);

  case SemanticColor::Accent:
    return resolve(SemanticColor::Blue);

  case SemanticColor::TextPrimary:
    return resolve(SemanticColor::Foreground);
  case SemanticColor::TextMuted:
    return withAlphaF(resolve(SemanticColor::TextPrimary), 0.7);
  case SemanticColor::TextDanger:
    return resolve(SemanticColor::Red);
  case SemanticColor::TextSuccess:
    return resolve(SemanticColor::Green);

  case SemanticColor::MainWindowBorder: {
    auto color = resolve(SemanticColor::BackgroundBorder);
    return isLight() ? color.darker(110) : color.lighter(110);
  }
  case SemanticColor::SettingsWindowBorder:
    return resolve(SemanticColor::MainWindowBorder);
  case SemanticColor::StatusBarBackground:
    return resolve(SemanticColor::SecondaryBackground);

  case SemanticColor::ShortcutIndicatorBorder:
    return resolve(SemanticColor::BackgroundBorder);

  default:
    break;
  }

  return inherit(color);
}

QColor ThemeFile::withAlphaF(const QColor &color, float alpha) {
  QColor col = color;
  col.setAlphaF(alpha);
  return col;
}

std::expected<ThemeFile, std::string> ThemeFile::fromFile(const fs::path &path) {
  return ThemeParser().parse(path);
}

std::optional<std::filesystem::path> ThemeFile::path() const { return m_data.path; }
