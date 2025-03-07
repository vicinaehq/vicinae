#pragma once
#include <qcolor.h>
#include <qmap.h>

enum ThemeColor {
  Blue,
  Green,
  Magenta,
  Orange,
  Purple,
  Red,
  Yellow,
  PrimaryText,
  SecondaryText
};

static QMap<QString, ThemeColor> nameToColor = {
    {"blue", ThemeColor::Blue},
    {"green", ThemeColor::Green},
    {"magenta", ThemeColor::Magenta},
    {"orange", ThemeColor::Orange},
    {"purple", ThemeColor::Purple},
    {"red", ThemeColor::Red},
    {"yellow", ThemeColor::Yellow},
    {"primary-text", ThemeColor::PrimaryText},
    {"secondary-text", ThemeColor::SecondaryText},
};

static QMap<ThemeColor, QColor> colorMap = {
    {ThemeColor::Blue, "#4CC9F0"},
    {ThemeColor::Green, "#2BF5A3"},
    {ThemeColor::Magenta, "#FF4D6D"},
    {ThemeColor::Orange, "#FF8C42"},
    {ThemeColor::Purple, "#9D4EDD"},
    {ThemeColor::Red, "#FF5A5F"},
    {ThemeColor::Yellow, "#F6E05E"},
    {ThemeColor::PrimaryText, "#E0E0E0"},
    {ThemeColor::SecondaryText, "#A1A1A1"},
};

class ThemeService {
public:
  QColor getColor(const QString &name) {
    if (auto it = nameToColor.find(name); it != nameToColor.end()) {
      return colorMap.value(*it);
    }

    return name;
  }
};
