#pragma once
#include "omni-icon.hpp"
#include "ui/selectable-omni-list-widget.hpp"

class DefaultListItemWidget : public SelectableOmniListWidget {
  OmniIcon *_icon;
  QLabel *_name;
  QLabel *_category;
  QLabel *_kind;

public:
  void setName(const QString &name);
  void setIcon(const QString &name);
  void setCategory(const QString &category);
  void setKind(const QString &kind);

  DefaultListItemWidget(const QString &iconDescriptor, const QString &name, const QString &category,
                        const QString &kind, QWidget *parent = nullptr);
};
