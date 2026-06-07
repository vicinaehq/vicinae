#include "font-demo-view-host.hpp"

QString FontDemoViewHost::showcaseMarkdown() const {
  return FontService::specimenMarkdown(m_family, m_category);
}
