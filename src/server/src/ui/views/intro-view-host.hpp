#pragma once
#include <QtQml/qqmlregistration.h>
#include "ui/views/bridge-view.hpp"
#include "ui/image/image-url.hpp"
#include "ui/image/url.hpp"
#include <functional>
#include <vector>

/**
 * A single page with one primary action. Either a markdown document with the icon on top, or a centered
 * icon, title and description for short setup screens.
 */
class IntroViewHost : public FormViewBase {
  Q_OBJECT
  QML_NAMED_ELEMENT(IntroViewHost)
  QML_UNCREATABLE("")

  Q_PROPERTY(bool centered READ centered CONSTANT)
  Q_PROPERTY(QString introMarkdown READ introMarkdown CONSTANT)
  Q_PROPERTY(QString title READ title CONSTANT)
  Q_PROPERTY(QString description READ description CONSTANT)
  Q_PROPERTY(ImageUrl icon READ icon CONSTANT)

public:
  IntroViewHost(const QString &markdown, const ImageURL &icon, const QString &actionLabel,
                std::function<void()> continueAction);
  IntroViewHost(const QString &title, const QString &description, const ImageURL &icon,
                const QString &actionLabel, std::function<void()> continueAction);

  void addSecondaryAction(const QString &label, const ImageURL &icon, std::function<void()> action);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;

  bool centered() const { return m_centered; }
  QString introMarkdown() const { return m_markdown; }
  QString title() const { return m_title; }
  QString description() const { return m_description; }
  ImageUrl icon() const { return ImageUrl{m_icon}; }

private:
  bool m_centered = false;
  QString m_markdown;
  QString m_title;
  QString m_description;
  ImageURL m_icon;
  QString m_actionLabel;
  std::function<void()> m_continueAction;

  struct SecondaryAction {
    QString label;
    ImageURL icon;
    std::function<void()> action;
  };
  std::vector<SecondaryAction> m_secondaryActions;
};
