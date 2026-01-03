#include "fuzzy/weighted-fuzzy-scorer.hpp"
#include "layout.hpp"
#include "services/emoji-service/emoji-service.hpp"
#include "emoji/emoji.hpp"
#include "ui/views/base-view.hpp"
#include "service-registry.hpp"
#include "ui/vlist/vlist.hpp"
#include <ranges>
#include "ui/vlist/common/simple-grid-model.hpp"

class EmojiGridModel : public vicinae::ui::SimpleGridModel<EmojiData, int> {
public:
  void setGroupedEmojis(EmojiService::GroupedEmojis emojis) {
    m_grouped = emojis;
    emit dataChanged();
  }

  void setSearchResults(std::span<Scored<const EmojiData *>> results) {
    m_results = results;
    emit dataChanged();
  }

  void setRootMode(bool root) {
    m_root = root;
    emit dataChanged();
  }

  GridData createItemData(const EmojiData &item) const override {
    return {.icon = ImageURL::emoji(QString::fromUtf8(item.emoji.data(), item.emoji.size()))};
  }

  VListModel::StableID stableId(const EmojiData &item, int sectionId) const override {
    return hash(item.emoji);
  }

  EmojiData sectionItemAt(int id, int itemIdx) const override {
    if (m_root) { return *m_grouped[id].second[itemIdx]; }
    return *m_results[itemIdx].data;
  }

  int sectionCount() const override {
    if (m_root) return m_grouped.size();
    return 1;
  }
  double sectionAspectRatio(int id) const override { return 1; }
  int sectionColumns(int id) const override { return 8; }
  int sectionItemCount(int id) const override {
    if (m_root) return m_grouped[id].second.size();
    return m_results.size();
  }
  int sectionIdFromIndex(int idx) const override { return idx; }
  std::string_view sectionName(int id) const override {
    if (m_root) return m_grouped[id].first;
    return "Results";
  }

private:
  bool m_root = true;
  std::span<Scored<const EmojiData *>> m_results;
  EmojiService::GroupedEmojis m_grouped;
};

class PlaygroundView : public BaseView {
public:
  PlaygroundView() { VStack().add(m_list).imbue(this); }

  void initialize() override {
    m_list->setModel(m_model);
    m_model->setGroupedEmojis(context()->services->emojiService()->grouped());
  }

  bool inputFilter(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Up:
      return m_list->selectUp();
    case Qt::Key_Down:
      return m_list->selectDown();
    case Qt::Key_Left:
      return m_list->selectLeft();
    case Qt::Key_Right:
      return m_list->selectRight();
    default:
      return false;
    }
  }

  void textChanged(const QString &text) override {
    if (text.isEmpty()) {
      m_model->setRootMode(true);
      m_list->selectFirst();
      return;
    }

    setFilter(text);
    m_list->selectFirst();
  }

  void setFilter(const QString &text) {
    std::string pattern = text.toStdString();
    auto withScore = [&](const EmojiData &data) -> Scored<const EmojiData *> {
      fuzzy::WeightedScorer scorer;
      scorer.add(std::string(data.name), 1);
      scorer.add(std::string(data.group), 0.7);
      for (const auto &kw : data.keywords) {
        scorer.add(std::string(kw), 0.3);
      }
      return {&data, scorer.score(pattern)};
    };

    auto filtered = emoji::emojis() | std::views::transform(withScore) |
                    std::views::filter([](auto &&s) { return s.score > 0; });
    m_results.clear();
    std::ranges::copy(filtered, std::back_inserter(m_results));
    std::ranges::stable_sort(m_results, std::greater{});

    m_model->setSearchResults(m_results);
    m_model->setRootMode(false);
  }

private:
  EmojiGridModel *m_model = new EmojiGridModel;
  vicinae::ui::VListWidget *m_list = new vicinae::ui::VListWidget;

  std::vector<Scored<const EmojiData *>> m_results;
};
