#pragma once
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

class RemoteImageWidget : public QWidget {
  Q_OBJECT
  QString url;
  QLabel *label;
  QNetworkAccessManager *netman;
  QSize size;

private slots:
  void requestFinished(QNetworkReply *reply) {
    QPixmap pix;

    pix.loadFromData(reply->readAll());

    /*
auto scaledPixmap =
    pix.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    */

    label->setAlignment(Qt::AlignCenter);
    label->setPixmap(pix.scaled(label->width(), label->height(),
                                Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // label->resize(pix.size());
  }

public:
  RemoteImageWidget(const QString &url, QSize size)
      : url(url), label(new QLabel), netman(new QNetworkAccessManager),
        size(size) {
    QNetworkRequest req;

    connect(netman, &QNetworkAccessManager::finished, this,
            &RemoteImageWidget::requestFinished);

    req.setUrl(url);
    netman->get(req);

    qDebug() << "lol" << url;

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);

    setLayout(layout);
  }

  ~RemoteImageWidget() { netman->deleteLater(); }
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
  MarkdownImage(cmark_node *node) : layout(new QVBoxLayout) {
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
    layout->addWidget(new RemoteImageWidget(url, {res.width, res.height}));

    setLayout(layout);
  }
};

class MarkdownParagraph : public QWidget {
  QVBoxLayout *layout;

public:
  MarkdownParagraph(cmark_node *node) : layout(new QVBoxLayout) {
    cmark_node *child = cmark_node_first_child(node);

    while (child) {
      switch (cmark_node_get_type(child)) {
      case CMARK_NODE_TEXT:
        layout->addWidget(new QLabel(cmark_node_get_literal(child)));
        break;
      case CMARK_NODE_IMAGE:
        qDebug() << "add image layout";
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
};

class MarkdownView : public QWidget {
  QVBoxLayout *layout;

public:
  MarkdownView() : layout(new QVBoxLayout) {
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);
  }

  void setMarkdown(const QString &markdown) {
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
};
