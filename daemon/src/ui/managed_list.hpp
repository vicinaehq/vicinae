#pragma once
#include "common.hpp"
#include <QListWidget>

class ManagedList : public QListWidget {
  Q_OBJECT

  QList<QString> sectionNames;
  QList<IActionnable *> items;
  QMap<QListWidgetItem *, IActionnable *> widgetToData;

signals:
  void itemSelected(const IActionnable &data);
  void itemActivated(const IActionnable &data);

protected slots:
  void selectionChanged(QListWidgetItem *item, QListWidgetItem *previous);
  void itemActivate(QListWidgetItem *item);

public:
  ManagedList(QWidget *parent = nullptr);

  void clear();
  void selectFirstEligible();
  void addSection(const QString &name);
  void addWidgetItem(IActionnable *data, QWidget *widget);
};
