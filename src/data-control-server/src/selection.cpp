#include "selection.hpp"
#include <glaze/base64/base64.hpp>

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
        const int currentPriority =
            -std::distance(preferredImageTypes.begin(), std::ranges::find(preferredImageTypes, mime));
        const int savedPriority = -std::distance(preferredImageTypes.begin(),
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

wlrclip::ClipboardSelection buildSelection(const std::set<std::string> &filteredMimes, OfferReceiver &offer) {
  wlrclip::ClipboardSelection selection;
  selection.offers.reserve(filteredMimes.size());

  for (const auto &mime : filteredMimes) {
    std::string data;
    if (!Wayland::isFlagMime(mime)) { data = glz::write_base64(offer.receive(mime)); }
    selection.offers.emplace_back(wlrclip::ClipboardOffer{.mime_type = mime, .data = std::move(data)});
  }

  return selection;
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

}; // namespace Selection
