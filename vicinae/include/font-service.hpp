#pragma once
#include "trie.hpp"
#include <cstdlib>
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

  Trie<QString> buildFontSearchIndex() {
    Trie<QString> trie;

    for (const auto &system : QFontDatabase::writingSystems()) {
      std::string sname = QFontDatabase::writingSystemName(system).toStdString();

      for (const auto &family : QFontDatabase::families(system)) {
        trie.index(sname, family);

        if (system == QFontDatabase::WritingSystem::Latin) {
          trie.indexLatinText(family.toStdString(), family);
        } else {
          trie.index(family.toStdString(), family);
        }
      }
    }

    return trie;
  }

  FontService();
};
