#pragma once
#include <QMimeData>

#include <memory>
#include <optional>

#include "ui/image/url.hpp"

class QObject;

namespace DragUtils {

void startDrag(QObject *source, std::unique_ptr<QMimeData> mimeData, std::optional<ImageURL> icon);

} // namespace DragUtils
