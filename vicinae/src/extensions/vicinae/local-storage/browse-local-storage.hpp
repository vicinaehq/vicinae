#pragma once
#include "common.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include "ui/views/typed-list-view.hpp"

using LocalStorageNamespace = QString;

class BrowseLocalStorageNamespaceView : public FilteredTypedListView<QString> {
  Q_OBJECT

signals:
  void valueShown() const;

public:
  BrowseLocalStorageNamespaceView(const QString &ns) : m_ns(ns) {
    connect(this, &BrowseLocalStorageNamespaceView::valueShown, this, [this]() {
      m_value = !m_value;
      refreshCurrent();
    });
  }

  QString initialSearchPlaceholderText() const override { return "Search items..."; }

protected:
  FilteredItemData mapFilteredData(const QString &item) const override {
    return FilteredItemData{
        .id = item.toStdString(), .title = item.toStdString(), .icon = ImageURL::builtin("coin")};
  }

  QWidget *generateDetail(const QString &item) const override {
    if (m_value) {
      auto md = new MarkdownRenderer;
      md->setMarkdown(context()->services->localStorage()->getItem(m_ns, item).toString());
      return md;
    }
    return nullptr;
  }

  std::unique_ptr<ActionPanelState> createActionPanel(const QString &item) const override {
    auto panel = std::make_unique<ActionPanelState>();
    auto section = panel->createSection();
    const char *title = m_value ? "Hide value" : "Show value";
    auto toggleDetails = new StaticAction(title, ImageURL::builtin("coin"),
                                          [this](ApplicationContext *ctx) { emit valueShown(); });

    section->addAction(toggleDetails);

    return panel;
  }

  DataSet initializeDataSet() override {
    Section namespaces{"Items", context()->services->localStorage()->listNamespaceItems(m_ns).keys() |
                                    std::ranges::to<std::vector>()};

    return {namespaces};
  }

private:
  QString m_ns;
  bool m_value = false;
};

class BrowseLocalStorageView : public FilteredTypedListView<LocalStorageNamespace> {
  std::unique_ptr<ActionPanelState> createActionPanel(const LocalStorageNamespace &item) const override {
    auto panel = std::make_unique<ActionPanelState>();
    auto section = panel->createSection();
    auto browse =
        new StaticAction("Browse namespace", ImageURL::builtin("coin"), [&](ApplicationContext *ctx) {
          ctx->navigation->pushView(new BrowseLocalStorageNamespaceView(item));
        });

    section->addAction(browse);

    return panel;
  }

  FilteredItemData mapFilteredData(const QString &item) const override {
    return FilteredItemData{
        .id = item.toStdString(), .title = item.toStdString(), .icon = ImageURL::builtin("coin")};
  }

  DataSet initializeDataSet() override {
    return {Section{"Namespaces", context()->services->localStorage()->namespaces()}};
  }

  QString initialSearchPlaceholderText() const override { return "Search namespaces..."; }
};
