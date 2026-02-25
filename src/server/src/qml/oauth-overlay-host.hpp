#pragma once
#include "common/context.hpp"
#include "proto/oauth.pb.h"
#include <QObject>
#include <QUrl>

class OAuthOverlayHost : public QObject {
  Q_OBJECT

  Q_PROPERTY(QUrl qmlComponentUrl READ qmlComponentUrl CONSTANT)
  Q_PROPERTY(QString providerName READ providerName NOTIFY stateChanged)
  Q_PROPERTY(QString providerDescription READ providerDescription NOTIFY stateChanged)
  Q_PROPERTY(QString providerIconSource READ providerIconSource NOTIFY stateChanged)
  Q_PROPERTY(bool success READ success NOTIFY stateChanged)

signals:
  void stateChanged();

public:
  OAuthOverlayHost(const ApplicationContext *ctx, const proto::ext::oauth::AuthorizeRequest &req,
                   QObject *parent = nullptr);

  QUrl qmlComponentUrl() const;
  QString providerName() const;
  QString providerDescription() const;
  QString providerIconSource() const;
  bool success() const;

  Q_INVOKABLE void openBrowser();
  Q_INVOKABLE void abort();
  void showSuccess();

private:
  const ApplicationContext *m_ctx;
  QString m_providerName;
  QString m_providerDescription;
  QString m_providerIconSource;
  QString m_authorizeUrl;
  bool m_success = false;
};
