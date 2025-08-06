#pragma once
#include "argument.hpp"
#include "navigation-controller.hpp"
#include "ui/image/image.hpp"
#include "ui/image/image.hpp"
#include "ui/image/url.hpp"
#include "ui/inline-input/inline_qline_edit.hpp"
#include <qboxlayout.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qwidget.h>

struct ArgCompleter : public QWidget {
  Q_OBJECT

public:
  ArgumentList m_args;
  QHBoxLayout *m_layout = new QHBoxLayout;
  ImageWidget *m_icon = new ImageWidget();
  std::vector<InlineQLineEdit *> m_inputs;

public:
  ArgCompleter(QWidget *parent = nullptr);

  void clear();
  void setIconUrl(const ImageURL &url);
  void setArguments(const ArgumentList &args);
  void setValues(const ArgumentValues values);
  std::vector<std::pair<QString, QString>> collect();
  void validate();

signals:
  void activated() const;
  void destroyed() const;
  void valueChanged(const std::vector<std::pair<QString, QString>> &arguments);
};
