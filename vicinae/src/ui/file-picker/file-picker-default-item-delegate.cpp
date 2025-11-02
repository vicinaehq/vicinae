#include "file-picker-default-item-delegate.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"

void DefaultFilePickerItemDelegate::handleOpen() {
  auto appDb = ServiceRegistry::instance()->appDb();

  if (auto browser = appDb->fileBrowser()) { appDb->launch(*browser, {file().path.c_str()}); }
}

void DefaultFilePickerItemDelegate::handleRemove() { picker()->removeFile(file().path); }

OmniListItemWidget *DefaultFilePickerItemDelegate::createWidget() const {
  SelectedFileWidget *widget = new SelectedFileWidget;
  widget->setRemovable(!readOnly());
  widget->setFile(file());
  return widget;
}

void DefaultFilePickerItemDelegate::attached(QWidget *w) {
  auto widget = static_cast<SelectedFileWidget *>(w);

  connect(widget, &SelectedFileWidget::openClicked, this, &DefaultFilePickerItemDelegate::handleOpen,
          Qt::UniqueConnection);
  connect(widget, &SelectedFileWidget::removeClicked, this, &DefaultFilePickerItemDelegate::handleRemove,
          Qt::UniqueConnection);
}

void DefaultFilePickerItemDelegate::detached(QWidget *w) { disconnect(w); }

bool DefaultFilePickerItemDelegate::hasUniformHeight() const { return true; }
