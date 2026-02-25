#pragma once
#include <QColor>
#include <algorithm>
#include <cmath>

class ContrastHelper {
public:
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

  static double getContrastRatio(const QColor &color1, const QColor &color2) {
    double lum1 = getRelativeLuminance(color1);
    double lum2 = getRelativeLuminance(color2);

    double lighter = std::max(lum1, lum2);
    double darker = std::min(lum1, lum2);

    return (lighter + 0.05) / (darker + 0.05);
  }

  static QColor getTonalContrastColor(const QColor &background, double minRatio = 4.5) {
    double bgLuminance = getRelativeLuminance(background);
    bool needsLighter = bgLuminance < 0.5;

    int h, s, l;
    background.getHsl(&h, &s, &l);

    int newH = h;
    int newS = s;
    int newL = l;

    if (needsLighter) {
      newS = std::max(150, s);
      newL = 200;
    } else {
      newS = std::max(150, s);
      newL = 50;
    }

    QColor derived = QColor::fromHsl(newH, newS, newL);

    double ratio = getContrastRatio(background, derived);
    int iterations = 0;

    while (ratio < minRatio && iterations < 30) {
      if (needsLighter) {
        newL = std::min(255, newL + 5);
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

  static QColor getTonalContrastColor(const QColor &background, double minRatio, double colorAmount) {
    colorAmount = std::max(0.0, std::min(1.0, colorAmount));

    double bgLuminance = getRelativeLuminance(background);
    bool needsLighter = bgLuminance < 0.5;

    int h, s, l;
    background.getHsl(&h, &s, &l);

    int newH = h;
    int newS = static_cast<int>(s * colorAmount);
    int newL;

    if (needsLighter) {
      newL = 245;
    } else {
      newL = 20;
    }

    QColor derived = QColor::fromHsl(newH, newS, newL);

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
