#pragma once
#include "ui/custom-list-view.hpp"

class FilesView : public CustomList<int> {
  ListModel search(const QString &s) override {
    ListModel model;

    QDir directory("/home/aurelle");
    QStringList filesAndDirs =
        directory.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);

    model.searchPlaceholderText = "Search file in home directory";

    for (const QString &entry : filesAndDirs) {
      ListItemViewModel item;
      QString fullpath = "/home/aurelle/" + entry;

      item.id = fullpath;
      item.title = entry;
      item.icon = ThemeIconModel{.iconName = "folder"};

      model.items.push_back(item);
    }

    return model;
  }

  void action(const int &action) override { qDebug() << "My wonderful file!"; }

public:
  FilesView(AppWindow &app) : CustomList(app) {}
};
