#pragma once
#include "extend/metadata-model.hpp"
#include <qboxlayout.h>
#include <qjsonvalue.h>
#include <qwidget.h>

class HorizontalMetadata : public QWidget {
private:
  QVBoxLayout *layout;
  QVBoxLayout *currentLayout = 0;

  void add(const QString &title, QWidget *widget);

public:
  HorizontalMetadata();

  void addItem(const MetadataItem &item);
  void clear();
};
