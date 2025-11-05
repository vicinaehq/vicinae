#pragma once
#include "common.hpp"
#include "../image/url.hpp"
#include "services/file-chooser/abstract-file-chooser.hpp"
#include "services/file-chooser/native/native-file-chooser.hpp"
#include "services/file-chooser/xdp-file-chooser/xdp-file-chooser.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/icon-button/icon-button.hpp"
#include "ui/image/image.hpp"
#include "ui/button/button.hpp"
#include "ui/omni-list/omni-list-item-widget.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/typography/typography.hpp"
#include "utils/utils.hpp"
#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qjsonarray.h>
#include <qmimedatabase.h>
#include <qmimetype.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qwidget.h>

struct File {
  QString name;
  std::filesystem::path path;
  QMimeType mime;
};

class SelectedFileWidget : public OmniListItemWidget {
  Q_OBJECT

public:
  void setRemovable(bool removable) {
    m_removeButton->setVisible(removable);
    if (!removable) {
      m_title->setColor(SemanticColor::TextMuted);
    } else {
      m_title->setColor(SemanticColor::Foreground);
    }
  }

  void setFile(const File &file) {
    auto icon = QIcon::fromTheme(file.mime.iconName());

    m_title->setText(compressPath(file.path).c_str());

    if (!icon.isNull()) {
      m_icon->setUrl(ImageURL::system(file.mime.iconName()));
      return;
    }

    icon = QIcon::fromTheme(file.mime.genericIconName());

    if (!icon.isNull()) {
      m_icon->setUrl(ImageURL::system(file.mime.iconName()));
      return;
    }

    m_icon->setUrl(ImageURL::builtin("blank-document"));
  }

public:
  SelectedFileWidget() { setupUI(); }

signals:
  void openClicked() const;
  void removeClicked() const;

private:
  ImageWidget *m_icon = new ImageWidget;
  TypographyWidget *m_title = new TypographyWidget;
  IconButton *m_showFolder = new IconButton();
  IconButton *m_removeButton = new IconButton();
  QHBoxLayout *m_rightLayout = new QHBoxLayout;
  QWidget *m_right = new QWidget;
  QWidget *m_left = new QWidget;
  QHBoxLayout *m_leftLayout = new QHBoxLayout;

  void setupUI() {
    auto layout = new QHBoxLayout;

    m_leftLayout->setContentsMargins(0, 0, 0, 0);
    m_leftLayout->addWidget(m_icon);
    m_leftLayout->addWidget(m_title);
    m_left->setLayout(m_leftLayout);

    m_rightLayout->setContentsMargins(0, 0, 0, 0);
    m_rightLayout->addWidget(m_showFolder);
    m_rightLayout->addWidget(m_removeButton);
    m_right->setLayout(m_rightLayout);

    m_title->setEllideMode(Qt::ElideMiddle);

    m_showFolder->setFixedSize(25, 25);
    m_showFolder->setUrl(ImageURL::builtin("arrow-right-circle"));

    m_removeButton->setFixedSize(25, 25);
    m_removeButton->setUrl(ImageURL::builtin("x-mark-circle"));

    connect(m_showFolder, &IconButton::clicked, this, &SelectedFileWidget::openClicked);
    connect(m_removeButton, &IconButton::clicked, this, &SelectedFileWidget::removeClicked);

    m_icon->setFixedSize(25, 25);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_left, 0, Qt::AlignLeft);
    layout->addWidget(m_right, 0, Qt::AlignRight);
    setLayout(layout);
  }
};

class FilePicker : public JsonFormItemWidget {
  Q_OBJECT

public:
  class AbstractFilePickerItemDelegate : public OmniList::AbstractVirtualItem {
    File m_file;
    FilePicker *m_picker;
    bool m_readOnly = false;

  public:
    FilePicker *picker() const { return m_picker; }
    void setFile(const File &file) { m_file = file; }
    void setPicker(FilePicker *picker) { m_picker = picker; }
    void setReadOnly(bool value) { m_readOnly = value; }
    const File &file() const { return m_file; }
    bool readOnly() const { return m_readOnly; }
    QString generateId() const override { return m_file.path.c_str(); }

    AbstractFilePickerItemDelegate() {}
  };

  struct AbstractDelegateFactory {
    virtual std::unique_ptr<AbstractFilePickerItemDelegate> operator()() = 0;
    virtual ~AbstractDelegateFactory() = default;
  };

  template <class T> class TypedDelegateFactory : public AbstractDelegateFactory {
    std::unique_ptr<AbstractFilePickerItemDelegate> operator()() override { return std::make_unique<T>(); }
  };

public:
  QJsonValue asJsonValue() const override;
  const std::vector<File> &files() const;
  ButtonWidget *button() const;
  void removeFile(const std::filesystem::path &path);
  void addFile(const std::filesystem::path &path);
  void setFile(const std::filesystem::path &path);
  void clear();
  void setMimeTypeFilters(const QStringList &filters);
  void setOnlyDirectories();
  void setMultiple(bool);
  void setReadOnly(bool value = true) {
    m_button->setVisible(!value);
    m_readOnly = value;
  }

  template <class T> void setDelegate() { m_delegateFactory = std::make_unique<TypedDelegateFactory<T>>(); }

  FocusNotifier *focusNotifier() const override { return m_focusNotifier; }

  FilePicker(QWidget *parent = nullptr);

signals:
  void valueChanged();

private:
  void handleFileChoice();
  void setupUI();
  void regenerateList();
  void updateButtonText();
  void addFileImpl(const std::filesystem::path &path);
  void setValueAsJson(const QJsonValue &value) override;
  void filesChosen(const std::vector<std::filesystem::path> &paths);

  FocusNotifier *m_focusNotifier = new FocusNotifier(this);
  AbstractFileChooser *m_chooser = nullptr;
  QMimeDatabase m_mimeDb;
  OmniList *m_fileList = new OmniList;
  TypographyWidget *m_fileCount = new TypographyWidget;
  ButtonWidget *m_button = new ButtonWidget;
  QStringList m_mimeTypeFilters;
  std::vector<File> m_files;
  std::unique_ptr<AbstractDelegateFactory> m_delegateFactory;

  bool m_multiple = false;
  bool m_readOnly = false;
  bool m_directoriesOnly = false;
};
