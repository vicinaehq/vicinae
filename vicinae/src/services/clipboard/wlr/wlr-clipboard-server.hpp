#pragma once
#include "services/clipboard/clipboard-server.hpp"
#include <QSocketNotifier>
#include <QSharedPointer>
#include <wayland-client.h>
#include <vector>
#include <QString>

struct zwlr_data_control_manager_v1;
struct zwlr_data_control_device_v1;
struct zwlr_data_control_offer_v1;

class WlrClipboardServer : public AbstractClipboardServer {
  Q_OBJECT

public:
  WlrClipboardServer();
  ~WlrClipboardServer() override;

  bool start() override;
  bool stop() override;
  bool isActivatable() const override;
  QString id() const override;
  int activationPriority() const override;
  bool isAlive() const override;

  void onDataOffer(zwlr_data_control_offer_v1 *offer);
  void onSelectionChanged(zwlr_data_control_offer_v1 *offer);
  void onOfferMimeType(zwlr_data_control_offer_v1 *offer, const char *mime_type);
  void setDataControlManager(zwlr_data_control_manager_v1 *manager);

private:
  void qtClipboardChanged();
  static bool isVicinaeFocused();
  struct PendingSelectionState {
    std::vector<QString> mimeTypes;
    ClipboardSelection selection;
    int offerCount = 0;
    int receivedCount = 0;
    std::vector<QSharedPointer<QSocketNotifier>> activeNotifiers;
    QTimer *timeoutTimer = nullptr;

    void clear() {
      mimeTypes.clear();
      selection.offers.clear();
      offerCount = 0;
      receivedCount = 0;

      for (auto &notifier : activeNotifiers) {
        if (notifier) { notifier->setEnabled(false); }
      }
      activeNotifiers.clear();

      if (timeoutTimer) {
        timeoutTimer->stop();
        timeoutTimer->deleteLater();
        timeoutTimer = nullptr;
      }
    }

    void cancelReads() {
      selection.offers.clear();
      offerCount = 0;
      receivedCount = 0;

      for (auto &notifier : activeNotifiers) {
        if (notifier) { notifier->setEnabled(false); }
      }
      activeNotifiers.clear();

      if (timeoutTimer) {
        timeoutTimer->stop();
        timeoutTimer->deleteLater();
        timeoutTimer = nullptr;
      }
    }
  };

  static bool isLegacyContentType(const QString &str);
  void receiveOffersAsync();
  void processSelection();
  void cancelPendingReads();

  wl_display *m_display = nullptr;
  wl_seat *m_seat = nullptr;
  wl_registry *m_registry = nullptr;
  zwlr_data_control_manager_v1 *m_manager = nullptr;
  zwlr_data_control_device_v1 *m_device = nullptr;
  zwlr_data_control_offer_v1 *m_currentOffer = nullptr;
  QSocketNotifier *m_displayNotifier = nullptr;

  PendingSelectionState m_pending;

  static constexpr size_t READ_BUFFER_SIZE = 64 * 1024;
  std::array<char, READ_BUFFER_SIZE> m_readBuffer;
  bool m_started = false;
  bool m_ignoreNextDataControlSelection = false;
  QByteArray m_lastQtClipboardHash;
};
