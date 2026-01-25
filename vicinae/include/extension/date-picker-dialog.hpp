#pragma once
#include "theme.hpp"
#include <optional>
#include <qdatetime.h>
#include <qdialog.h>
#include <qtmetamacros.h>

class QDateTimeEdit;

class DatePickerDialog : public QDialog {
  Q_OBJECT

  QDateTimeEdit *m_picker;
  bool m_includeTime;

public:
  DatePickerDialog(bool includeTime, std::optional<QDateTime> min, std::optional<QDateTime> max,
                   QWidget *parent = nullptr);

signals:
  void dateSelected(const QString &isoDate);
};
