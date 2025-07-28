#include "lib/emoji-detect.hpp"
#include <QList>
#include <qdebug.h>

static bool isEmojiCodePoint(uint codePoint) {

  if ((codePoint >= 0x1F600 && codePoint <= 0x1F64F) || // Emoticons
      (codePoint >= 0x1F300 && codePoint <= 0x1F5FF) || // Miscellaneous symbols and pictographs
      (codePoint >= 0x1F680 && codePoint <= 0x1F6FF) || // Transport and map symbols
      (codePoint >= 0x2600 && codePoint <= 0x26FF) ||   // Miscellaneous symbols
      (codePoint >= 0x2700 && codePoint <= 0x27BF) ||   // Dingbats
      (codePoint >= 0x1F900 && codePoint <= 0x1F9FF) || // Supplemental symbols and pictographs
      (codePoint >= 0x1F1E6 && codePoint <= 0x1F1FF)) { // Regional indicator symbols (flags)
    return true;
  }

  return false;
}

bool isEmoji(const QString &emoji) {
  for (const auto &c : emoji.toUcs4()) {
    if (!isEmojiCodePoint(c)) { return false; }
  }

  return true;
}
