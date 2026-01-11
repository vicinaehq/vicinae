#include "extension/date-picker-dialog.hpp"
#include <qboxlayout.h>
#include <qdatetimeedit.h>
#include <qpushbutton.h>

DatePickerDialog::DatePickerDialog(bool includeTime, std::optional<QDateTime> min,
                                   std::optional<QDateTime> max, QWidget *parent)
    : QDialog(parent), m_includeTime(includeTime) {
  setWindowTitle("Select Date");
  setModal(false);
  setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
  setAttribute(Qt::WA_ShowWithoutActivating, false);
  setFocusPolicy(Qt::StrongFocus);

  auto layout = new QVBoxLayout(this);

  m_picker = new QDateTimeEdit(this);
  m_picker->setCalendarPopup(true);
  m_picker->setDateTime(QDateTime::currentDateTime());
  m_picker->setButtonSymbols(QAbstractSpinBox::NoButtons);

  if (includeTime) {
    m_picker->setDisplayFormat("yyyy-MM-dd HH:mm");
  } else {
    m_picker->setDisplayFormat("yyyy-MM-dd");
  }

  if (min && min->isValid()) m_picker->setMinimumDateTime(*min);
  if (max && max->isValid()) m_picker->setMaximumDateTime(*max);

  layout->addWidget(m_picker);

  auto buttonLayout = new QHBoxLayout;
  auto cancelBtn = new QPushButton("Cancel", this);
  auto okBtn = new QPushButton("OK", this);
  okBtn->setDefault(true);

  buttonLayout->addStretch();
  buttonLayout->addWidget(cancelBtn);
  buttonLayout->addWidget(okBtn);
  layout->addLayout(buttonLayout);

  connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
  connect(okBtn, &QPushButton::clicked, this, [this]() {
    QString isoDate;
    if (m_includeTime) {
      isoDate = m_picker->dateTime().toUTC().toString(Qt::ISODateWithMs) + "Z";
    } else {
      // For date-only, use UTC noon to avoid timezone day-shift issues
      isoDate = m_picker->date().toString("yyyy-MM-dd") + "T12:00:00.000Z";
    }
    emit dateSelected(isoDate);
    accept();
  });

  setStyleSheet(ThemeService::instance().inputStyleSheet());
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this,
          [this]() { setStyleSheet(ThemeService::instance().inputStyleSheet()); });
}
