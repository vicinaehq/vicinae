#pragma once
#include <QString>
#include <qlist.h>

struct Command {
  QString name;
  QString iconName;
  QString category;
  bool usableWith;
  QString normalizedName;
};

struct CommandDatabase {
  QList<Command> commands;

  CommandDatabase() {
    commands.push_back(
        Command{"Search Files", "search", "File Search", true, "search files"});
    commands.push_back(
        Command{"Search Google", "firefox", "Firefox", true, "search google"});
    commands.push_back(Command{"Next workspace", ":/assets/icons/hyprland.svg",
                               "Hyprland", false, "next workspace"});
    commands.push_back(Command{"Focus left window",
                               ":/assets/icons/hyprland.svg", "Hyprland", false,
                               "focus left window"});
    commands.push_back(Command{"Focus right window",
                               ":/assets/icons/hyprland.svg", "Hyprland", false,
                               "focus right window"});
    commands.push_back(Command{"Next workspace", ":/assets/icons/sway.svg",
                               "Sway", false, "next workspace"});
    commands.push_back(Command{"Focus left window", ":/assets/icons/sway.svg",
                               "Sway", false, "focus left window"});
    commands.push_back(Command{"Focus right window", ":/assets/icons/sway.svg",
                               "Sway", false, "focus right window"});
  }
};
