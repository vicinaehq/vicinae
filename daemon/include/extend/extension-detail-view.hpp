#pragma once
#include "common.hpp"
#include "extend/detail-model.hpp"
#include "extend/list-model.hpp"
#include "extend/root-detail-model.hpp"
#include "extension.hpp"
#include "image-viewer.hpp"
#include "markdown-renderer.hpp"
#include "tag.hpp"
#include "theme.hpp"
#include "view.hpp"
#include <QListWidget>
#include <QTextEdit>
#include <qboxlayout.h>
#include <qdir.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qnamespace.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qsizepolicy.h>
#include <qtextedit.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ListItemWidget : public QWidget {
  QWidget *icon;
  QLabel *name;
  QLabel *category;
  QLabel *kind;

public:
  ListItemWidget(QWidget *image, const QString &name, const QString &category,
                 const QString &kind, QWidget *parent = nullptr)
      : QWidget(parent), icon(image), name(new QLabel), category(new QLabel),
        kind(new QLabel) {

    auto mainLayout = new QHBoxLayout();

    setLayout(mainLayout);

    auto left = new QWidget();
    auto leftLayout = new QHBoxLayout();

    this->name->setText(name);
    this->category->setText(category);
    this->category->setProperty("class", "minor");

    left->setLayout(leftLayout);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(this->icon);
    leftLayout->addWidget(this->name);
    leftLayout->addWidget(this->category);

    mainLayout->addWidget(left, 0, Qt::AlignLeft);

    this->kind->setText(kind);
    this->kind->setProperty("class", "minor");
    mainLayout->addWidget(this->kind, 0, Qt::AlignRight);
  }
};

class MetadataWidget : public QWidget {
  QVBoxLayout *layout;

public:
  MetadataWidget() : layout(new QVBoxLayout) {
    layout->setContentsMargins(10, 10, 10, 10);
    setLayout(layout);
  }

  void addRow(QWidget *left, QWidget *right) {
    auto widget = new QWidget();
    auto rowLayout = new QHBoxLayout();

    rowLayout->setContentsMargins(0, 2, 0, 2);
    rowLayout->addWidget(left, 0, Qt::AlignLeft | Qt::AlignVCenter);
    rowLayout->addWidget(right, 0, Qt::AlignRight | Qt::AlignVCenter);
    widget->setLayout(rowLayout);
    layout->addWidget(widget);
  }

  void addSeparator() { layout->addWidget(new HDivider); }
};

class DetailWidget : public QWidget {
  QVBoxLayout *layout;
  MarkdownView *markdownEditor;
  MetadataWidget *metadata;
  HDivider *divider;

public:
  DetailWidget()
      : layout(new QVBoxLayout), markdownEditor(new MarkdownView()),
        metadata(new MetadataWidget), divider(new HDivider) {
    layout->addWidget(markdownEditor, 1);
    layout->addWidget(divider);
    layout->addWidget(metadata);
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);
  }

  void dispatchModel(const DetailModel &model) {
    ThemeService theme;

    markdownEditor->setMarkdown(model.markdown);

    if (model.metadata.children.isEmpty()) {
      divider->hide();
      metadata->hide();
    } else {
      divider->show();
      metadata->show();
    }

    for (size_t i = 0; i != model.metadata.children.size(); ++i) {
      auto &item = model.metadata.children.at(i);

      if (auto label = std::get_if<MetadataLabel>(&item)) {
        metadata->addRow(new QLabel(label->title), new QLabel(label->text));
      }

      if (std::get_if<MetadataSeparator>(&item)) {
        metadata->addSeparator();
      }

      if (auto model = std::get_if<TagListModel>(&item)) {
        auto list = new TagList;

        for (const auto &item : model->items) {
          auto tag = new Tag();

          tag->setText(item.text);

          if (item.color) {
            tag->setColor(theme.getColor(*item.color));
          } else {
            tag->setColor(theme.getColor("primary-text"));
          }

          if (item.icon) {
            tag->addLeftWidget(
                ImageViewer::createFromModel(*item.icon, {16, 16}));
          }

          list->addTag(tag);
        }

        metadata->addRow(new QLabel(model->title), list);
      }
    }
  }
};

class ExtensionDetailView : public ExtensionComponent {
  Q_OBJECT

  View &parent;
  QHBoxLayout *layout;
  MarkdownView *markdownEditor;

private slots:
public:
  ExtensionDetailView(const RootDetailModel &model, View &parent)
      : parent(parent), layout(new QHBoxLayout),
        markdownEditor(new MarkdownView) {
    layout->setSpacing(0);
    layout->addWidget(markdownEditor);
    layout->addWidget(new VDivider());
    layout->addWidget(new MetadataWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    parent.hideInput();

    setLayout(layout);
    dispatchModel(model);
  }

  void dispatchModel(const RootDetailModel &model) {
    markdownEditor->setMarkdown(model.markdown);

    if (model.metadata) {
    } else {
    }

    qDebug() << "dispatching model update";
  }

  void onActionActivated(ActionModel model) {
    qDebug() << "activated" << model.title;
  }
};
