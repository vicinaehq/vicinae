#pragma once
#include "bridge-view.hpp"

class FontBrowserModel;

class FontBrowserViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)
  Q_PROPERTY(QString selectedFont READ selectedFont NOTIFY selectedFontChanged)
  Q_PROPERTY(QString showcaseMarkdown READ showcaseMarkdown CONSTANT)

signals:
  void selectedFontChanged();

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;
  void beforePop() override;

  QObject *listModel() const;
  QString selectedFont() const { return m_selectedFont; }
  QString showcaseMarkdown() const;

private:
  FontBrowserModel *m_model = nullptr;
  QString m_selectedFont;
};
