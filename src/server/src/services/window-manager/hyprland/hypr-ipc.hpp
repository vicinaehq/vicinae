#pragma once
#include <array>
#include <glaze/glaze.hpp>
#include <string>

namespace Hyprland::ipc {

struct WorkspaceRef {
  int id = -1;
};

struct Window {
  std::string address;
  std::string title;
  std::string wmClass;
  WorkspaceRef workspace;
  int pid = 0;
  int focusHistoryId = -1;
  std::array<int, 2> at{};
  std::array<int, 2> size{};
};

struct Workspace {
  int id = -1;
  std::string name;
  bool hasFullScreen = false;
  std::string monitor;
  int monitorId = -1;
};

} // namespace Hyprland::ipc

template <> struct glz::meta<Hyprland::ipc::Window> {
  using T = Hyprland::ipc::Window;
  static constexpr auto value =
      object("address", &T::address, "title", &T::title, "class", &T::wmClass, "workspace", &T::workspace,
             "pid", &T::pid, "focusHistoryID", &T::focusHistoryId, "at", &T::at, "size", &T::size);
};

template <> struct glz::meta<Hyprland::ipc::Workspace> {
  using T = Hyprland::ipc::Workspace;
  static constexpr auto value = object("id", &T::id, "name", &T::name, "hasfullscreen", &T::hasFullScreen,
                                       "monitorID", &T::monitorId, "monitor", &T::monitor);
};
