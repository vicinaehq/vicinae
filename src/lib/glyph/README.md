# libglyph

A static table of emoji and curated Unicode symbols, plus emoji-specific helpers.

- `glyph/glyph.hpp` — the table and lookups (`glyph::`).
- `glyph/emoji.hpp` — emoji-only helpers: skin tones, segmentation, detection (`emoji::`).

## Dependencies

- ICU (`uc` component).

## Glyph table

`src/glyph.cpp` and `include/glyph/glyph.hpp` are generated offline from the
Unicode Character Database, CLDR annotations and emoji-test data, then committed.

## Emoji segmentation

Extensions may pass emoji strings as icons, so we need to tell whether an
arbitrary string is an emoji. Matching against the table is not enough — it
doesn't cover every sequence and variation. We use the
[google emoji segmenter](https://github.com/google/emoji-segmenter).
