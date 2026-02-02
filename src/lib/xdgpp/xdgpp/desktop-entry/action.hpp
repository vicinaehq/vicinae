#pragma once
#include <optional>
#include <string>
#include <vector>
#include "group.hpp"

namespace xdgpp {

/**
 * A desktop action belonging to a given desktop entry.
 * This class owns its own copy of the data and its lifetime is not actually tied
 * to its parent entry, so you can copy it freely.
 */
class DesktopEntryAction {
public:
  static DesktopEntryAction fromGroup(const DesktopEntryGroup &group);

  std::string id() const;

  std::string name() const;
  std::optional<std::string> icon() const;
  std::optional<std::string> exec() const;
  std::vector<std::string> parseExec(const std::vector<std::string> &s = {}, bool forceAppend = false,
                                     const std::optional<std::string> &launchPrefix = {}) const;

private:
  DesktopEntryAction(const DesktopEntryGroup &group);

  std::string m_id;
  std::string m_name;
  std::optional<std::string> m_icon;
  std::optional<std::string> m_exec;
};
}; // namespace xdgpp
