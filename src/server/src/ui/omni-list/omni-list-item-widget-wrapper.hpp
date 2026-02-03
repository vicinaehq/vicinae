#pragma once
#include "ui/omni-list/omni-list-item-widget.hpp"
#include <qevent.h>
#include <qtmetamacros.h>

class OmniListItemWidgetWrapper : public QWidget {
  Q_OBJECT

  OmniListItemWidget *_widget;
  int _index;

  void resizeEvent(QResizeEvent *event) override;
  void handleClicked() const;
  void handleDoubleClicked() const;
  void handleRightClicked() const;

public:
  OmniListItemWidgetWrapper(QWidget *parent = nullptr);
  ~OmniListItemWidgetWrapper() { /*qDebug() << "delete item widget wrapper";*/ }

  OmniListItemWidget *widget() const;
  int index() const;

  void setSelected(bool selected);
  void setWidget(OmniListItemWidget *widget);
  void setIndex(int index);

signals:
  void clicked(int index) const;
  void doubleClicked(int index) const;
  void rightClicked(int index) const;
};
