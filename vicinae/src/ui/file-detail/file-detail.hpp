#pragma once
#include "ui/detail/detail-widget.hpp"
#include <qmimedatabase.h>
#include <qmimetype.h>

class FileDetail : public DetailWidget {
public:
  void setPath(const std::filesystem::path &path, bool withMetadata = true);

private:
  std::vector<MetadataItem> createEntryMetadata(const std::filesystem::path &path) const;
  QWidget *createEntryWidget(const std::filesystem::path &path);

  std::filesystem::path m_path;
  QMimeDatabase m_mimeDb;
};
