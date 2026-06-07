#include "font-demo-view-host.hpp"

QString FontDemoViewHost::showcaseMarkdown() const {
  if (m_category == FontCategory::CJK) {
    const QString ja = FontService::scriptSample(FontCategory::Japanese, m_family);
    const QString ko = FontService::scriptSample(FontCategory::Korean, m_family);
    const QString zh = FontService::scriptSample(FontCategory::SimplifiedChinese, m_family);
    return QStringLiteral("# %1\n\n## %1\n\n%1\n\n# %2\n\n## %2\n\n%2\n\n# %3\n\n## %3\n\n%3\n")
        .arg(ja, ko, zh);
  }

  const QString sample = FontService::scriptSample(m_category, m_family);
  return QStringLiteral("# %1\n\n## %1\n\n### %1\n\n%1\n\n**%1**\n\n*%1*\n\n> %1\n\n`%1`\n").arg(sample);
}
