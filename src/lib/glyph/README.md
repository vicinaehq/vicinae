# libglyph

A static table of emoji and curated Unicode symbols, plus emoji-specific helpers.

- `glyph/glyph.hpp` — the table and lookups (`glyph::`).
- `glyph/emoji.hpp` — emoji-only helpers: skin tones, segmentation, detection (`emoji::`).

## Generated data

`src/glyph.cpp` (+ `include/glyph/glyph.hpp`) and `src/emoji-properties.cpp` are
generated offline from the Unicode Character Database, CLDR annotations and
emoji-test/emoji-data, then committed. `emoji-properties.cpp` holds the emoji
binary-property ranges (Emoji, Emoji_Presentation, Emoji_Modifier,
Emoji_Modifier_Base) feeding `lookupProperties` in `src/emoji-properties.hpp`,
keeping the lib free of any Unicode-property runtime dependency.

## Emoji segmentation

Extensions may pass emoji strings as icons, so we need to tell whether an
arbitrary string is an emoji. Matching against the table is not enough — it
doesn't cover every sequence and variation. We use the
[google emoji segmenter](https://github.com/google/emoji-segmenter).
