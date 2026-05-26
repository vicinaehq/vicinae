#pragma once
#include <QString>

/**
 * QT doesn't offer public APIs to get information about the available icon themes.
 * This class fixes this, using QIcon::themeSearchPaths to know where to look.
 * This class also uses implicit sharing: we load icon themes only once no matter how many instances of the
 * database are initialized, making constructing such an object trivial.
 */

class IconThemeDatabase {
public:
  struct IconThemeInfo {
    QString name;
    bool hidden;
  };
  using IconThemeList = std::vector<IconThemeInfo>;

  IconThemeList themes(bool includeHidden = true) const;

  bool hasTheme(const QString &name) const;
  bool isSuitableTheme(const QString &name) const;
  /**
   * Tries to get the best theme we can from the system list.
   */
  QString guessBestTheme() const;

  IconThemeDatabase();

private:
  static IconThemeList scan();
};
