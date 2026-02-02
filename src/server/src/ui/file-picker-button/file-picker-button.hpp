#include "services/file-chooser/xdp-file-chooser/xdp-file-chooser.hpp"
#include "ui/icon-button/icon-button.hpp"
#include <qcontainerfwd.h>
#include <qtmetamacros.h>

class FilePickerButton : public IconButton {
  void handleClick();

public:
  void setMimeTypeFilters(QStringList filters);
  void setDirectoriesOnly();
  void setAllowMultiple(bool value);

  FilePickerButton();

private:
  Q_OBJECT
  std::unique_ptr<XdpFileChooser> m_fileChooser;

signals:
  void filesChosen(const std::vector<std::filesystem::path> &path) const;
};
