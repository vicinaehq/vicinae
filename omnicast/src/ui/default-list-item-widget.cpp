#include "ui/default-list-item-widget.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/list-accessory-widget.hpp"
#include <qwidget.h>

void DefaultListItemWidget::setName(const QString &name) {
  _name->setText(name);
  _name->setVisible(!name.isEmpty());
}

void DefaultListItemWidget::setIconUrl(const OmniIconUrl &url) { _icon->setUrl(url); }

void DefaultListItemWidget::setAccessories(const AccessoryList &list) {
  _accessoryList->setAccessories(list);
}

void DefaultListItemWidget::setCategory(const QString &category) {
  _category->setText(category);
  _category->setVisible(!category.isEmpty());
}

void DefaultListItemWidget::setAlias(const QString &alias) {
  if (!alias.isEmpty()) {
    m_alias->setAccessory(ListAccessory{
        .text = alias,
        .color = ColorTint::TextPrimary,
        .fillBackground = true,
    });
  }
  m_alias->setVisible(!alias.isEmpty());
}

DefaultListItemWidget::DefaultListItemWidget(const OmniIconUrl &iconUrl, const QString &name,
                                             const QString &category, const AccessoryList &accessories,
                                             QWidget *parent)
    : SelectableOmniListWidget(parent), _icon(new Omnimg::ImageWidget), _name(new TypographyWidget),
      _category(new TypographyWidget()), _accessoryList(new AccessoryListWidget(this)),
      m_alias(new ListAccessoryWidget) {

  _category->setColor(ColorTint::TextSecondary);
  _icon->setFixedSize(25, 25);
  _icon->setUrl(iconUrl);

  setAttribute(Qt::WA_Hover);

  auto mainLayout = new QHBoxLayout();

  mainLayout->setContentsMargins(10, 8, 10, 8);

  auto left = new QWidget();
  auto leftLayout = new QHBoxLayout();

  this->_name->setText(name);
  this->_category->setText(category);

  left->setLayout(leftLayout);
  leftLayout->setSpacing(15);
  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->addWidget(this->_icon);
  leftLayout->addWidget(this->_name);
  leftLayout->addWidget(this->_category);
  leftLayout->addWidget(this->m_alias);

  m_alias->hide();

  mainLayout->addWidget(left, 0, Qt::AlignLeft);
  mainLayout->addWidget(this->_accessoryList, 0, Qt::AlignRight);

  this->_accessoryList->setAccessories(accessories);

  setLayout(mainLayout);
}
