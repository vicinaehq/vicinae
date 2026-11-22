#pragma once

#include <QCoreApplication>
#include <optional>
#include <string>
#include <vector>
#include "command/preference-schema.hpp"

namespace Notes {
struct Preferences {
  std::optional<std::string> defaultDirectory;
  std::vector<std::string> directories;
};
} // namespace Notes

template <> struct PreferenceSchema<Notes::Preferences> {
  Q_DECLARE_TR_FUNCTIONS(NotesPreferences)
public:
  PreferenceMeta defaultDirectory{
      .title = tr("Default directory"),
      .description = tr("Save new notes as Markdown files in this directory. It is also included in search. "
                        "Choose a writable directory, or leave unset to use Vicinae's notes directory."),
      .kind = PreferenceMeta::Kind::Directories,
      .required = false,
  };
  PreferenceMeta directories{
      .title = tr("Additional directories"),
      .description = tr("Search Markdown files in these directories and their subdirectories, including "
                        "read-only notes. Removing a directory from this list keeps its files."),
      .kind = PreferenceMeta::Kind::Directories,
      .required = false,
  };
};
