#pragma once
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPainter>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qicon.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qlocalserver.h>
#include <qlogging.h>
#include <qmainwindow.h>
#include <qmath.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qprocess.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class Chip : public QLabel {
public:
  Chip(const QString &s) {
    setText(s);
    setContentsMargins(10, 5, 10, 5);
    setProperty("class", "chip");
  }
};
