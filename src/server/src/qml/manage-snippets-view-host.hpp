#pragma once
#include "bridge-view.hpp"
#include "services/snippet/snippet-db.hpp"
#include "services/snippet/snippet-expander.hpp"
#include <QVariantList>

class ManageSnippetsModel;
class SnippetService;

class ManageSnippetsViewHost : public ViewHostBase {
  Q_OBJECT

  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)
  Q_PROPERTY(bool hasDetail READ hasDetail NOTIFY detailChanged)
  Q_PROPERTY(QString detailContent READ detailContent NOTIFY detailChanged)
  Q_PROPERTY(QVariantList detailMetadata READ detailMetadata NOTIFY detailChanged)

signals:
  void detailChanged();

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;
  void beforePop() override;

  QObject *listModel() const;
  bool hasDetail() const { return m_hasDetail; }
  QString detailContent() const { return m_detailContent; }
  QVariantList detailMetadata() const { return m_detailMetadata; }

private:
  void loadDetail(const snippet::SerializedSnippet &snippet);
  void updateExpandedText();
  void clearDetail();
  void reload();

  ManageSnippetsModel *m_model = nullptr;
  SnippetService *m_snippetService = nullptr;
  SnippetExpander m_expander;

  std::optional<snippet::SerializedSnippet> m_currentSnippet;
  bool m_hasDetail = false;
  QString m_detailContent;
  QVariantList m_detailMetadata;
};
