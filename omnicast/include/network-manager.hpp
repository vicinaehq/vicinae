#pragma once
#include <qnetworkaccessmanager.h>
#include <qnetworkdiskcache.h>
#include <qstandardpaths.h>

class NetworkManager {
  QNetworkAccessManager *m_manager = new QNetworkAccessManager;
  QNetworkDiskCache *m_diskCache = new QNetworkDiskCache;

public:
  NetworkManager() {
    QString directory =
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1StringView("/omnicast/");
    qDebug() << "cache dir" << directory;
    m_diskCache->setCacheDirectory(directory);
    m_manager->setCache(m_diskCache);
  }

  QNetworkAccessManager *manager() { return m_manager; }

  static NetworkManager *instance() {
    static NetworkManager instance;

    return &instance;
  }
};
