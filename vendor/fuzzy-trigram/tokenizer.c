/*
** 2024-10-21
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*/

#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint16_t u32;

#include "fts5_unicode2.c"

/*
** The following macro - WRITE_UTF8 - have been copied
** from the sqlite3 source file:
*https://github.com/sqlite/sqlite/blob/88282af521692b398b0d0cc58a8bdb220a8ff58c/ext/fts5/fts5_tokenize.c.
*/
static const unsigned char sqlite3Utf8Trans1[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
    0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x00,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x00, 0x00,
};

#define READ_UTF8(zIn, zTerm, c)                                               \
  c = *(zIn++);                                                                \
  if (c >= 0xc0) {                                                             \
    c = sqlite3Utf8Trans1[c - 0xc0];                                           \
    while (zIn < zTerm && (*zIn & 0xc0) == 0x80) {                             \
      c = (c << 6) + (0x3f & *(zIn++));                                        \
    }                                                                          \
    if (c < 0x80 || (c & 0xFFFFF800) == 0xD800 ||                              \
        (c & 0xFFFFFFFE) == 0xFFFE) {                                          \
      c = 0xFFFD;                                                              \
    }                                                                          \
  }

#define WRITE_UTF8(zOut, c)                                                    \
  {                                                                            \
    if (c < 0x00080) {                                                         \
      *zOut++ = (unsigned char)(c & 0xFF);                                     \
    } else if (c < 0x00800) {                                                  \
      *zOut++ = 0xC0 + (unsigned char)((c >> 6) & 0x1F);                       \
      *zOut++ = 0x80 + (unsigned char)(c & 0x3F);                              \
    } else if (c < 0x10000) {                                                  \
      *zOut++ = 0xE0 + (unsigned char)((c >> 12) & 0x0F);                      \
      *zOut++ = 0x80 + (unsigned char)((c >> 6) & 0x3F);                       \
      *zOut++ = 0x80 + (unsigned char)(c & 0x3F);                              \
    } else {                                                                   \
      *zOut++ = 0xF0 + (unsigned char)((c >> 18) & 0x07);                      \
      *zOut++ = 0x80 + (unsigned char)((c >> 12) & 0x3F);                      \
      *zOut++ = 0x80 + (unsigned char)((c >> 6) & 0x3F);                       \
      *zOut++ = 0x80 + (unsigned char)(c & 0x3F);                              \
    }                                                                          \
  }

#define FTS5_SKIP_UTF8(zIn)                                                    \
  {                                                                            \
    if (((unsigned char)(*(zIn++))) >= 0xc0) {                                 \
      while ((((unsigned char)*zIn) & 0xc0) == 0x80) {                         \
        zIn++;                                                                 \
      }                                                                        \
    }                                                                          \
  }

static const int CJK[8][2] = {
    {0x3000, 0x30FF},   // CJK Symbols, Hiragana, Katakana
    {0x3400, 0x4DBF},   // CJK Unified Ideographs Extension A
    {0x4E00, 0x9FFF},   // CJK Unified Ideographs
    {0xF900, 0xFAFF},   // CJK Compatibility Ideographs
    {0xFF00, 0xFFEF},   // Halfwidth and Fullwidth Forms (incl. Halfwidth Katakana)
    {0x20000, 0x2EBEF}, // CJK Unified Ideographs Extension B/C/D/E/F
    {0x2F800, 0x2FA1F}, // CJK Compatibility Ideographs Supplement
    {0x30000, 0x3134F}  // CJK Unified Ideographs Extension G
};

// https://jrgraphix.net/research/unicode_blocks.php
// https://en.wikipedia.org/wiki/CJK_Unified_Ideographs
// 3000 — 30FF    CJK Symbols and Punctuation, Hiragana, Katakana
// 3400 — 4DBF    CJK Unified Ideographs Extension A
// 4E00 — 9FFF    CJK Unified Ideographs
// F900 — FAFF    CJK Compatibility Ideographs
// FF00 — FFEF    Halfwidth and Fullwidth Forms
// 20000 — 2EBEF  CJK Unified Ideographs Extension B/C/D/E/F 
// 2F800 — 2FA1F  CJK Compatibility Ideographs Supplement
// 30000 — 3134F  CJK Unified Ideographs Extension G

static inline int isCJK(int iCode) {
  for (int i = 0; i < 8; i++) {
    if (iCode < CJK[i][0]) { // smaller
      break;
    }
    if (iCode <= CJK[i][1]) { // in range
      return 1;
    }
    // bigger than range
  }
  return 0;
}

