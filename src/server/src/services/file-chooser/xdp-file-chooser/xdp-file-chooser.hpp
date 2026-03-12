#pragma once
#include "services/file-chooser/abstract-file-chooser.hpp"
#include <QtDBus/QtDBus>
#include <QtDBus/qdbusconnection.h>
#include <QtDBus/qdbusinterface.h>
#include <qobject.h>
#include <qtmetamacros.h>

class XdpFileChooser : public AbstractFileChooser {
  Q_OBJECT

  bool m_ongoing = false;

public:
  XdpFileChooser(QObject *parent = nullptr);

  bool open(const FileChooserOptions &options) override;

public slots:
  void handleResponse(uint response, const QVariantMap &results);

private:
  QString generateToken() const;

  QDBusConnection m_bus;
  QDBusInterface *m_interface;
};
