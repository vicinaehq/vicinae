#pragma once
#include "ui/divider/hdivider.hpp"
#include "ui/horizontal-metadata.hpp"
#include <qscrollarea.h>

class DetailWidget : public QWidget {
public:
  DetailWidget(QWidget *parent = nullptr) : QWidget(parent) { setupUI(); }

  void setMetadata(const std::vector<MetadataItem> &items);
  void clearMetadata();
  QWidget *content() const;
  void setContent(QWidget *widget);

private:
  void resizeEvent(QResizeEvent *event) override;
  void setupUI();

  HorizontalMetadata *m_metadata = new HorizontalMetadata;
  QScrollArea *m_contentScrollArea = new QScrollArea(this);
  HDivider *hdivider = new HDivider;
};
