#include "empty-view.hpp"
#include "ui/typography/typography.hpp"
#include "utils/layout.hpp"
#include <google/protobuf/stubs/common.h>
#include <qboxlayout.h>
#include <qnamespace.h>

void EmptyViewWidget::setupUi() {
  auto layout = new QVBoxLayout();
  auto container = new QVBoxLayout();

  m_title = new TypographyWidget(this);
  m_description = new TypographyWidget(this);
  m_description->setColor(SemanticColor::TextMuted);
  m_description->setWordWrap(true);
  m_icon->setFixedSize(48, 48);

  auto content =
      VStack().spacing(10).add(m_icon, 0, Qt::AlignCenter).add(m_title).add(m_description).buildWidget();

  m_title->setAlignment(Qt::AlignCenter);
  m_description->setAlignment(Qt::AlignCenter);
  content->setFixedWidth(400);

  HStack().add(VStack().add(content).center()).center().imbue(this);
}

void EmptyViewWidget::setTitle(const QString &title) {
  m_title->setText(title);
  m_title->setVisible(!title.isEmpty());
}

void EmptyViewWidget::setDescription(const QString &description) {
  m_description->setText(description);
  m_description->setVisible(!description.isEmpty());
}

void EmptyViewWidget::setIcon(const std::optional<ImageURL> url) {
  if (url) m_icon->setUrl(*url);
  m_icon->setVisible(url.has_value());
}

EmptyViewWidget::EmptyViewWidget(QWidget *parent) : QWidget(parent) { setupUi(); }
