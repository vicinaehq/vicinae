#include "empty-view.hpp"
#include "ui/typography/typography.hpp"

void EmptyViewWidget::setupUi() {
  auto layout = new QVBoxLayout();
  auto container = new QVBoxLayout();

  m_title = new TypographyWidget(this);
  m_description = new TypographyWidget(this);
  m_description->setColor(SemanticColor::TextSecondary);
  m_description->setWordWrap(true);
  m_icon->setFixedSize(48, 48);
  container->setAlignment(Qt::AlignCenter);
  layout->setSpacing(10);
  layout->addWidget(m_icon, 0, Qt::AlignCenter);
  layout->addWidget(m_title, 0, Qt::AlignCenter);
  layout->addWidget(m_description, 0, Qt::AlignCenter);
  container->addLayout(layout);
  setLayout(container);
}

void EmptyViewWidget::setTitle(const QString &title) {
  m_title->setText(title);
  m_title->setVisible(!title.isEmpty());
}

void EmptyViewWidget::setDescription(const QString &description) {
  m_description->setText(description);
  m_description->setVisible(!description.isEmpty());
}

void EmptyViewWidget::setIcon(const std::optional<OmniIconUrl> url) {
  if (url) m_icon->setUrl(*url);
  m_icon->setVisible(url.has_value());
}

EmptyViewWidget::EmptyViewWidget(QWidget *parent) : QWidget(parent) { setupUi(); }
