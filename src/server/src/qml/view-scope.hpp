#pragma once
#include "common/context.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"

class ViewScope {
public:
  ViewScope() = default;
  ViewScope(ApplicationContext *ctx, const BaseView *owner) : m_ctx(ctx), m_owner(owner) {}

  // --- Scoped navigation (auto-provides owner) ---
  void setActions(std::unique_ptr<ActionPanelState> panel) const { nav()->setActions(std::move(panel), m_owner); }
  void clearActions() const { nav()->clearActions(m_owner); }
  void setNavigationTitle(const QString &title) const { nav()->setNavigationTitle(title, m_owner); }

  // --- Unscoped navigation ---
  bool executePrimaryAction() const { return nav()->executePrimaryAction(); }
  void closeWindow(const CloseWindowOptions &opts = {}) const { nav()->closeWindow(opts); }
  void pushView(BaseView *view) const { nav()->pushView(view); }
  template <typename T> void pushView() const { nav()->pushView<T>(); }
  void showHud(const QString &title, const std::optional<ImageURL> &icon = std::nullopt) const {
    nav()->showHud(title, icon);
  }
  void confirmAlert(const QString &title, const QString &desc, const std::function<void()> &onConfirm) const {
    nav()->confirmAlert(title, desc, onConfirm);
  }
  const NavigationController::ViewState *topState() const { return nav()->topState(); }
  ArgumentValues completionValues() const { return nav()->completionValues(); }
  void createCompletion(const ArgumentList &args, const ImageURL &icon) const {
    nav()->createCompletion(args, icon);
  }
  void destroyCurrentCompletion() const { nav()->destroyCurrentCompletion(); }

  // --- Non-navigation access ---
  ServiceRegistry *services() const { return m_ctx->services; }
  ApplicationContext *appContext() const { return m_ctx; }

private:
  NavigationController *nav() const { return m_ctx->navigation.get(); }

  ApplicationContext *m_ctx = nullptr;
  const BaseView *m_owner = nullptr;
};
