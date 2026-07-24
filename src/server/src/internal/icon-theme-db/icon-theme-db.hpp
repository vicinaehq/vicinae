#pragma once
#include <QString>
#include <optional>

/**
 * QT doesn't offer public APIs to get information about the available icon themes.
 * This class fixes this, using QIcon::themeSearchPaths to know where to look.
 * This class also uses implicit sharing: we load icon themes only once no matter how many instances of the
 * database are initialized, making constructing such an object trivial.
 */

class IconThemeDatabase {
public:
  struct IconThemeInfo {
    /**
     * The directory basename of the theme (e.g. "kora"). This is the canonical identifier and the
     * value that must be passed to QIcon::setThemeName: Qt resolves themes by directory name, NOT by
     * the human-readable "Name" found in index.theme.
     */
    QString id;
    /**
     * The display name from index.theme's "Name" key (e.g. "Kora"). For presentation only.
     */
    QString name;
    bool hidden;
  };
  using IconThemeList = std::vector<IconThemeInfo>;

  IconThemeList themes(bool includeHidden = true) const;

  /**
   * Look a theme up by its directory id (e.g. "kora").
   */
  bool hasThemeId(const QString &id) const;
  bool isSuitableTheme(const QString &id) const;

  /**
   * Resolve a stored config value to a canonical theme id. Accepts either a directory id (returned
   * as-is when valid) or a legacy display name (e.g. "Kora") which is mapped back to its directory
   * id. Falls back to the input unchanged when nothing matches.
   */
  QString resolveThemeId(const QString &value) const;

  /**
   * Display name for a given theme id, falling back to the id itself when unknown.
   */
  QString displayName(const QString &id) const;

  /**
   * Tries to get the best theme we can, following the system icon theme when possible.
   * The returned value is always a directory id suitable for QIcon::setThemeName.
   */
  QString guessBestTheme() const;

  /**
   * The icon theme configured at the system/desktop level (GNOME gsettings, KDE kdeglobals, ...),
   * if it can be determined and is actually installed. Linux only.
   */
  std::optional<QString> systemIconThemeId() const;

  IconThemeDatabase();

private:
  static IconThemeList scan();
};
