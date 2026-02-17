#pragma once
#include "qml-bridge-view.hpp"
#include "ui/image/url.hpp"
#include <functional>

class QmlStoreIntroViewHost : public QmlFormViewBase {
  Q_OBJECT

  Q_PROPERTY(QString introMarkdown READ introMarkdown CONSTANT)

public:
  QmlStoreIntroViewHost(const QString &markdown, const ImageURL &icon,
                        const QString &actionLabel, std::function<void()> continueAction);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;

  QString introMarkdown() const { return m_markdown; }

private:
  QString m_markdown;
  ImageURL m_icon;
  QString m_actionLabel;
  std::function<void()> m_continueAction;
};
