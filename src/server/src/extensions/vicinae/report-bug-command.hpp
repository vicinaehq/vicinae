#pragma once
#include "builtin-url-command.hpp"
#include <qapplication.h>
#include <qurlquery.h>
#include "environment.hpp"
#include "lib/os-release.hpp"
#include "version.h"

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

class ReportVicinaeBugCommand : public BuiltinUrlCommand {

  QString id() const override { return "report-bug"; }
  QString name() const override { return "Report a Vicinae Bug"; }

  QString description() const override {
    return "Navigate to Vicinae issue creation page with all relevant informations pre-filled.";
  }

  ImageURL iconUrl() const override {
    return ImageURL::builtin("bug").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  ArgumentList arguments() const override {
    return {CommandArgument{.name = "title", .placeholder = "Title", .required = false}};
  }

  std::vector<QString> keywords() const override { return {"create issue"}; }

  QUrl url(const ArgumentValues &values) const override {
    OsRelease os;
    QString osString =
        os.isValid() ? QString("%1 - %2").arg(os.prettyName()).arg(os.version()) : "Unknown OS";

    QString content = ISSUE_TEMPLATE.arg(VICINAE_GIT_TAG)
                          .arg(VICINAE_GIT_COMMIT_HASH)
                          .arg(BUILD_INFO)
                          .arg(VICINAE_PROVENANCE)
                          .arg(osString)
                          .arg(QApplication::platformName())
                          .arg(Environment::getEnvironmentDescription());
    QUrl url(Omnicast::GH_REPO_CREATE_ISSUE);
    QUrlQuery query;

    if (!values.empty()) { query.addQueryItem("title", values.front().second); }

    query.addQueryItem("body", content);
    query.addQueryItem("type", "bug");
    url.setQuery(query);

    return url;
  }
};
