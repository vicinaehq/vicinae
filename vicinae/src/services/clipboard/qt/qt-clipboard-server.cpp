#include "qt-clipboard-server.hpp"
#include <QCryptographicHash>
#include <QUrl>
#include <QDebug>
#include <QApplication>
#include <QMimeData>
#include <QBuffer>
#include <QImage>

QtClipboardServer::QtClipboardServer() { qDebug() << "QtClipboardServer: Initializing Qt clipboard server"; }

QtClipboardServer::~QtClipboardServer() { disconnectQtFallback(); }

bool QtClipboardServer::isActivatable() const {
  // This server is universally available as a fallback
  return true;
}

int QtClipboardServer::activationPriority() const {
  // Low priority for fallback behavior
  return 1;
}

QString QtClipboardServer::id() const { return "qt-clipboard"; }

bool QtClipboardServer::isAlive() const { return m_isConnected && m_qtFallbackActive; }

bool QtClipboardServer::start() {
  // Set up Qt clipboard monitoring
  if (!setupQtFallback()) {
    qCritical() << "QtClipboardServer: Failed to set up Qt clipboard monitoring";
    return false;
  }

  m_isConnected = true;
  return true;
}

bool QtClipboardServer::setupQtFallback() {
  m_qtClipboard = QApplication::clipboard();
  if (!m_qtClipboard) {
    qWarning() << "QtClipboardServer: Qt clipboard not available";
    return false;
  }

  // Connect to clipboard change signal
  connect(m_qtClipboard, &QClipboard::dataChanged, this, &QtClipboardServer::handleQtClipboardChange);

  m_qtFallbackActive = true;
  return true;
}

void QtClipboardServer::disconnectQtFallback() {
  if (m_qtClipboard) {
    disconnect(m_qtClipboard, nullptr, this, nullptr);
    m_qtClipboard = nullptr;
  }

  m_qtFallbackActive = false;
}

void QtClipboardServer::handleQtClipboardChange() {
  if (!m_qtClipboard) { return; }

  const QMimeData *mimeData = m_qtClipboard->mimeData();
  if (!mimeData) { return; }

  // Check if content actually changed
  if (!hasContentChanged(mimeData)) { return; }

  try {
    ClipboardSelection selection = parseQtMimeData(mimeData);

    if (!selection.offers.empty()) {
      m_lastClipboardHash = calculateContentHash(selection);
      emit selectionAdded(selection);
    }
  } catch (const std::exception &e) {
    qWarning() << "QtClipboardServer: Error processing clipboard content:" << e.what();
  }
}

ClipboardSelection QtClipboardServer::parseQtMimeData(const QMimeData *mimeData) const {
  ClipboardSelection selection;

  if (!mimeData) { return selection; }

  // Handle text content
  if (mimeData->hasText()) {
    const QString text = mimeData->text();
    if (!text.isEmpty()) { selection.offers.push_back({"text/plain", text.toUtf8()}); }
  }

  // Handle HTML content
  if (mimeData->hasHtml()) {
    const QString html = mimeData->html();
    if (!html.isEmpty()) { selection.offers.push_back({"text/html", html.toUtf8()}); }
  }

  // Handle URLs/files
  if (mimeData->hasUrls()) {
    QStringList urls;
    for (const QUrl &url : mimeData->urls()) {
      urls << url.toString();
    }
    if (!urls.isEmpty()) { selection.offers.push_back({"text/uri-list", urls.join('\n').toUtf8()}); }
  }

  // Handle images - try to get raw data first, then convert QImage if needed
  if (mimeData->hasImage()) {
    QByteArray imageData;
    const QStringList imageFormats = {"image/png", "image/jpeg", "image/bmp"};

    // Try to get raw image data first
    for (const QString &format : imageFormats) {
      if (mimeData->hasFormat(format)) {
        imageData = mimeData->data(format);
        if (!imageData.isEmpty()) {
          selection.offers.push_back({format, imageData});
          break;
        }
      }
    }

    // If no raw data but QImage is available, convert to PNG
    if (imageData.isEmpty() && mimeData->hasImage()) {
      QImage image = mimeData->imageData().value<QImage>();
      if (!image.isNull()) {
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        if (image.save(&buffer, "PNG")) { selection.offers.push_back({"image/png", imageData}); }
      }
    }
  }

  // Handle other formats
  const QStringList formats = mimeData->formats();
  for (const QString &format : formats) {
    // Skip formats we already handled
    if (format.startsWith("text/") || format.startsWith("image/")) { continue; }

    QByteArray data = mimeData->data(format);
    if (!data.isEmpty() && data.size() < 1024 * 1024) { // Limit to 1MB
      selection.offers.push_back({format, data});
    }
  }

  return selection;
}

QByteArray QtClipboardServer::calculateContentHash(const ClipboardSelection &selection) const {
  QCryptographicHash hash(QCryptographicHash::Sha256);

  // Hash each offer's MIME type and data
  for (const auto &offer : selection.offers) {
    hash.addData(offer.mimeType.toUtf8());
    hash.addData(offer.data);
  }

  return hash.result();
}

bool QtClipboardServer::hasContentChanged(const QMimeData *mimeData) const {
  ClipboardSelection selection = parseQtMimeData(mimeData);
  QByteArray currentHash = calculateContentHash(selection);
  return currentHash != m_lastClipboardHash;
}
