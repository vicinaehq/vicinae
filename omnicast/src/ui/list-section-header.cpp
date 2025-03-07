#include "ui/list-section-header.hpp"
#include "ui/text-label.hpp"

OmniListSectionHeader::OmniListSectionHeader(const QString &title, const QString &subtitle, size_t count) {
  setAttribute(Qt::WA_StyledBackground);

  auto layout = new QHBoxLayout();

  layout->setContentsMargins(8, 8, 8, 8);

  auto leftWidget = new QWidget();
  auto leftLayout = new QHBoxLayout();

  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->setSpacing(10);
  leftLayout->addWidget(new TextLabel(title));
  if (count > 0) { leftLayout->addWidget(new TextLabel(QString::number(count))); }
  leftWidget->setLayout(leftLayout);

  layout->addWidget(leftWidget, 0, Qt::AlignLeft | Qt::AlignVCenter);
  layout->addWidget(new TextLabel(subtitle), 0, Qt::AlignRight | Qt::AlignVCenter);

  setLayout(layout);
}
