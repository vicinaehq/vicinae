#include "ui/default-list-item-widget.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include <qwidget.h>

void DefaultListItemWidget::setName(const QString &name) { this->_name->setText(name); }

void DefaultListItemWidget::setIconUrl(const OmniIconUrl &url) { _icon->setUrl(url); }

void DefaultListItemWidget::setAccessories(const AccessoryList &list) {
  _accessoryList->setAccessories(list);
}

void DefaultListItemWidget::setCategory(const QString &category) { _category->setText(category); }

DefaultListItemWidget::DefaultListItemWidget(const OmniIconUrl &iconUrl, const QString &name,
                                             const QString &category, const AccessoryList &accessories,
                                             QWidget *parent)
    : SelectableOmniListWidget(parent), _icon(new OmniIcon),
      _name(new TypographyWidget(TextSize::TextRegular)),
      _category(new TypographyWidget(TextSize::TextRegular, ColorTint::TextSecondary)),
      _accessoryList(new AccessoryListWidget(this)) {

  _icon->setFixedSize(25, 25);
  _icon->setUrl(iconUrl);

  auto mainLayout = new QHBoxLayout();

  mainLayout->setContentsMargins(10, 8, 10, 8);

  auto left = new QWidget();
  auto leftLayout = new QHBoxLayout();

  this->_name->setText(name);
  this->_category->setText(category);
  this->_category->setProperty("subtext", true);

  left->setLayout(leftLayout);
  leftLayout->setSpacing(15);
  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->addWidget(this->_icon);
  leftLayout->addWidget(this->_name);
  leftLayout->addWidget(this->_category);

  mainLayout->addWidget(left, 0, Qt::AlignLeft);
  mainLayout->addWidget(this->_accessoryList, 0, Qt::AlignRight);

  this->_accessoryList->setAccessories(accessories);

  setLayout(mainLayout);
}
