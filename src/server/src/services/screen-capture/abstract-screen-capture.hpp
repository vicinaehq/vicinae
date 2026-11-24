#pragma once

#include <QFuture>
#include <QImage>
#include <QObject>
#include <QScreen>
#include <expected>
#include <string>

/**
 * Errors are meant for logs and AI models, so they are not translated.
 */
class AbstractScreenCapture : public QObject {
public:
  using Result = std::expected<QImage, std::string>;

  using QObject::QObject;

  virtual bool isSupported() const = 0;
  virtual QFuture<Result> capture(const QScreen &screen) = 0;
};
