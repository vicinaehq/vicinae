#pragma once
#include <QColor>
#include <QtQml/qqmlregistration.h>
#include "ui/quick/document-highlighter.hpp"

class PlaceholderHighlighter : public DocumentHighlighter {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QColor braceColor READ braceColor WRITE setBraceColor NOTIFY colorsChanged)
  Q_PROPERTY(QColor nameColor READ nameColor WRITE setNameColor NOTIFY colorsChanged)
  Q_PROPERTY(QColor keyColor READ keyColor WRITE setKeyColor NOTIFY colorsChanged)
  Q_PROPERTY(QColor stringColor READ stringColor WRITE setStringColor NOTIFY colorsChanged)

public:
  explicit PlaceholderHighlighter(QObject *parent = nullptr);

  QColor braceColor() const { return m_braceColor; }
  void setBraceColor(const QColor &color) { setColor(m_braceColor, color); }
  QColor nameColor() const { return m_nameColor; }
  void setNameColor(const QColor &color) { setColor(m_nameColor, color); }
  QColor keyColor() const { return m_keyColor; }
  void setKeyColor(const QColor &color) { setColor(m_keyColor, color); }
  QColor stringColor() const { return m_stringColor; }
  void setStringColor(const QColor &color) { setColor(m_stringColor, color); }

signals:
  void colorsChanged();

protected:
  void highlightBlock(const QString &text) override;

private:
  void setColor(QColor &slot, const QColor &color);

  QColor m_braceColor;
  QColor m_nameColor;
  QColor m_keyColor;
  QColor m_stringColor;
};
