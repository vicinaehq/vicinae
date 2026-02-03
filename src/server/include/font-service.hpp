#pragma once
#include <qfont.h>
#include <qfontdatabase.h>
#include <qfontinfo.h>
#include <qhash.h>
#include <qstring.h>
#include <qlist.h>
#include <qdebug.h>

class FontService {
  QFont m_emojiFont;
  QFont findEmojiFont();

public:
  const QFont &emojiFont() const { return m_emojiFont; }
  QStringList families() const { return QFontDatabase::families(); }

  FontService();
};
