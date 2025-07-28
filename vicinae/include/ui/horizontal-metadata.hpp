#pragma once
#include "extend/metadata-model.hpp"
#include "settings/extension-settings.hpp"
#include <qboxlayout.h>
#include <qjsonvalue.h>
#include <qscrollarea.h>
#include <qwidget.h>

class HorizontalMetadata : public VerticalScrollArea {
private:
  QVBoxLayout *layout;
  QVBoxLayout *currentLayout = 0;

  void add(const QString &title, QWidget *widget);

public:
  HorizontalMetadata();

  void addItem(const MetadataItem &item);
  void clear();
};
