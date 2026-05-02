#pragma once
#include "bridge-view.hpp"
#include "config/config.hpp"
#include "section-list-model.hpp"
#include "theme.hpp"
#include "theme-list-model.hpp"

class ThemeViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  QUrl qmlComponentUrl() const override { return QUrl(QStringLiteral("qrc:/Vicinae/ThemeListView.qml")); }

  QVariantMap qmlProperties() override {
    return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(&m_model))}};
  }

  void initialize() override;
  void textChanged(const QString &text) override;
  void onReactivated() override { m_model.refreshActionPanel(); }
  void beforePop() override;

  QObject *listModel() const { return const_cast<SectionListModel *>(&m_model); }

private:
  void regenerateThemes();

  SectionListModel m_model{this};
  ThemeSection m_currentSection;
  ThemeSection m_availableSection;
  ThemeService *m_themeService = nullptr;
  config::Manager *m_config = nullptr;
  QString m_query;
};
