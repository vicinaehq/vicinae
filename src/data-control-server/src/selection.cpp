#include "selection.hpp"
#include <algorithm>

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

clipboard_proto::Selection buildSelection(const std::set<std::string> &filteredMimes, OfferReceiver &offer) {
  clipboard_proto::Selection selection;
  selection.offers.reserve(filteredMimes.size());

  for (const auto &mime : filteredMimes) {
    std::vector<uint8_t> data;
    if (!Wayland::isFlagMime(mime)) {
      auto raw = offer.receive(mime);
      data.assign(raw.begin(), raw.end());
    }
    selection.offers.emplace_back(clipboard_proto::Offer{.mime_type = mime, .data = std::move(data)});
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

clipboard_proto::Selection buildPrimarySelection(OfferReceiver &offer) {
  constexpr size_t maxSize = 1 << 20;
  const auto &mimes = offer.mimes();
  clipboard_proto::Selection selection;

  if (std::ranges::find(mimes, Clipboard::CONCEALED_MIME_TYPE) != mimes.end()) return selection;

  for (std::string_view mime : {"text/plain;charset=utf-8", "text/plain"}) {
    if (std::ranges::find(mimes, mime) == mimes.end()) continue;
    auto raw = offer.receive(std::string(mime));
    if (raw.size() > maxSize) return selection;
    selection.offers.emplace_back(
        clipboard_proto::Offer{.mime_type = std::string(mime), .data = {raw.begin(), raw.end()}});
    break;
  }

  return selection;
}

}; // namespace Selection
