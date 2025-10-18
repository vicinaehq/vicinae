#include "theme.hpp"
#include "cli/theme.hpp"
#include "daemon/ipc-client.hpp"
#include "theme/theme-db.hpp"
#include "theme/theme-parser.hpp"
#include <stdexcept>

static const char *THEME_TEMPLATE = R"(# Example Theme Configuration
# This file demonstrates all available theme configuration options for Vicinae.
# You don't need to define all of these - only override what you want to customize.
# Undefined colors will be automatically derived from core colors or inherited from base themes.
#
# Where a color name is expected, you can use the following hex formats:
# #RGB
# #RRGGBB
# #AARRGGBB (with alpha)
# 'transparent' (no color)
# <color_ref> (e.g 'colors.accents.red'): references this theme's definition if available, the parent's otherwise
#
# Where a color is expected, you can either use a simple string for the color name or the following table syntax:
# [colors.core]
# accent = "accents.color.red"
# OR:
# accent = { name = "accents.color.red", opacity = 0.5 }
# OR:
# accent = { name = "accents.color.red", lighter = 100 }
# OR:
# accent = { name = "accents.color.red", darker = 150 }

[meta]
name = "Vicinae Dark"                         # Display name shown in theme picker
description = "Default Vicinae dark palette"  # Description shown in theme picker
variant = "dark"                              # Theme variant: "dark" or "light". Every dark theme inherits from vicinae-dark and every light theme from vicinae-light. 
inherits = "vicinae-dark"                     # Optional: inherit from another theme (e.g., "vicinae-dark")
icon = "icons/theme-name.png"                 # Optional: path to theme icon (relative to the theme directory it is placed in if not an absolute path)

# ============================================================================
# Core Colors
# ============================================================================
# These are the most important colors that define your theme's overall look.
# Most other colors can be automatically derived from these if not specified.

[colors.core]
accent = "#2f6fed"              # Primary accent color (used for input borders, text selection background...)
accent_foreground = "#ffffff"   # Text color on accent-colored backgrounds
background = "#1a1a1a"          # Main application background
foreground = "#e8e6e1"          # Primary text color
secondary_background = "#242424" # Background for surfaces elevated above main (popups, tooltips, panels)
border = "#2e2e2e"              # Default border color throughout the UI

# ============================================================================
# Window-Specific Colors
# ============================================================================
# Override border colors for specific window types

[colors.main_window]
border = "#2e2e2e"

[colors.settings_window]
border = "#2e2e2e"

# ============================================================================
# Accent Colors
# ============================================================================
# Your color palette, central to your theme's identity

[colors.accents]
blue = "#2f6fed"
green = "#3a9c61"
magenta = "#bc8cff"
orange = "#f0883e"
red = "#b9543b"
yellow = "#bfae78"
cyan = "#18a5b3"
purple = "#bc8cff"

# ============================================================================
# Text Colors
# ============================================================================
# Colors for various text states and meanings

[colors.text]
default = "#e8e6e1"             # Usually same as core.foreground
muted = "#e8e6e1"               # Secondary text: hints, less important info
danger = "#b9543b"              # Errors or warnings
success = "#3a9c61"             # Success or positive states
placeholder = "#e8e6e1"			# input placeholder, search placeholder...
selection = { background = "#2f6fed", foreground = "#ffffff" }

[colors.text.links]
default = "#2f6fed"
visited = "#bc8cff"

# ============================================================================
# Input Fields
# ============================================================================
# Colors for text inputs, search boxes, etc.

[colors.input]
border = "#2e2e2e"
border_focus = "#2f6fed"
border_error = "#b9543b"

# ============================================================================
# Buttons
# ============================================================================
# Colors for button states

[colors.button.primary]
background = "#2e2e2e"
foreground = "#e8e6e1"
hover = { background = "#bb2e2e2e" }
focus = { outline = "colors.core.accent" }

# ============================================================================
# Lists
# ============================================================================
# Colors for list items (horizontal list views)

[colors.list.item.hover]
background = "#2e2e2e"
foreground = "#e8e6e1"

[colors.list.item.selection]
background = "#2e2e2e"          # Selected item on main background
foreground = "#e8e6e1"
secondary_background = "#2e2e2e" # Selected item on secondary surfaces (popups, panels)
                                 # Light themes: lighter than primary selection
                                 # Dark themes: lighter/brighter than primary selection
secondary_foreground = "#e8e6e1"

# ============================================================================
# Grid Items
# ============================================================================
# Colors for grid-based layouts (icon grids, tiles, etc.)

[colors.grid.item]
background = "#242424"
hover = { outline = "#e8e6e1" }
selection = { outline = "#e8e6e1" }

# ============================================================================
# Scrollbars
# ============================================================================

[colors.scrollbars]
background = "#2e2e2e"

# ============================================================================
# Loading Indicators
# ============================================================================

[colors.loading]
bar = "#e8e6e1"     # shown right below the search bar when something is loading
spinner = "#e8e6e1" # used by dynamic toasts
)";

class SetCliThemeCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "set"; }
  std::string description() const override { return "Set theme command"; }
  void setup(CLI::App *app) override { app->add_option("theme_id", m_path)->required(); }

  void run(CLI::App *app) override {
    DaemonIpcClient client;
    if (auto res = client.deeplink(QString("vicinae://theme/set/%1").arg(m_path.c_str())); !res) {
      throw std::runtime_error("Failed to set theme: " + res.error().toStdString());
    }
  }

private:
  std::filesystem::path m_path;
};

class CheckThemeCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "check"; }
  std::string description() const override { return "Check whether the target theme file is valid"; }
  void setup(CLI::App *app) override { app->add_option("file", m_path)->required(); }

  void run(CLI::App *app) override {
    ThemeParser parser;
    auto res = parser.parse(m_path);

    if (!res) { throw std::runtime_error("Theme is invalid: " + res.error()); }

    for (const auto &diag : parser.diagnostics()) {
      std::cout << rang::fg::yellow << "Warning: " << rang::fg::reset << diag << "\n";
    }
    std::cout << rang::fg::green << "Theme file is valid" << rang::fg::reset << "\n";
  }

private:
  std::filesystem::path m_path;
};

class TemplateThemeCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "template"; }
  std::string description() const override { return "Print out template"; }
  void setup(CLI::App *app) override {
    app->alias("tmpl");
    app->add_option("-o,--output", m_path);
  }

  void run(CLI::App *app) override {
    std::cout << THEME_TEMPLATE << std::endl;
    return;
  }

private:
  std::optional<std::filesystem::path> m_path;
};

class ThemeSearchPathsCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "paths"; }
  std::string description() const override { return "Print the paths themes are searched at"; }

  void run(CLI::App *app) override {
    for (const auto &path : ThemeDatabase::defaultSearchPaths()) {
      std::cout << path.c_str() << "\n";
    }
  }
};

ThemeCommand::ThemeCommand() {
  registerCommand<CheckThemeCommand>();
  registerCommand<ThemeSearchPathsCommand>();
  registerCommand<TemplateThemeCommand>();
  registerCommand<SetCliThemeCommand>();
}
