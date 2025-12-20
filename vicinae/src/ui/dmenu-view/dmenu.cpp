#include "dmenu.hpp"

namespace DMenu {
Payload Payload::fromProto(const proto::ext::daemon::DmenuRequest &req) {
  auto nullable = [](const std::string &str) -> std::optional<std::string> {
    if (str.empty()) return std::nullopt;
    return str;
  };
  Payload payload;
  payload.raw = req.raw_content();
  payload.placeholder = nullable(req.placeholder());
  payload.sectionTitle = nullable(req.section_title());
  payload.navigationTitle = nullable(req.navigation_title());
  payload.query = nullable(req.query());
  payload.noSection = req.no_section();
  payload.noQuickLook = req.no_quick_look();
  payload.noMetadata = req.no_metadata();
  payload.noFooter = req.no_footer();
  if (req.has_width()) payload.width = req.width();
  if (req.has_height()) payload.height = req.height();
  return payload;
}

proto::ext::daemon::DmenuRequest Payload::toProto() const {
  proto::ext::daemon::DmenuRequest req;
  req.set_raw_content(raw);
  req.set_query(query.value_or(""));
  req.set_placeholder(placeholder.value_or(""));
  req.set_navigation_title(navigationTitle.value_or(""));
  req.set_section_title(sectionTitle.value_or(""));
  req.set_no_section(noSection);
  req.set_no_quick_look(noQuickLook);
  req.set_no_metadata(noMetadata);
  req.set_no_footer(noFooter);
  if (width) req.set_width(*width);
  if (height) req.set_height(*height);
  return req;
}
}; // namespace DMenu
