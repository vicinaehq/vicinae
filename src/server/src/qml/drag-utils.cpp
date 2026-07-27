#include <QDrag>
#include <QGuiApplication>
#include <QPixmap>
#include <QPointer>
#include <QUrl>

#include <memory>
#include <ranges>
#include <utility>

#include "drag-utils.hpp"
#include "ui/image/image-renderer.hpp"
#include "ui/image/url.hpp"

namespace DragUtils {

static constexpr int DRAG_ICON_SIZE = 40;

static ImageURL fallbackIcon(const QMimeData &mimeData) {
  if (mimeData.hasUrls()) {
    const auto urls = mimeData.urls();
    const bool onlyLocalFiles =
        !urls.empty() && std::ranges::all_of(urls, [](const QUrl &url) { return url.isLocalFile(); });
    return ImageURL::builtin(onlyLocalFiles ? "blank-document" : "link");
  }
  if (mimeData.hasText() || mimeData.hasHtml()) return ImageURL::builtin("text");
  return ImageURL::builtin("blank-document");
}

static void executeDrag(QObject *source, std::unique_ptr<QMimeData> mimeData,
                        const std::optional<QImage> &frame) {
  auto *drag = new QDrag(source);
  drag->setMimeData(mimeData.release());

  if (frame) {
    auto const dpr = qGuiApp->devicePixelRatio();
    auto const physicalSize = qRound(DRAG_ICON_SIZE * dpr);
    auto image = frame->scaled(physicalSize, physicalSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    auto pixmap = QPixmap::fromImage(std::move(image));
    pixmap.setDevicePixelRatio(dpr);
    drag->setPixmap(pixmap);
    drag->setHotSpot({DRAG_ICON_SIZE / 2, DRAG_ICON_SIZE / 2});
  }

  drag->exec(Qt::CopyAction);
  drag->deleteLater();
}

void startDrag(QObject *source, std::unique_ptr<QMimeData> mimeData, const QString &iconSource) {
  if (!source || !mimeData) return;

  const auto icon = iconSource.isEmpty() ? fallbackIcon(*mimeData) : ImageURL(iconSource);
  if (auto frame = ImageRendering::cachedFrame(icon)) {
    executeDrag(source, std::move(mimeData), frame);
    return;
  }

  const auto dpr = qGuiApp->devicePixelRatio();
  const auto physicalSize = qRound(DRAG_ICON_SIZE * dpr);
  auto retainedMimeData = std::make_shared<std::unique_ptr<QMimeData>>(std::move(mimeData));

  ImageRendering::renderFirstFrame(icon, {physicalSize, physicalSize})
      .then(qGuiApp, [source = QPointer(source), retainedMimeData](QImage frame) mutable {
        if (!source || !(QGuiApplication::mouseButtons() & Qt::LeftButton)) return;
        executeDrag(source, std::move(*retainedMimeData),
                    frame.isNull() ? std::nullopt : std::optional(std::move(frame)));
      });
}

} // namespace DragUtils
