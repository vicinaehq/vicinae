#pragma once
#include "bug-report-url.hpp"
#include "builtin-url-command.hpp"
#include <QCoreApplication>

class ReportVicinaeBugCommand : public BuiltinUrlCommand {
  Q_DECLARE_TR_FUNCTIONS(ReportVicinaeBugCommand)

  QString id() const override { return "report-bug"; }
  QString name() const override { return tr("Report a Vicinae Bug"); }

  QString description() const override {
    return tr("Navigate to Vicinae issue creation page with all relevant informations pre-filled.");
  }

  ImageURL iconUrl() const override {
    return ImageURL::builtin("bug").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  ArgumentList arguments() const override {
    return {CommandArgument{.name = "title", .placeholder = tr("Title"), .required = false}};
  }

  std::vector<QString> keywords() const override { return {"create issue", "Report a Vicinae Bug"}; }

  QUrl url(const ArgumentValues &values) const override {
    return makeVicinaeBugReportUrl(values.empty() ? QString() : values.front().second);
  }
};
