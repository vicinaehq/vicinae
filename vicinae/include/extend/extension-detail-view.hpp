#pragma once
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

/*
class ExtensionDetailView : public ExtensionComponent {
  Q_OBJECT

  View &parent;
  QHBoxLayout *layout;
  MarkdownView *markdownEditor;
  VerticalMetadata *metadata;

private slots:
public:
  ExtensionDetailView(const RootDetailModel &model, View &parent)
      : parent(parent), layout(new QHBoxLayout),
        markdownEditor(new MarkdownView), metadata(new VerticalMetadata()) {
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
      auto newMeta = new VerticalMetadata();

      for (const auto &child : model.metadata->children) {
        newMeta->addItem(child);
      }

      layout->replaceWidget(metadata, newMeta);
      newMeta->show();

      metadata->deleteLater();
      metadata = newMeta;
    } else {
      metadata->hide();
    }

    if (model.actions) {
      // parent.setActions(*model.actions);
    }

    qDebug() << "dispatching model update";
  }

  void onActionActivated(ActionModel model) {
    qDebug() << "activated" << model.title;
  }
};
*/
