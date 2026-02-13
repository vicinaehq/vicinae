#pragma once
#include "qml-command-list-model.hpp"
#include "config/config.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"

class QmlThemeListModel : public QmlCommandListModel {
  Q_OBJECT

public:
  enum ThemeRole {
    PaletteColor0 = QmlCommandListModel::Accessory + 1,
    PaletteColor1,
    PaletteColor2,
    PaletteColor3,
    PaletteColor4,
    PaletteColor5,
    PaletteColor6,
    PaletteColor7,
  };

  explicit QmlThemeListModel(QObject *parent = nullptr);

  void initialize(ApplicationContext *ctx) override;
  void setFilter(const QString &text) override;
  QString searchPlaceholder() const override { return QStringLiteral("Search for a theme..."); }
  QUrl qmlComponentUrl() const override { return QUrl(QStringLiteral("qrc:/qml/ThemeListView.qml")); }

  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  void onItemSelected(int section, int item) override;
  void beforePop() override;

protected:
  QString itemTitle(int s, int i) const override;
  QString itemSubtitle(int s, int i) const override;
  QString itemIconSource(int s, int i) const override;
  std::unique_ptr<ActionPanelState> createActionPanel(int s, int i) const override;

private:
  void regenerateThemes();
  const std::shared_ptr<ThemeFile> &themeAt(int s, int i) const;

  ThemeService *m_themeService = nullptr;
  config::Manager *m_config = nullptr;
  QString m_query;

  std::optional<std::shared_ptr<ThemeFile>> m_selectedTheme;
  std::vector<std::shared_ptr<ThemeFile>> m_availableThemes;
  QString m_previousThemeId;
};
