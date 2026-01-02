#include "ui/file-picker/file-picker.hpp"
#include "common.hpp"
#include "layout.hpp"
#include "services/file-chooser/abstract-file-chooser.hpp"
#include "services/file-chooser/native/native-file-chooser.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/file-picker/file-picker-default-item-delegate.hpp"
#include "ui/button/button.hpp"
#include <filesystem>
#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qfiledialog.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qobject.h>
#include <qwidget.h>

namespace fs = std::filesystem;

FilePicker::FilePicker(QWidget *parent) : JsonFormItemWidget(parent) { setupUI(); }

void FilePicker::setMimeTypeFilters(const QStringList &filters) {
  m_mimeTypeFilters = filters;
  m_directoriesOnly = m_mimeTypeFilters.contains("inode/directory");
  updateButtonText();
}

const std::vector<File> &FilePicker::files() const { return m_files; }

void FilePicker::removeFile(const std::filesystem::path &path) {
  auto it = std::ranges::find_if(m_files, [&](auto &&file) { return file.path == path; });

  if (it != m_files.end()) { m_files.erase(it); }

  regenerateList();
  emit valueChanged();
}

void FilePicker::handleFileChoice() {
  if (m_chooser) { m_chooser->deleteLater(); }

  m_chooser = new NativeFileChooser(this);
  m_chooser->setMimeTypeFilters(m_mimeTypeFilters);
  m_chooser->setMultipleSelection(m_multiple);
  m_chooser->openFile();
  connect(m_chooser, &AbstractFileChooser::filesChosen, this, &FilePicker::filesChosen);
}

void FilePicker::regenerateList() {
  if (m_files.size() > 1) { m_fileCount->setText(QString("%1 Files").arg(m_files.size())); }

  // m_fileCount->setVisible(!m_files.empty());
  m_fileList->updateModel(
      [&]() {
        for (const auto &file : m_files) {
          auto &section = m_fileList->addSection();
          auto item = (*m_delegateFactory)();
          item->setReadOnly(m_readOnly);
          item->setPicker(this);
          item->setFile(file);
          section.addItem(std::move(item));
        }
      },
      OmniList::PreserveSelection);
}

ButtonWidget *FilePicker::button() const { return m_button; }

void FilePicker::setMultiple(bool value) {
  m_multiple = value;
  updateButtonText();
}

void FilePicker::clear() {
  if (!m_files.empty()) {
    m_files.clear();
    emit valueChanged();
  }
}

void FilePicker::setFile(const fs::path &path) {
  clear();
  addFile(path);
}

void FilePicker::setOnlyDirectories() {
  setMimeTypeFilters({"inode/directory"});
  m_directoriesOnly = true;
  updateButtonText();
}

void FilePicker::filesChosen(const std::vector<fs::path> &paths) {
  if (!m_multiple) {
    if (paths.empty()) {
      qWarning() << "filesChosen with empty list of files";
      return;
    }
    setFile(paths.front());
    return;
  }

  for (const auto &path : paths) {
    addFileImpl(path);
  }

  regenerateList();
  emit valueChanged();
}

void FilePicker::addFileImpl(const std::filesystem::path &path) {
  QMimeType mime = m_mimeDb.mimeTypeForFile(path.c_str());
  bool isFilteredMimeType = m_mimeTypeFilters.empty();
  QStringList allMimes;

  allMimes << mime.name() << mime.parentMimeTypes();

  for (const auto &mime : allMimes) {
    if (m_mimeTypeFilters.contains(mime)) {
      isFilteredMimeType = true;
      break;
    }
  }

  if (!isFilteredMimeType) return;

  File file;

  file.name = path.filename().c_str();
  file.path = path;
  file.mime = mime;

  if (!std::ranges::any_of(m_files, [&](auto &&f) { return f.path == file.path; })) {
    m_files.emplace_back(file);
  }
}

void FilePicker::addFile(const std::filesystem::path &path) {
  addFileImpl(path);
  regenerateList();
  emit valueChanged();
}

QJsonValue FilePicker::asJsonValue() const {
  if (!m_multiple) {
    if (!m_files.empty()) return QString::fromStdString(m_files.front().path.string());
    return QJsonValue();
  }

  QJsonArray array;

  for (const auto &file : m_files) {
    array.push_back(QString::fromStdString(file.path.string()));
  }

  return array;
}

void FilePicker::setValueAsJson(const QJsonValue &value) {
  if (value.isArray()) {
    clear();
    for (const auto &file : value.toArray()) {
      addFileImpl(file.toString().toStdString());
    }
    regenerateList();
    return;
  }

  if (value.isString()) {
    setFile(value.toString().toStdString());
    return;
  }

  qWarning() << "FilePicker::setValueAsJson: not an array or string";
}

void FilePicker::setupUI() {
  setDelegate<DefaultFilePickerItemDelegate>();
  setFocusProxy(m_button);

  m_button->setBackgroundColor(SemanticColor::ListItemHoverBackground);
  m_button->setHoverBackgroundColor(SemanticColor::ListItemSelectionBackground);
  updateButtonText();
  m_fileCount->hide();

  VStack().add(m_button).add(m_fileList).add(m_fileCount).imbue(this);

  connect(m_button, &ButtonWidget::clicked, this, &FilePicker::handleFileChoice);
  connect(m_fileList, &OmniList::virtualHeightChanged, this,
          [this](int height) { m_fileList->setFixedHeight(std::min(300, height)); });
}

void FilePicker::updateButtonText() {
  if (!m_button) return;
  if (m_directoriesOnly) {
    m_button->setText(m_multiple ? "Pick folders" : "Pick a folder");
  } else {
    m_button->setText(m_multiple ? "Pick files" : "Pick a file");
  }
}
