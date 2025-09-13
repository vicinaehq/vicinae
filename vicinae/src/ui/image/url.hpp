#pragma once
#include "extend/image-model.hpp"
#include "proto/ui.pb.h"
#include "theme.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <filesystem>
#include <qstringview.h>
#include <vector>
#include <QString>

enum ImageURLType { Invalid, Builtin, Favicon, System, Http, Local, Emoji, DataURI };

static std::vector<std::pair<QString, ImageURLType>> iconTypes = {
    {"favicon", Favicon}, {"omnicast", Builtin}, {"system", System},
    {"http", Http},       {"https", Http},       {"local", Local},
};

static std::vector<std::pair<QString, SemanticColor>> colorTints = {
    // Basic colors
    {"blue", SemanticColor::Blue},
    {"green", SemanticColor::Green},
    {"magenta", SemanticColor::Magenta},
    {"orange", SemanticColor::Orange},
    {"purple", SemanticColor::Purple},
    {"red", SemanticColor::Red},
    {"yellow", SemanticColor::Yellow},
    {"cyan", SemanticColor::Cyan},

    // Text colors
    {"text-primary", SemanticColor::TextPrimary},
    {"text-secondary", SemanticColor::TextSecondary},
    {"text-tertiary", SemanticColor::TextTertiary},
    {"text-disabled", SemanticColor::TextDisabled},
    {"text-on-accent", SemanticColor::TextOnAccent},
    {"text-error", SemanticColor::TextError},
    {"text-success", SemanticColor::TextSuccess},
    {"text-warning", SemanticColor::TextWarning},

    // Background colors
    {"main-background", SemanticColor::MainBackground},
    {"main-hover-background", SemanticColor::MainHoverBackground},
    {"main-selected-background", SemanticColor::MainSelectedBackground},
    {"secondary-background", SemanticColor::SecondaryBackground},
    {"tertiary-background", SemanticColor::TertiaryBackground},

    // Button states
    {"button-primary", SemanticColor::ButtonPrimary},
    {"button-primary-hover", SemanticColor::ButtonPrimaryHover},
    {"button-primary-pressed", SemanticColor::ButtonPrimaryPressed},
    {"button-primary-disabled", SemanticColor::ButtonPrimaryDisabled},
    {"button-secondary", SemanticColor::ButtonSecondary},
    {"button-secondary-hover", SemanticColor::ButtonSecondaryHover},
    {"button-secondary-pressed", SemanticColor::ButtonSecondaryPressed},
    {"button-secondary-disabled", SemanticColor::ButtonSecondaryDisabled},
    {"button-destructive", SemanticColor::ButtonDestructive},
    {"button-destructive-hover", SemanticColor::ButtonDestructiveHover},
    {"button-destructive-pressed", SemanticColor::ButtonDestructivePressed},

    // Input states
    {"input-background", SemanticColor::InputBackground},
    {"input-border", SemanticColor::InputBorder},
    {"input-border-focus", SemanticColor::InputBorderFocus},
    {"input-border-error", SemanticColor::InputBorderError},
    {"input-placeholder", SemanticColor::InputPlaceholder},

    // UI elements
    {"border", SemanticColor::Border},
    {"border-subtle", SemanticColor::BorderSubtle},
    {"border-strong", SemanticColor::BorderStrong},
    {"separator", SemanticColor::Separator},
    {"shadow", SemanticColor::Shadow},

    // Status colors
    {"status-background", SemanticColor::StatusBackground},
    {"status-border", SemanticColor::StatusBorder},
    {"status-hover", SemanticColor::StatusHover},
    {"error-background", SemanticColor::ErrorBackground},
    {"error-border", SemanticColor::ErrorBorder},
    {"success-background", SemanticColor::SuccessBackground},
    {"success-border", SemanticColor::SuccessBorder},
    {"warning-background", SemanticColor::WarningBackground},
    {"warning-border", SemanticColor::WarningBorder},

    // Interactive elements
    {"link-default", SemanticColor::LinkDefault},
    {"link-hover", SemanticColor::LinkHover},
    {"link-visited", SemanticColor::LinkVisited},

    // Special states
    {"focus", SemanticColor::Focus},
    {"overlay", SemanticColor::Overlay},
    {"tooltip", SemanticColor::Tooltip},
    {"tooltip-text", SemanticColor::TooltipText}};

class ImageURL {
  ImageURLType _type = ImageURLType::Invalid;
  bool _isValid = false;
  QString _name;
  SemanticColor _bgTint;
  SemanticColor _fgTint;
  OmniPainter::ImageMaskType _mask = OmniPainter::ImageMaskType::NoMask;
  std::optional<QString> _fallback;
  std::optional<ColorLike> _fillColor = std::nullopt;

  // url dependant custom params
  std::map<QString, QString> m_params;

public:
  ImageURL &circle() {
    setMask(OmniPainter::CircleMask);
    return *this;
  }

  static SemanticColor tintForName(const QString &name) {
    for (const auto &[n, t] : colorTints) {
      if (name == n) return t;
    }

    return SemanticColor::InvalidTint;
  }

  static QString nameForTint(SemanticColor type) {
    for (const auto &[n, t] : colorTints) {
      if (t == type) return n;
    }

    return {};
  }

  static ImageURLType typeForName(const QString &name) {
    for (const auto &[n, t] : iconTypes) {
      if (name == n) return t;
    }

    return ImageURLType::Invalid;
  }

  static QString nameForType(ImageURLType type) {
    for (const auto &[n, t] : iconTypes) {
      if (t == type) return n;
    }

    return {};
  }

  bool isValid() { return _isValid; }
  operator bool() { return isValid(); }

  QString toString() const { return url().toString(); }

  std::optional<QString> fallback() const { return _fallback; }
  ImageURL &withFallback(const ImageURL &fallback);

  QUrl url() const;

  ImageURL &param(const QString &name, const QString &value);
  std::optional<QString> param(const QString &name) const;

  ImageURLType type() const;
  const QString &name() const;
  SemanticColor foregroundTint() const;
  SemanticColor backgroundTint() const;
  const std::optional<ColorLike> &fillColor() const;
  OmniPainter::ImageMaskType mask() const;

  void setType(ImageURLType type);
  void setName(const QString &name);
  void setSize(QSize size);

  ImageURL &setFill(const std::optional<ColorLike> &color);
  ImageURL &setMask(OmniPainter::ImageMaskType mask);
  ImageURL &setForegroundTint(SemanticColor tint);
  ImageURL &setBackgroundTint(SemanticColor tint);

  ImageURL();
  ImageURL(const QString &s) noexcept;
  ImageURL(const proto::ext::ui::Image &image);
  ImageURL(const ImageLikeModel &imageLike);
  ImageURL(const QUrl &url);

  bool operator==(const ImageURL &rhs) const;
  operator QString() const { return toString(); }

  static ImageURL builtin(const QString &name);
  static ImageURL favicon(const QString &domain);
  static ImageURL system(const QString &name);
  static ImageURL local(const QString &path);
  static ImageURL local(const std::filesystem::path &path);
  static ImageURL http(const QUrl &httpUrl);
  static ImageURL emoji(const QString &emoji);
  static ImageURL rawData(const QByteArray &data, const QString &mimeType);
};
