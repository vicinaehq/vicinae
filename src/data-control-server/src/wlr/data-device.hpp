#pragma once
#include "data-offer.hpp"
#include "wlr-data-control-unstable-v1-client-protocol.h"

class WlrDataDevice {
public:
  class Listener {
  public:
    virtual void dataOffer(WlrDataDevice &device, WlrDataOffer &offer) {}
    virtual void selection(WlrDataDevice &device, WlrDataOffer &offer) {}
    virtual void finished(WlrDataDevice &device) {}
    virtual void primarySelection(WlrDataDevice &device, WlrDataOffer &offer) {}
  };

  void registerListener(Listener *listener) { _listeners.push_back(listener); }

  WlrDataDevice(zwlr_data_control_device_v1 *dev);
  ~WlrDataDevice();

private:
  zwlr_data_control_device_v1 *_dev;
  std::vector<Listener *> _listeners;
  std::unique_ptr<WlrDataOffer> m_offer;

  static void dataOffer(void *data, zwlr_data_control_device_v1 *device, zwlr_data_control_offer_v1 *id);
  static void selection(void *data, zwlr_data_control_device_v1 *device, zwlr_data_control_offer_v1 *id);
  static void finished(void *data, zwlr_data_control_device_v1 *device);
  static void primarySelection(void *data, zwlr_data_control_device_v1 *device,
                               zwlr_data_control_offer_v1 *id);

  constexpr static const struct zwlr_data_control_device_v1_listener _listener = {.data_offer = dataOffer,
                                                                                  .selection = selection,
                                                                                  .finished = finished,
                                                                                  .primary_selection =
                                                                                      primarySelection};
};
