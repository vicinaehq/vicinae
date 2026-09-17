#pragma once
#include <QtQml/qqmlregistration.h>
#include "ui/views/view-utils.hpp"
#include "ui/views/bridge-view.hpp"
#include "services/font-service/font-service.hpp"

class FontDemoViewHost : public ViewHostBase {
  Q_OBJECT
  QML_NAMED_ELEMENT(FontDemoViewHost)
  QML_UNCREATABLE("")
  Q_PROPERTY(QString fontFamily READ fontFamily CONSTANT)
  Q_PROPERTY(QString showcaseMarkdown READ showcaseMarkdown CONSTANT)

public:
  FontDemoViewHost(QString family, FontCategory category)
      : m_family(std::move(family)), m_category(category) {}

  QUrl qmlComponentUrl() const override { return qml::componentUrl(u"FontDemoView"); }
  QVariantMap qmlProperties() override { return {{QStringLiteral("host"), QVariant::fromValue(this)}}; }
  QString initialNavigationTitle() const override { return m_family; }
  bool searchInteractive() const override { return false; }

  QString fontFamily() const { return m_family; }
  QString showcaseMarkdown() const;

private:
  QString m_family;
  FontCategory m_category;
};
