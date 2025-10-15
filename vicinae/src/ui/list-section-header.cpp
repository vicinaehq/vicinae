#include "ui/list-section-header.hpp"
#include "theme.hpp"
#include "ui/typography/typography.hpp"

OmniListSectionHeader::OmniListSectionHeader(const QString &title, const QString &subtitle, size_t count) {
  setAttribute(Qt::WA_StyledBackground);

  auto layout = new QHBoxLayout();

  layout->setContentsMargins(8, 8, 8, 8);

  auto leftWidget = new QWidget();
  auto leftLayout = new QHBoxLayout();

  auto titleLabel = new TypographyWidget();

  titleLabel->setColor(SemanticColor::TextMuted);
  titleLabel->setText(title);
  titleLabel->setFontWeight(QFont::Bold);
  titleLabel->setSize(TextSize::TextSmaller);
  // titleLabel->setVisible(!title.isEmpty());

  auto subtitleLabel = new TypographyWidget();

  subtitleLabel->setColor(SemanticColor::TextMuted);
  subtitleLabel->setText(subtitle);
  subtitleLabel->setSize(TextSize::TextSmaller);
  // subtitleLabel->setVisible(!subtitle.isEmpty());

  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->setSpacing(10);
  leftLayout->addWidget(titleLabel);
  leftLayout->addWidget(subtitleLabel);
  leftWidget->setLayout(leftLayout);

  layout->addWidget(leftWidget, 0, Qt::AlignLeft | Qt::AlignVCenter);
  // layout->addWidget(new TextLabel(subtitle), 0, Qt::AlignRight | Qt::AlignVCenter);

  setLayout(layout);
}
