#pragma once
#include <qboxlayout.h>
#include <qdir.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qstringview.h>
#include <qsyntaxhighlighter.h>
#include <qtextedit.h>
#include <qwidget.h>
#include <QTextEdit>

/**
 * A simple text file viewer.
 * You can give it a path and it will load the content and show it.
 */
class TextFileViewer : public QWidget {
  QTextEdit *edit;

public:
  void load(const std::filesystem::path &path);
  void load(const QByteArray &data);
  void updateStyle();

  TextFileViewer();
};
