#pragma once
#include "bridge-view.hpp"
#include "font-service.hpp"

class FontDemoViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QString fontFamily READ fontFamily CONSTANT)
  Q_PROPERTY(QString showcaseMarkdown READ showcaseMarkdown CONSTANT)

public:
  FontDemoViewHost(QString family, FontCategory category)
      : m_family(std::move(family)), m_category(category) {}

  QUrl qmlComponentUrl() const override { return QUrl(QStringLiteral("qrc:/Vicinae/FontDemoView.qml")); }
  QVariantMap qmlProperties() override { return {{QStringLiteral("host"), QVariant::fromValue(this)}}; }
  QString initialNavigationTitle() const override { return m_family; }
  bool searchInteractive() const override { return false; }

  QString fontFamily() const { return m_family; }
  QString showcaseMarkdown() const;

private:
  QString m_family;
  FontCategory m_category;
};
