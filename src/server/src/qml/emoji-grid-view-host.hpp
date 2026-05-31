#pragma once
#include "bridge-view.hpp"
#include "emoji-grid-model.hpp"
#include "glyph/glyph.hpp"
#include <QStringList>

class EmojiGridViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)
  Q_PROPERTY(QStringList categoryFilterOptions READ categoryFilterOptions CONSTANT)
  Q_PROPERTY(int currentCategoryFilter READ currentCategoryFilter NOTIFY currentCategoryFilterChanged)

public:
  QUrl qmlComponentUrl() const override { return QUrl(QStringLiteral("qrc:/Vicinae/EmojiGridView.qml")); }
  QUrl qmlSearchAccessoryUrl() const override {
    return QUrl(QStringLiteral("qrc:/Vicinae/EmojiCategoryFilterAccessory.qml"));
  }

  QVariantMap qmlProperties() override {
    return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(&m_model))}};
  }

  void initialize() override {
    BaseView::initialize();
    m_model.setScope(ViewScope(context(), this));
    m_model.initialize();
    setSearchPlaceholderText(m_model.searchPlaceholder());
  }

  void textChanged(const QString &text) override { m_model.setFilter(text); }
  void beforePop() override { m_model.beforePop(); }
  void onReactivated() override { m_model.refreshActionPanel(); }
  void loadInitialData() override { m_model.setFilter(searchText()); }

  QObject *listModel() const { return const_cast<EmojiGridModel *>(&m_model); }

  QStringList categoryFilterOptions() const {
    QStringList options{QStringLiteral("All")};
    for (const auto &section : glyph::sections())
      options.append(QString::fromUtf8(section.label.data(), section.label.size()));
    return options;
  }

  int currentCategoryFilter() const { return m_currentCategoryFilter; }

  Q_INVOKABLE void setCategoryFilter(int index) {
    if (m_currentCategoryFilter == index) return;
    m_currentCategoryFilter = index;
    emit currentCategoryFilterChanged();

    auto sections = glyph::sections();
    std::optional<glyph::Category> category;
    if (index >= 1 && index - 1 < static_cast<int>(sections.size())) category = sections[index - 1].category;
    m_model.setCategoryFilter(category);

    if (!searchText().isEmpty()) clearSearchText();
  }

signals:
  void currentCategoryFilterChanged();

private:
  EmojiGridModel m_model{this};
  int m_currentCategoryFilter = 0;
};
