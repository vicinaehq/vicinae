#include "selection.hpp"
#include <netinet/in.h>

namespace Selection {

std::set<std::string> filterMimes(const std::vector<std::string> &offerMimes) {
  std::set<std::string> filteredMimes;
  auto filter = [](const std::string &mime) {
    return Wayland::isFlagMime(mime) || Wayland::isDataMime(mime);
  };

  std::optional<std::string> savedImageType;

  for (const auto &mime : offerMimes | std::views::filter(filter)) {
    if (mime.starts_with("image/")) {
      if (savedImageType) {
        int currentPriority =
            -std::distance(preferredImageTypes.begin(), std::ranges::find(preferredImageTypes, mime));
        int savedPriority = -std::distance(preferredImageTypes.begin(),
                                           std::ranges::find(preferredImageTypes, *savedImageType));

        if (currentPriority <= savedPriority) continue;

        filteredMimes.erase(*savedImageType);
      }
      savedImageType = mime;
    }
    filteredMimes.insert(mime);
  }

  // utf-8 is always preferrable
  if (filteredMimes.contains("text/plain") && filteredMimes.contains("text/plain;charset=utf-8")) {
    filteredMimes.erase("text/plain");
  }

  return filteredMimes;
}

void writeToStdout(const proto::ext::wlrclip::Selection &selection) {
  std::string data;
  selection.SerializeToString(&data);

  uint32_t size = htonl(data.size());

  std::cout.write(reinterpret_cast<const char *>(&size), sizeof(size));
  std::cout.write(data.data(), data.size());
  std::cout.flush();
}

void printDebug(const std::set<std::string> &filteredMimes, OfferReceiver &offer, const char *label) {
  std::cout << "********** BEGIN " << label << " **********" << std::endl;
  for (const auto &mime : filteredMimes) {
    auto data = offer.receive(mime);
    std::cout << std::left << std::setw(30) << mime << data << std::endl;
  }
  std::cout << "********** END " << label << " **********" << std::endl;
}

void printPrimarySelectionDebug(OfferReceiver &offer) {
  std::cout << "********** BEGIN PRIMARY SELECTION **********" << std::endl;
  for (const auto &mime : offer.mimes()) {
    auto data = offer.receive(mime);
    std::cout << std::left << std::setw(30) << mime << data << std::endl;
  }
  std::cout << "********** END PRIMARY SELECTION **********" << std::endl;
}

void serializeAndWrite(const std::set<std::string> &filteredMimes, OfferReceiver &offer) {
  if (isatty(STDOUT_FILENO)) {
    printDebug(filteredMimes, offer, "SELECTION");
    return;
  }

  proto::ext::wlrclip::Selection selection;

  for (const auto &mime : filteredMimes) {
    auto dataOffer = selection.add_offers();
    dataOffer->set_mime_type(mime);
    if (!Wayland::isFlagMime(mime)) { dataOffer->set_data(offer.receive(mime)); }
  }

  writeToStdout(selection);
}

}; // namespace Selection
