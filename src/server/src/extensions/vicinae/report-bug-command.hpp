#pragma once
#include "bug-report-url.hpp"
#include "builtin-url-command.hpp"

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
    return makeVicinaeBugReportUrl(values.empty() ? QString() : values.front().second);
  }
};
