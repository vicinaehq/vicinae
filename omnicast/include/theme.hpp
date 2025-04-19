#pragma once
#include <qapplication.h>
#include <qcolor.h>
#include <qobject.h>
#include <QWidget>
#include <qpalette.h>
#include <qtmetamacros.h>

enum ColorTint { InvalidTint, Blue, Green, Magenta, Orange, Purple, Red, Yellow, TextPrimary, TextSecondary };
enum TextSize { TextHeading, TextRegular, TextTitle };

struct ThemeLinearGradient {
  std::vector<QColor> points;
};

struct ThemeRadialGradient {
  std::vector<QColor> points;
};

using ColorLike = std::variant<QColor, ThemeLinearGradient, ThemeRadialGradient, ColorTint>;

using DeclarativeColor = std::variant<ColorTint, QColor>;

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
    QColor statusBackgroundBorder;

    ColorLike blue;
    ColorLike green;
    ColorLike magenta;
    ColorLike orange;
    ColorLike purple;
    ColorLike red;
    ColorLike yellow;
  } colors;
};

class ThemeService : public QObject {
  Q_OBJECT

  std::vector<ThemeInfo> _themeDb;
  ThemeInfo _theme;
  int _baseFontPointSize = 10;

  ThemeService(const ThemeService &rhs) = delete;

public:
  static ThemeService &instance() {
    static ThemeService _instance;

    return _instance;
  }

