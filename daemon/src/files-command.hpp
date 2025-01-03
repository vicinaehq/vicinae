#pragma once

#include "app-database.hpp"
#include "app.hpp"
#include "extend/metadata-model.hpp"
#include "filesystem-database.hpp"
#include "indexer-service.hpp"
#include "navigation-list-view.hpp"
#include "ui/test-list.hpp"
#include <qboxlayout.h>
#include <qfileinfo.h>
#include <qicon.h>
#include <qmimedatabase.h>
#include <qnamespace.h>
#include <qthreadpool.h>

class FilesView : public NavigationListView {
  QString baseQuery;
  Service<IndexerService> indexer;

  class FileListItemDetail : public AbstractNativeListItemDetail {
    QFileInfo info;
    QString iconName;

    QWidget *createView() const override {
      auto widget = new QWidget();
      auto layout = new QHBoxLayout();

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

    QList<AbstractAction *> createActions() const override { return {}; }

  public:
    FileListItem(const FileInfo &info, Service<AppDatabase> appDb)
        : file(info), appDb(appDb) {
      auto mime = mimeDb.mimeTypeForFile(info.path);
      QIcon icon = QIcon::fromTheme(mime.iconName());

      if (icon.isNull())
        iconName = mime.genericIconName();
      else
        iconName = mime.iconName();
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
    if (!baseQuery.isEmpty())
      onSearchChanged(baseQuery);
  }

  FilesView(AppWindow &app, const QString &text)
      : NavigationListView(app), indexer(service<IndexerService>()),
        baseQuery(text) {}
};
