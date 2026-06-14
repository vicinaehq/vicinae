#include "search-files-model.hpp"
#include "clipboard-actions.hpp"
#include "utils/file-list-item.hpp"
#include "service-registry.hpp"
#include "utils/utils.hpp"
#include <QDebug>
#include <QUrl>

void SearchFilesSection::setFiles(std::vector<std::filesystem::path> files, const QString &sectionName) {
  m_files = std::move(files);
  m_sectionName = sectionName;
  notifyChanged();
}

QString SearchFilesSection::itemId(int i) const { return QString::fromStdString(m_files.at(i).string()); }

QString SearchFilesSection::itemTitle(int i) const {
  return QString::fromStdString(getLastPathComponent(m_files.at(i)));
}

QString SearchFilesSection::itemSubtitle(int i) const {
  return QString::fromStdString(compressPath(m_files.at(i).parent_path()).string());
}

QString SearchFilesSection::itemIconSource(int i) const {
  return imageSourceFor(ImageURL::fileIcon(m_files.at(i)));
}

std::unique_ptr<ActionPanelState> SearchFilesSection::actionPanel(int i) const {
  return FileActions::actionPanel(m_files.at(i), scope().services()->appDb());
}

QVariant SearchFilesSection::customData(int i, int role) const {
  if (std::cmp_greater_equal(i, m_files.size())) return {};
  static int dbgCount = 0;
  switch (role) {
  case SectionListModel::FilePath: {
    auto val = QString::fromStdString(m_files[i].string());
    if (dbgCount++ < 3) qDebug() << "[DRAG] customData FilePathRole i=" << i << "val=" << val;
    return val;
  }
  case SectionListModel::FileUrl: {
    auto val = QUrl::fromLocalFile(QString::fromStdString(m_files[i].string())).toString();
    if (dbgCount++ < 3) qDebug() << "[DRAG] customData FileUrlRole i=" << i << "val=" << val;
    return val;
  }
  default:
    return {};
  }
}

QHash<int, QByteArray> SearchFilesSection::customRoleNames() const {
  static int callCount = 0;
  if (callCount++ == 0) {
    qDebug() << "[DRAG] SearchFilesSection::customRoleNames() called, roles:" << SectionListModel::FilePath << SectionListModel::FileUrl;
  }
  return {
      {SectionListModel::FilePath, "filePath"},
      {SectionListModel::FileUrl, "fileUrl"},
  };
}

QHash<int, QVariant> SearchFilesSection::customRoleDefaults() const {
  return {
      {SectionListModel::FilePath, QString()},
      {SectionListModel::FileUrl, QString()},
  };
}
