#pragma once
#include <qobject.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <QMimeData>
#include <QClipboard>
#include <QApplication>
#include <vector>

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
   * Sent when clipboard monitoring is disabled.
   * Ideally, you should close all open connections and/or terminate monitoring processes
   * upon receiving this.
   *
   * It is guaranteed that `start` has been called at least once before this.
   *
   * The method returns whether the server could be stopped successfully. This is only used for
   * reporting and no retry will be attempted.
   */
  virtual bool stop() { return true; }

  /**
   * Set clipboard content synchronously.
   * Default implementation uses Qt's clipboard.
   */
  virtual bool setClipboardContent(QMimeData *data) {
    QApplication::clipboard()->setMimeData(data);
    return true;
  }

  AbstractClipboardServer() {}

signals:
  void selectionAdded(const ClipboardSelection &selection);
};
