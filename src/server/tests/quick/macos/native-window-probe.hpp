#pragma once
#include <QObject>
#include <QVariantList>
#include <QWindow>
#include <qqmlregistration.h>
#include <array>

class NativeWindowProbe : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QVariantList frames READ frames NOTIFY framesChanged)

public:
  Q_INVOKABLE void start(QWindow *window);
  Q_INVOKABLE void stop();

signals:
  void framesChanged();

public:
  explicit NativeWindowProbe(QObject *parent = nullptr) : QObject(parent) {}
  ~NativeWindowProbe() override;
  QVariantList frames() const { return m_frames; }

private:
  QVariantList m_frames;
  std::array<void *, 2> m_observers{};
};
