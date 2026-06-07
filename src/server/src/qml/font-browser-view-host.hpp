#pragma once
#include "bridge-view.hpp"
#include "font-grid-model.hpp"
#include "view-scope.hpp"
#include <QStringList>

class FontBrowserViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QStringList categoryFilterOptions READ categoryFilterOptions CONSTANT)
  Q_PROPERTY(int currentCategoryFilter READ currentCategoryFilter NOTIFY currentCategoryFilterChanged)

signals:
  void currentCategoryFilterChanged();

public:
  QUrl qmlComponentUrl() const override { return QUrl(QStringLiteral("qrc:/Vicinae/BrowseFontsView.qml")); }
  QUrl qmlSearchAccessoryUrl() const override {
    return QUrl(QStringLiteral("qrc:/Vicinae/CategoryFilterAccessory.qml"));
  }

  QVariantMap qmlProperties() override {
    return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(&m_model))}};
  }

  void initialize() override {
    BaseView::initialize();
    m_model.setScope(ViewScope(context(), this));
    m_model.initialize();
    setSearchPlaceholderText(m_model.searchPlaceholder());
    restoreCategoryFilter();
  }

  void textChanged(const QString &text) override { m_model.setFilter(text); }
  void loadInitialData() override { m_model.setFilter(searchText()); }

  QStringList categoryFilterOptions() const {
    QStringList options{QStringLiteral("All")};
    options << m_model.categoryNames();
    return options;
  }

  int currentCategoryFilter() const { return m_currentCategoryFilter; }

  Q_INVOKABLE void setCategoryFilter(int index) {
    if (index == m_currentCategoryFilter) return;
    m_currentCategoryFilter = index;
    emit currentCategoryFilterChanged();

    m_model.setCategoryFilter(index <= 0 ? std::nullopt : std::optional<int>(index - 1));
    command()->storage().setItem("fontCategory", categoryFilterOptions().value(index));

    if (!searchText().isEmpty()) m_model.setFilter(searchText());
  }

private:
  void restoreCategoryFilter() {
    const auto saved = command()->storage().getItem("fontCategory");
    if (saved.isUndefined() || saved.isNull()) return;
    const int index = categoryFilterOptions().indexOf(saved.toString());
    if (index > 0) setCategoryFilter(index);
  }

  FontGridModel m_model{this};
  int m_currentCategoryFilter = 0;
};
