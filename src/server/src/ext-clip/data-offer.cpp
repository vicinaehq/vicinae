#include "data-offer.hpp"
#include "app.hpp"
#include <cstring>

ExtDataOffer::ExtDataOffer(ext_data_control_offer_v1 *offer) : _offer(offer) {
  ext_data_control_offer_v1_add_listener(offer, &_listener, this);
}

const std::vector<std::string> &ExtDataOffer::mimes() const { return _mimes; }

void ExtDataOffer::offer(void *data, ext_data_control_offer_v1 *offer, const char *mime) {
  auto self = static_cast<ExtDataOffer *>(data);

  self->_mimes.push_back(mime);
}

std::string ExtDataOffer::receive(const std::string &mime) {
  std::string data;
  int pipefd[2];

  if (pipe(pipefd) == -1) { throw std::runtime_error(std::string("Failed to pipe(): ") + strerror(errno)); }

  ext_data_control_offer_v1_receive(_offer, mime.c_str(), pipefd[1]);
  // Important, otherwise we will block on read forever
  ExtClipman::instance()->flush();
  close(pipefd[1]);

  int rc = 0;

  while ((rc = read(pipefd[0], _buf, sizeof(_buf))) > 0) {
    data += std::string_view(_buf, rc);
  }

  if (rc == -1) { perror("failed to read read end of the pipe"); }

  close(pipefd[0]);

  return data;
}

ExtDataOffer::~ExtDataOffer() {
  if (_offer) { ext_data_control_offer_v1_destroy(_offer); }
}
