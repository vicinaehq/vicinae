#include "theme-file.hpp"
#include "common.hpp"
#include "expected.hpp"
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
                               if (ref.opacity) color.setAlphaF(std::clamp(*ref.opacity, 0.0, 1.0));
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
  data.name = "Vicinae Dark";
  data.variant = ThemeVariant::Dark;
  data.description = "Default Vicinae dark palette";
  data.semantics = {
      {SemanticColor::Background, QColor("#1A1A1A")},
      {SemanticColor::ListItemSelectionBackground, withAlphaF("#2E2E2E", 0.7)},
      {SemanticColor::Foreground, QColor("#E8E6E1")},
      {SemanticColor::BackgroundBorder, QColor("#2E2E2E")},
      {SemanticColor::SecondaryBackground, QColor("#242424")},

      {SemanticColor::Red, QColor("#B9543B")},
      {SemanticColor::Orange, QColor("#F0883E")},
      {SemanticColor::Yellow, QColor("#BFAE78")},
      {SemanticColor::Green, QColor("#3A9C61")},
      {SemanticColor::Cyan, QColor("#18A5B3")},
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
  data.name = "Vicinae Light";
  data.description = "Default Vicinae light palette";
  data.semantics = {
      {SemanticColor::Background, QColor("#F4F2EE")},
      {SemanticColor::ListItemSelectionBackground, withAlphaF("#D8D6D1", 0.7)},
      {SemanticColor::Foreground, QColor("#1A1A1A")},
      {SemanticColor::BackgroundBorder, QColor("#D8D6D1")},
      {SemanticColor::SecondaryBackground, QColor("#E6E4DF")},

      {SemanticColor::Red, QColor("#C25C49")},
      {SemanticColor::Orange, QColor("#DA8A48")},
      {SemanticColor::Yellow, QColor("#BFAE78")},
      {SemanticColor::Green, QColor("#3A9C61")},
      {SemanticColor::Cyan, QColor("#18A5B3")},
      {SemanticColor::Blue, QColor("#1F6FEB")},
      {SemanticColor::Magenta, QColor("#A48ED6")},
      {SemanticColor::Purple, QColor("#A48ED6")},
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
  case SemanticColor::ListItemSecondaryHoverBackground:
    return withAlphaF(resolve(SemanticColor::ListItemSecondarySelectionBackground), 0.7);

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
    return resolve(SemanticColor::ListItemSelectionBackground);
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
    return withAlphaF(resolve(SemanticColor::SecondaryBackground), 0.7);
  case SemanticColor::ListItemHoverBackground:
    return withAlphaF(resolve(SemanticColor::ListItemSelectionBackground), 0.7);
  case SemanticColor::ListItemHoverForegroud:
    return resolve(SemanticColor::Foreground);

  case SemanticColor::GridItemBackground:
    return withAlphaF(resolve(SemanticColor::SecondaryBackground), 0.7);
  case SemanticColor::GridItemSelectionOutline:
    return resolve(SemanticColor::Foreground);
  case SemanticColor::GridItemHoverOutline:
    return withAlphaF(resolve(SemanticColor::GridItemSelectionOutline));

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

tl::expected<ThemeFile, std::string> ThemeFile::fromFile(const fs::path &path) {
  return ThemeParser().parse(path);
}

std::optional<std::filesystem::path> ThemeFile::path() const { return m_data.path; }
