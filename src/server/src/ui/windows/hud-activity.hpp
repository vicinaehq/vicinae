#pragma once
#include <utility>
#include <QObject>
#include <QString>
#include <QtQml/qqmlregistration.h>

/**
 * Background work shown in the HUD while it runs: a label with a spinner, then an optional final message.
 * Emits finished once, after which the HUD lets go of it.
 */
class HudActivity : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(HudActivity)
  QML_UNCREATABLE("")

  Q_PROPERTY(QString text READ text NOTIFY changed)
  Q_PROPERTY(QString message READ message NOTIFY changed)

public:
  Q_INVOKABLE virtual void cancel() = 0;

signals:
  void changed();
  void finished();

public:
  using QObject::QObject;

  QString text() const { return m_text; }
  QString message() const { return m_message; }

protected:
  void setText(QString text) {
    m_text = std::move(text);
    emit changed();
  }

  void setMessage(QString message) {
    m_message = std::move(message);
    emit changed();
  }

private:
  QString m_text;
  QString m_message;
};
