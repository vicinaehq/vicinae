#pragma once
#include <QObject>

/**
 * Per-platform design tokens for shared QML components. Platform variance in
 * shared QML belongs here (or in per-OS root files / component slots), never
 * in inline Qt.platform.os checks.
 */
class StyleBridge : public QObject {
  Q_OBJECT

  // Space pages reserve for the overlaid window header (0 = header is stacked).
  Q_PROPERTY(qreal contentTopInset READ contentTopInset CONSTANT)
  Q_PROPERTY(qreal cardRadius READ cardRadius CONSTANT)
  Q_PROPERTY(qreal cardBorderWidth READ cardBorderWidth CONSTANT)
  Q_PROPERTY(qreal settingsRowVPad READ settingsRowVPad CONSTANT)
  Q_PROPERTY(qreal switchKnobWidth READ switchKnobWidth CONSTANT)

public:
  explicit StyleBridge(QObject *parent = nullptr) : QObject(parent) {}

  qreal contentTopInset() const {
#ifdef Q_OS_MACOS
    return 44;
#else
    return 0;
#endif
  }

  qreal cardRadius() const {
#ifdef Q_OS_MACOS
    return 12;
#else
    return 10;
#endif
  }

  qreal cardBorderWidth() const {
#ifdef Q_OS_MACOS
    return 0;
#else
    return 1;
#endif
  }

  qreal settingsRowVPad() const {
#ifdef Q_OS_MACOS
    return 11;
#else
    return 9;
#endif
  }

  // Wider than tall on macOS: the native knob is a stadium, not a circle.
  qreal switchKnobWidth() const {
#ifdef Q_OS_MACOS
    return 20;
#else
    return 14;
#endif
  }
};
