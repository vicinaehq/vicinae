# libemoji

`libemoji` is a very simple library providing a simple API to work with emojis. It is used by vicinae to provide emoji picking capabilities and decide when to render emoji icons.

## Dependencies

- [ICU](https://github.com/unicode-org/icu). Already required by Qt and many other packages, should be installed on most systems by default.

## Static emoji database

Due to how emojis and unicode work, there is no good way to enumerate emojis. In order to provide emoji picking, the most reliable way is to generate a static list of emojis and use that.
This is what the stuff in `scripts` does. The list is regenerated from time to time and is directly bundled with the library. This is automated through the use of the `make gen-db` command.

## Emoji segmentation

The vicinae extension ecosystem allows extensions to make use of regular emoji strings as valid icons. For that to work reliably, we need a way to identify whether a given string is an emoji or something else entirely.

Matching against the static database is not a viable option because it doesn't handle all the emoji variations, of which there are a ton. See [this article](https://www.qt.io/blog/emoji-in-qt-6.9) for a nice overview of how this stuff works.

This library uses the [google emoji segmenter](https://github.com/google/emoji-segmenter) to achieve that.
