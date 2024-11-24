#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>

struct RateResponse {
  QString currencySymbol;
  QMap<QString, double> rates;
  int lastUpdateUnix;
  int nextUpdateUnix;
};

class ExchangeRateService : public QObject {
  Q_OBJECT
  QNetworkAccessManager *manager;

  QString baseUrl = "https://open.er-api.com/v6/latest/%1";

public:
  ExchangeRateService() : manager(new QNetworkAccessManager()) {
    connect(manager, &QNetworkAccessManager::finished, this,
            &ExchangeRateService::requestFinished);
  }

  ~ExchangeRateService() { delete manager; }

  void fetchSymbol(const QString &symbol) {
    QNetworkRequest req;

    req.setUrl(baseUrl.arg(symbol));
    manager->get(req);
  }

signals:
  void rateResponse(const RateResponse &res);

public slots:
  void requestFinished(QNetworkReply *reply) {
    auto body = reply->readAll();
    auto jdoc = QJsonDocument::fromJson(body);
    QJsonObject obj = jdoc.object();

    auto rates = obj.value("rates");
    auto baseCode = obj.value("base_code").toString();
    auto lastUpdateUnix = obj.value("time_last_update_unix").toInt();
    auto nextUpdateUnix = obj.value("time_next_update_unix").toInt();

    if (!rates.isObject()) {
      qWarning() << "rates is not an object";
      return;
    }

    auto ratesObj = rates.toObject();

    QMap<QString, double> rateMap;

    for (const auto &key : ratesObj.keys()) {
      auto rate = ratesObj.value(key).toDouble();

      rateMap.insert(key, rate);

      qDebug() << key << " => " << ratesObj.value(key).toDouble();
    }

    emit rateResponse({baseCode, rateMap, lastUpdateUnix, nextUpdateUnix});
  }
};
