#include "ui/default-list-item-widget.hpp"
#include "omni-icon.hpp"
#include <qwidget.h>

void DefaultListItemWidget::setName(const QString &name) { this->_name->setText(name); }

void DefaultListItemWidget::setIcon(const QString &name) { _icon->setIcon(name, {25, 25}); }

void DefaultListItemWidget::setCategory(const QString &category) { _category->setText(category); }

void DefaultListItemWidget::setKind(const QString &kind) { _kind->setText(kind); }

DefaultListItemWidget::DefaultListItemWidget(const OmniIconUrl &iconUrl, const QString &name,
                                             const QString &category, const QString &kind, QWidget *parent)
    : SelectableOmniListWidget(parent), _icon(new OmniIcon), _name(new QLabel), _category(new QLabel),
      _kind(new QLabel) {

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

  this->_kind->setText(kind);
  this->_kind->setProperty("subtext", true);
  mainLayout->addWidget(this->_kind, 0, Qt::AlignRight);

  setLayout(mainLayout);
}
