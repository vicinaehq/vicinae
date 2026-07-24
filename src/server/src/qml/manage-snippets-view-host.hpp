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
  Q_PROPERTY(ImageUrl emptyIcon MEMBER m_emptyIcon CONSTANT)

signals:
  void detailChanged();

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void beforePop() override;

  bool hasDetail() const { return m_hasDetail; }
  QString detailContent() const { return m_detailContent; }
  QVariantList detailMetadata() const { return m_detailMetadata; }

  Q_INVOKABLE void createSnippet();

protected:
  std::unique_ptr<ActionPanelState> emptyActionPanel() override;

private:
  void loadDetail(const snippet::SerializedSnippet &snippet);
  void updateExpandedText();
  void clearDetail();
  void reload();

  ManageSnippetsSection m_section;
  SnippetService *m_snippetService = nullptr;
  SnippetExpander m_expander;

  std::optional<snippet::SerializedSnippet> m_currentSnippet;
  bool m_hasDetail = false;
  QString m_detailContent;
  QVariantList m_detailMetadata;
  QString m_emptyTitle = tr("No snippets");
  QString m_emptyDescription = tr("Create a snippet to get started");
  ImageUrl m_emptyIcon = ImageUrl(ImageURL(BuiltinIcon::Snippets));
};
