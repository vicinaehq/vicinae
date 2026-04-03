#pragma once
#include <iostream>
#include <iomanip>
#include <ranges>
#include <set>
#include <string>
#include <vector>
#include <optional>
#include <unistd.h>
#include "wayland/mime.hpp"
#include "generated/wlr-clipboard.hpp"

class OfferReceiver {
public:
  virtual ~OfferReceiver() = default;
  virtual const std::vector<std::string> &mimes() const = 0;
  virtual std::string receive(const std::string &mime) = 0;
};

namespace Selection {

inline const std::vector<std::string_view> preferredImageTypes = {"image/gif", "image/png", "image/jpeg",
                                                                  "image/jpg", "image/webp"};

std::set<std::string> filterMimes(const std::vector<std::string> &offerMimes);
wlrclip::ClipboardSelection buildSelection(const std::set<std::string> &filteredMimes, OfferReceiver &offer);
void printDebug(const std::set<std::string> &filteredMimes, OfferReceiver &offer, const char *label);
void printPrimarySelectionDebug(OfferReceiver &offer);

}; // namespace Selection
