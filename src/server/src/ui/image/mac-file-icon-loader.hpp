#pragma once
#include <QImage>
#include <QSize>
#include <QString>

QImage renderMacFileIcon(const QString &path, const QSize &size);
QImage renderMacSymbolIcon(const QString &name, const QSize &size);
