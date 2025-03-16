#pragma once
#include <QVBoxLayout>
#include <QWidget>
#include <cmark.h>

struct Resolution {
  int width;
  int height;
};

static Resolution parseResolution(const QString &s) {
  auto ss = s.split("x");

  if (ss.size() != 2) { return {0, 0}; }

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
