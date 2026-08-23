#pragma once
#include "services/selection/abstract-selection-service.hpp"

class DummySelectionService : public AbstractSelectionService {
public:
  QFuture<Result> selectedText() override {
    return QtFuture::makeReadyValueFuture<Result>(
        std::unexpected(QStringLiteral("Selected text is not supported on this platform")));
  }
};
