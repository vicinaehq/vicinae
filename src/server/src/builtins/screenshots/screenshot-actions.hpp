#pragma once

#include <QCoreApplication>
#include <filesystem>
#include <memory>

struct ApplicationContext;
class ActionPanelState;
struct Screenshot;

class ScreenshotActions {
  Q_DECLARE_TR_FUNCTIONS(ScreenshotActions)

public:
  static void transfer(const Screenshot &item, bool paste, const ApplicationContext *ctx);
  static void pasteLast(const ApplicationContext *ctx);
  static void refresh(const ApplicationContext *ctx);
  static std::unique_ptr<ActionPanelState> panel(const Screenshot &item, ApplicationContext *ctx);
  static std::unique_ptr<ActionPanelState> emptyPanel();
};
