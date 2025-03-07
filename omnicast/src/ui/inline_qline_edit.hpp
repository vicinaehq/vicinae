#pragma once
#include <QLineEdit>

class InlineQLineEdit : public QLineEdit {
  Q_OBJECT

private:
  void resizeFromText(const QString &s);

public:
  InlineQLineEdit(const QString &placeholder, QWidget *parent = nullptr);

protected slots:
  void textChanged(const QString &s);
};
