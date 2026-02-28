#pragma once
#include "bridge-view.hpp"
#include <QVariantList>
#include <QVariantMap>

class CreateExtensionViewHost : public FormViewBase {
  Q_OBJECT

  Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY formChanged)
  Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY formChanged)
  Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY formChanged)
  Q_PROPERTY(QString location READ location WRITE setLocation NOTIFY formChanged)

  Q_PROPERTY(QString commandTitle READ commandTitle WRITE setCommandTitle NOTIFY formChanged)
  Q_PROPERTY(QString commandSubtitle READ commandSubtitle WRITE setCommandSubtitle NOTIFY formChanged)
  Q_PROPERTY(
      QString commandDescription READ commandDescription WRITE setCommandDescription NOTIFY formChanged)
  Q_PROPERTY(QVariantMap selectedTemplate READ selectedTemplate NOTIFY formChanged)

  Q_PROPERTY(QVariantList templateItems READ templateItems CONSTANT)

  Q_PROPERTY(QString authorError READ authorError NOTIFY errorsChanged)
  Q_PROPERTY(QString titleError READ titleError NOTIFY errorsChanged)
  Q_PROPERTY(QString descriptionError READ descriptionError NOTIFY errorsChanged)
  Q_PROPERTY(QString locationError READ locationError NOTIFY errorsChanged)
  Q_PROPERTY(QString commandTitleError READ commandTitleError NOTIFY errorsChanged)
  Q_PROPERTY(QString commandSubtitleError READ commandSubtitleError NOTIFY errorsChanged)
  Q_PROPERTY(QString commandDescriptionError READ commandDescriptionError NOTIFY errorsChanged)

public:
  explicit CreateExtensionViewHost();

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;

  QString author() const { return m_author; }
  QString title() const { return m_title; }
  QString description() const { return m_description; }
  QString location() const { return m_location; }

  QString commandTitle() const { return m_commandTitle; }
  QString commandSubtitle() const { return m_commandSubtitle; }
  QString commandDescription() const { return m_commandDescription; }
  QVariantMap selectedTemplate() const { return m_selectedTemplate; }

  QVariantList templateItems() const { return m_templateItems; }

  QString authorError() const { return m_authorError; }
  QString titleError() const { return m_titleError; }
  QString descriptionError() const { return m_descriptionError; }
  QString locationError() const { return m_locationError; }
  QString commandTitleError() const { return m_commandTitleError; }
  QString commandSubtitleError() const { return m_commandSubtitleError; }
  QString commandDescriptionError() const { return m_commandDescriptionError; }

  void setAuthor(const QString &v) {
    if (m_author != v) {
      m_author = v;
      emit formChanged();
    }
  }
  void setTitle(const QString &v) {
    if (m_title != v) {
      m_title = v;
      emit formChanged();
    }
  }
  void setDescription(const QString &v) {
    if (m_description != v) {
      m_description = v;
      emit formChanged();
    }
  }
  void setLocation(const QString &v) {
    if (m_location != v) {
      m_location = v;
      emit formChanged();
    }
  }

  void setCommandTitle(const QString &v) {
    if (m_commandTitle != v) {
      m_commandTitle = v;
      emit formChanged();
    }
  }
  void setCommandSubtitle(const QString &v) {
    if (m_commandSubtitle != v) {
      m_commandSubtitle = v;
      emit formChanged();
    }
  }
  void setCommandDescription(const QString &v) {
    if (m_commandDescription != v) {
      m_commandDescription = v;
      emit formChanged();
    }
  }

  Q_INVOKABLE void submit();
  Q_INVOKABLE void selectTemplate(const QVariantMap &item);

signals:
  void formChanged();
  void errorsChanged();

private:
  QString m_author;
  QString m_title;
  QString m_description;
  QString m_location;

  QString m_commandTitle;
  QString m_commandSubtitle;
  QString m_commandDescription;
  QVariantMap m_selectedTemplate;

  QVariantList m_templateItems;

  QString m_authorError;
  QString m_titleError;
  QString m_descriptionError;
  QString m_locationError;
  QString m_commandTitleError;
  QString m_commandSubtitleError;
  QString m_commandDescriptionError;
};
