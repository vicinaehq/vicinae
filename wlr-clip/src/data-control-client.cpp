#include "data-control-client.hpp"
#include "seat.hpp"
#include "display.hpp"
#include "utils.hpp"
#include "wayland-wlr-data-control-client-protocol.h"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdexcept>

/* DataOffer */
DataControlManager::DataDevice::DataOffer::DataOffer(zwlr_data_control_offer_v1 *offer) : _offer(offer) {
  zwlr_data_control_offer_v1_add_listener(offer, &_listener, this);
}

const std::vector<std::string> &DataControlManager::DataDevice::DataOffer::mimes() const { return _mimes; }

void DataControlManager::DataDevice::DataOffer::offer(void *data, zwlr_data_control_offer_v1 *offer,
                                                      const char *mime) {
  auto self = static_cast<DataOffer *>(data);

  self->_mimes.push_back(mime);
}

std::filesystem::path
DataControlManager::DataDevice::DataOffer::DataOffer::receive(const WaylandDisplay &display,
                                                              const std::string &mime) {
  int pipefd[2];
  auto filePath = std::filesystem::path("/tmp") / ("omni-wlr-clip-" + generateFileId());
  std::ofstream file(filePath);

  if (!file.is_open()) {
    throw std::runtime_error(std::string("Failed to open file at ") + filePath.c_str());
  }

  if (pipe(pipefd) == -1) { throw std::runtime_error(std::string("Failed to pipe(): ") + strerror(errno)); }

  zwlr_data_control_offer_v1_receive(_offer, mime.c_str(), pipefd[1]);
  // Important, otherwise we will block on read forever
  display.flush();
  close(pipefd[1]);

  int rc = 0;

  while ((rc = read(pipefd[0], _buf, sizeof(_buf))) > 0) {
    file.write(_buf, rc);
  }

  if (rc == -1) { perror("failed to read read end of the pipe"); }

  close(pipefd[0]);

  return filePath;
}

DataControlManager::DataDevice::DataOffer::~DataOffer() {
  if (_offer) { zwlr_data_control_offer_v1_destroy(_offer); }
}

void DataControlManager::DataDevice::dataOffer(void *data, zwlr_data_control_device_v1 *device,
                                               zwlr_data_control_offer_v1 *id) {
  auto self = static_cast<DataDevice *>(data);

  self->_offer = std::make_unique<DataOffer>(id);

  for (auto lstn : self->_listeners) {
    lstn->dataOffer(*self, *self->_offer);
  }
}

void DataControlManager::DataDevice::selection(void *data, zwlr_data_control_device_v1 *device,
                                               zwlr_data_control_offer_v1 *id) {
  auto self = static_cast<DataDevice *>(data);

  if (!self->_offer) { return; }

  for (auto lstn : self->_listeners) {
    lstn->selection(*self, *self->_offer);
  }
}

void DataControlManager::DataDevice::finished(void *data, zwlr_data_control_device_v1 *device) {
  auto self = static_cast<DataDevice *>(data);

  for (auto lstn : self->_listeners) {
    lstn->finished(*self);
  }
}

void DataControlManager::DataDevice::primarySelection(void *data, zwlr_data_control_device_v1 *device,
                                                      zwlr_data_control_offer_v1 *id) {
  auto self = static_cast<DataDevice *>(data);

  for (auto lstn : self->_listeners) {
    lstn->primarySelection(*self, *self->_offer);
  }
}

DataControlManager::DataDevice::DataDevice(zwlr_data_control_device_v1 *dev) : _dev(dev) {
  zwlr_data_control_device_v1_add_listener(_dev, &_listener, this);
}

/*
void DataControlManager::DataDevice::DataDevice::registerListener(Listener *listener) {
  _listeners.push_back(listener);
}
*/

DataControlManager::DataDevice::~DataDevice() { zwlr_data_control_device_v1_destroy(_dev); }

DataControlManager::DataControlManager(zwlr_data_control_manager_v1 *manager) : _manager(manager) {}

std::unique_ptr<DataControlManager::DataDevice> DataControlManager::getDataDevice(const WaylandSeat &seat) {
  return std::make_unique<DataDevice>(zwlr_data_control_manager_v1_get_data_device(_manager, seat.data()));
}
