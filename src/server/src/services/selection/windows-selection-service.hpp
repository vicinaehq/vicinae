#pragma once
#include "services/selection/abstract-selection-service.hpp"

class WindowsSelectionService : public AbstractSelectionService {
public:
  WindowsSelectionService();
  ~WindowsSelectionService() override;

  QFuture<Result> selectedText() override;

private:
  void *m_hook = nullptr;
};