  int pointSize(TextSize size) const {
    switch (size) {
    case TextSize::TextRegular:
      return _baseFontPointSize;
    case TextSize::TextTitle:
      return _baseFontPointSize * 1.5;
    }

    return _baseFontPointSize;
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

  ColorLike getTintColor(ColorTint tint) const {
    switch (tint) {
    case ColorTint::Blue:
      return _theme.colors.blue;
    case ColorTint::Green:
      return _theme.colors.green;
    case ColorTint::Magenta:
      return _theme.colors.magenta;
    case ColorTint::Orange:
      return _theme.colors.orange;
    case ColorTint::Purple:
      return _theme.colors.purple;
    case ColorTint::Red:
      return _theme.colors.red;
    case ColorTint::Yellow:
      return _theme.colors.yellow;
    case ColorTint::TextPrimary:
      return _theme.colors.text;
    case ColorTint::TextSecondary:
      return _theme.colors.subtext;
    default:
      break;
    }

    return {};
  }

  void setTheme(const ThemeInfo &info);

  void registerTheme(const ThemeInfo &info) { _themeDb.push_back(info); }

  const std::vector<ThemeInfo> &themes() const { return _themeDb; }

  void registerBuiltinThemes() {
    {
      ThemeInfo theme;
      theme.name = "Raycast Industrial";
      theme.colors.text = QColor("#EBEBEB");                   // Light gray for text
      theme.colors.subtext = QColor("#9A9A9A");                // Medium gray for secondary text
      theme.colors.border = QColor("#342A28");                 // Border color
      theme.colors.mainBackground = QColor("#231A18");         // Darker color for main background
      theme.colors.mainSelectedBackground = QColor("#3C312F"); // Lighter for selection
      theme.colors.mainHoveredBackground = QColor("#291F1D");  // Hover color
      theme.colors.statusBackground = QColor("#2B211F"); // Original color from screenshot for status bar
      theme.colors.statusBackgroundBorder = QColor("#342A28"); // Border for status bar

      ThemeLinearGradient redGradient;
      redGradient.points = {QColor("#FF3B30"), QColor("#CC2E26")};

      ThemeLinearGradient blueGradient;
      blueGradient.points = {QColor("#007AFF"), QColor("#005BBF")};

      ThemeLinearGradient greenGradient;
      greenGradient.points = {QColor("#34C759"), QColor("#28A745")};

      ThemeLinearGradient purpleGradient;
      purpleGradient.points = {QColor("#AF52DE"), QColor("#8A3FB9")};

      ThemeLinearGradient orangeGradient;
      orangeGradient.points = {QColor("#FF9500"), QColor("#CC7700")};

      ThemeLinearGradient yellowGradient;
      yellowGradient.points = {QColor("#FFCC00"), QColor("#CCA300")};

      ThemeLinearGradient magentaGradient;
      magentaGradient.points = {QColor("#FF2D55"), QColor("#CC2444")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;

      registerTheme(theme);
    }

    // 2. Dark Mode (Neutral)
    {
      ThemeInfo theme;
      theme.name = "Dark Mode";
      theme.colors.text = QColor("#F5F5F5");                   // Light gray
      theme.colors.subtext = QColor("#9A9A9A");                // Medium gray
      theme.colors.border = QColor("#2C2C2C");                 // Dark border
      theme.colors.mainBackground = QColor("#18181A");         // Darker color for main background
      theme.colors.mainSelectedBackground = QColor("#2C2C2E"); // Selection color
      theme.colors.mainHoveredBackground = QColor("#1D1D1F");  // Hover color
      theme.colors.statusBackground = QColor("#1C1C1E");       // Lighter color for status bar
      theme.colors.statusBackgroundBorder = QColor("#2C2C2C"); // Status bar border

      ThemeLinearGradient redGradient;
      redGradient.points = {QColor("#FF453A"), QColor("#CF3830")};

      ThemeLinearGradient blueGradient;
      blueGradient.points = {QColor("#0A84FF"), QColor("#0869CC")};

      ThemeLinearGradient greenGradient;
      greenGradient.points = {QColor("#30D158"), QColor("#25A844")};

      ThemeLinearGradient purpleGradient;
      purpleGradient.points = {QColor("#BF5AF2"), QColor("#9A48C2")};

      ThemeLinearGradient orangeGradient;
      orangeGradient.points = {QColor("#FF9F0A"), QColor("#CC7F08")};

      ThemeLinearGradient yellowGradient;
      yellowGradient.points = {QColor("#FFD60A"), QColor("#CCAB08")};

      ThemeLinearGradient magentaGradient;
      magentaGradient.points = {QColor("#FF375F"), QColor("#CC2C4C")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;

      registerTheme(theme);
    }

    // 3. GitHub Dark
    {
      ThemeInfo theme;
      theme.name = "GitHub Dark";
      theme.colors.text = QColor("#E6EDF3");                   // Light text color
      theme.colors.subtext = QColor("#8B949E");                // Secondary text
      theme.colors.border = QColor("#30363D");                 // Border color
      theme.colors.mainBackground = QColor("#090C10");         // Darker color for main background
      theme.colors.mainSelectedBackground = QColor("#21262D"); // Selection color
      theme.colors.mainHoveredBackground = QColor("#0B0E14");  // Hover color
      theme.colors.statusBackground = QColor("#0D1117");       // Lighter color for status bar
      theme.colors.statusBackgroundBorder = QColor("#30363D"); // Status bar border

      ThemeLinearGradient redGradient;
      redGradient.points = {QColor("#F85149"), QColor("#DA3633")};

      ThemeLinearGradient blueGradient;
      blueGradient.points = {QColor("#58A6FF"), QColor("#2188FF")};

      ThemeLinearGradient greenGradient;
      greenGradient.points = {QColor("#3FB950"), QColor("#2EA043")};

      ThemeLinearGradient purpleGradient;
      purpleGradient.points = {QColor("#BC8CFF"), QColor("#8957E5")};

      ThemeLinearGradient orangeGradient;
      orangeGradient.points = {QColor("#F0883E"), QColor("#DB6D28")};

      ThemeLinearGradient yellowGradient;
      yellowGradient.points = {QColor("#F7D747"), QColor("#E3B341")};

      ThemeLinearGradient magentaGradient;
      magentaGradient.points = {QColor("#DB61A2"), QColor("#BF4B8A")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;

      registerTheme(theme);
    }

    // 4. Monokai Pro
    {
      ThemeInfo theme;
      theme.name = "Monokai Pro";
      theme.colors.text = QColor("#FCFCFA");                   // Light text
      theme.colors.subtext = QColor("#939293");                // Secondary text
      theme.colors.border = QColor("#403E41");                 // Border color
      theme.colors.mainBackground = QColor("#221F22");         // Darker color for main background
      theme.colors.mainSelectedBackground = QColor("#403E41"); // Selection
      theme.colors.mainHoveredBackground = QColor("#272427");  // Hover
      theme.colors.statusBackground = QColor("#2D2A2E");       // Lighter color for status bar
      theme.colors.statusBackgroundBorder = QColor("#403E41"); // Status bar border

      ThemeLinearGradient redGradient;
      redGradient.points = {QColor("#FF6188"), QColor("#E54B6B")};

      ThemeLinearGradient blueGradient;
      blueGradient.points = {QColor("#78DCE8"), QColor("#5CBFCB")};

      ThemeLinearGradient greenGradient;
      greenGradient.points = {QColor("#A9DC76"), QColor("#8BBD59")};

      ThemeLinearGradient purpleGradient;
      purpleGradient.points = {QColor("#AB9DF2"), QColor("#8E80D5")};

      ThemeLinearGradient orangeGradient;
      orangeGradient.points = {QColor("#FC9867"), QColor("#E07C4C")};

      ThemeLinearGradient yellowGradient;
      yellowGradient.points = {QColor("#FFD866"), QColor("#E5BF4C")};

      ThemeLinearGradient magentaGradient;
      magentaGradient.points = {QColor("#FF6188"), QColor("#E54B6B")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;

      registerTheme(theme);
    }

    // 5. Nord
    {
      ThemeInfo theme;
      theme.name = "Nord";
      theme.colors.text = QColor("#ECEFF4");                   // Snow Storm light
      theme.colors.subtext = QColor("#D8DEE9");                // Snow Storm darker
      theme.colors.border = QColor("#3B4252");                 // Polar Night 1
      theme.colors.mainBackground = QColor("#242933");         // Darker color for main background
      theme.colors.mainSelectedBackground = QColor("#434C5E"); // Polar Night 2
      theme.colors.mainHoveredBackground = QColor("#292F3A");  // Hover color
      theme.colors.statusBackground = QColor("#2E3440");       // Lighter color for status bar
      theme.colors.statusBackgroundBorder = QColor("#3B4252"); // Status bar border

      ThemeLinearGradient redGradient;
      redGradient.points = {QColor("#BF616A"), QColor("#A54A52")};

      ThemeLinearGradient blueGradient;
      blueGradient.points = {QColor("#5E81AC"), QColor("#4C6A8A")};

      ThemeLinearGradient greenGradient;
      greenGradient.points = {QColor("#A3BE8C"), QColor("#869E74")};

      ThemeLinearGradient purpleGradient;
      purpleGradient.points = {QColor("#B48EAD"), QColor("#9A7991")};

      ThemeLinearGradient orangeGradient;
      orangeGradient.points = {QColor("#D08770"), QColor("#B16D59")};

      ThemeLinearGradient yellowGradient;
      yellowGradient.points = {QColor("#EBCB8B"), QColor("#CAB173")};

      ThemeLinearGradient magentaGradient;
      magentaGradient.points = {QColor("#B48EAD"), QColor("#9A7991")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;

      registerTheme(theme);
    }

    // 6. Dracula
    {
      ThemeInfo theme;
      theme.name = "Dracula";
      theme.colors.text = QColor("#F8F8F2");           // Foreground
      theme.colors.subtext = QColor("#BFBFB4");        // Comment color
      theme.colors.border = QColor("#44475A");         // Current Line/Selection
      theme.colors.mainBackground = QColor("#191A21"); // Darker color for main background (activity bar)
      theme.colors.mainSelectedBackground = QColor("#44475A"); // Current Line/Selection
      theme.colors.mainHoveredBackground = QColor("#21222C");  // Hover color (sidebar)
      theme.colors.statusBackground = QColor("#282A36");       // Lighter color for status bar (editor)
      theme.colors.statusBackgroundBorder = QColor("#44475A"); // Status border

      ThemeRadialGradient redGradient;
      redGradient.points = {QColor("#FF5555"), QColor("#E64747")};

      ThemeRadialGradient blueGradient;
      blueGradient.points = {QColor("#8BE9FD"), QColor("#6DCBE0")};

      ThemeRadialGradient greenGradient;
      greenGradient.points = {QColor("#50FA7B"), QColor("#3FD968")};

      ThemeRadialGradient purpleGradient;
      purpleGradient.points = {QColor("#BD93F9"), QColor("#9D79E0")};

      ThemeRadialGradient orangeGradient;
      orangeGradient.points = {QColor("#FFB86C"), QColor("#E59F55")};

      ThemeRadialGradient yellowGradient;
      yellowGradient.points = {QColor("#F1FA8C"), QColor("#DDE775")};

      ThemeRadialGradient magentaGradient;
      magentaGradient.points = {QColor("#FF79C6"), QColor("#E563AD")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;

      registerTheme(theme);
    }

    // 7. Solarized Dark
    {
      ThemeInfo theme;
      theme.name = "Solarized Dark";
      theme.colors.text = QColor("#FDF6E3");                   // Base3
      theme.colors.subtext = QColor("#93A1A1");                // Base1
      theme.colors.border = QColor("#073642");                 // Base02
      theme.colors.mainBackground = QColor("#001A22");         // Darkest color for main background
      theme.colors.mainSelectedBackground = QColor("#073642"); // Base02
      theme.colors.mainHoveredBackground = QColor("#00232C");  // Hover color
      theme.colors.statusBackground = QColor("#002B36");       // Lighter color for status bar (original base)
      theme.colors.statusBackgroundBorder = QColor("#073642"); // Base02

      ThemeLinearGradient redGradient;
      redGradient.points = {QColor("#DC322F"), QColor("#B3271F")};

      ThemeLinearGradient blueGradient;
      blueGradient.points = {QColor("#268BD2"), QColor("#2076B1")};

      ThemeLinearGradient greenGradient;
      greenGradient.points = {QColor("#859900"), QColor("#6C7D00")};

      ThemeLinearGradient purpleGradient;
      purpleGradient.points = {QColor("#6C71C4"), QColor("#565CA0")};

      ThemeLinearGradient orangeGradient;
      orangeGradient.points = {QColor("#CB4B16"), QColor("#A73D12")};

      ThemeLinearGradient yellowGradient;
      yellowGradient.points = {QColor("#B58900"), QColor("#946E00")};

      ThemeLinearGradient magentaGradient;
      magentaGradient.points = {QColor("#D33682"), QColor("#B72D6F")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;

      registerTheme(theme);
    }

    // 8. Catppuccin Mocha
    {
      ThemeInfo theme;
      theme.name = "Catppuccin Mocha";
      theme.colors.text = QColor("#CDD6F4");                   // Text
      theme.colors.subtext = QColor("#A6ADC8");                // Subtext 0
      theme.colors.border = QColor("#313244");                 // Surface 0
      theme.colors.mainBackground = QColor("#11111B");         // Darker color for main background
      theme.colors.mainSelectedBackground = QColor("#313244"); // Surface 0
      theme.colors.mainHoveredBackground = QColor("#181825");  // Hover color (crust)
      theme.colors.statusBackground = QColor("#1E1E2E");       // Lighter color for status bar (base)
      theme.colors.statusBackgroundBorder = QColor("#313244"); // Surface 0

      ThemeRadialGradient redGradient;
      redGradient.points = {QColor("#F38BA8"), QColor("#E56F91")};

      ThemeRadialGradient blueGradient;
      blueGradient.points = {QColor("#89B4FA"), QColor("#7099E3")};

      ThemeRadialGradient greenGradient;
      greenGradient.points = {QColor("#A6E3A1"), QColor("#8BCA87")};

      ThemeRadialGradient purpleGradient;
      purpleGradient.points = {QColor("#CBA6F7"), QColor("#B68AE2")};

      ThemeRadialGradient orangeGradient;
      orangeGradient.points = {QColor("#FAB387"), QColor("#E5996F")};

      ThemeRadialGradient yellowGradient;
      yellowGradient.points = {QColor("#F9E2AF"), QColor("#E4CE91")};

      ThemeRadialGradient magentaGradient;
      magentaGradient.points = {QColor("#F5C2E7"), QColor("#E0A8D0")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;

      registerTheme(theme);
    }

    // 9. Tokyo Night
    {
      ThemeInfo theme;
      theme.name = "Tokyo Night";
      theme.colors.text = QColor("#A9B1D6");                   // Foreground
      theme.colors.subtext = QColor("#787C99");                // Comment
      theme.colors.border = QColor("#232433");                 // Border color
      theme.colors.mainBackground = QColor("#13131A");         // Darker color for main background
      theme.colors.mainSelectedBackground = QColor("#2C2E3F"); // Selection background
      theme.colors.mainHoveredBackground = QColor("#16161E");  // Hover background
      theme.colors.statusBackground = QColor("#1A1B26");       // Lighter color for status bar (original)
      theme.colors.statusBackgroundBorder = QColor("#232433"); // Status border

      ThemeLinearGradient redGradient;
      redGradient.points = {QColor("#F7768E"), QColor("#D75D78")};

      ThemeLinearGradient blueGradient;
      blueGradient.points = {QColor("#7AA2F7"), QColor("#6889DE")};

      ThemeLinearGradient greenGradient;
      greenGradient.points = {QColor("#9ECE6A"), QColor("#7FAD4F")};

      ThemeLinearGradient purpleGradient;
      purpleGradient.points = {QColor("#9D7CD8"), QColor("#8366C1")};

      ThemeLinearGradient orangeGradient;
      orangeGradient.points = {QColor("#FF9E64"), QColor("#E08752")};

      ThemeLinearGradient yellowGradient;
      yellowGradient.points = {QColor("#E0AF68"), QColor("#C79650")};

      ThemeLinearGradient magentaGradient;
      magentaGradient.points = {QColor("#BB9AF7"), QColor("#A182DF")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;

      registerTheme(theme);
    }

    // 10. One Dark Pro
    {
      ThemeInfo theme;
      theme.name = "One Dark Pro";
      theme.colors.text = QColor("#ABB2BF");                   // Foreground
      theme.colors.subtext = QColor("#7F848E");                // Comment
      theme.colors.border = QColor("#2C323C");                 // Border
      theme.colors.mainBackground = QColor("#1B1D23");         // Darker color for main background
      theme.colors.mainSelectedBackground = QColor("#3E4451"); // Selection background
      theme.colors.mainHoveredBackground = QColor("#21252B");  // Hover background (previous status bar)
      theme.colors.statusBackground = QColor("#282C34");       // Lighter color for status bar (original)
      theme.colors.statusBackgroundBorder = QColor("#2C323C"); // Status border

      ThemeLinearGradient redGradient;
      redGradient.points = {QColor("#E06C75"), QColor("#C55A62")};

      ThemeLinearGradient blueGradient;
      blueGradient.points = {QColor("#61AFEF"), QColor("#4E95CE")};

      ThemeLinearGradient greenGradient;
      greenGradient.points = {QColor("#98C379"), QColor("#7DA763")};

      ThemeLinearGradient purpleGradient;
      purpleGradient.points = {QColor("#C678DD"), QColor("#A962BD")};

      ThemeLinearGradient orangeGradient;
      orangeGradient.points = {QColor("#D19A66"), QColor("#B58254")};

      ThemeLinearGradient yellowGradient;
      yellowGradient.points = {QColor("#E5C07B"), QColor("#CAAA68")};

      ThemeLinearGradient magentaGradient;
      magentaGradient.points = {QColor("#C678DD"), QColor("#A962BD")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;

      registerTheme(theme);
    }

    {
      ThemeInfo theme;
      theme.name = "Ayu Mirage";
      theme.colors.text = QColor("#CBCCC6");                   // Foreground
      theme.colors.subtext = QColor("#8A9199");                // Comment/subtle color
      theme.colors.border = QColor("#1F2430");                 // Border color
      theme.colors.mainBackground = QColor("#171B24");         // Darker color for main background
      theme.colors.mainSelectedBackground = QColor("#34455A"); // Selection background
      theme.colors.mainHoveredBackground = QColor("#1C212B");  // Hover background
      theme.colors.statusBackground = QColor("#1F2430");       // Original background for status bar
      theme.colors.statusBackgroundBorder = QColor("#1F2430"); // Status border

      ThemeLinearGradient redGradient;
      redGradient.points = {QColor("#F28779"), QColor("#E36D5E")};

      ThemeLinearGradient blueGradient;
      blueGradient.points = {QColor("#5CCFE6"), QColor("#39B9D4")};

      ThemeLinearGradient greenGradient;
      greenGradient.points = {QColor("#BAE67E"), QColor("#A1D15C")};

      ThemeLinearGradient purpleGradient;
      purpleGradient.points = {QColor("#D4BFFF"), QColor("#B799E3")};

      ThemeLinearGradient orangeGradient;
      orangeGradient.points = {QColor("#FFA759"), QColor("#F28C38")};

      ThemeLinearGradient yellowGradient;
      yellowGradient.points = {QColor("#FFD580"), QColor("#FFB454")};

      ThemeLinearGradient magentaGradient;
      magentaGradient.points = {QColor("#FF99C9"), QColor("#F279AB")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;

      registerTheme(theme);
    }

    // 12. Ayu Dark
    {
      ThemeInfo theme;
      theme.name = "Ayu Dark";
      theme.colors.text = QColor("#B3B1AD");                   // Foreground
      theme.colors.subtext = QColor("#788889");                // Comment/subtle color
      theme.colors.border = QColor("#0A0E14");                 // Border color
      theme.colors.mainBackground = QColor("#09080F");         // Darker color for main background
      theme.colors.mainSelectedBackground = QColor("#1F2430"); // Selection background
      theme.colors.mainHoveredBackground = QColor("#0D1017");  // Hover background
      theme.colors.statusBackground = QColor("#0A0E14");       // Original background for status bar
      theme.colors.statusBackgroundBorder = QColor("#0A0E14"); // Status border

      ThemeLinearGradient redGradient;
      redGradient.points = {QColor("#F07178"), QColor("#E25A62")};

      ThemeLinearGradient blueGradient;
      blueGradient.points = {QColor("#39BAE6"), QColor("#229DC5")};

      ThemeLinearGradient greenGradient;
      greenGradient.points = {QColor("#C2D94C"), QColor("#A9C12C")};

      ThemeLinearGradient purpleGradient;
      purpleGradient.points = {QColor("#A37ACC"), QColor("#895EB3")};

      ThemeLinearGradient orangeGradient;
      orangeGradient.points = {QColor("#FF8F40"), QColor("#FF7216")};

      ThemeLinearGradient yellowGradient;
      yellowGradient.points = {QColor("#FFB454"), QColor("#FF9A20")};

      ThemeLinearGradient magentaGradient;
      magentaGradient.points = {QColor("#E6B673"), QColor("#D69C54")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;

      registerTheme(theme);
    }

    {
      ThemeInfo theme;
      theme.name = "Kanagawa Dragon";
      theme.colors.text = QColor("#DCD7BA");                   // Foreground
      theme.colors.subtext = QColor("#727169");                // Comment/subtle color
      theme.colors.border = QColor("#1F1F28");                 // Border color
      theme.colors.mainBackground = QColor("#181820");         // Darker color for main background
      theme.colors.mainSelectedBackground = QColor("#2A2A37"); // Selection background
      theme.colors.mainHoveredBackground = QColor("#1C1C27");  // Hover background
      theme.colors.statusBackground = QColor("#1F1F28");       // Original background for status bar
      theme.colors.statusBackgroundBorder = QColor("#2A2A37"); // Status border

      ThemeLinearGradient redGradient;
      redGradient.points = {QColor("#E82424"), QColor("#C41C1C")};

      ThemeLinearGradient blueGradient;
      blueGradient.points = {QColor("#7E9CD8"), QColor("#5D7AB8")};

      ThemeLinearGradient greenGradient;
      greenGradient.points = {QColor("#98BB6C"), QColor("#7FA252")};

      ThemeLinearGradient purpleGradient;
      purpleGradient.points = {QColor("#957FB8"), QColor("#7C689E")};

      ThemeLinearGradient orangeGradient;
      orangeGradient.points = {QColor("#FF9E3B"), QColor("#E58225")};

      ThemeLinearGradient yellowGradient;
      yellowGradient.points = {QColor("#E6C384"), QColor("#D1AA64")};

      ThemeLinearGradient magentaGradient;
      magentaGradient.points = {QColor("#D27E99"), QColor("#B66179")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;
    }

    // 15. Kanagawa Wave (standard variant)
    {
      ThemeInfo theme;
      theme.name = "Kanagawa Wave";
      theme.colors.text = QColor("#DCD7BA");                   // Foreground
      theme.colors.subtext = QColor("#727169");                // Comment/subtle color
      theme.colors.border = QColor("#252525");                 // Border color
      theme.colors.mainBackground = QColor("#12121B");         // Darker color for main background
      theme.colors.mainSelectedBackground = QColor("#223249"); // Selection background
      theme.colors.mainHoveredBackground = QColor("#16161D");  // Hover background
      theme.colors.statusBackground = QColor("#1F1F28");       // Original background for status bar
      theme.colors.statusBackgroundBorder = QColor("#16161D"); // Status border

      ThemeLinearGradient redGradient;
      redGradient.points = {QColor("#C34043"), QColor("#A63538")};

      ThemeLinearGradient blueGradient;
      blueGradient.points = {QColor("#7FB4CA"), QColor("#5D99B0")};

      ThemeLinearGradient greenGradient;
      greenGradient.points = {QColor("#76946A"), QColor("#5F7954")};

      ThemeLinearGradient purpleGradient;
      purpleGradient.points = {QColor("#938AA9"), QColor("#7A7290")};

      ThemeLinearGradient orangeGradient;
      orangeGradient.points = {QColor("#FFA066"), QColor("#FF884D")};

      ThemeLinearGradient yellowGradient;
      yellowGradient.points = {QColor("#DCA561"), QColor("#C88D45")};

      ThemeLinearGradient magentaGradient;
      magentaGradient.points = {QColor("#D27E99"), QColor("#B66179")};

      theme.colors.red = redGradient;
      theme.colors.blue = blueGradient;
      theme.colors.green = greenGradient;
      theme.colors.purple = purpleGradient;
      theme.colors.orange = orangeGradient;
      theme.colors.yellow = yellowGradient;
      theme.colors.magenta = magentaGradient;

      registerTheme(theme);
    }
  }

  ThemeService() { registerBuiltinThemes(); }

signals:
  bool themeChanged(const ThemeInfo &info) const;
};
