#pragma once
#include "common.hpp"
#include "extend/detail-model.hpp"
#include "extend/list-model.hpp"
#include "extend/list-view.hpp"
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

class VerticalMetadataPane : public QWidget {
  QVBoxLayout *layout;

public:
  VerticalMetadataPane() : layout(new QVBoxLayout) {
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setAlignment(Qt::AlignTop);
    setLayout(layout);
  }

  void addRow(QWidget *left, QWidget *right) {
    auto widget = new QWidget();
    auto rowLayout = new QVBoxLayout();

    rowLayout->setContentsMargins(0, 2, 0, 2);
    rowLayout->addWidget(left);
    rowLayout->addWidget(right);
    widget->setLayout(rowLayout);
    layout->addWidget(widget);
  }

  void addSeparator() { layout->addWidget(new HDivider); }
};

class ExtensionDetailView : public ExtensionComponent {
  Q_OBJECT

  View &parent;
  QHBoxLayout *layout;
  MarkdownView *markdownEditor;
  VerticalMetadataPane *metadata;

private slots:
public:
  ExtensionDetailView(const RootDetailModel &model, View &parent)
      : parent(parent), layout(new QHBoxLayout),
        markdownEditor(new MarkdownView), metadata(new VerticalMetadataPane) {
    layout->setSpacing(0);
    layout->addWidget(markdownEditor, 2);
    layout->addWidget(new VDivider());
    layout->addWidget(metadata, 1);
    layout->setContentsMargins(0, 0, 0, 0);

    parent.hideInput();

    setLayout(layout);
    dispatchModel(model);
  }

  void dispatchModel(const RootDetailModel &model) {
    qDebug() << "set markdown" << model.markdown;
    markdownEditor->setMarkdown(model.markdown);

    ThemeService theme;

    if (model.metadata) {
      auto newMeta = new VerticalMetadataPane();

      for (const auto &item : model.metadata->children) {
        if (auto label = std::get_if<MetadataLabel>(&item)) {
          newMeta->addRow(new QLabel(label->title), new QLabel(label->text));
        }

        if (std::get_if<MetadataSeparator>(&item)) {
          newMeta->addSeparator();
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

          newMeta->addRow(new QLabel(model->title), list);
        }
      }

      layout->replaceWidget(metadata, newMeta);
      newMeta->show();

      metadata->deleteLater();
      metadata = newMeta;
    } else {
      metadata->hide();
    }

    if (model.actions) {
      parent.setActions(*model.actions);
    }

    qDebug() << "dispatching model update";
  }

  void onActionActivated(ActionModel model) {
    qDebug() << "activated" << model.title;
  }
};
