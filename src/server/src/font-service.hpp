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
  QString m_builtinFamily;
  QString m_builtinMonoFamily;
  QStringList m_symbolFamilies;
  QFont findEmojiFont();

public:
  const QFont &emojiFont() const { return m_emojiFont; }
  const QString &builtinFontFamily() const { return m_builtinFamily; }
  const QString &builtinMonoFontFamily() const { return m_builtinMonoFamily; }
  // Preferred family order for monochrome symbol glyphs (bundled Noto symbol fonts first).
  const QStringList &symbolFontFamilies() const { return m_symbolFamilies; }
  QStringList families() const { return QFontDatabase::families(); }

  FontService();
};
