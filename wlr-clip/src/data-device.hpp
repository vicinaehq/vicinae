#pragma once
#include "data-offer.hpp"
#include "wlr-data-control-unstable-v1-client-protocol.h"

class DataDevice {
public:
  class Listener {
  public:
    virtual void dataOffer(DataDevice &device, DataOffer &offer) {}
    virtual void selection(DataDevice &device, DataOffer &offer) {}
    virtual void finished(DataDevice &device) {}
    virtual void primarySelection(DataDevice &device, DataOffer &offer) {}
  };

  void registerListener(Listener *listener) { _listeners.push_back(listener); }

  DataDevice(zwlr_data_control_device_v1 *dev);
  ~DataDevice();

private:
  zwlr_data_control_device_v1 *_dev;
  std::vector<Listener *> _listeners;
  std::unique_ptr<DataOffer> m_offer;

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
