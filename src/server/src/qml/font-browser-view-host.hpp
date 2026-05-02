#pragma once
#include "font-browser-model.hpp"
#include "list-view-host.hpp"

class FontBrowserViewHost : public ListViewHost {
  Q_OBJECT
  Q_PROPERTY(QString selectedFont READ selectedFont NOTIFY selectedFontChanged)
  Q_PROPERTY(QString showcaseMarkdown READ showcaseMarkdown CONSTANT)

signals:
  void selectedFontChanged();

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;

  QString selectedFont() const { return m_selectedFont; }
  QString showcaseMarkdown() const;

private:
  FontBrowserSection m_section;
  QString m_selectedFont;
};
