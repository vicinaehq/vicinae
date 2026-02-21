#pragma once
#include "config/config.hpp"
#include "service-registry.hpp"
#include <QObject>
#include <QVariantList>

class GeneralSettingsModel : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool searchFilesInRoot READ searchFilesInRoot WRITE setSearchFilesInRoot NOTIFY configChanged)
  Q_PROPERTY(bool closeOnFocusLoss READ closeOnFocusLoss WRITE setCloseOnFocusLoss NOTIFY configChanged)
  Q_PROPERTY(bool considerPreedit READ considerPreedit WRITE setConsiderPreedit NOTIFY configChanged)
  Q_PROPERTY(bool popToRootOnClose READ popToRootOnClose WRITE setPopToRootOnClose NOTIFY configChanged)
  Q_PROPERTY(bool clientSideDecorations READ clientSideDecorations WRITE setClientSideDecorations NOTIFY configChanged)
  Q_PROPERTY(QString windowOpacity READ windowOpacity WRITE setWindowOpacity NOTIFY configChanged)
  Q_PROPERTY(QString fontSize READ fontSize WRITE setFontSize NOTIFY configChanged)
  Q_PROPERTY(QVariantList themeItems READ themeItems NOTIFY configChanged)
  Q_PROPERTY(QVariantList fontItems READ fontItems NOTIFY configChanged)
  Q_PROPERTY(QVariantList iconThemeItems READ iconThemeItems NOTIFY configChanged)
  Q_PROPERTY(QVariantList faviconServiceItems READ faviconServiceItems NOTIFY configChanged)
  Q_PROPERTY(QVariantList keybindingSchemeItems READ keybindingSchemeItems NOTIFY configChanged)
  Q_PROPERTY(QVariant currentTheme READ currentTheme NOTIFY configChanged)
  Q_PROPERTY(QVariant currentFont READ currentFont NOTIFY configChanged)
  Q_PROPERTY(QVariant currentIconTheme READ currentIconTheme NOTIFY configChanged)
  Q_PROPERTY(QVariant currentFaviconService READ currentFaviconService NOTIFY configChanged)
  Q_PROPERTY(QVariant currentKeybindingScheme READ currentKeybindingScheme NOTIFY configChanged)

signals:
  void configChanged();

public:
  explicit GeneralSettingsModel(QObject *parent = nullptr);

  bool searchFilesInRoot() const;
  void setSearchFilesInRoot(bool v);
  bool closeOnFocusLoss() const;
  void setCloseOnFocusLoss(bool v);
  bool considerPreedit() const;
  void setConsiderPreedit(bool v);
  bool popToRootOnClose() const;
  void setPopToRootOnClose(bool v);
  bool clientSideDecorations() const;
  void setClientSideDecorations(bool v);
  QString windowOpacity() const;
  void setWindowOpacity(const QString &v);
  QString fontSize() const;
  void setFontSize(const QString &v);

  QVariantList themeItems() const;
  QVariantList fontItems() const;
  QVariantList iconThemeItems() const;
  QVariantList faviconServiceItems() const;
  QVariantList keybindingSchemeItems() const;
  QVariant currentTheme() const;
  QVariant currentFont() const;
  QVariant currentIconTheme() const;
  QVariant currentFaviconService() const;
  QVariant currentKeybindingScheme() const;

  Q_INVOKABLE void selectTheme(const QString &id);
  Q_INVOKABLE void selectFont(const QString &id);
  Q_INVOKABLE void selectIconTheme(const QString &id);
  Q_INVOKABLE void selectFaviconService(const QString &id);
  Q_INVOKABLE void selectKeybindingScheme(const QString &id);

private:
  const config::ConfigValue &cfg() const;
  config::Manager &cfgManager() const;
};
