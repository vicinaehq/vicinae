#pragma once
#include "data-offer.hpp"
#include "ext-data-control-v1-client-protocol.h"

class ExtDataDevice {
public:
  class Listener {
  public:
    virtual void dataOffer(ExtDataDevice &device, ExtDataOffer &offer) {}
    virtual void selection(ExtDataDevice &device, ExtDataOffer &offer) {}
    virtual void finished(ExtDataDevice &device) {}
    virtual void primarySelection(ExtDataDevice &device, ExtDataOffer &offer) {}
  };

  void registerListener(Listener *listener) { _listeners.push_back(listener); }

  ExtDataDevice(ext_data_control_device_v1 *dev);
  ~ExtDataDevice();

private:
  ext_data_control_device_v1 *_dev;
  std::vector<Listener *> _listeners;
  std::unique_ptr<ExtDataOffer> m_offer;

  static void dataOffer(void *data, ext_data_control_device_v1 *device, ext_data_control_offer_v1 *id);
  static void selection(void *data, ext_data_control_device_v1 *device, ext_data_control_offer_v1 *id);
  static void finished(void *data, ext_data_control_device_v1 *device);
  static void primarySelection(void *data, ext_data_control_device_v1 *device,
                               ext_data_control_offer_v1 *id);

  constexpr static const struct ext_data_control_device_v1_listener _listener = {.data_offer = dataOffer,
                                                                                  .selection = selection,
                                                                                  .finished = finished,
                                                                                  .primary_selection =
                                                                                      primarySelection};
};
