#pragma once
#include <QImage>
#include <QSize>
#include <QString>

// Native icon for a shell parsing name (a path, .lnk, or shell:AppsFolder\<AUMID>) via
// IShellItemImageFactory. Self-initializes COM, so it is safe to call from the decoding pool.
QImage renderWinShellIcon(const QString &parsingName, const QSize &size);
