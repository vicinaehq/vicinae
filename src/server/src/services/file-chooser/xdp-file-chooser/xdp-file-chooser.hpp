#pragma once
#include "services/file-chooser/abstract-file-chooser.hpp"
#include <QtDBus/QtDBus>
#include <QtDBus/qdbusconnection.h>
#include <QtDBus/qdbusinterface.h>
#include <qcontainerfwd.h>
#include <qdbusreply.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>

Q_DECLARE_METATYPE(QVariantMap)

class XdpFileChooser : public AbstractFileChooser {
  Q_OBJECT

  bool m_ongoing = false;

public:
  struct FilterItem {
    uint type;
    QString value;
  };

  struct Filter {
    QString text;
    std::vector<FilterItem> items;
  };

  struct FilterRequest {
    QString token;
    bool modal;
    bool multiple;
    bool directory;
    std::vector<Filter> filters;
  };

private:
  QDBusConnection m_bus;
  QDBusInterface *m_interface;
  std::filesystem::path m_currentFolder;
  QString m_acceptLabel;
  QStringList m_mimeTypeFilters;
  bool m_multiple = false;
  bool m_directory = false;

  QString generateToken() const;

public:
  XdpFileChooser();

  bool openFile() override;
  void setMimeTypeFilters(const QStringList &filters) override;
  void setCurrentFolder(const std::filesystem::path &path) override;
  void setAcceptLabel(const QString &value) override;
  void setMultipleSelection(bool value) override;

public slots:
  void handleResponse(uint response, const QVariantMap &results);
};

Q_DECLARE_METATYPE(XdpFileChooser::Filter);
Q_DECLARE_METATYPE(XdpFileChooser::FilterItem);
Q_DECLARE_METATYPE(XdpFileChooser::FilterRequest);
Q_DECLARE_METATYPE(QList<XdpFileChooser::Filter>);

QDBusArgument &operator<<(QDBusArgument &argument, const XdpFileChooser::FilterItem &myStruct);
QDBusArgument &operator<<(QDBusArgument &argument, const XdpFileChooser::Filter &myStruct);

const QDBusArgument &operator>>(const QDBusArgument &argument, XdpFileChooser::FilterItem &myStruct);
const QDBusArgument &operator>>(const QDBusArgument &argument, XdpFileChooser::Filter &myStruct);

QDBusArgument &operator<<(QDBusArgument &argument, const XdpFileChooser::FilterRequest &myStruct);
const QDBusArgument &operator>>(const QDBusArgument &argument, XdpFileChooser::FilterRequest &myStruct);
