#pragma once
#include "fuzzy-section.hpp"

class FontBrowserSection : public FuzzySection<QString> {
public:
  QString sectionName() const override { return QStringLiteral("Fonts ({count})"); }

  void setOnFontSelected(std::function<void(const QString &)> cb) { m_onFontSelected = std::move(cb); }

  void onSelected(int i) override {
    if (m_onFontSelected) m_onFontSelected(at(i));
  }

protected:
  QString displayTitle(const QString &item) const override;
  QString displayIconSource(const QString &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const QString &item) const override;

private:
  std::function<void(const QString &)> m_onFontSelected;
};
