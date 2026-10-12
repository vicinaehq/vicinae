#include "xdp-file-chooser.hpp"
#include <QFile>
#include <qlogging.h>
#include <qobject.h>

XdpFileChooser::XdpFileChooser(QObject *parent)
    : AbstractFileChooser(parent), m_bus(QDBusConnection::sessionBus()) {
  if (!m_bus.isConnected()) { qWarning() << "Failed to connect to dbus" << m_bus.lastError(); }

  m_interface = new QDBusInterface("org.freedesktop.portal.Desktop", "/org/freedesktop/portal/desktop",
                                   "org.freedesktop.portal.FileChooser", m_bus, this);
}

bool XdpFileChooser::isAvailable() const { return m_bus.isConnected() && m_interface->isValid(); }

QString XdpFileChooser::generateToken() const {
  return QString("qtfilechooser%1").arg(QRandomGenerator::global()->generate());
}

bool XdpFileChooser::open(const FileChooserOptions &options) {
  if (m_ongoing) {
    qWarning() << "XdpFileChooser: open called during file choosing";
    return false;
  }

  if (!m_interface->isValid()) {
    qWarning() << "FileChooser portal interface is not valid";
    return false;
  }

  bool const saveMode = options.mode == FileChooserMode::Save;
  bool const directoryMode = !saveMode && options.canChooseDirectories && !options.canChooseFiles;

  QVariantMap payload;
  payload["token"] = generateToken();
  payload["modal"] = true;

  if (options.currentFolder) {
    QByteArray folder = QFile::encodeName(QString::fromStdString(options.currentFolder->string()));
    folder.append('\0');
    payload["current_folder"] = folder;
  }

  QString const windowHandle;
  QString title;

  if (saveMode) {
    payload["accept_label"] = tr("Save");
    if (options.suggestedName) payload["current_name"] = QString::fromStdString(*options.suggestedName);
    title = tr("Save as");
  } else {
    payload["multiple"] = options.allowMultipleSelection;
    payload["directory"] = directoryMode;
    payload["accept_label"] = tr("Select");
    title = directoryMode ? tr("Select a directory")
                          : (options.allowMultipleSelection ? tr("Select files") : tr("Select a file"));
  }

  QDBusReply<QDBusObjectPath> const message =
      m_interface->call(saveMode ? "SaveFile" : "OpenFile", windowHandle, title, payload);

  if (message.error().isValid()) {
    qCritical() << "File chooser portal request failed" << message.error();
    return false;
  }

  m_requestPath = message.value().path();

  // clang-format off
    bool const connected =
        m_bus.connect("", m_requestPath, "org.freedesktop.portal.Request",
                      "Response", this, SLOT(handleResponse(uint,QVariantMap)));
  // clang-format on

  if (!connected) { qCritical() << "Failed to connect" << m_bus.lastError(); }

  m_ongoing = true;
  return true;
}

void XdpFileChooser::close() {
  if (!m_ongoing || m_requestPath.isEmpty()) return;
  QDBusInterface request("org.freedesktop.portal.Desktop", m_requestPath, "org.freedesktop.portal.Request",
                         m_bus);
  request.call("Close");
  m_ongoing = false;
  m_requestPath.clear();
}

void XdpFileChooser::handleResponse(uint response, const QVariantMap &results) {
  m_ongoing = false;
  m_requestPath.clear();

  if (response == 1) {
    emit rejected();
    return;
  }

  if (response != 0) {
    qWarning() << "File chooser failed with response:" << response;
    emit rejected();
    return;
  }

  if (results.contains("uris")) {
    QStringList const uris = results["uris"].toStringList();
    std::vector<std::filesystem::path> filePaths;
    for (const QString &uri : uris) {
      QUrl const url(uri);
      if (url.isLocalFile()) { filePaths.emplace_back(url.toLocalFile().toStdString()); }
    }
    if (!filePaths.empty()) { emit filesChosen(filePaths); }
  }
}
