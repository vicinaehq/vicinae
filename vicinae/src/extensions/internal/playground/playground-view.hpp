#include "layout.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/views/base-view.hpp"
#include "ui/vlist.hpp"
#include <absl/strings/internal/str_format/extension.h>
#include <format>

class SimpleVListStaticModel : public vicinae::ui::VListModel {
public:
  SimpleVListStaticModel() {
    int n = 10000;
    m_items.reserve(n);
    for (int i = 0; i != n; ++i) {
      m_items.emplace_back(StandardItem{.title = std::format("Item {}", i), .subtitle = "Fancy subitle"});
    }
    setFilter("");
  }

  void setFilter(const std::string &query) {
    m_filteredItems.reserve(m_items.size());
    m_filteredItems.clear();

    auto matches = [&](const std::string &s) { return s.contains(query); };
    for (const auto &item : m_items | std::views::filter([&](const auto &item) {
                              return matches(item.title) || matches(item.subtitle);
                            })) {
      m_filteredItems.push_back(&item);
    }
  }

  size_t height(size_t idx) const override { return 40; }

  size_t count() const override { return m_filteredItems.size(); };

  QWidget *createWidget(size_t idx) const override { return new DefaultListItemWidget; }

  void refreshWidget(size_t idx, QWidget *widget) const override {
    auto w = static_cast<DefaultListItemWidget *>(widget);
    auto &item = m_filteredItems[idx];
    w->setName(item->title.c_str());
    w->setSubtitle(item->subtitle);
    w->setIconUrl(ImageURL::builtin("cog"));
  }

private:
  struct StandardItem {
    std::string title;
    std::string subtitle;
  };

  std::vector<const StandardItem *> m_filteredItems;
  std::vector<StandardItem> m_items;
};

class PlaygroundView : public BaseView {
public:
  PlaygroundView() {
    VStack().add(m_list).imbue(this);
    m_list->setModel(m_model);
  }

  void textChanged(const QString &text) override {
    m_model->setFilter(text.toStdString());
    m_list->calculate();
  }

private:
  SimpleVListStaticModel *m_model = new SimpleVListStaticModel();
  vicinae::ui::VListWidget *m_list = new vicinae::ui::VListWidget;
};
