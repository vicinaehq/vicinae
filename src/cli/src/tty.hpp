#pragma once
#include <cstdio>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#else
#include <unistd.h>
#endif

namespace cli {

inline bool isTerminal(std::FILE *stream) {
#ifdef _WIN32
  return _isatty(_fileno(stream)) != 0;
#else
  return isatty(fileno(stream)) != 0;
#endif
}

// Windows text mode translates line endings and stops at Ctrl-Z, which mangles piped input
inline void setBinaryMode(std::FILE *stream) {
#ifdef _WIN32
  _setmode(_fileno(stream), _O_BINARY);
#else
  (void)stream;
#endif
}

} // namespace cli
