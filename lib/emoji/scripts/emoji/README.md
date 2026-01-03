# emoji builder

This script is meant to be ran from time to time and update the static emoji database. 
It's _not_ done on per build basis. 
The files produced are commited inside the git repository as any other source file.

This script encodes a static list of emojis inside an `emoji.cpp` file.
The list is exposed to the application using the static method `StaticEmojiList::orderedList`.

This is used to provide the builtin emoji picking capability and also provide some degree of emoji recognition, using the additional mapping created by the script.

## Usage

```
npm install
npm run build
node dist/main.js
```

The node script will produce two files: `dist/emoji.hpp` and `dist/emoji.cpp`.
