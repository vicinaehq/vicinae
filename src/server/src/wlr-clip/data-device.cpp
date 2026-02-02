#include "data-device.hpp"
#include "data-offer.hpp"

void DataDevice::dataOffer(void *data, zwlr_data_control_device_v1 *device, zwlr_data_control_offer_v1 *id) {
  auto self = static_cast<DataDevice *>(data);

  if (!id) {
    self->m_offer.reset();
    return;
  }

  auto offer = std::make_unique<DataOffer>(id);

  for (auto lstn : self->_listeners) {
    lstn->dataOffer(*self, *offer);
  }

  self->m_offer = std::move(offer);
}

void DataDevice::selection(void *data, zwlr_data_control_device_v1 *device, zwlr_data_control_offer_v1 *id) {
  auto self = static_cast<DataDevice *>(data);

  if (!self->m_offer) return;

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

  if (!self->m_offer) return;

  for (auto lstn : self->_listeners) {
    lstn->primarySelection(*self, *self->m_offer);
  }
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
