#pragma once
#include <qboxlayout.h>
#include "ui/omni-list/omni-list-item-widget.hpp"
#include "ui/typography/typography.hpp"

class OmniListSectionHeader : public OmniListItemWidget {
public:
  OmniListSectionHeader(const QString &title, const QString &subtitle, size_t count);

  void setTitle(const QString &text) { m_title->setText(text); }

protected:
  void setupUI();

private:
  TypographyWidget *m_title = nullptr;
};
