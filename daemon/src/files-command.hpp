#pragma once

#include "app.hpp"
#include "indexer-service.hpp"
#include "navigation-list-view.hpp"

class FilesView : public NavigationListView {
  Service<IndexerService> indexer;

public slots:
  void onFileSearchFinished(const QList<FileInfo> &files) {
    model->beginReset();
    qDebug() << "Got" << files.size() << "files";
    model->endReset();
  }

public:
  void onSearchChanged(const QString &s) override {
    auto request = indexer.search(s);

    connect(request, &SearchRequest::finished, this,
            [this](auto files) { onFileSearchFinished(files); });
  }

  FilesView(AppWindow &app)
      : NavigationListView(app), indexer(service<IndexerService>()) {}
};
