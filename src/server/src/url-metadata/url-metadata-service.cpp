#include "url-metadata/url-metadata-service.hpp"
#include <QNetworkReply>
#include <QTimer>
#include <libxml/HTMLparser.h>
#include <libxml/tree.h>
#include <qlogging.h>
#include "lib/http-client.hpp"

UrlMetadataService::UrlMetadataService(QObject *parent) : QObject(parent) {}

UrlMetadataService::~UrlMetadataService() { abortAll(); }

void UrlMetadataService::abortAll() {
  for (auto &[url, req] : m_inFlight) {
    req.reply->abort();
    req.reply->deleteLater();
  }
  m_inFlight.clear();
}

UrlMetadataService::UrlMetadata UrlMetadataService::parseMetaTags(const QByteArray &html) {
  UrlMetadata result;

  auto *doc = htmlReadMemory(html.constData(), html.size(), nullptr, "UTF-8",
                             HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_RECOVER);
  if (!doc) return result;

  std::function<void(xmlNode *)> walk = [&](xmlNode *node) {
    for (auto *cur = node; cur; cur = cur->next) {
      if (cur->type == XML_ELEMENT_NODE) {
        if (xmlStrcasecmp(cur->name, BAD_CAST "meta") == 0) {
          xmlChar *prop = xmlGetProp(cur, BAD_CAST "property");
          xmlChar *name = xmlGetProp(cur, BAD_CAST "name");
          xmlChar *content = xmlGetProp(cur, BAD_CAST "content");

          if (content) {
            auto *c = reinterpret_cast<const char *>(content);
            auto *p = reinterpret_cast<const char *>(prop);
            auto *n = reinterpret_cast<const char *>(name);

            if (p && strcasecmp(p, "og:title") == 0 && !result.ogTitle) {
              result.ogTitle = QString::fromUtf8(c);
            } else if (p && strcasecmp(p, "og:description") == 0 && !result.ogDescription) {
              result.ogDescription = QString::fromUtf8(c);
            } else if (p && strcasecmp(p, "og:image") == 0 && !result.ogImage) {
              result.ogImage = QString::fromUtf8(c);
            } else if (n && strcasecmp(n, "description") == 0 && !result.ogDescription) {
              result.ogDescription = QString::fromUtf8(c);
            }
          }

          xmlFree(prop);
          xmlFree(name);
          xmlFree(content);
        } else if (xmlStrcasecmp(cur->name, BAD_CAST "title") == 0 && !result.ogTitle) {
          auto *text = xmlNodeGetContent(cur);
          if (text) {
            result.ogTitle = QString::fromUtf8(reinterpret_cast<const char *>(text));
            xmlFree(text);
          }
        }

        if (xmlStrcasecmp(cur->name, BAD_CAST "body") == 0) return;
      }

      walk(cur->children);
    }
  };

  walk(xmlDocGetRootElement(doc));
  xmlFreeDoc(doc);

  return result;
}

void UrlMetadataService::fetchMetadata(const QString &selectionId, const QUrl &url) {
  QString const urlStr = url.toString();

  if (!url.scheme().startsWith("http")) return;

  if (auto it = m_cache.find(urlStr); it != m_cache.end()) {
    const auto &cached = it->second;
    if (cached.ogTitle || cached.ogDescription || cached.ogImage) {
      emit metadataReady(selectionId, cached.ogTitle.value_or(QString()),
                         cached.ogDescription.value_or(QString()), cached.ogImage.value_or(QString()));
    }
    return;
  }

  if (auto it = m_inFlight.find(urlStr); it != m_inFlight.end()) {
    it->second.selectionIds.push_back(selectionId);
    return;
  }

  QNetworkRequest req(url);
  req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                   static_cast<int>(QNetworkRequest::NoLessSafeRedirectPolicy));
  req.setHeader(QNetworkRequest::UserAgentHeader,
                QStringLiteral("Mozilla/5.0 (compatible; Vicinae/1.0)"));
  req.setRawHeader("Accept", "text/html");

  auto *reply = http::networkManager()->get(req);
  m_inFlight.insert({urlStr, {reply, QByteArray(), {selectionId}}});
  m_inFlight[urlStr].buffer.reserve(MAX_BUFFER_SIZE);

  auto *timer = new QTimer(reply);
  timer->setSingleShot(true);

  connect(timer, &QTimer::timeout, reply, [reply]() { reply->abort(); });

  connect(reply, &QNetworkReply::readyRead, reply, [this, urlStr]() {
    auto it = m_inFlight.find(urlStr);
    if (it == m_inFlight.end()) return;
    it->second.buffer.append(it->second.reply->readAll());
    if (it->second.buffer.size() >= MAX_BUFFER_SIZE) { it->second.reply->abort(); }
  });

  connect(reply, &QNetworkReply::finished, this, [this, reply, urlStr]() {
    auto it = m_inFlight.find(urlStr);
    if (it == m_inFlight.end()) return;

    if (reply->error() == QNetworkReply::NoError) { it->second.buffer.append(reply->readAll()); }
    auto metadata = parseMetaTags(it->second.buffer);
    auto selectionIds = std::move(it->second.selectionIds);

    m_inFlight.erase(it);
    reply->deleteLater();

    if (m_cache.size() >= MAX_CACHE_SIZE) { m_cache.erase(m_cache.begin()); }
    m_cache.insert({urlStr, metadata});

    if (metadata.ogTitle || metadata.ogDescription || metadata.ogImage) {
      for (const auto &id : selectionIds) {
        emit metadataReady(id, metadata.ogTitle.value_or(QString()), metadata.ogDescription.value_or(QString()),
                           metadata.ogImage.value_or(QString()));
      }
    }
  });

  timer->start(TIMEOUT_MS);
}
