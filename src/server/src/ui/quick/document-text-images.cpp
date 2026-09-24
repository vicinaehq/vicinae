#include <QAbstractTextDocumentLayout>
#include <QQmlEngine>
#include <QTextImageFormat>
#include <QFontInfo>
#include <QTimer>
#include <algorithm>
#include <cmath>
#include <limits>
#include "document-text-images.hpp"
#include "math-renderer.hpp"

namespace {
constexpr int IMAGE_CACHE_KIB = 64 * 1024;
constexpr int IMAGE_SIZE_CACHE_BYTES = 2 * 1024 * 1024;
constexpr qreal UNKNOWN_IMAGE_SIZE = 16; // Qt Quick's unloaded-image size.
} // namespace

DocumentImageCache::DocumentImageCache(QObject *parent)
    : QObject(parent), m_images(IMAGE_CACHE_KIB), m_sizes(IMAGE_SIZE_CACHE_BYTES) {}

DocumentImageCache *DocumentImageCache::forEngine(QQmlEngine *engine) {
  if (!engine) return nullptr;
  if (auto *cache = engine->findChild<DocumentImageCache *>(QString(), Qt::FindDirectChildrenOnly))
    return cache;
  return new DocumentImageCache(engine);
}

QImage DocumentImageCache::image(const QUrl &url) const {
  const auto *image = m_images.object(url);
  return image ? *image : QImage{};
}

std::optional<QSize> DocumentImageCache::size(const QUrl &url) const {
  if (const auto *size = m_sizes.object(url)) return *size;
  return {};
}

void DocumentImageCache::remember(const QUrl &url, const QImage &image) {
  if (image.isNull()) return;
  const int keyCost = url.toEncoded().size() + 64;
  if (const auto *cached = m_images.object(url); !cached || cached->cacheKey() != image.cacheKey())
    m_images.insert(url, new QImage(image), std::max(qsizetype(1), (image.sizeInBytes() + keyCost) / 1024));
  if (size(url) == image.size()) return;
  m_sizes.insert(url, new QSize(image.size()), keyCost);
  if (m_notificationPending) return;
  m_notificationPending = true;
  QTimer::singleShot(0, this, [this] {
    m_notificationPending = false;
    emit sizesChanged();
  });
}

DocumentTextImages::DocumentTextImages(QObject *parent) : QObject(parent) {}

DocumentTextImages::~DocumentTextImages() { attach(nullptr); }

void DocumentTextImages::setDocument(QQuickTextDocument *document) {
  if (m_document == document) return;
  if (m_document) disconnect(m_document, nullptr, this, nullptr);
  m_document = document;
  m_cache = DocumentImageCache::forEngine(qmlEngine(this));
  if (document)
    connect(document, &QQuickTextDocument::textDocumentChanged, this,
            [this] { attach(m_document ? m_document->textDocument() : nullptr); });
  attach(document ? document->textDocument() : nullptr);
  emit documentChanged();
}

void DocumentTextImages::attach(QTextDocument *document) {
  if (m_textDocument == document) return;
  if (m_textDocument) m_textDocument->documentLayout()->unregisterHandler(QTextFormat::ImageObject, this);
  m_textDocument = document;
  if (document) {
    document->documentLayout()->registerHandler(QTextFormat::ImageObject, this);
    document->markContentsDirty(0, document->characterCount());
  }
}

void DocumentTextImages::setMeasuring(bool measuring) {
  if (m_measuring == measuring) return;
  m_measuring = measuring;
  if (m_textDocument) {
    if (!measuring)
      for (const auto &format : m_textDocument->allFormats())
        if (format.isImageFormat())
          m_textDocument->addResource(QTextDocument::ImageResource,
                                      m_textDocument->baseUrl().resolved(QUrl(format.toImageFormat().name())),
                                      QVariant{});
    m_textDocument->markContentsDirty(0, m_textDocument->characterCount());
  }
  emit measuringChanged();
}

void DocumentTextImages::measure(QTextDocument *document, DocumentImageCache *cache) {
  auto *handler = new DocumentTextImages(document);
  handler->m_measuring = true;
  handler->m_cache = cache;
  handler->attach(document);
}

void DocumentTextImages::setDevicePixelRatio(qreal ratio) {
  if (qFuzzyCompare(m_devicePixelRatio, ratio)) return;
  m_devicePixelRatio = ratio;
  if (m_textDocument) m_textDocument->markContentsDirty(0, m_textDocument->characterCount());
  emit devicePixelRatioChanged();
}

QSizeF DocumentTextImages::intrinsicSize(QTextDocument *document, int, const QTextFormat &format) {
  const auto imageFormat = format.toImageFormat();
  const auto url = document->baseUrl().resolved(QUrl(imageFormat.name()));
  const auto font = imageFormat.font().resolve(document->defaultFont());
  if (const auto size = math::size(url, QFontInfo(font).pixelSize())) {
    // An empty image makes Qt Quick call drawObject at the window's device pixel ratio.
    document->addResource(QTextDocument::ImageResource, url, QImage{});
    return *size;
  }
  auto naturalSize = m_cache ? m_cache->size(url) : std::optional<QSize>{};
  if (m_measuring) {
    // Qt may also request resources outside intrinsicSize while preparing text nodes.
    document->addResource(QTextDocument::ImageResource, url, QImage{});
  } else {
    const auto cached = m_cache ? m_cache->image(url) : QImage{};
    if (!cached.isNull()) document->addResource(QTextDocument::ImageResource, url, cached);
    const auto image = document->resource(QTextDocument::ImageResource, url).value<QImage>();
    if (!image.isNull()) {
      naturalSize = image.size();
      if (m_cache) m_cache->remember(url, image);
    }
  }
  const bool hasWidth = imageFormat.width() > 0;
  const bool hasHeight = imageFormat.height() > 0;
  const auto maximum = imageFormat.maximumWidth();
  const auto maxWidth = maximum.type() == QTextLength::VariableLength
                            ? std::numeric_limits<qreal>::max()
                            : maximum.value(document->pageSize().width() - 2 * document->documentMargin());
  qreal width = hasWidth ? std::min(imageFormat.width(), maxWidth) : UNKNOWN_IMAGE_SIZE;
  qreal height = hasHeight ? imageFormat.height() : UNKNOWN_IMAGE_SIZE;
  if (naturalSize) {
    const qreal ratio = qreal(naturalSize->width()) / naturalSize->height();
    if (!hasWidth) width = std::min(maxWidth, hasHeight ? std::round(height * ratio) : naturalSize->width());
    if (!hasHeight) height = std::round(width / ratio);
  }
  return {width, height};
}

void DocumentTextImages::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *document, int,
                                    const QTextFormat &format) {
  const auto url = document->baseUrl().resolved(QUrl(format.toImageFormat().name()));
  if (!m_measuring && url.scheme() == "vicinae-math") math::draw(painter, rect, url);
}
