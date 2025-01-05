#pragma once

#include "app-database.hpp"
#include "app.hpp"
#include "extend/metadata-model.hpp"
#include "filesystem-database.hpp"
#include "indexer-service.hpp"
#include "navigation-list-view.hpp"
#include "ui/test-list.hpp"
#include <QtPdf/QPdfDocument>
#include <qboxlayout.h>
#include <qfileinfo.h>
#include <qicon.h>
#include <qmimedatabase.h>
#include <qnamespace.h>
#include <qnetworkrequest.h>
#include <qthreadpool.h>

class FilesView : public NavigationListView {
  AppWindow &app;
  QString baseQuery;
  Service<IndexerService> indexer;

  class FileListItemDetail : public AbstractNativeListItemDetail {
    QFileInfo info;
    QString iconName;

    QWidget *createView() const override {
      auto widget = new QWidget();
      auto layout = new QHBoxLayout();

      if (info.fileName().endsWith("pdf")) {
        auto doc = new QPdfDocument();

        auto err = doc->load(info.absoluteFilePath());

        if (doc->error() != QPdfDocument::Error::None) {
          qDebug() << "error loading" << info.absoluteFilePath();
        } else {
          qDebug() << "load pdf " << info.absoluteFilePath();
        }

        auto originalSize = doc->pagePointSize(0);
        auto factor = 500 / originalSize.height();
        int targetWidth = originalSize.width() * factor;

        auto image = doc->render(0, {targetWidth, 500});
        auto label = new QLabel();

        label->setPixmap(QPixmap::fromImage(image));

        return label;
      }

      layout->addWidget(ImageViewer::createFromModel(
                            ThemeIconModel{.iconName = iconName}, {128, 128}),
                        0, Qt::AlignCenter);
      widget->setLayout(layout);

      return widget;
    }

    MetadataModel createMetadata() const override {
      return {.children = {MetadataLabel{
                               .text = info.absoluteFilePath(),
                               .title = "Path",
                           },
                           MetadataSeparator{},
                           MetadataLabel{
                               .text = "text/csv",
                               .title = "Type",
                           },
                           MetadataSeparator{},
                           MetadataLabel{.text = info.lastRead().toString(),
                                         .title = "Last accessed"},
                           MetadataSeparator{},
                           MetadataLabel{.text = info.lastModified().toString(),
                                         .title = "Last modified"}}};
    }

  public:
    FileListItemDetail(const QString &path, const QString &iconName)
        : info(path), iconName(iconName) {}
  };

  class FileListItem : public AbstractNativeListItem {
    QMimeDatabase mimeDb;
    QString iconName;
    FileInfo file;
    Service<AppDatabase> appDb;

    QWidget *createItem() const override {
      return new ListItemWidget(
          ImageViewer::createFromModel(ThemeIconModel{.iconName = iconName},
                                       {25, 25}),
          file.name, "", "");
    }

    std::unique_ptr<AbstractNativeListItemDetail>
    createDetail() const override {
      return std::make_unique<FileListItemDetail>(file.path, iconName);
    }

    QList<AbstractAction *> createActions() const override {
      QList<AbstractAction *> actions;

      for (const auto &app : appDb.findMimeOpeners(file.mime)) {
        actions << new OpenAppAction(app, app->name, {file.path});
      }

      if (auto browser = appDb.defaultFileBrowser()) {
        actions << new OpenAppAction(browser, "Open in " + browser->name,
                                     {file.path});
      }

      actions << new CopyTextAction("Copy path", file.path);
      actions << new CopyTextAction("Copy name", file.name);

      return actions;
    }

  public:
    FileListItem(const FileInfo &info, Service<AppDatabase> appDb)
        : file(info), appDb(appDb) {
      auto mime = mimeDb.mimeTypeForName(info.mime);
      QIcon icon = QIcon::fromTheme(mime.iconName());

      if (!icon.isNull()) {
        iconName = mime.iconName();
      } else {
        iconName = mime.genericIconName();
      }
    }
  };

public slots:
  void onFileSearchFinished(const QList<FileInfo> &files) {
    model->beginReset();
    model->beginSection("Files");

    for (const auto &file : files) {
      model->addItem(
          std::make_unique<FileListItem>(file, service<AppDatabase>()));
    }

    model->endReset();
  }

public:
  void onSearchChanged(const QString &s) override {
    auto request = indexer.search(s);

    connect(request, &SearchRequest::finished, this,
            [this](auto files) { onFileSearchFinished(files); });
  }

  void onMount() override {
    if (!baseQuery.isEmpty()) {
      app.topBar->input->setText(baseQuery);
    }
  }

  FilesView(AppWindow &app, const QString &text)
      : NavigationListView(app), app(app), indexer(service<IndexerService>()),
        baseQuery(text) {
    qDebug() << "hello text" << text;
  }
};
