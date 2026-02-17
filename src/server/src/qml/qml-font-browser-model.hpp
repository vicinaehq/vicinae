#pragma once
#include "qml-fuzzy-list-model.hpp"

template <> struct fuzzy::FuzzySearchable<QString> {
  static int score(const QString &item, std::string_view query) {
    auto name = item.toStdString();
    return fuzzy::scoreWeighted({{name, 1.0}}, query);
  }
};

class QmlFontBrowserModel : public QmlFuzzyListModel<QString> {
  Q_OBJECT

public:
  using QmlFuzzyListModel::QmlFuzzyListModel;

signals:
  void fontSelected(const QString &family);

protected:
  QString displayTitle(const QString &item) const override;
  QString displayIconSource(const QString &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const QString &item) const override;
  void itemSelected(const QString &item) override { emit fontSelected(item); }
  QString sectionLabel() const override { return QStringLiteral("Fonts ({count})"); }
};
