#pragma once
#include <QImage>
#include <QString>
#include <vector>
#include "ui/image/url.hpp"

struct TrayItem {
  enum class Status { Passive, Active, NeedsAttention };
  enum class Category { ApplicationStatus, Communications, SystemServices, Hardware };

  QString busName;
  QString path;
  QString id;
  QString title;
  Status status = Status::Active;
  Category category = Category::ApplicationStatus;
  QString iconName;
  QString iconThemePath;
  QImage iconPixmap;
  QString attentionIconName;
  QImage attentionIconPixmap;
  QString tooltipTitle;
  QString tooltipDescription;
  bool itemIsMenu = false;
  QString menuPath;

  QString key() const { return busName + path; }
  bool hasMenu() const { return !menuPath.isEmpty() && menuPath != "/"; }
  ImageURL icon() const;
};

struct TrayMenuItem {
  enum class ToggleType { None, Checkmark, Radio };

  int id = 0;
  QString label;
  bool enabled = true;
  bool visible = true;
  bool separator = false;
  bool submenu = false;
  ToggleType toggleType = ToggleType::None;
  int toggleState = -1;
  QString iconName;
  QImage iconData;
  std::vector<TrayMenuItem> children;

  QString plainLabel() const;
};

struct TrayEntry {
  TrayItem item;
  std::vector<TrayMenuItem> menu;
};
