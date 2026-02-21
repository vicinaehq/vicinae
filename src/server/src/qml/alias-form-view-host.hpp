#pragma once
#include "bridge-view.hpp"
#include "common/entrypoint.hpp"

class AliasFormViewHost : public FormViewBase {
  Q_OBJECT

  Q_PROPERTY(QString alias READ alias WRITE setAlias NOTIFY formChanged)
  Q_PROPERTY(QString aliasError READ aliasError NOTIFY errorsChanged)

public:
  explicit AliasFormViewHost(EntrypointId id);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;

  Q_INVOKABLE void submit();

  QString alias() const { return m_alias; }
  QString aliasError() const { return m_aliasError; }

  void setAlias(const QString &v) { if (m_alias != v) { m_alias = v; emit formChanged(); } }

signals:
  void formChanged();
  void errorsChanged();

private:
  EntrypointId m_id;
  QString m_alias;
  QString m_aliasError;
};
