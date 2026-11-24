#pragma once

#include "services/ai/ai-tool.hpp"

class ScreenCaptureService;

class ScreenshotTool : public AI::AbstractTool {
public:
  explicit ScreenshotTool(ScreenCaptureService &capture) : m_capture(capture) {}

  std::string name() const override { return "screenshot"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  bool isEnabled() const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
  RawToolTask runRaw(std::string_view arguments) override;

private:
  ScreenCaptureService &m_capture;
};
