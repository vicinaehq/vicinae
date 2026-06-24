#pragma once
#include "environment.hpp"
#include "generated/version.h"
#include "internal/os-release.hpp"
#include "vicinae.hpp"
#include <QGuiApplication>
#include <QUrlQuery>

static const QString ISSUE_TEMPLATE = R"(**System information**

- Version: %1 (%2)
- Build info: %3
- Provenance: %4
- OS: %5
- QT Platform: %6
- DE: %7

**Describe the bug**

A clear and concise description of what the bug is.

**To Reproduce**

Steps to reproduce the behavior.

**Expected behavior**

A clear and concise description of what you expected to happen.

**Screenshots**

If applicable, add screenshots to help explain your problem.

**Additional context**

Add any other context about the problem here.
)";

inline QUrl makeVicinaeBugReportUrl(const QString &title = {}) {
  OsRelease os;
  QString osString = os.isValid() ? QString("%1 - %2").arg(os.prettyName()).arg(os.version()) : "Unknown OS";

  QString content = ISSUE_TEMPLATE.arg(VICINAE_GIT_TAG)
                        .arg(VICINAE_GIT_COMMIT_HASH)
                        .arg(BUILD_INFO)
                        .arg(VICINAE_PROVENANCE)
                        .arg(osString)
                        .arg(QGuiApplication::platformName())
                        .arg(Environment::getEnvironmentDescription());
  QUrl url(Omnicast::GH_REPO_CREATE_ISSUE);
  QUrlQuery query;

  if (!title.isEmpty()) { query.addQueryItem("title", title); }

  query.addQueryItem("body", content);
  query.addQueryItem("type", "bug");
  url.setQuery(query);

  return url;
}
