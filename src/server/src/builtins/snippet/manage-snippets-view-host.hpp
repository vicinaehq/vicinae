#pragma once
#include "services/builtin-icon/builtin-icon.hpp"
#include "ui/views/detail-list-view-host.hpp"
#include "builtins/snippet/manage-snippets-model.hpp"
#include "services/snippet/snippet-expander.hpp"
#include "ui/views/view-utils.hpp"
#include <QVariantList>

class SnippetService;

class ManageSnippetsViewHost : public DetailListViewHost {
  Q_OBJECT

public:
  ManageSnippetsViewHost();

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void beforePop() override;

  bool hasDetail() const override { return m_hasDetail; }
  QString detailContent() const override { return m_detailContent; }
  QVariantList detailMetadata() const override { return m_detailMetadata; }

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

  std::optional<snippet::SerializedSnippet> m_currentSnippet;
  bool m_hasDetail = false;
  QString m_detailContent;
  QVariantList m_detailMetadata;
};
