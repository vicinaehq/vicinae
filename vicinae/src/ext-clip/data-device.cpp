#include "data-device.hpp"
#include "data-offer.hpp"

void ExtDataDevice::dataOffer(void *data, ext_data_control_device_v1 *device, ext_data_control_offer_v1 *id) {
  auto self = static_cast<ExtDataDevice *>(data);

  if (!id) {
    self->m_offer.reset();
    return;
  }

  auto offer = std::make_unique<ExtDataOffer>(id);

  for (auto lstn : self->_listeners) {
    lstn->dataOffer(*self, *offer);
  }

  self->m_offer = std::move(offer);
}

void ExtDataDevice::selection(void *data, ext_data_control_device_v1 *device, ext_data_control_offer_v1 *id) {
  auto self = static_cast<ExtDataDevice *>(data);

  if (!self->m_offer) return;

  for (auto lstn : self->_listeners) {
    lstn->selection(*self, *self->m_offer);
  }
}

void ExtDataDevice::finished(void *data, ext_data_control_device_v1 *device) {
  auto self = static_cast<ExtDataDevice *>(data);

  for (auto lstn : self->_listeners) {
    lstn->finished(*self);
  }
}

void ExtDataDevice::primarySelection(void *data, ext_data_control_device_v1 *device,
                                  ext_data_control_offer_v1 *id) {
  auto self = static_cast<ExtDataDevice *>(data);

  if (!self->m_offer) return;

  for (auto lstn : self->_listeners) {
    lstn->primarySelection(*self, *self->m_offer);
  }
}

ExtDataDevice::ExtDataDevice(ext_data_control_device_v1 *dev) : _dev(dev) {
  ext_data_control_device_v1_add_listener(_dev, &_listener, this);
}

ExtDataDevice::~ExtDataDevice() { ext_data_control_device_v1_destroy(_dev); }
