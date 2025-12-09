#pragma once
#include "proto/daemon.pb.h"

namespace DMenu {
struct Payload {
  std::string raw;
  std::optional<std::string> navigationTitle;
  std::optional<std::string> placeholder;
  std::optional<std::string> sectionTitle;
  std::optional<std::string> query;
  std::optional<int> width;
  std::optional<int> height;
  bool noSection = false;
  bool noQuickLook = false;
  bool noMetadata = false;
  bool noFooter = false;

  static Payload fromProto(const proto::ext::daemon::DmenuRequest &req);
  proto::ext::daemon::DmenuRequest toProto() const;
};
}; // namespace DMenu
