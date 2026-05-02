#include "search-files-model.hpp"
#include "clipboard-actions.hpp"
#include "misc/file-list-item.hpp"
#include "service-registry.hpp"
#include "utils/utils.hpp"

void SearchFilesSection::setFiles(std::vector<std::filesystem::path> files, const QString &sectionName) {
  m_files = std::move(files);
  m_sectionName = sectionName;
  notifyChanged();
}

QString SearchFilesSection::itemTitle(int i) const {
  return QString::fromStdString(getLastPathComponent(m_files.at(i)));
}

QString SearchFilesSection::itemIconSource(int i) const {
  return imageSourceFor(ImageURL::fileIcon(m_files.at(i)));
}

std::unique_ptr<ActionPanelState> SearchFilesSection::actionPanel(int i) const {
  return FileActions::actionPanel(m_files.at(i), scope().services()->appDb());
}
