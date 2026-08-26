#include "file-chooser.hpp"

#ifdef Q_OS_LINUX
#include "xdp-file-chooser/xdp-file-chooser.hpp"
#include <cstdlib>
#elif defined(Q_OS_MACOS)
#include "macos/mac-file-chooser.hpp"
#endif

AbstractFileChooser *createPlatformFileChooser([[maybe_unused]] QObject *parent) {
#ifdef Q_OS_LINUX
  if (std::getenv("VICINAE_FORCE_QT_DIALOG") != nullptr) return nullptr;
  return new XdpFileChooser(parent);
#elif defined(Q_OS_MACOS)
  return new MacFileChooser(parent);
#else
  return nullptr;
#endif
}
