#pragma once
#include <QLineEdit>

class InlineQLineEdit : public QLineEdit {
  Q_OBJECT

public:
  InlineQLineEdit(const QString &placeholder, QWidget *parent = nullptr);

  void setError(const QString &error);
  void clearError();
  void handleTextChanged(const QString &s);

protected:
  // void paintEvent(QPaintEvent *) override;

private:
  void resizeFromText(const QString &s);
  void updateStyle();

  QString m_error;
};
