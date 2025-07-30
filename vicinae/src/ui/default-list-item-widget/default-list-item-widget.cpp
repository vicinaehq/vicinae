#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "common.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include <qnamespace.h>
#include <qsizepolicy.h>
#include <qwidget.h>

void DefaultListItemWidget::setName(const QString &name) {
  _name->setText(name);
  _name->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  _name->setVisible(!name.isEmpty());
  //_name->setFixedWidth(_name->sizeHint().width());
}

void DefaultListItemWidget::setIconUrl(const std::optional<OmniIconUrl> &url) {
  if (url) { _icon->setUrl(*url); }

  _icon->setVisible(url.has_value());
}

void DefaultListItemWidget::setAccessories(const AccessoryList &list) {
  _accessoryList->setAccessories(list);
  _accessoryList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void DefaultListItemWidget::setSubtitle(const std::variant<QString, std::filesystem::path> &subtitle) {
  // clang-format off
  const auto visitor = overloads {
	   [&](const std::filesystem::path& path){
  		_category->setText(path.c_str());
  		_category->setEllideMode(Qt::ElideMiddle);
	   },
	   [&](const QString& text){
  		_category->setText(text);
  		_category->setEllideMode(Qt::ElideRight);
	   }
  };
  // clang-format on

  std::visit(visitor, subtitle);
}

void DefaultListItemWidget::setAlias(const QString &alias) {
  if (!alias.isEmpty()) {
    m_alias->setAccessory(ListAccessory{
        .text = alias,
        .color = SemanticColor::TextPrimary,
        .fillBackground = true,
    });
  }
  m_alias->setVisible(!alias.isEmpty());
}

DefaultListItemWidget::DefaultListItemWidget(QWidget *parent) : SelectableOmniListWidget(parent) {

  _category->setColor(SemanticColor::TextSecondary);
  _icon->setFixedSize(25, 25);

  setAttribute(Qt::WA_Hover);

  auto mainLayout = new QHBoxLayout();

  mainLayout->setContentsMargins(10, 8, 10, 8);

  auto left = new QWidget();
  auto leftLayout = new QHBoxLayout();

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

  setLayout(mainLayout);
}