/* Function to optionally fold case and remove diacritics */
static inline int customFold(int iCode, int foldCase, int removeDiacritics) {
  if (iCode == 0)
    return iCode;
  if (foldCase)
    return sqlite3Fts5UnicodeFold(iCode, removeDiacritics);
  return iCode;
}

/*
** Trigram tokenizer with word bridging and optional fault-tolerance modes.
**
** The text is decoded into a single codepoint stream with every separator
** (any ASCII non-alphanumeric: space, -, _, ., /, ...) REMOVED: "la belle
** data" is windowed as "labelledata", "sf-pro" as "sfpro". Trigram phrases
** therefore match across word boundaries regardless of how either side
** spells the separators: 'screenshot' finds "Screen Shot ...", 'sfpro' and
** 'sf pro' both find "sf-pro". Punctuation stops discriminating at retrieval;
** the fzf rerank against the original query restores it in the ordering.
** Word boundaries still matter for two things: short (1-2 char) words are
** additionally emitted as whole tokens so bigram prefix queries keep working,
** and skeleton mode keeps its first-character rule per word.
**
** Skeleton mode collapses each word to its abbreviation skeleton before
** windowing: the first character is kept, later vowels are dropped and
** consecutive duplicates are collapsed ('downloads' -> 'dwnlds'). Applied to
** both documents and queries, it makes devowelized abbreviations match
** through plain trigram search.
**
** Skip-gram mode (documents only) emits every contiguous trigram as usual
** plus, colocated at the same position, the two skip-one trigrams that drop
** the second or third of the four characters starting there:
**
**   c0 c1 c2 c3  ->  c0c1c2 (advances position), c0c1c3, c0c2c3 (colocated)
**
** Colocation preserves position arithmetic, so multi-trigram phrase queries
** built from contiguous trigrams still require adjacency and can match through
** the skip variants ('dwld' -> dwl@p, wld@p+1 inside 'dwnlds').
*/

#ifndef FTS5_TOKEN_COLOCATED
#define FTS5_TOKEN_COLOCATED 0x0001
#endif

/* Longest buffered stream; longer texts are flushed and continued. */
#define STREAM_CAP 1024
/* Short words recorded per stream flush; extras are simply not emitted. */
#define SHORT_WORD_CAP 64

typedef struct {
  int aCode[STREAM_CAP]; /* folded codepoints, separators removed */
  int aOff[STREAM_CAP];  /* source byte offset of each kept codepoint */
  int n;
  int aShortStart[SHORT_WORD_CAP]; /* 1-2 char words, emitted standalone */
  int aShortLen[SHORT_WORD_CAP];
  int nShort;
} TokenStream;

/* non-ascii codepoints are word characters: case folding already normalizes
** the letters we care about, and CJK is handled separately */
static int isWordChar(int iCode) {
  if (iCode >= 0x80)
    return 1;
  return (iCode >= '0' && iCode <= '9') || (iCode >= 'a' && iCode <= 'z') ||
         (iCode >= 'A' && iCode <= 'Z');
}

static int isSkeletonVowel(int iCode) {
  return iCode == 'a' || iCode == 'e' || iCode == 'i' || iCode == 'o' ||
         iCode == 'u';
}

/* In-place abbreviation-skeleton transform; returns the new length.
** Codepoints are already case-folded at this point. */
static int skeletonizeWord(int *aCode, int *aOff, int nChar) {
  int nOut = 0;
  int last = 0;
  int j;

  for (j = 0; j < nChar; j++) {
    int iCode = aCode[j];

    if (nOut > 0) {
      if (isSkeletonVowel(iCode))
        continue;
      if (iCode == last)
        continue;
    }

    aCode[nOut] = iCode;
    aOff[nOut] = aOff[j];
    last = iCode;
    nOut++;
  }

  return nOut;
}

static int emitToken(const int *aCode, int nChar, int tokenFlags, int iStart,
                     int iEnd, void *pCtx,
                     int (*xToken)(void *, int, const char *, int, int, int)) {
  char aBuf[16];
  char *zOut = aBuf;
  int j;

  for (j = 0; j < nChar; j++) {
    WRITE_UTF8(zOut, aCode[j]);
  }

  return xToken(pCtx, tokenFlags, aBuf, zOut - aBuf, iStart, iEnd);
}

