#pragma once
#include <qobject.h>
#include <qstringliteral.h>
#include <qstringlist.h>
#include <filesystem>
#include <qtmetamacros.h>

/**
 * Generic FileChooser interface used to accomodate different file choosing methods.
 */

class AbstractFileChooser : public QObject {
  Q_OBJECT

signals:
  void filesChosen(const std::vector<std::filesystem::path> &paths);
  void rejected() const;

public:
  AbstractFileChooser(QObject *parent = nullptr) : QObject(parent) {}

  /**
   * Open the file chooser.
   * Return whether the operation succeeded or not.
   * The filesChosen signal will be emitted if files are chosen
   * during this session (this can be aborted).
   */
  virtual bool openFile() = 0;

  virtual void setMimeTypeFilters(const QStringList &filters) {}
  virtual void setMultipleSelection(bool value) {}
  virtual void setAcceptLabel(const QString &value) {}
  virtual void setCurrentFolder(const std::filesystem::path &path) {}
};
