#include "ui/omni-list/omni-list-item-widget-wrapper.hpp"
#include "ui/omni-list/omni-list-item-widget.hpp"
#include <qnamespace.h>

void OmniListItemWidgetWrapper::handleRightClicked() const { emit rightClicked(_index); }

void OmniListItemWidgetWrapper::handleDoubleClicked() const { emit doubleClicked(_index); }

void OmniListItemWidgetWrapper::handleClicked() const { emit clicked(_index); }

void OmniListItemWidgetWrapper::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  if (_widget) { _widget->setFixedSize(event->size()); }
}

void OmniListItemWidgetWrapper::setWidget(OmniListItemWidget *widget) {
  widget->setParent(this);
  connect(widget, &OmniListItemWidget::clicked, this, &OmniListItemWidgetWrapper::handleClicked);
  connect(widget, &OmniListItemWidget::doubleClicked, this, &OmniListItemWidgetWrapper::handleDoubleClicked);
  connect(widget, &OmniListItemWidget::rightClicked, this, &OmniListItemWidgetWrapper::handleRightClicked);
  _widget = widget;
}

void OmniListItemWidgetWrapper::setIndex(int index) { _index = index; }

void OmniListItemWidgetWrapper::setSelected(bool selected) {
  if (_widget) _widget->selectionChanged(selected);
}

OmniListItemWidget *OmniListItemWidgetWrapper::widget() const { return _widget; }
int OmniListItemWidgetWrapper::index() const { return _index; }

OmniListItemWidgetWrapper::OmniListItemWidgetWrapper(QWidget *parent) : QWidget(parent), _widget(nullptr) {
  setAttribute(Qt::WA_Hover);
}
