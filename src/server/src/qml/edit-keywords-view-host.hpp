#pragma once
#include "bridge-view.hpp"
#include <functional>

class EditKeywordsViewHost : public FormViewBase {
  Q_OBJECT

signals:
  void formChanged();

public:
  Q_PROPERTY(QString keywords READ keywords WRITE setKeywords NOTIFY formChanged)
  Q_PROPERTY(QString infoText READ infoText CONSTANT)

  using LoadFn = std::function<QString()>;
  using SaveFn = std::function<bool(const QString &)>;

  EditKeywordsViewHost(LoadFn loadKeywords, SaveFn saveKeywords, const QString &infoText);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;

  QString keywords() const { return m_keywords; }
  QString infoText() const { return m_infoText; }

  void setKeywords(const QString &v) {
    if (m_keywords != v) {
      m_keywords = v;
      emit formChanged();
    }
  }

  Q_INVOKABLE void submit();

private:
  LoadFn m_loadKeywords;
  SaveFn m_saveKeywords;
  QString m_infoText;
  QString m_keywords;
};
