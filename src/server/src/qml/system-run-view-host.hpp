#pragma once
#include "program-db/program-db.hpp"
#include "bridge-view.hpp"

class SystemRunModel;

class SystemRunViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;

  QObject *listModel() const;

private:
  void refresh(const QString &text);

  SystemRunModel *m_model = nullptr;
  ProgramDb m_programDb;
};
