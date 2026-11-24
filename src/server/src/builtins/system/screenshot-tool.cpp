#include <QBuffer>
#include <QGuiApplication>
#include <QScreen>
#include <QWindow>
#include <algorithm>
#include <format>
#include <ranges>
#include "screenshot-tool.hpp"
#include "services/screen-capture/screen-capture-service.hpp"

// not anonymous: glaze can't reflect types with internal linkage
namespace screenshot_tool {

constexpr int MAX_IMAGE_EDGE = 2048;

struct ScreenshotInput {
  std::optional<std::string> screen;
};

AI::AbstractTool::RawToolTask ready(AI::AbstractTool::RawToolResult result) {
  return {QtFuture::makeReadyValueFuture(std::move(result)), {}};
}

std::string joinNames(const QList<QScreen *> &screens, const QScreen *except = nullptr) {
  return screens | std::views::filter([&](const QScreen *screen) { return screen != except; }) |
         std::views::transform([](const QScreen *screen) { return screen->name().toStdString(); }) |
         std::views::join_with(std::string_view(", ")) | std::ranges::to<std::string>();
}

QScreen *currentScreen() {
  if (const auto *window = QGuiApplication::focusWindow()) return window->screen();
  return QGuiApplication::primaryScreen();
}

AI::AbstractTool::RawToolResult encode(QImage image, std::string description) {
  if (image.width() > MAX_IMAGE_EDGE || image.height() > MAX_IMAGE_EDGE)
    image = image.scaled(MAX_IMAGE_EDGE, MAX_IMAGE_EDGE, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  QByteArray png;
  QBuffer output(&png);
  if (!image.save(&output, "PNG"))
    return std::unexpected(std::string("The screenshot could not be encoded."));

  AI::ToolOutput result{.content = std::move(description)};
  result.images.reserve(1);
  result.images.emplace_back(AI::ImagePart{.mimeType = "image/png", .base64 = png.toBase64()});
  return result;
}

} // namespace screenshot_tool

using namespace screenshot_tool;

std::string ScreenshotTool::description() const {
  return "Take a screenshot of one of the user's screens to see what they are looking at. The screen the "
         "user is on is captured unless another one is named, and the result names the other screens.";
}

std::string ScreenshotTool::generateInputSchema() const {
  return R"({"type":"object","properties":{"screen":{"type":"string","description":"Name of the screen to capture. Defaults to the screen the user is on."}},"additionalProperties":false})";
}

bool ScreenshotTool::isEnabled() const { return m_capture.isAvailable(); }

std::optional<std::string> ScreenshotTool::invocationSummary(std::string_view arguments) const {
  ScreenshotInput input;
  if (glz::read_json(input, arguments)) return std::nullopt;
  return input.screen;
}

AI::AbstractTool::RawToolTask ScreenshotTool::runRaw(std::string_view arguments) {
  ScreenshotInput input;
  if (!arguments.empty()) {
    if (const auto error = glz::read_json(input, arguments))
      return ready(std::unexpected(glz::format_error(error, arguments)));
  }

  const auto screens = QGuiApplication::screens();
  auto *screen = currentScreen();

  if (input.screen) {
    const auto named = std::ranges::find(screens, QString::fromStdString(*input.screen), &QScreen::name);
    if (named == screens.end())
      return ready(std::unexpected(std::format("There is no screen named {}. Available screens: {}.",
                                               *input.screen, joinNames(screens))));
    screen = *named;
  }

  if (!screen) return ready(std::unexpected(std::string("No screen is connected.")));

  auto description = std::format("Screenshot of the screen {}.", screen->name().toStdString());
  if (const auto others = joinNames(screens, screen); !others.empty())
    description += std::format(" Other screens: {}.", others);

  auto result = m_capture.capture(screen).then(
      QtFuture::Launch::Async,
      [description = std::move(description)](ScreenCaptureService::Result image) mutable -> RawToolResult {
        if (!image) return std::unexpected(std::move(image.error()));
        return encode(std::move(*image), std::move(description));
      });

  return {std::move(result), {}};
}
