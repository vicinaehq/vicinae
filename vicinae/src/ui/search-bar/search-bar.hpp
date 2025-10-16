#pragma once
#include "argument.hpp"
#include "ui/image/url.hpp"
#include <qcontainerfwd.h>
#include <qlineedit.h>
#include <qtimer.h>

struct CompleterData {
  QList<QString> placeholders;
  QList<QString> values;
  ImageURL iconUrl;
  ArgumentList arguments;
};

class SearchBar : public QLineEdit {
  Q_OBJECT

signals:
  void debouncedTextEdited(const QString &text);
  void pop();

public:
  SearchBar(QWidget *parent = nullptr);
  void setInline(bool isInline);
  void debounce();

protected:
  bool event(QEvent *event) override;

private:
  void refreshStyle();

  bool m_inline = false;
};
