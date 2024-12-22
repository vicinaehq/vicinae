#include "markdown-renderer.hpp"
#include "remote-image-viewer.hpp"
#include <qnamespace.h>

MarkdownImage::MarkdownImage(cmark_node *node) : layout(new QVBoxLayout) {
  const char *p = cmark_node_get_url(node);
  QString url;

  // TODO: handle error better

  if (p)
    url = p;

  Resolution res = {640, 360};
  auto altNode = cmark_node_first_child(node);

  if (cmark_node_get_type(altNode) == CMARK_NODE_TEXT) {
    QString lit = cmark_node_get_literal(altNode);
    auto ss = lit.split("|");
    QString alt = ss.at(0);

    if (ss.size() > 1) {
      res = parseResolution(ss.at(1));
    }
  }

  layout->setContentsMargins(0, 0, 0, 0);

  auto remoteImg = new RemoteImageViewer();

  remoteImg->load(url, Qt::AlignCenter);
  layout->addWidget(remoteImg, 0);
  setLayout(layout);
}

MarkdownParagraph::MarkdownParagraph(cmark_node *node)
    : layout(new QVBoxLayout) {
  cmark_node *child = cmark_node_first_child(node);

  while (child) {
    switch (cmark_node_get_type(child)) {
    case CMARK_NODE_TEXT:
      layout->addWidget(new QLabel(cmark_node_get_literal(child)));
      break;
    case CMARK_NODE_IMAGE:
      layout->addWidget(new MarkdownImage(child));
      break;
    default:
      qDebug() << "unhandled paragraph child of type"
               << cmark_node_first_child(node);
      break;
    }

    child = cmark_node_next(child);
  }

  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);
}

MarkdownView::MarkdownView() : layout(new QVBoxLayout) {
  layout->setContentsMargins(0, 0, 0, 0);

  setLayout(layout);
}

void MarkdownView::setMarkdown(const QString &markdown) {
  auto buf = markdown.toUtf8();
  qDebug() << "markdown" << markdown;
  cmark_node *root = cmark_parse_document(buf.data(), buf.size(), 0);
  cmark_node *currentNode = cmark_node_first_child(root);

  while (!layout->isEmpty())
    layout->takeAt(0);

  while (currentNode) {
    auto type = cmark_node_get_type(currentNode);

    qDebug() << "markdown type" << cmark_node_get_type_string(currentNode);

    if (type == CMARK_NODE_PARAGRAPH) {
      layout->addWidget(new MarkdownParagraph(currentNode));
    }

    if (type == CMARK_NODE_HEADING) {
      auto textNode = cmark_node_first_child(currentNode);

      if (cmark_node_get_type(textNode) == CMARK_NODE_TEXT) {
        auto literal = cmark_node_get_literal(textNode);

        qDebug() << "heading" << literal;

        layout->addWidget(new QLabel(literal));
      }
    }

    currentNode = cmark_node_next(currentNode);
  }

  cmark_node_free(root);
}
