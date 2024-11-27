#include <qboxlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qwidget.h>

class FormInput : public QWidget {
  QLabel *error;

public:
  FormInput(const QString &label, QWidget *item) {
    // input = new QLineEdit();
    auto layout = new QHBoxLayout();
    error = new QLabel();

    // input->setProperty("class", "form-input");
    layout->setSpacing(20);
    layout->addWidget(new QLabel(label), 1, Qt::AlignVCenter | Qt::AlignRight);
    layout->addWidget(item, 4, Qt::AlignVCenter);
    layout->addWidget(error, 2, Qt::AlignVCenter);

    setLayout(layout);
  }
};
