#include "data-device.hpp"
#include <iostream>

void DataDevice::dataOffer(void *data, zwlr_data_control_device_v1 *device, zwlr_data_control_offer_v1 *id) {
  auto self = static_cast<DataDevice *>(data);
  auto offer = std::make_unique<DataOffer>(id);

  for (auto lstn : self->_listeners) {
    lstn->dataOffer(*self, *offer);
  }

  self->m_pendingOffer = std::move(offer);
}

void DataDevice::selection(void *data, zwlr_data_control_device_v1 *device, zwlr_data_control_offer_v1 *id) {
  auto self = static_cast<DataDevice *>(data);

  // safety debug check, this should normally never happen
  if (id != self->m_pendingOffer->pointer()) {
    std::cerr << "[Warning] DataDevice::selection: offer id in selection handler does not match "
                 "pending offer's";
  }

  // will destroy the previous offer (as requested by the protocol)
  self->m_offer = std::move(self->m_pendingOffer);

  for (auto lstn : self->_listeners) {
    lstn->selection(*self, *self->m_offer);
  }
}

void DataDevice::finished(void *data, zwlr_data_control_device_v1 *device) {
  auto self = static_cast<DataDevice *>(data);

  for (auto lstn : self->_listeners) {
    lstn->finished(*self);
  }
}

void DataDevice::primarySelection(void *data, zwlr_data_control_device_v1 *device,
                                  zwlr_data_control_offer_v1 *id) {
  auto self = static_cast<DataDevice *>(data);
  // we don't use primary selection
}

DataDevice::DataDevice(zwlr_data_control_device_v1 *dev) : _dev(dev) {
  zwlr_data_control_device_v1_add_listener(_dev, &_listener, this);
}

/*
void DataDevice::DataDevice::registerListener(Listener *listener) {
  _listeners.push_back(listener);
}
*/

DataDevice::~DataDevice() { zwlr_data_control_device_v1_destroy(_dev); }
