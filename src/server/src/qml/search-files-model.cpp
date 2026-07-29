#include <QUrl>

#include "search-files-model.hpp"
#include "utils/file-list-item.hpp"
#include "utils/utils.hpp"

void SearchFilesSection::setFiles(std::vector<std::filesystem::path> files, const QString &sectionName) {
  m_files = std::move(files);
  m_sectionName = sectionName;
  notifyChanged();
}

QString SearchFilesSection::itemId(int i) const { return QString::fromStdString(m_files.at(i).string()); }

QString SearchFilesSection::itemTitle(int i) const {
  return QString::fromStdString(getLastPathComponent(m_files.at(i)));
}

QString SearchFilesSection::itemSubtitle(int i) const { return {}; }

std::optional<ImageURL> SearchFilesSection::itemIcon(int i) const {
  return ImageURL::fileIcon(m_files.at(i));
}

std::unique_ptr<ActionPanelState> SearchFilesSection::actionPanel(int i) const {
  return FileActions::actionPanel(m_files.at(i), scope().appContext());
}

std::unique_ptr<QMimeData> SearchFilesSection::dragMimeData(int i) const {
  auto data = std::make_unique<QMimeData>();
  auto path = QString::fromStdString(m_files.at(i).string());
  data->setUrls({QUrl::fromLocalFile(path)});
  data->setText(path);
  return data;
}
