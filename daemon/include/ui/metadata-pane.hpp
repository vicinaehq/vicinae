#pragma once
#include "extend/metadata-model.hpp"
#include <qboxlayout.h>
#include <qjsonvalue.h>
#include <qwidget.h>

class MetadataPane : public QWidget {
public:
  enum Direction {
    Vertical,
    Horizontal,
  };

private:
  Direction direction;
  QVBoxLayout *layout;

  void add(const QString &title, QWidget *widget, Direction direction);

public:
  MetadataPane(Direction direction);

  void addItem(const MetadataItem &item, Direction direction = Horizontal);
};
