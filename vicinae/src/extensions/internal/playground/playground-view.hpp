#include "layout.hpp"
#include "services/emoji-service/emoji.hpp"
#include "ui/views/base-view.hpp"
#include "ui/vlist/vlist.hpp"
#include <qwizard.h>
#include "ui/vlist/common/simple-grid-model.hpp"

enum class EmojiSection { Main };

class EmojiGridModel : public vicinae::ui::SimpleGridModel<EmojiData, EmojiSection> {
  GridData createItemData(const EmojiData &item) const override {
    return {.icon = ImageURL::emoji(QString::fromUtf8(item.emoji.data(), item.emoji.size()))};
  }

  VListModel::StableID stableId(const EmojiData &item) const override { return hash(item.emoji); }

  EmojiData sectionItemAt(EmojiSection id, int itemIdx) const override {
    return StaticEmojiDatabase::orderedList()[itemIdx];
  }

  int sectionCount() const override { return 1; }
  double sectionAspectRatio(EmojiSection id) const override { return 1; }
  int sectionColumns(EmojiSection id) const override { return 8; }
  int sectionItemCount(EmojiSection id) const override { return StaticEmojiDatabase::orderedList().size(); }
  EmojiSection sectionIdFromIndex(int idx) const override { return EmojiSection::Main; }
  std::string_view sectionName(EmojiSection id) const override { return "Emojis"; }
};

class PlaygroundView : public BaseView {
public:
  PlaygroundView() {
    VStack().add(m_list).imbue(this);
    m_list->setModel(m_model);
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
