#include "directory-input.hpp"
#include "ui/form/directory-input.hpp"
#include "ui/file-picker-button/file-picker-button.hpp"
#include "utils/utils.hpp"

namespace fs = std::filesystem;

DirectoryInput::DirectoryInput() {
  auto btn = new FilePickerButton;

  btn->setDirectoriesOnly();

  connect(btn, &FilePickerButton::filesChosen, this, [this](const std::vector<fs::path> &paths) {
    if (paths.empty()) return;
    setText(compressPath(paths.front()).c_str());
  });

  setRightAccessory(btn);
}
