#include "data-device.hpp"
#include "data-offer.hpp"

void WlrDataDevice::dataOffer(void *data, zwlr_data_control_device_v1 *device, zwlr_data_control_offer_v1 *id) {
  auto self = static_cast<WlrDataDevice *>(data);

  if (!id) {
    self->m_offer.reset();
    return;
  }

  auto offer = std::make_unique<WlrDataOffer>(id);

  for (auto lstn : self->_listeners) {
    lstn->dataOffer(*self, *offer);
  }

  self->m_offer = std::move(offer);
}

void WlrDataDevice::selection(void *data, zwlr_data_control_device_v1 *device, zwlr_data_control_offer_v1 *id) {
  auto self = static_cast<WlrDataDevice *>(data);

  if (!self->m_offer) return;

  for (auto lstn : self->_listeners) {
    lstn->selection(*self, *self->m_offer);
  }
}

void WlrDataDevice::finished(void *data, zwlr_data_control_device_v1 *device) {
  auto self = static_cast<WlrDataDevice *>(data);

  for (auto lstn : self->_listeners) {
    lstn->finished(*self);
  }
}

void WlrDataDevice::primarySelection(void *data, zwlr_data_control_device_v1 *device,
                                  zwlr_data_control_offer_v1 *id) {
  auto self = static_cast<WlrDataDevice *>(data);

  if (!self->m_offer) return;

  for (auto lstn : self->_listeners) {
    lstn->primarySelection(*self, *self->m_offer);
  }
}

WlrDataDevice::WlrDataDevice(zwlr_data_control_device_v1 *dev) : _dev(dev) {
  zwlr_data_control_device_v1_add_listener(_dev, &_listener, this);
}

WlrDataDevice::~WlrDataDevice() { zwlr_data_control_device_v1_destroy(_dev); }
