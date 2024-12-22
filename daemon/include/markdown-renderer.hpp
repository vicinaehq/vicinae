#pragma once
#include "remote-image-viewer.hpp"
#include <QDebug>
#include <QString>
#include <cmark.h>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qwidget.h>

struct Resolution {
  int width;
  int height;
};

static Resolution parseResolution(const QString &s) {
  auto ss = s.split("x");

  if (ss.size() != 2) {
    return {0, 0};
  }

  return {ss.at(0).toInt(), ss.at(1).toInt()};
}

class MarkdownImage : public QWidget {
  QVBoxLayout *layout;

public:
  MarkdownImage(cmark_node *node);
};

class MarkdownParagraph : public QWidget {
  QVBoxLayout *layout;

public:
  MarkdownParagraph(cmark_node *node);
};

class MarkdownView : public QWidget {
  QVBoxLayout *layout;

public:
  MarkdownView();
  void setMarkdown(const QString &markdown);
};