static int emitStream(const TokenStream *s, int iEndOff, int bSkipgrams,
                      void *pCtx,
                      int (*xToken)(void *, int, const char *, int, int,
                                    int)) {
  int nChar = s->n;
  int j;
  int k;

#define CHAR_END(idx) ((idx) + 1 < nChar ? s->aOff[(idx) + 1] : iEndOff)

  if (nChar == 0)
    return 0;

  if (nChar < 3) {
    if (emitToken(s->aCode, nChar, 0, s->aOff[0], iEndOff, pCtx, xToken))
      return 1;

    for (k = 0; k < s->nShort; k++) {
      if (s->aShortLen[k] == nChar) /* identical to the whole stream */
        continue;
      if (emitToken(s->aCode + s->aShortStart[k], s->aShortLen[k],
                    FTS5_TOKEN_COLOCATED, s->aOff[s->aShortStart[k]],
                    CHAR_END(s->aShortStart[k] + s->aShortLen[k] - 1), pCtx,
                    xToken))
        return 1;
    }
    return 0;
  }

  for (j = 0; j + 2 < nChar; j++) {
    int v;

    if (emitToken(s->aCode + j, 3, 0, s->aOff[j], CHAR_END(j + 2), pCtx,
                  xToken))
      return 1;

    if (!bSkipgrams || j + 3 >= nChar)
      continue;

    for (v = 0; v < 2; v++) {
      int aSkip[3];

      aSkip[0] = s->aCode[j];
      aSkip[1] = s->aCode[v == 0 ? j + 1 : j + 2];
      aSkip[2] = s->aCode[j + 3];
      if (emitToken(aSkip, 3, FTS5_TOKEN_COLOCATED, s->aOff[j],
                    CHAR_END(j + 3), pCtx, xToken))
        return 1;
    }
  }

  /* standalone short words ride colocated on the last trigram: only prefix
  ** and equality lookups ever target them, so their position is irrelevant */
  for (k = 0; k < s->nShort; k++) {
    if (emitToken(s->aCode + s->aShortStart[k], s->aShortLen[k],
                  FTS5_TOKEN_COLOCATED, s->aOff[s->aShortStart[k]],
                  CHAR_END(s->aShortStart[k] + s->aShortLen[k] - 1), pCtx,
                  xToken))
      return 1;
  }

#undef CHAR_END
  return 0;
}

static void tokenizeBuffered(const char *pText, int nText, int foldCase,
                             int removeDiacritics, int bSkeleton,
                             int bSkipgrams, void *pCtx,
                             int (*xToken)(void *, int, const char *, int, int,
                                           int)) {
  const unsigned char *zIn = (const unsigned char *)pText;
  const unsigned char *zEnd = zIn + nText;
  TokenStream s;
  int wordStart = 0;
  int iCode;

  s.n = 0;
  s.nShort = 0;

/* closes the current word: applies the skeleton transform in place and
** records 1-2 char words for standalone emission */
#define FINISH_WORD()                                                         \
  {                                                                           \
    int wordLen = s.n - wordStart;                                            \
    if (wordLen > 0) {                                                        \
      if (bSkeleton) {                                                        \
        wordLen =                                                             \
            skeletonizeWord(s.aCode + wordStart, s.aOff + wordStart, wordLen);\
        s.n = wordStart + wordLen;                                            \
      }                                                                       \
      if (wordLen <= 2 && s.nShort < SHORT_WORD_CAP) {                        \
        s.aShortStart[s.nShort] = wordStart;                                  \
        s.aShortLen[s.nShort] = wordLen;                                      \
        s.nShort++;                                                           \
      }                                                                       \
    }                                                                         \
    wordStart = s.n;                                                          \
  }

  while (zIn < zEnd) {
    int start;

    do {
      start = zIn - (const unsigned char *)pText;
      READ_UTF8(zIn, zEnd, iCode);
      if (iCode == 0)
        break;
      iCode = customFold(iCode, foldCase, removeDiacritics);
    } while (iCode == 0);

    if (iCode == 0)
      break;

    if (!isWordChar(iCode)) { /* separators close the word, not the window */
      FINISH_WORD();
      continue;
    }

    if (isCJK(iCode)) {
      FINISH_WORD();
      if (emitStream(&s, start, bSkipgrams, pCtx, xToken))
        return;
      s.n = 0;
      s.nShort = 0;
      wordStart = 0;

      if (emitToken(&iCode, 1, 0, start, zIn - (const unsigned char *)pText,
                    pCtx, xToken))
        return;
      continue;
    }

    if (s.n == STREAM_CAP) {
      FINISH_WORD();
      if (emitStream(&s, start, bSkipgrams, pCtx, xToken))
        return;
      s.n = 0;
      s.nShort = 0;
      wordStart = 0;
    }

    s.aCode[s.n] = iCode;
    s.aOff[s.n] = start;
    s.n++;
  }

  FINISH_WORD();
  emitStream(&s, zIn - (const unsigned char *)pText, bSkipgrams, pCtx, xToken);
#undef FINISH_WORD
}
