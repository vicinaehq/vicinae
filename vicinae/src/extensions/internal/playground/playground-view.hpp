#include "layout.hpp"
#include "services/emoji-service/emoji-service.hpp"
#include "services/emoji-service/emoji.hpp"
#include "ui/views/base-view.hpp"
#include "service-registry.hpp"
#include "ui/vlist/vlist.hpp"
#include <qwizard.h>
#include "ui/vlist/common/simple-grid-model.hpp"

class EmojiGridModel : public vicinae::ui::SimpleGridModel<EmojiData, int> {
public:
  void setGroupedEmojis(EmojiService::GroupedEmojis emojis) {
    m_grouped = emojis;
    emit dataChanged();
  }

  GridData createItemData(const EmojiData &item) const override {
    return {.icon = ImageURL::emoji(QString::fromUtf8(item.emoji.data(), item.emoji.size()))};
  }

  VListModel::StableID stableId(const EmojiData &item) const override { return hash(item.emoji); }

  EmojiData sectionItemAt(int id, int itemIdx) const override { return *m_grouped[id].second[itemIdx]; }

  int sectionCount() const override { return m_grouped.size(); }
  double sectionAspectRatio(int id) const override { return 1; }
  int sectionColumns(int id) const override { return 8; }
  int sectionItemCount(int id) const override { return m_grouped[id].second.size(); }
  int sectionIdFromIndex(int idx) const override { return idx; }
  std::string_view sectionName(int id) const override { return m_grouped[id].first; }

private:
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
    }
    return false;
  }

  void textChanged(const QString &text) override { m_list->selectFirst(); }

private:
  EmojiGridModel *m_model = new EmojiGridModel;
  vicinae::ui::VListWidget *m_list = new vicinae::ui::VListWidget;
};
