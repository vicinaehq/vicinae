#pragma once
#include <QFuture>
#include <QString>
#include <expected>

class AbstractSelectionService {
public:
  using Result = std::expected<QString, QString>;

  virtual ~AbstractSelectionService() = default;

  virtual QFuture<Result> selectedText() = 0;
};
