#pragma once
#include "extend/metadata-model.hpp"
#include "ui/horizontal-metadata.hpp"
#include <qboxlayout.h>
#include <qwidget.h>

class DetailWidget : public QWidget {
  QWidget *m_contentWidget = new QWidget;
  HorizontalMetadata *m_metadata = new HorizontalMetadata;
  QVBoxLayout *m_layout = new QVBoxLayout;

public:
  void setContentWidget(QWidget *widget);
  QWidget *contentWidget() const;

  void clearMetadata();
  void setMetadata(const MetadataModel &metadata);

  DetailWidget(QWidget *parent = nullptr);
};
