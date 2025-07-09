#pragma once
#include "action-panel/action-panel.hpp"
#include "actions/app/app-actions.hpp"
#include "base-view.hpp"
#include "clipboard-history-view.hpp"
#include "manage-quicklinks-command.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "timer.hpp"
#include "ui/omni-list.hpp"
#include "utils/utils.hpp"
#include <filesystem>
#include <qlocale.h>
#include <qmimedatabase.h>

class FileListItemMetadata : public DetailWithMetadataWidget {
  std::filesystem::path m_path;
  QMimeDatabase m_mimeDb;

  std::vector<MetadataItem> createEntryMetadata(const std::filesystem::path &path) const {
    auto mimeType = m_mimeDb.mimeTypeForFile(path.c_str());
    auto stat = std::filesystem::status(path);
    QFileInfo info(path);

    auto lastModifiedAt = MetadataLabel{
        .text = info.lastModified().toString(),
        .title = "Last modified at",
    };
    auto mime = MetadataLabel{
        .text = mimeType.name(),
        .title = "Type",
    };
    auto name = MetadataLabel{
        .text = path.filename().c_str(),
        .title = "Name",
    };
    auto where = MetadataLabel{
        .text = compressPath(path).c_str(),
        .title = "Where",
    };

    return {name, where, mime, lastModifiedAt};
  }

  QWidget *createEntryWidget(const std::filesystem::path &path) {
    auto mime = m_mimeDb.mimeTypeForFile(path.c_str());

    if (mime.name().startsWith("image/")) {
      auto icon = new Omnimg::ImageWidget;

      icon->setContentsMargins(10, 10, 10, 10);
      icon->setUrl(LocalOmniIconUrl(path));

      return icon;
    }

    if (isTextMimeType(mime)) {
      auto container = new TextContainer;
      auto viewer = new TextFileViewer;

      container->setWidget(viewer);
      viewer->load(path.c_str());

      return container;
    }

    auto icon = new Omnimg::ImageWidget;

    icon->setContentsMargins(10, 10, 10, 10);
    icon->setUrl(SystemOmniIconUrl(mime.iconName()).withFallback(SystemOmniIconUrl(mime.genericIconName())));

    return icon;
  }

public:
  void setPath(const std::filesystem::path &path) {
    if (auto previous = content()) { previous->deleteLater(); }

    m_path = path;

    auto widget = createEntryWidget(path);
    auto metadata = createEntryMetadata(path);

    setContent(widget);
    setMetadata(metadata);
  }
};

class FileListItem : public AbstractDefaultListItem, public ListView::Actionnable {
  std::filesystem::path m_path;
  QMimeDatabase m_mimeDb;

  OmniIconUrl getIcon() const {
    auto mime = m_mimeDb.mimeTypeForFile(m_path.c_str());

    if (!mime.name().isEmpty()) {
      if (!QIcon::fromTheme(mime.iconName()).isNull()) { return SystemOmniIconUrl(mime.iconName()); }

      return SystemOmniIconUrl(mime.genericIconName());
    }

    return BuiltinOmniIconUrl("question-mark-circle");
  }

  QWidget *generateDetail() const override {
    auto detail = new FileListItemMetadata();
    detail->setPath(m_path);

    return detail;
  }

  ActionPanelView *actionPanel() const override {
    auto appDb = ServiceRegistry::instance()->appDb();
    auto panel = new ActionPanelStaticListView;

    if (auto app = appDb->findBestOpener(m_path.c_str())) {
      auto open = new OpenAppAction(app, "Open", {m_path.c_str()});
      open->setPrimary(true);
      panel->addAction(open);
    }

    return panel;
  }

public:
  QString generateId() const override { return m_path.c_str(); }

  ItemData data() const override {
    return {
        .iconUrl = getIcon(),
        .name = m_path.filename().c_str(),
    };
  }

  FileListItem(const std::filesystem::path &path) : m_path(path) {}
};

class SearchFilesView : public ListView {
  void initialize() override {
    auto ui = ServiceRegistry::instance()->UI();
    ui->setSearchPlaceholderText("Search for files...");
  }

  void generateFilteredList(const QString &query) {
    auto fileService = ServiceRegistry::instance()->fileService();
    Timer timer;
    auto items = fileService->search(query.toStdString());
    timer.time("search");

    m_list->updateModel([&]() {
      auto &section = m_list->addSection("Files");
      for (const auto &result : items) {
        section.addItem(std::make_unique<FileListItem>(result.path));
      }
    });
  }

  void textChanged(const QString &query) override {
    if (!query.isEmpty()) generateFilteredList(query);
  }
};
