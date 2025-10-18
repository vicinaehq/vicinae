#include "builtin-icon-loader.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/image/image.hpp"
#include "svg-image-loader.hpp"
#include <qcolor.h>

class ContrastHelper {
public:
  // Calculate relative luminance according to WCAG
  static double getRelativeLuminance(const QColor &color) {
    auto linearize = [](double val) {
      val = val / 255.0;
      return (val <= 0.03928) ? val / 12.92 : std::pow((val + 0.055) / 1.055, 2.4);
    };

    double r = linearize(color.red());
    double g = linearize(color.green());
    double b = linearize(color.blue());

    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
  }

  // Calculate contrast ratio between two colors
  static double getContrastRatio(const QColor &color1, const QColor &color2) {
    double lum1 = getRelativeLuminance(color1);
    double lum2 = getRelativeLuminance(color2);

    double lighter = std::max(lum1, lum2);
    double darker = std::min(lum1, lum2);

    return (lighter + 0.05) / (darker + 0.05);
  }

  // Get a lighter/darker version of the same color with sufficient contrast
  // This creates the "light blue on blue" or "dark cyan on cyan" effect
  static QColor getTonalContrastColor(const QColor &background, double minRatio = 4.5) {
    double bgLuminance = getRelativeLuminance(background);
    bool needsLighter = bgLuminance < 0.5;

    // Convert to HSL (better for lightness manipulation)
    int h, s, l;
    background.getHsl(&h, &s, &l);

    int newH = h; // Keep the same hue
    int newS = s; // Keep similar saturation
    int newL = l;

    if (needsLighter) {
      // Background is dark, make icon much lighter (almost white but tinted)
      // Keep high saturation to maintain strong visible color tint
      newS = std::max(150, s); // Keep high saturation for visible tint
      newL = 200;              // Light but not too light
    } else {
      // Background is light, make icon much darker (almost black but tinted)
      newS = std::max(150, s); // Keep saturation high
      newL = 50;               // Very dark, but not pure black (0)
    }

    QColor derived = QColor::fromHsl(newH, newS, newL);

    // Fine-tune lightness to meet contrast requirement
    double ratio = getContrastRatio(background, derived);
    int iterations = 0;

    while (ratio < minRatio && iterations < 30) {
      if (needsLighter) {
        newL = std::min(255, newL + 5);
        // Keep saturation more stable as we adjust
        if (newL > 240) { newS = std::max(50, newS - 2); }
      } else {
        newL = std::max(0, newL - 5);
      }
      derived = QColor::fromHsl(newH, newS, newL);
      ratio = getContrastRatio(background, derived);
      iterations++;
    }

    return derived;
  }

  // Alternative: Specify how "pure" the contrast should be (0.0 = pure white/black, 1.0 = full color)
  static QColor getTonalContrastColor(const QColor &background, double minRatio, double colorAmount) {
    // Clamp colorAmount between 0 and 1
    colorAmount = std::max(0.0, std::min(1.0, colorAmount));

    double bgLuminance = getRelativeLuminance(background);
    bool needsLighter = bgLuminance < 0.5;

    int h, s, l;
    background.getHsl(&h, &s, &l);

    int newH = h;
    int newS = static_cast<int>(s * colorAmount);
    int newL;

    if (needsLighter) {
      // Start very light
      newL = 245;
    } else {
      // Start very dark
      newL = 20;
    }

    QColor derived = QColor::fromHsl(newH, newS, newL);

    // Adjust to meet contrast requirement
    double ratio = getContrastRatio(background, derived);
    int iterations = 0;

    while (ratio < minRatio && iterations < 30) {
      if (needsLighter) {
        newL = std::min(255, newL + 3);
      } else {
        newL = std::max(0, newL - 3);
      }
      derived = QColor::fromHsl(newH, newS, newL);
      ratio = getContrastRatio(background, derived);
      iterations++;
    }

    return derived;
  }
};

void BuiltinIconLoader::render(const RenderConfig &config) { emit dataUpdated(renderSync(config)); }

QPixmap BuiltinIconLoader::renderSync(const RenderConfig &config) {
  QPixmap canva(config.size * config.devicePixelRatio);
  int margin = 0;

  canva.fill(Qt::transparent);

  if (m_backgroundColor) {
    OmniPainter painter(&canva);
    int side = qMin(config.size.width(), config.size.height());
    qreal radius = side * 0.25 * config.devicePixelRatio;
    margin = qRound(side * 0.15 * config.devicePixelRatio);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(painter.colorBrush(*m_backgroundColor));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(canva.rect(), radius, radius);
  }

  QMargins margins{margin, margin, margin, margin};
  QRect iconRect = canva.rect().marginsRemoved(margins);
  SvgImageLoader loader(m_iconName);

  if (m_backgroundColor) {
    QColor color = OmniPainter::resolveColor(*m_backgroundColor);
    loader.render(canva, iconRect, ContrastHelper::getTonalContrastColor(color, 3));
  } else {
    loader.render(canva, iconRect, config.fill.value_or(SemanticColor::Foreground));
  }

  canva.setDevicePixelRatio(config.devicePixelRatio);

  return canva;
}

void BuiltinIconLoader::setFillColor(const std::optional<ColorLike> &color) { m_fillColor = color; }
void BuiltinIconLoader::setBackgroundColor(const std::optional<ColorLike> &color) {
  m_backgroundColor = color;
}

BuiltinIconLoader::BuiltinIconLoader(const QString &iconName)
    : m_iconName(iconName), m_fillColor(SemanticColor::Foreground) {}
