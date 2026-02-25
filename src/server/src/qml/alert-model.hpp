#pragma once
#include <QColor>
#include <QObject>
#include <QString>

class AlertWidget;
class DialogContentWidget;
class NavigationController;

class AlertModel : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
  Q_PROPERTY(QString title READ title NOTIFY changed)
  Q_PROPERTY(QString message READ message NOTIFY changed)
  Q_PROPERTY(QString confirmText READ confirmText NOTIFY changed)
  Q_PROPERTY(QString cancelText READ cancelText NOTIFY changed)
  Q_PROPERTY(QColor confirmColor READ confirmColor NOTIFY changed)
  Q_PROPERTY(QColor cancelColor READ cancelColor NOTIFY changed)
  Q_PROPERTY(QString iconSource READ iconSource NOTIFY changed)

public:
  explicit AlertModel(NavigationController &nav, QObject *parent = nullptr);

  bool visible() const { return m_visible; }
  QString title() const { return m_title; }
  QString message() const { return m_message; }
  QString confirmText() const { return m_confirmText; }
  QString cancelText() const { return m_cancelText; }
  QColor confirmColor() const { return m_confirmColor; }
  QColor cancelColor() const { return m_cancelColor; }
  QString iconSource() const { return m_iconSource; }

  Q_INVOKABLE void confirm();
  Q_INVOKABLE void cancel();

  void dismiss();

signals:
  void visibleChanged();
  void changed();

private:
  void handleAlertRequested(DialogContentWidget *widget);

  bool m_visible = false;
  QString m_title;
  QString m_message;
  QString m_confirmText;
  QString m_cancelText;
  QColor m_confirmColor;
  QColor m_cancelColor;
  QString m_iconSource;
  AlertWidget *m_widget = nullptr;
};
