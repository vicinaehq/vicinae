#pragma once
#include "extend/metadata-model.hpp"
#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include <qboxlayout.h>
#include <qjsonvalue.h>
#include <qwidget.h>

class VerticalMetadata : public VerticalScrollArea {
private:
  QWidget *container = nullptr;

public:
  void setMetadata(const std::vector<MetadataItem> &metadatas);

  VerticalMetadata();
};
