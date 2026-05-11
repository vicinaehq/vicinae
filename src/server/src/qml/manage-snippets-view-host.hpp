#pragma once
#include "builtin_icon.hpp"
#include "list-view-host.hpp"
#include "manage-snippets-model.hpp"
#include "services/snippet/snippet-expander.hpp"
#include "view-utils.hpp"
#include <QVariantList>

class SnippetService;

class ManageSnippetsViewHost : public ListViewHost {
  Q_OBJECT

  Q_PROPERTY(bool hasDetail READ hasDetail NOTIFY detailChanged)
  Q_PROPERTY(QString detailContent READ detailContent NOTIFY detailChanged)
  Q_PROPERTY(QVariantList detailMetadata READ detailMetadata NOTIFY detailChanged)
  Q_PROPERTY(QString emptyTitle MEMBER m_emptyTitle CONSTANT)
  Q_PROPERTY(QString emptyDescription MEMBER m_emptyDescription CONSTANT)
  Q_PROPERTY(QString emptyIcon MEMBER m_emptyIcon CONSTANT)

signals:
  void detailChanged();

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void onReactivated() override;
  void beforePop() override;

  bool hasDetail() const { return m_hasDetail; }
  QString detailContent() const { return m_detailContent; }
  QVariantList detailMetadata() const { return m_detailMetadata; }

  Q_INVOKABLE void createSnippet();

private:
  void loadDetail(const snippet::SerializedSnippet &snippet);
  void updateExpandedText();
  void clearDetail();
  void setEmptyActions();
  void reload();

  ManageSnippetsSection m_section;
  SnippetService *m_snippetService = nullptr;
  SnippetExpander m_expander;

  std::optional<snippet::SerializedSnippet> m_currentSnippet;
  bool m_hasDetail = false;
  QString m_detailContent;
  QVariantList m_detailMetadata;
  QString m_emptyTitle = QStringLiteral("No snippets");
  QString m_emptyDescription = QStringLiteral("Create a snippet to get started");
  QString m_emptyIcon = qml::imageSourceFor(ImageURL(BuiltinIcon::Snippets));
};
