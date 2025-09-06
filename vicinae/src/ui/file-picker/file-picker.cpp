#include "ui/file-picker/file-picker.hpp"
#include "common.hpp"
#include "services/file-chooser/abstract-file-chooser.hpp"
#include "theme.hpp"
#include "ui/file-picker/file-picker-default-item-delegate.hpp"
#include "ui/button/button.hpp"
#include <cstdio>
#include <filesystem>
#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qfiledialog.h>
#include <qwidget.h>

FilePicker::FilePicker(QWidget *parent) : JsonFormItemWidget(parent) { setupUI(); }

void FilePicker::setMimeTypeFilters(const QStringList &filters) { m_mimeTypeFilters = filters; }

std::vector<File> FilePicker::files() const { return m_files; }

void FilePicker::removeFile(const std::filesystem::path &path) {
  auto it = std::ranges::find_if(m_files, [&](auto &&file) { return file.path == path; });

  if (it != m_files.end()) { m_files.erase(it); }

  regenerateList();
}

void FilePicker::handleFileChoice() {
  m_fileChooser->setMimeTypeFilters({"inode/directory"});
  m_fileChooser->setMultipleSelection(true);
  m_fileChooser->openFile();
}

void FilePicker::regenerateList() {
  m_fileCount->setText(QString("%1 Files").arg(m_files.size()));
  m_fileCount->setVisible(!m_files.empty());

  m_fileList->updateModel(
      [&]() {
        for (const auto &file : m_files) {
          auto &section = m_fileList->addSection();
          auto item = (*m_delegateFactory)();

          item->setPicker(this);
          item->setFile(file);
          section.addItem(std::move(item));
        }
      },
      OmniList::PreserveSelection);
}

OmniButtonWidget *FilePicker::button() const { return m_button; }

void FilePicker::filesChosen(const std::vector<std::filesystem::path> &paths) {
  for (const auto &path : paths) {
    addFileImpl(path);
  }

  regenerateList();
}

void FilePicker::addFileImpl(const std::filesystem::path &path) {
  QMimeType mime = m_mimeDb.mimeTypeForFile(path.c_str());
  bool isFilteredMimeType = m_mimeTypeFilters.empty();
  QStringList allMimes;

  allMimes << mime.name() << mime.parentMimeTypes();

  for (const auto mime : allMimes) {
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
}

void FilePicker::setupUI() {
  auto layout = new QVBoxLayout;
  auto &theme = ThemeService::instance().theme();

  setDelegate<DefaultFilePickerItemDelegate>();
  m_button->setBackgroundColor(theme.resolveTint(SemanticColor::MainHoverBackground));
  m_button->setHoverBackgroundColor(theme.resolveTint(SemanticColor::MainSelectedBackground));
  m_button->setText("Pick a file");
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_button);
  layout->addWidget(m_fileList);
  layout->addWidget(m_fileCount);
  m_fileCount->hide();

  setLayout(layout);

  connect(m_button, &OmniButtonWidget::clicked, this, &FilePicker::handleFileChoice);
  connect(m_fileChooser.get(), &AbstractFileChooser::filesChosen, this, &FilePicker::filesChosen);
  connect(m_fileList, &OmniList::virtualHeightChanged, this,
          [this](int height) { m_fileList->setFixedHeight(std::min(300, height)); });
}
