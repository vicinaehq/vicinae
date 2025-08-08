#pragma once
#include <qobject.h>
#include <qtmetamacros.h>
#include <string>
#include <filesystem>
#include <vector>

struct ClipboardDataOffer {
  std::string mimeType;
  std::filesystem::path path;
};

struct ClipboardSelection {
  std::vector<ClipboardDataOffer> offers;

  ~ClipboardSelection() {
    for (const auto &offer : offers) {
      std::filesystem::remove(offer.path);
    }
  }
};

enum ClipboardServerType { WlrootsDataControlClipboardServer = 0, InvalidClipboardServer };

class AbstractClipboardServer : public QObject {
  Q_OBJECT

  ClipboardServerType _type;

public:
  virtual bool start() = 0;
  virtual bool isAlive() const = 0;
  ClipboardServerType type() const { return _type; }

  /**
   * Called to figure out whether the clipboard server is able function
   * in the environment omnicast has been started in.
   * This function is called on all clipboard server implementations until one returns true.
   */
  virtual bool isActivatable() const = 0;

  AbstractClipboardServer(ClipboardServerType type) : _type(type) {}

signals:
  void selection(const ClipboardSelection &selection);
};
