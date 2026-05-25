#pragma once
#include <qobject.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <QMimeData>
#include <QClipboard>
#include <QGuiApplication>
#include <vector>

namespace Clipboard {
struct CopyOptions {
  bool concealed = false;
  /**
   * Bundle identifier (or platform-equivalent) of the application this content originally
   * came from. Used by macOS to set org.nspasteboard.source. If unset, the platform server
   * defaults to its own identity (we wrote it).
   */
  std::optional<QString> sourceApp;
};
} // namespace Clipboard

struct ClipboardDataOffer {
  QString mimeType;

  /**
   * The selection content.
   * If the offer is an image, this is the entire image data.
   */
  QByteArray data;
};

/**
 * A clipboard selection, made of an arbitrary number of offers.
 * If the list of offers contains the same mime type multiple times, only the
 * first one is considered.
 * This also means you don't need to worry about deduping yourself.
 */
struct ClipboardSelection {
  std::vector<ClipboardDataOffer> offers;
  /**
   * The id of the application that added the selection.
   * Some servers can't know this for security reasons.
   */
  std::optional<QString> sourceApp;
  /**
   * True if the platform detected a password-manager marker on this selection.
   * Service applies the user's "ignore passwords" preference based on this.
   * Selections marked as concealed/transient/auto-generated are silently dropped
   * by the platform server and never reach this point.
   */
  bool isPassword = false;
};

class AbstractClipboardServer : public QObject {
  Q_OBJECT

public:
  /**
   * Called when the clipboard server is started.
   * The server is started only after it has been selected as the suitable one for the current environment:
   * that is; if `isActivatable` returned `true`.
   */
  virtual bool start() = 0;

  virtual QString id() const = 0;

  /**
   * Simple healthcheck, return true if you are not able to implement this
   */
  virtual bool isAlive() const = 0;

  /**
   * If multiple servers are activatable for the same environment, the one with the highest priority
   * will be started.
   */
  virtual int activationPriority() const { return 1; }

  /**
   * Called to decide whether this server is suitable to run in the current environment.
   * You should make this as specific as you can.
   */
  virtual bool isActivatable() const = 0;

  /**
   * Called when clipboard monitoring is disabled.
   * Servers may keep their process alive if needed for other purposes (e.g. clipboard writes).
   */
  virtual bool stop() { return true; }

  /**
   * Write the given QMimeData to the clipboard. When options.concealed is set, the platform
   * server is responsible for attaching the platform-native concealment marker so the write
   * is filtered on the next read (by us and ideally by other clipboard managers).
   * Default implementation uses Qt's clipboard and ignores options (suitable only for platforms
   * that don't need an out-of-process write path).
   */
  virtual bool setClipboardContent(QMimeData *data, const Clipboard::CopyOptions &options = {}) {
    Q_UNUSED(options);
    QGuiApplication::clipboard()->setMimeData(data);
    return true;
  }

  AbstractClipboardServer() {}

signals:
  void selectionAdded(const ClipboardSelection &selection);
};
