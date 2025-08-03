#pragma once
#include "extend/metadata-model.hpp"
#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include <qboxlayout.h>
#include <qjsonvalue.h>
#include <qscrollarea.h>
#include <qwidget.h>

class HorizontalMetadata : public VerticalScrollArea {
private:
  QWidget *container;

public:
  HorizontalMetadata();

  void setMetadata(const std::vector<MetadataItem> &metadatas);
};
