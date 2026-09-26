#include "document-integration.hpp"
#include "ui/bridges/theme-bridge.hpp"
#include "ui/bridges/config-bridge.hpp"
#include "ui/image/image-url.hpp"
#include "services/app-service/app-service.hpp"

DocumentIntegration::DocumentIntegration(QObject *parent) : QObject(parent) {
  const auto *theme = QmlEngineScope::global<ThemeBridge>();
  connect(theme, &ThemeBridge::changed, this, &DocumentIntegration::updateStyle);
  connect(theme, &ThemeBridge::fontFamilyChanged, this, &DocumentIntegration::updateStyle);
  connect(QmlEngineScope::global<ConfigBridge>(), &ConfigBridge::changed, this,
          &DocumentIntegration::updateStyle);
  updateStyle();
}

void DocumentIntegration::updateStyle() {
  const QScopedPropertyUpdateGroup update;
  const auto withAlpha = [](QColor color, qreal alpha) {
    color.setAlphaF(alpha);
    return color;
  };
  const auto *theme = QmlEngineScope::global<ThemeBridge>();
  m_style.fontFamily = theme->fontFamily();
  m_style.monoFontFamily = theme->monoFontFamily();
  m_style.regularFontSize = theme->regularFontSize();
  m_style.smallerFontSize = theme->smallerFontSize();
  m_style.isDark = theme->isDark();
  m_style.foreground = theme->foreground();
  m_style.textMuted = theme->textMuted();
  m_style.divider = theme->divider();
  m_style.secondaryBackground = theme->secondaryBackground();
  m_style.textSelectionBg = theme->textSelectionBg();
  m_style.textSelectionFg = theme->textSelectionFg();
  m_style.accent = theme->accent();
  m_style.linkColor = theme->linkColor();
  m_style.codeBackground = withAlpha(theme->secondaryBackground(), theme->surfaceOpacity());
  m_style.codeBorder = withAlpha(theme->divider(), QmlEngineScope::global<ConfigBridge>()->windowOpacity());
  m_style.inlineCodeBackground = withAlpha(theme->foreground(), 0.08);
  m_style.tableBorder = withAlpha(theme->foreground(), 0.1);
  m_style.tableHeaderBackground = withAlpha(theme->foreground(), 0.06);
  m_style.scrollBarColor = theme->scrollBarBackground();
  m_style.infoColor = theme->toastInfo();
  m_style.warningColor = theme->toastWarning();
  m_style.dangerColor = theme->toastDanger();
  m_style.successColor = theme->toastSuccess();
  const auto &palette = ThemeService::instance().theme();
  m_style.keywordColor = palette.resolve(SemanticColor::Purple);
  m_style.functionColor = palette.resolve(SemanticColor::Blue);
  m_style.variableColor = palette.resolve(SemanticColor::Red);
  m_style.builtinColor = palette.resolve(SemanticColor::Cyan);
  m_style.numberColor = palette.resolve(SemanticColor::Orange);
  m_style.stringColor = palette.resolve(SemanticColor::Green);
  m_style.commentColor = palette.resolve(SemanticColor::TextMuted);
}

void DocumentIntegration::openLink(const QString &url) {
  if (!ServiceRegistry::instance()->appDb()->openTarget(url))
    qWarning() << "Failed to open document link" << url;
}

QString DocumentIntegration::imageSource(const QString &rawUrl) const {
  const QUrl url(rawUrl);
  const auto scheme = url.scheme();
  if (scheme == "icon") return rawUrl;
  if (scheme == "https" || scheme == "http") return ImageURL::http(url).toString();
  if (scheme == "data") {
    ImageURL image;
    image.setType(ImageURLType::DataURI);
    image.setName(rawUrl);
    return image.toString();
  }
  if (scheme == "file") {
    const auto path = url.host().isEmpty() ? url.path() : url.host() + url.path();
    return ImageURL::local(path).toString();
  }
  if (scheme.isEmpty()) return ImageURL::local(rawUrl).toString();
  return {};
}
