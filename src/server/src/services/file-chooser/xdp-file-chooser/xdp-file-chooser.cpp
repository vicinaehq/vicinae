#include "xdp-file-chooser.hpp"
#include <cstdlib>
#include <qdbusmetatype.h>
#include <qlogging.h>
#include <qobject.h>

XdpFileChooser::XdpFileChooser() : m_bus(QDBusConnection::sessionBus()) {
  qDBusRegisterMetaType<Filter>();
  qDBusRegisterMetaType<FilterItem>();
  qRegisterMetaType<FilterRequest>();
  qDBusRegisterMetaType<FilterRequest>();
  qDBusRegisterMetaType<QList<XdpFileChooser::Filter>>();

  if (!m_bus.isConnected()) { qWarning() << "Failed to connec to dbus" << m_bus.lastError(); }

  m_interface = new QDBusInterface("org.freedesktop.portal.Desktop", "/org/freedesktop/portal/desktop",
                                   "org.freedesktop.portal.FileChooser", m_bus, this);
}

QString XdpFileChooser::generateToken() const {
  return QString("qtfilechooser%1").arg(QRandomGenerator::global()->generate());
}

void XdpFileChooser::setMimeTypeFilters(const QStringList &filters) {
  m_mimeTypeFilters = filters;
  m_directory = filters.size() == 1 && filters.at(0) == "inode/directory";
}

void XdpFileChooser::setCurrentFolder(const std::filesystem::path &path) { m_currentFolder = path; }

void XdpFileChooser::setAcceptLabel(const QString &value) { m_acceptLabel = value; }

void XdpFileChooser::setMultipleSelection(bool value) { m_multiple = value; }

bool XdpFileChooser::openFile() {
  if (m_ongoing) {
    qWarning() << "XdpFileChooser: openFile called during file choosing";
    return false;
  }
  QString windowHandle = "";

  if (!m_interface->isValid()) {
    qWarning() << "FileChooser portal interface is not valid";
    return false;
  }

  FilterRequest filterRequest;

  filterRequest.token = generateToken();
  filterRequest.modal = true;
  filterRequest.multiple = m_multiple;
  filterRequest.directory = m_directory;

  QVariantMap payload;

  payload["token"] = generateToken();
  payload["modal"] = true;
  payload["multiple"] = m_multiple;
  payload["directory"] = m_directory;

  Filter filter;

  filter.text = "Mimes";

  for (const auto &mime : m_mimeTypeFilters) {
    filter.items.emplace_back(FilterItem{1, mime});
  }

  if (!filter.items.empty()) { payload["filters"] = QVariant::fromValue(QList<Filter>{filter}); }

  QDBusReply<QDBusObjectPath> message = m_interface->call("OpenFile", windowHandle, "Open File", payload);

  if (message.error().isValid()) {
    qCritical() << "Failed to OpenFile" << message.error();
    return false;
  }

  QString requestPath = message.value().path();

  // clang-format off
    bool connected =
        m_bus.connect("", requestPath, "org.freedesktop.portal.Request",
                      "Response", this, SLOT(handleResponse(uint,QVariantMap)));
  // clang-format on

  if (!connected) { qCritical() << "Failed to connect" << m_bus.lastError(); }

  // Connect to the Response signal before making the call

  m_ongoing = true;

  return true;
}

void XdpFileChooser::handleResponse(uint response, const QVariantMap &results) {
  m_ongoing = false;
  if (response == 1) { return; }

  if (response != 0) {
    qWarning() << "File chooser failed with response:" << response;
    return;
  }

  if (results.contains("uris")) {
    QStringList uris = results["uris"].toStringList();
    std::vector<std::filesystem::path> filePaths;
    for (const QString &uri : uris) {
      QUrl url(uri);
      if (url.isLocalFile()) { filePaths.emplace_back(url.toLocalFile().toStdString()); }
    }
    if (!filePaths.empty()) { emit filesChosen(filePaths); }
  }
}

QDBusArgument &operator<<(QDBusArgument &argument, const XdpFileChooser::FilterItem &myStruct) {
  argument.beginStructure();
  argument << myStruct.type << myStruct.value;
  argument.endStructure();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const XdpFileChooser::Filter &myStruct) {
  argument.beginStructure();
  argument << myStruct.text;

  argument.beginArray(qMetaTypeId<XdpFileChooser::FilterItem>());
  for (const auto &item : myStruct.items) {
    argument << item;
  }
  argument.endArray();
  argument.endStructure();

  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, XdpFileChooser::FilterItem &myStruct) {
  argument.beginStructure();
  argument >> myStruct.type >> myStruct.value;
  argument.endStructure();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, XdpFileChooser::Filter &myStruct) {
  /*
argument.beginStructure();
argument >> myStruct.text;
argument.endStructure();
*/

  /*
  argument.beginArray();
  while (!argument.atEnd()) {
    XdpFileChooser::FilterItem item;

    argument >> item;
    myStruct.items.emplace_back(item);
  }
  argument.endArray();
  */
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const XdpFileChooser::FilterRequest &myStruct) {
  argument.beginStructure();
  argument.beginMap(QMetaType::fromType<QString>(), QMetaType::fromType<QDBusVariant>());

  argument.beginMapEntry();
  argument << "handle_token" << QDBusVariant(myStruct.token);
  argument.endMapEntry();

  argument.beginMapEntry();
  argument << "modal" << QDBusVariant(myStruct.modal);
  argument.endMapEntry();

  argument.beginMapEntry();
  argument << "multiple" << QDBusVariant(myStruct.multiple);
  argument.endMapEntry();

  argument.beginMapEntry();
  argument << "directory" << QDBusVariant(myStruct.directory);
  argument.endMapEntry();

  /*
  argument.beginMapEntry();
  argument << "filters";
  argument.beginArray();
  for (const auto &filter : myStruct.filters) {
    argument << filter;
  }
  argument.endArray();
  argument.endMapEntry();
  */

  argument.endMap();
  argument.endStructure();

  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, XdpFileChooser::FilterRequest &myStruct) {
  argument.beginMap();
  while (!argument.atEnd()) {
    argument.beginMapEntry();
    QString key;
    QVariant value;
    argument >> key >> value;

    if (key == "handle_token")
      myStruct.token = value.toString();
    else if (key == "modal")
      myStruct.modal = value.toBool();
    else if (key == "multiple")
      myStruct.multiple = value.toBool();
    else if (key == "directory")
      myStruct.directory = value.toBool();

    argument.endMapEntry();
  }
  argument.endMap();
  return argument;
  return argument;
}
