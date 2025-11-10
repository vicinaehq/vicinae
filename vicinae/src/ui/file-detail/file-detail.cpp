#include "file-detail.hpp"
#include "ui/image/image.hpp"
#include "ui/text-file-viewer/text-file-viewer.hpp"
#include "utils.hpp"
#include "layout.hpp"

namespace fs = std::filesystem;

void FileDetail::setPath(const fs::path &path, bool withMetadata) {
  if (auto previous = content()) { previous->deleteLater(); }

  m_path = path;

  auto widget = createEntryWidget(path);

  setContent(widget);

  if (withMetadata) {
    auto metadata = createEntryMetadata(path);
    setMetadata(metadata);
  }
}

std::vector<MetadataItem> FileDetail::createEntryMetadata(const fs::path &path) const {
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

QWidget *FileDetail::createEntryWidget(const fs::path &path) {
  auto mime = m_mimeDb.mimeTypeForFile(path.c_str());

  if (mime.name().startsWith("image/")) {
    auto icon = new ImageWidget;
    icon->setContentsMargins(10, 10, 10, 10);
    icon->setUrl(ImageURL::local(path));
    return icon;
  }

  if (Utils::isTextMimeType(mime)) {
    auto viewer = new TextFileViewer;
    viewer->load(path);
    return VStack().add(viewer).buildWidget();
  }

  auto icon = new ImageWidget;

  icon->setContentsMargins(10, 10, 10, 10);
  icon->setUrl(ImageURL::system(mime.iconName()).withFallback(ImageURL::system(mime.genericIconName())));

  return icon;
}
