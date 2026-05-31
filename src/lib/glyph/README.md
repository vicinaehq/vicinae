# libglyph

`libglyph` provides a simple API over a curated, unified table of glyphs — emoji
and notable Unicode symbols alike — plus emoji-specific helpers. It is used by
vicinae to power the glyph picker and to decide when/how to render glyph icons.

The data API lives in the `glyph::` namespace (`glyph/glyph.hpp`); emoji-specific
utilities (skin tones, segmentation, emoji detection) live in `emoji::`
(`glyph/emoji.hpp`).

## Dependencies

- [ICU](https://github.com/unicode-org/icu). Already required by Qt and many other packages, should be installed on most systems by default.

## Static glyph table

Due to how emoji and Unicode work, there is no good way to enumerate the
interesting glyphs at runtime. Instead, the table is generated offline from the
Unicode Character Database, CLDR annotations and emoji-test data, and the
resulting `src/glyph.cpp` / `include/glyph/glyph.hpp` are committed and bundled
directly with the library (zero startup cost — it's all `constexpr` rodata).

The generator is a separate TypeScript project (`gen.ts`); regenerate and copy
its output over `src/glyph.cpp` and `include/glyph/glyph.hpp` when refreshing.

## Emoji segmentation

The vicinae extension ecosystem allows extensions to make use of regular emoji strings as valid icons. For that to work reliably, we need a way to identify whether a given string is an emoji or something else entirely.

Matching against the static table is not a viable option because it doesn't handle all the emoji variations, of which there are a ton. See [this article](https://www.qt.io/blog/emoji-in-qt-6.9) for a nice overview of how this stuff works.

This library uses the [google emoji segmenter](https://github.com/google/emoji-segmenter) to achieve that.
