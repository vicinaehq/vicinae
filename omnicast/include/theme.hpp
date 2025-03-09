#pragma once
#include <qapplication.h>
#include <qcolor.h>
#include <qobject.h>
#include <QWidget>
#include <qtmetamacros.h>

struct ThemeLinearGradient {
  std::vector<QColor> points;
};

using ColorLike = std::variant<QColor, ThemeLinearGradient>;

struct ThemeInfo {
  QString name;

  struct {
    QColor text;
    QColor subtext;
    QColor border;

    QColor mainBackground;
    QColor mainSelectedBackground;
    QColor mainHoveredBackground;

    QColor statusBackground;
  } colors;
};

class ThemeService : public QObject {
  Q_OBJECT

  std::vector<ThemeInfo> _themeDb;
  ThemeInfo _theme;

  ThemeService(const ThemeService &rhs) = delete;

public:
  static ThemeService &instance() {
    static ThemeService _instance;

    return _instance;
  }

  /**
   * Returns the theme that is currently in use
   */
  const ThemeInfo &theme() const { return _theme; }

  std::optional<ThemeInfo> theme(const QString &name) const {
    for (const auto &info : _themeDb) {
      if (info.name == name) { return info; }
    }

    return std::nullopt;
  }

  bool setTheme(const QString &name) {
    for (const auto &info : _themeDb) {
      if (info.name == name) {
        setTheme(info);
        return true;
      }
    }

    return false;
  }

  void setTheme(const ThemeInfo &info) {
    _theme = info;

    auto style = QString(R"(
	QWidget {
		font-family: 'SF Pro Text';
        font-size: 10pt;
        font-weight: lighter;
        letter-spacing: -0.5px;
        color: %1;
	}
	QLineEdit {
		background-color: transparent;
		border: none;
	}

	.top-bar QLineEdit {
		font-size: 12pt;
	}

	QLabel[subtext="true"] {
		color: %2;
	}
	)")
                     .arg(info.colors.text.name(), info.colors.subtext.name());

    qDebug() << "style" << style;

    qApp->setStyleSheet(style);

    emit themeChanged(info);
  }

  void registerTheme(const ThemeInfo &info) { _themeDb.push_back(info); }

  const std::vector<ThemeInfo> &themes() const { return _themeDb; }

  // Alternative darker version if preferred
  void registerRaycastDarkTheme() {
    ThemeInfo raycastDarkTheme;

    // Theme name
    raycastDarkTheme.name = "Raycast Dark Industrial";

    // Theme colors
    raycastDarkTheme.colors.text = QColor(235, 235, 235);                // Light gray for primary text
    raycastDarkTheme.colors.subtext = QColor(160, 160, 160);             // Medium gray for secondary text
    raycastDarkTheme.colors.border = QColor(50, 40, 38);                 // Subtle border color
    raycastDarkTheme.colors.mainBackground = QColor(32, 22, 20);         // Darker version of main background
    raycastDarkTheme.colors.mainSelectedBackground = QColor(55, 45, 42); // Lighter version for selections
    raycastDarkTheme.colors.mainHoveredBackground = QColor(45, 35, 33);  // Hover state color
    raycastDarkTheme.colors.statusBackground = QColor(26, 18, 16);       // Even darker status bar

    // Register the theme
    registerTheme(raycastDarkTheme);
  }

  void registerRaycastNeutralTheme() {
    ThemeInfo raycastNeutralTheme;

    // Theme name
    raycastNeutralTheme.name = "Raycast Neutral";

    // Theme colors
    raycastNeutralTheme.colors.text = QColor(235, 235, 235);                // Light gray for primary text
    raycastNeutralTheme.colors.subtext = QColor(160, 160, 160);             // Medium gray for secondary text
    raycastNeutralTheme.colors.border = QColor(55, 55, 55);                 // Subtle border color
    raycastNeutralTheme.colors.mainBackground = QColor(42, 42, 42);         // Neutral dark gray
    raycastNeutralTheme.colors.mainSelectedBackground = QColor(65, 65, 65); // Lighter selection
    raycastNeutralTheme.colors.mainHoveredBackground = QColor(52, 52, 52);  // Hover state
    raycastNeutralTheme.colors.statusBackground = QColor(32, 32, 32);       // Darker status bar

    // Register the theme
    registerTheme(raycastNeutralTheme);
  }

  ThemeService() {
    registerRaycastNeutralTheme();
    registerRaycastDarkTheme();
  }

signals:
  bool themeChanged(const ThemeInfo &info) const;
};
