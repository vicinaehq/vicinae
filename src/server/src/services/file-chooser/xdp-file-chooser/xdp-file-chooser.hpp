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

  bool isAvailable() const override;
  bool open(const FileChooserOptions &options) override;
  void close();

public slots:
  void handleResponse(uint response, const QVariantMap &results);

private:
  QString generateToken() const;

  QDBusConnection m_bus;
  QDBusInterface *m_interface;
  QString m_requestPath;
};
