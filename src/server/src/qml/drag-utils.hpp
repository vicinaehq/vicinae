#pragma once
#include <QMimeData>

#include <memory>

class QObject;
class QString;

namespace DragUtils {

void startDrag(QObject *source, std::unique_ptr<QMimeData> mimeData, const QString &iconSource);

} // namespace DragUtils
