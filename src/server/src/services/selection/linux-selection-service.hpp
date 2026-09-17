#pragma once
#include "services/selection/abstract-selection-service.hpp"
#include <QObject>

class ClipboardService;

class LinuxSelectionService : public QObject, public AbstractSelectionService {
  Q_OBJECT

public:
  explicit LinuxSelectionService(ClipboardService &clipboard);
  QFuture<Result> selectedText() override;

private:
  QString m_primaryText;
};
