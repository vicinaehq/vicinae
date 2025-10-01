#pragma once
#include "common.hpp"
#include "daemon/ipc-client.hpp"
#include "navigation-controller.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/views/list-view.hpp"
#include <qconstructormacros.h>
#include <qnamespace.h>
#include "services/clipboard/clipboard-service.hpp"
#include <qtmetamacros.h>
#include <ranges>

class DMenuListView : public SearchableListView {
  Q_OBJECT

signals:
  void selected(const QString &text) const;
  void aborted() const;

private:
  class Item : public SearchableListView::Actionnable {
  public:
    Item(const QString &text) : m_text(text) {}

    QString generateId() const override { return QUuid::createUuid().toString(); };
    std::vector<QString> searchStrings() const override { return {m_text}; }
    ItemData data() const override {
      return {
          .name = m_text,
      };
    }
    const QString &text() const { return m_text; }

  private:
    QString m_text;
  };

public:
  void itemSelected(const OmniList::AbstractVirtualItem *item) override {
    QString text = static_cast<const Item *>(item)->text();
    auto panel = std::make_unique<ListActionPanelState>();
    auto main = panel->createSection();
    auto select = new StaticAction("Select entry", ImageURL::builtin("save-document"),
                                   [this, text]() { selectEntry(text); });
    auto selectAndCopy = new StaticAction("Select and copy entry", ImageURL::builtin("copy-clipboard"),
                                          [this, text](ApplicationContext *ctx) {
                                            ctx->services->clipman()->copyText(text);
                                            selectEntry(text);
                                          });

    main->addAction(select);
    main->addAction(selectAndCopy);
    setActions(std::move(panel));
  }

  bool showBackButton() const override { return false; }
  bool onBackspace() override { return true; }

  QString initialNavigationTitle() const override { return m_data.navigationTitle.c_str(); }

  QString initialSearchPlaceholderText() const override {
    return m_data.placeholder.empty() ? "Search entries..." : m_data.placeholder.c_str();
  }

  QString sectionName() const override {
    if (m_data.noSection) return QString();
    return m_data.sectionTitle.empty() ? "Entries ({count})" : m_data.sectionTitle.c_str();
  }

  Data initData() const override {
    Data data;
    auto source = QString::fromStdString(m_data.raw);
    auto lines = source.split('\n', Qt::SkipEmptyParts);
    auto trim = [](const QString &s) { return s.trimmed(); };
    auto filter = [](const QString &s) { return !s.isEmpty(); };

    data.reserve(lines.size());

    for (const auto &menu : lines | std::views::transform(trim) | std::views::filter(filter)) {
      data.emplace_back(std::make_shared<Item>(menu));
    }

    return data;
  }

  void beforePop() override {
    if (!m_selected) { emit selected(""); }
  }

  DMenuListView(DaemonIpcClient::DmenuPayload data) : m_data(data) {}

private:
  void selectEntry(const QString &text) {
    m_selected = true;
    emit selected(text);
    context()->navigation->closeWindow();
  }

  bool m_selected = false;
  DaemonIpcClient::DmenuPayload m_data;
};
