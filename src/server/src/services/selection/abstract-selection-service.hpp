#pragma once
#include <QFuture>
#include <QString>
#include <expected>
#include <qeventloop.h>

class AbstractSelectionService {
public:
  using Result = std::expected<QString, QString>;

  virtual ~AbstractSelectionService() = default;

  virtual QFuture<Result> selectedText() = 0;

  // Spins up a nested event loop to get the selected text synchronously
  Result selectedTextSync() {
    QEventLoop loop;
    auto future = selectedText();
    future.then(&loop, [&loop](AbstractSelectionService::Result) {
      QMetaObject::invokeMethod(&loop, &QEventLoop::quit, Qt::QueuedConnection);
    });
    loop.exec();
    return future.result();
  }
};
