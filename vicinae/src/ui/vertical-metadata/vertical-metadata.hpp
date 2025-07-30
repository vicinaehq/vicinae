#pragma once
#include "extend/metadata-model.hpp"
#include <qboxlayout.h>
#include <qjsonvalue.h>
#include <qwidget.h>

class VerticalMetadata : public QWidget {
private:
  QVBoxLayout *layout;
  QVBoxLayout *currentLayout = 0;

  void add(const QString &title, QWidget *widget);

public:
  VerticalMetadata();

  void addItem(const MetadataItem &item);
};
