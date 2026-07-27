#include <QDrag>
#include <QGuiApplication>
#include <QPixmap>
#include <utility>

#include "drag-utils.hpp"
#include "ui/image/image-renderer.hpp"
#include "ui/image/url.hpp"

namespace DragUtils {

void startDrag(QObject *source, std::unique_ptr<QMimeData> mimeData, const QString &iconSource) {
  if (!source || !mimeData) return;

  auto *drag = new QDrag(source);
  drag->setMimeData(mimeData.release());

  if (auto frame = ImageRendering::cachedFrame(ImageURL(iconSource))) {
    constexpr int DRAG_ICON_SIZE = 40;
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

} // namespace DragUtils
