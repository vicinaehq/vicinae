#include "ui/list-section-header.hpp"
#include "layout.hpp"
#include "theme.hpp"
#include "ui/typography/typography.hpp"

OmniListSectionHeader::OmniListSectionHeader(const QString &title, const QString &subtitle, size_t count) {
  setupUI();
  setTitle(title);
}

void OmniListSectionHeader::setupUI() {
  m_title = new TypographyWidget(this);
  m_title->setColor(SemanticColor::TextMuted);
  m_title->setFontWeight(QFont::Weight::DemiBold);
  m_title->setSize(TextSize::TextSmaller);
  HStack().add(m_title).margins(8).imbue(this);
}
