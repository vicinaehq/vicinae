#pragma once
#include "file-picker.hpp"

class DefaultFilePickerItemDelegate : public QObject, public FilePicker::AbstractFilePickerItemDelegate {
  void handleOpen();
  void handleRemove();

  OmniListItemWidget *createWidget() const override;
  void attached(QWidget *w) override;
  void detached(QWidget *w) override;
  bool hasUniformHeight() const override;

public:
  virtual ~DefaultFilePickerItemDelegate() {}
};
