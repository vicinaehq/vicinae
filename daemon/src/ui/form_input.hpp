#include <qboxlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qwidget.h>

class FormInput : public QWidget {

public:
  QLineEdit *input;

  FormInput(const QString &label) {
    input = new QLineEdit();
    auto layout = new QHBoxLayout();

    input->setProperty("class", "form-input");
    layout->setSpacing(20);
    layout->addWidget(new QLabel(label), 1, Qt::AlignVCenter | Qt::AlignRight);
    layout->addWidget(input, 7, Qt::AlignVCenter);

    setLayout(layout);
  }

  void setPlaceholder(const QString &text) { input->setPlaceholderText(text); }

  void setFocus() { input->setFocus(); }
};
