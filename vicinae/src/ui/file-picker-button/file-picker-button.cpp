#include "file-picker-button.hpp"

void FilePickerButton::handleClick() { m_fileChooser->openFile(); }
void FilePickerButton::setMimeTypeFilters(QStringList filters) { m_fileChooser->setMimeTypeFilters(filters); }
void FilePickerButton::setDirectoriesOnly() { setMimeTypeFilters({"inode/directory"}); }
void FilePickerButton::setAllowMultiple(bool value) { m_fileChooser->setMultipleSelection(value); };

FilePickerButton::FilePickerButton() {
  m_fileChooser = std::make_unique<XdpFileChooser>();
  setUrl(ImageURL::builtin("folder"));
  connect(this, &IconButton::clicked, this, &FilePickerButton::handleClick);
  connect(m_fileChooser.get(), &AbstractFileChooser::filesChosen, this, &FilePickerButton::filesChosen);
}
