#include "action.hpp"
#include "exec.hpp"

namespace xdgpp {
std::string DesktopEntryAction::id() const { return m_id; }

std::string DesktopEntryAction::name() const { return m_name; }
std::optional<std::string> DesktopEntryAction::icon() const { return m_icon; }
std::optional<std::string> DesktopEntryAction::exec() const { return m_exec; }

std::vector<std::string> DesktopEntryAction::parseExec(const std::vector<std::string> &uris, bool forceAppend,
                                                       const std::optional<std::string> &launchPrefix) const {
  if (!m_exec) return {};

  ExecParser parser(m_name);

  parser.setForceAppend(forceAppend);

  if (m_icon) { parser.setIcon(*m_icon); }

  std::string input;

  if (launchPrefix) { input = launchPrefix.value() + ' '; }
  input += m_exec.value();

  return parser.parse(input, uris);
}

DesktopEntryAction xdgpp::DesktopEntryAction::fromGroup(const DesktopEntryGroup &group) {
  return DesktopEntryAction(group);
}

DesktopEntryAction::DesktopEntryAction(const DesktopEntryGroup &group) {
  std::string prefix = "Desktop Action ";

  if (group.name().starts_with(prefix)) { m_id = group.name().substr(prefix.size()); }
  if (auto name = group.key("Name")) { m_name = name->asString(); }
  if (auto icon = group.key("Icon")) { m_icon = icon->asString(); }
  if (auto exec = group.key("Exec")) { m_exec = exec->asString(); }
}
}; // namespace xdgpp
