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

      layout->addWidget(ImageViewer::createFromModel(ThemeIconModel{.iconName = iconName}, {128, 128}), 0,
                        Qt::AlignCenter);
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
                           MetadataLabel{.text = info.lastRead().toString(), .title = "Last accessed"},
                           MetadataSeparator{},
                           MetadataLabel{.text = info.lastModified().toString(), .title = "Last modified"}}};
    }

  public:
    FileListItemDetail(const QString &path, const QString &iconName) : info(path), iconName(iconName) {}
  };

  class FileListItem : public StandardListItem {
    QMimeDatabase mimeDb;
    FileInfo file;
    Service<AppDatabase> appDb;

    std::unique_ptr<AbstractNativeListItemDetail> createDetail() const override {
      return std::make_unique<FileListItemDetail>(file.path, iconNameFromMime(file.mime));
    }

    QList<AbstractAction *> createActions() const override {
      QList<AbstractAction *> actions;

      for (const auto &app : appDb.findMimeOpeners(file.mime)) {
        actions << new OpenAppAction(app, app->name, {file.path});
      }

      if (auto browser = appDb.defaultFileBrowser()) {
        actions << new OpenAppAction(browser, "Open in " + browser->name, {file.path});
      }

      actions << new CopyTextAction("Copy path", file.path);
      actions << new CopyTextAction("Copy name", file.name);

      return actions;
    }

    QString iconNameFromMime(const QString &name) const {
      auto mime = mimeDb.mimeTypeForName(name);
      QIcon icon = QIcon::fromTheme(mime.iconName());

      if (!icon.isNull()) { return mime.iconName(); }

      return mime.genericIconName();
    }

  public:
    FileListItem(const FileInfo &info, Service<AppDatabase> appDb)
        : StandardListItem(file.name, "", "", ThemeIconModel{.iconName = iconNameFromMime(info.mime)}),
          file(info), appDb(appDb) {}
  };

public slots:
  void onFileSearchFinished(const QList<FileInfo> &files) {
    model->beginReset();
    model->beginSection("Files");

    for (const auto &file : files) {
      model->addItem(std::make_unique<FileListItem>(file, service<AppDatabase>()));
    }

    model->endReset();
  }

public:
  void onSearchChanged(const QString &s) override {
    auto request = indexer.search(s);

    connect(request, &SearchRequest::finished, this, [this](auto files) { onFileSearchFinished(files); });
  }

  void onMount() override {}

  FilesView(AppWindow &app) : NavigationListView(app), app(app), indexer(service<IndexerService>()) {}
};
