#pragma once
#include "../locale/locale.hpp"
#include "action.hpp"
#include "reader.hpp"
#include <filesystem>
#include <optional>
#include <vector>

namespace xdgpp {
class DesktopEntry {
public:
  struct ParseOptions {

    std::string id;
    std::optional<Locale> locale;
  };

  struct TerminalExec {
    std::optional<std::string> exec;
    std::optional<std::string> appId;
    std::optional<std::string> title;
    std::optional<std::string> dir;
    std::optional<std::string> hold;
  };

  static DesktopEntry fromFile(const std::filesystem::path &path, const ParseOptions &opts = {});
  static DesktopEntry fromData(std::string_view data, const ParseOptions &opts = {});

  enum class Type { Application = 1, Link, Directory };

  /**
   * Id that uniquely identifies the entry according
   * to the specification.
   * If the desktop entry is not constructed with the id set explicitly
   * (directly from data) then this will an empty string.
   */
  const std::string &id() const;

  /**
   * This specification defines 3 types of desktop entries:
   * Application (type 1), Link (type 2) and Directory (type 3).
   * To allow the addition of new types in the future, implementations should
   * ignore desktop entries with an unknown type.
   * We provide `isApplication`, `isLink` and `isDirectory` for convenience.
   */
  Type type() const;

  bool isApplication() const;
  bool isLink() const;
  bool isDirectory() const;

  /**
   * Version of the Desktop Entry Specification that the desktop entry conforms
   * with. Note that the version field is not required to be present.
   */
  std::optional<std::string> version() const;

  /**
   * Specific name of the application, for example "Mozilla".
   * This always return the non-localized version of the name.
   * Use `localizedName` to get the localized name, if available.
   */
  std::string name() const;

  /**
   * Generic name of the application, for example "Web Browser".
   */
  std::optional<std::string> genericName() const;

  /**
   * NoDisplay means "this application exists, but don't display it
   * in the menus".
   * This can be useful to e.g. associate this application with
   * MIME types, so that it gets launched from a file manager (or other apps),
   * without having a menu entry for it .
   */
  bool noDisplay() const;

  std::optional<std::string> url() const;

  /**
   * Tooltip for the entry, for example "View sites on the Internet".
   * The value should not be redundant with the values of Name and GenericName.
   */
  std::optional<std::string> comment() const;

  /**
   * Icon to display in file manager, menus, etc. If the name is an absolute
   * path, the given file will be used. If the name is not an absolute path, the
   * algorithm described in the Icon Theme Specification will be used to locate
   * the icon.
   */
  std::optional<std::string> icon() const;

  /**
   * Maps to the `Hidden` key, renamed for clarity.
   *
   * The user deleted (at their level) something that was present (at
   * an upper level, e.g. in the system dirs).
   * It's strictly equivalent to the .desktop file not existing at all,
   * as far as that user is concerned.
   * This can also be used to "uninstall" existing files (e.g. due to a
   * renaming) - by letting make install install a file with Hidden=true in it.
   */
  bool deleted() const;

  /**
   * Program to execute, possibly with arguments.
   * This is a single string value escaped according to regular string parsing rules.
   * If you are looking for the fully parsed exec argument vector, use `parseExec`.
   */
  std::optional<std::string> exec() const;

  /**
   * Maps to the `Path` key, we changed the name of this one to make it clearer.
   */
  std::optional<std::filesystem::path> workingDirectory() const;

  bool terminal() const;

  /**
   * Path to an executable file on disk used to determine if the program is actually installed.
   * If the path is not an absolute path, the file is looked up in the $PATH environment variable.
   * If the file is not present or if it is not executable,
   * the entry may be ignored (not be used in menus, for example)
   */
  std::optional<std::string_view> tryExec() const;

  std::vector<std::string> parseExec() const;

  std::vector<std::string> parseExec(const std::vector<std::string> &uris, bool forceAppend = false,
                                     const std::optional<std::string> &launchPrefix = {}) const;

  const std::vector<std::string> &onlyShowIn() const;

  const std::vector<std::string> &notShowIn() const;

  bool singleMainWindow() const;

  /**
   * Utility method to know whether the entry should be shown in the current
   * environment we are in. This checks the 'Hidden', 'NoDisplay', 'ShowIn' and 'NotShowIn' keys
   * all at once.
   */
  bool shouldBeShownInCurrentContext() const;

  /**
   * If specified, it is known that the application will map at least one window
   * with the given string as its WM class or WM name hint (see the Startup
   * Notification Protocol Specification for more details).
   */
  std::optional<std::string> startupWMClass() const;

  /**
   * The MIME type(s) supported by this application.
   */
  const std::vector<std::string> &mimes() const;

  const std::vector<std::string> &categories() const;

  bool hasCategory(std::string_view category) const;

  bool supportsMime(std::string_view mime) const;

  /**
   * A list of strings which may be used in addition to other metadata to
   * describe this entry. This can be useful e.g. to facilitate searching
   * through entries. The values are not meant for display, and should not be
   * redundant with the values of Name or GenericName.
   */
  const std::vector<std::string> &keywords() const;

  const std::vector<DesktopEntryAction> &actions() const;

  std::optional<std::string> errorMessage() const;
  bool isValid() const;

  // https://gitlab.freedesktop.org/terminal-wg/specifications/-/merge_requests/3/diffs
  // Only available if app has the TerminalEmulator category
  const std::optional<TerminalExec> &terminalExec() const;

private:
  DesktopEntry(std::string_view data, const ParseOptions &opts = {});
  DesktopEntry(const std::filesystem::path &path, const ParseOptions &opts = {});

  std::optional<std::string> m_error;
  Type m_type;
  std::string m_version;
  std::string m_name;
  std::string m_genericName;
  bool m_noDisplay = false;
  std::optional<std::string> m_comment;
  bool m_hidden = false;
  bool m_terminal = false;
  bool m_singleMainWindow = false;
  std::optional<std::string> m_icon;
  std::optional<std::string> m_exec;
  std::optional<std::string> m_tryExec;
  std::optional<std::string> m_wmClass;
  std::vector<std::string> m_mimes;
  std::vector<std::string> m_categories;
  std::vector<std::string> m_keywords;
  std::vector<std::string> m_onlyShowIn;
  std::vector<std::string> m_notShowIn;
  std::optional<std::filesystem::path> m_path;
  std::vector<DesktopEntryAction> m_actions;
  std::optional<std::string> m_url;
  std::optional<TerminalExec> m_terminalExec;
};

}; // namespace xdgpp

std::ostream &operator<<(std::ostream &os, const xdgpp::DesktopEntry &entry);
