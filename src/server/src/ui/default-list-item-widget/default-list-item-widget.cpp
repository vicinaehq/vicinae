#include <qsizepolicy.h>
#include <qwidget.h>
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "common/types.hpp"
#include "ui/image/url.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/image/image.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include "utils/layout.hpp"

namespace fs = std::filesystem;

void DefaultListItemWidget::setName(const QString &name) {
  m_name->setText(name);
  m_name->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  m_name->setVisible(!name.isEmpty());
  //_name->setFixedWidth(_name->sizeHint().width());
}

void DefaultListItemWidget::setIconUrl(const std::optional<ImageURL> &url) {
  if (url) { m_icon->setUrl(*url); }

  m_icon->setVisible(url.has_value());
}

void DefaultListItemWidget::setAccessories(const AccessoryList &list) {
  m_accessoryList->setAccessories(list);
  m_accessoryList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void DefaultListItemWidget::setSubtitle(const std::variant<QString, std::filesystem::path> &subtitle) {
  // clang-format off
  const auto visitor = overloads {
	   [&](const fs::path& path){
  		m_category->setText(path.c_str());
  		m_category->setEllideMode(Qt::ElideMiddle);
	   },
	   [&](const QString& text){
  		m_category->setText(text);
  		m_category->setEllideMode(Qt::ElideRight);
		m_category->setVisible(!text.isEmpty());
	   }
  };
  // clang-format on

  std::visit(visitor, subtitle);
}

void DefaultListItemWidget::setActive(bool active) {
  if (active) positionActiveIndicator();
  m_activeIndicator->setVisible(active);
}

void DefaultListItemWidget::setAlias(const QString &alias) {
  if (!alias.isEmpty()) { m_alias->setText(alias); }
  m_alias->setVisible(!alias.isEmpty());
}

void DefaultListItemWidget::selectionChanged(bool selected) {
  m_name->setColor(selected ? SemanticColor::ListItemSelectionForeground : SemanticColor::TextPrimary);
  m_category->setColor(selected ? SemanticColor::ListItemSelectionForeground : SemanticColor::TextMuted);
  SelectableOmniListWidget::selectionChanged(selected);
}

void DefaultListItemWidget::positionActiveIndicator() {
  auto margins = layout()->contentsMargins();
  int x = margins.left() + m_icon->width() / 2 - m_activeIndicator->width() / 2;
  int y = margins.top() + m_icon->height() + 3;

  m_activeIndicator->move(x, y);
}

void DefaultListItemWidget::resizeEvent(QResizeEvent *event) {
  if (m_activeIndicator->isVisible()) positionActiveIndicator();
  SelectableOmniListWidget::resizeEvent(event);
}

DefaultListItemWidget::DefaultListItemWidget(QWidget *parent) : SelectableOmniListWidget(parent) {
  auto left = HStack().spacing(15).add(m_icon).add(m_name).add(m_category).add(m_alias);

  HStack().mx(10).my(8).spacing(10).add(left).add(m_accessoryList).justifyBetween().imbue(this);

  m_category->setColor(SemanticColor::TextMuted);
  m_icon->setFixedSize(25, 25);
  m_activeIndicator->setFixedSize(4, 4);
  m_activeIndicator->setColor(SemanticColor::TextMuted);
  m_alias->hide();
  setActive();
}
