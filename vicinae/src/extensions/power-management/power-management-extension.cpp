#include "power-management-extension.hpp"
#include "command-controller.hpp"
#include <qprocess.h>
#include "common.hpp"
#include "preference.hpp"
#include "single-view-command-context.hpp"
#include "service-registry.hpp"
#include "services/power-manager/power-manager.hpp"
#include "services/toast/toast-service.hpp"

class PowerManagementCommand : public BuiltinCallbackCommand {
public:
  virtual bool requiresDefaultConfirmation() const { return true; }
  bool supportsConfirmation() const { return true; }

  std::vector<Preference> preferences() const override {
    if (!supportsConfirmation()) return {};

    auto confirm = Preference::makeCheckbox("confirm", "Ask for confirmation");

    confirm.setDefaultValue(requiresDefaultConfirmation());

    return {confirm};
  }

  void execute(CommandController *controller) const override final {
    auto ctx = controller->context();
    auto pm = controller->context()->services->powerManager();
    auto toast = controller->context()->services->toastService();
    auto &nav = controller->context()->navigation;
    bool shouldConfirm = controller->preferenceValues().value("confirm").toBool();

    if (shouldConfirm) {
      nav->confirmAlert("Are you sure", "High-impact operation, please confirm", [this, ctx, &nav]() {
        confirm(ctx);
        nav->closeWindow({.clearRootSearch = true});
      });
      return;
    }

    confirm(controller->context());
    nav->closeWindow({.clearRootSearch = true});
  }

  /**
   * Code to be executed by the power management command, after confirmation step
   * confirmed, if any.
   */
  virtual void confirm(const ApplicationContext *ctx) const = 0;
};

class LockCommand : public BuiltinCallbackCommand {
  QString id() const override { return "lock"; }
  QString name() const override { return "Lock Session"; }
  QString description() const override { return "Lock the current user session"; }
  std::vector<QString> keywords() const override { return {"lock"}; }

  std::vector<Preference> preferences() const override {
    auto program = Preference::makeText("customProgram");

    program.setRequired(false);
    program.setTitle("Custom locker");
    program.setDescription("Path or name of the program to use to lock the current session. This is normally "
                           "not needed if the system session locking mechanism was properly configured");

    return {program};
  }

  ImageURL iconUrl() const override {
    return ImageURL::builtin("lock").setBackgroundTint(SemanticColor::Orange);
  }

  void execute(CommandController *controller) const override {
    auto ctx = controller->context();
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();
    auto program = controller->preferenceValues().value("customProgram").toString();

    if (!program.isEmpty()) {
      QProcess process;

      process.start(program);
      if (!process.waitForFinished()) { toast->failure("Failed to lock using custom program " + program); }
    } else {
      if (!pm->provider()->canLock()) { return toast->failure("System can't lock"); }
      if (!pm->provider()->lock()) { return toast->failure("Failed to lock"); }
    }

    ctx->navigation->closeWindow({.clearRootSearch = true});
  }
};

class HibernateCommand : public PowerManagementCommand {
  QString id() const override { return "hibernate"; }
  QString name() const override { return "Hibernate System"; }
  QString description() const override {
    return "Suspend the system to disk. This turns off the system completely and saves its "
           "state on disk, to be restored on next boot.";
  }
  std::vector<QString> keywords() const override { return {"disk", "suspend"}; }
  bool requiresDefaultConfirmation() const override { return true; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("hard-drive").setBackgroundTint(SemanticColor::Orange);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canHibernate()) { return toast->failure("System can't hibernate"); }
    if (!pm->provider()->hibernate()) { return toast->failure("Failed to hibernate"); }
  }
};

class RebootCommand : public PowerManagementCommand {
  QString id() const override { return "reboot"; }
  QString name() const override { return "Reboot System"; }
  QString description() const override { return "Reboot the system"; }
  std::vector<QString> keywords() const override { return {"restart"}; }
  bool requiresDefaultConfirmation() const override { return true; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("rotate-anti-clockwise").setBackgroundTint(SemanticColor::Orange);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canReboot()) { return toast->failure("System can't reboot"); }
    if (!pm->provider()->reboot()) { return toast->failure("Failed to reboot"); }
  }
};

class SoftRebootCommand : public PowerManagementCommand {
  QString id() const override { return "soft-reboot"; }
  QString name() const override { return "Soft Reboot System"; }
  QString description() const override {
    return "Soft reboot the system, which usually means only userspace is rebooted.";
  }
  std::vector<QString> keywords() const override { return {"restart"}; }
  bool requiresDefaultConfirmation() const override { return true; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("rotate-anti-clockwise").setBackgroundTint(SemanticColor::Blue);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canSoftReboot()) { return toast->failure("System can't soft reboot"); }
    if (!pm->provider()->softReboot()) { return toast->failure("Failed to soft reboot"); }
  }
};

class PowerOffCommand : public PowerManagementCommand {
  QString id() const override { return "power-off"; }
  QString name() const override { return "Power Off System"; }
  QString description() const override { return "Power off the system"; }
  std::vector<QString> keywords() const override { return {"shutdown"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("power").setBackgroundTint(SemanticColor::Red);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canPowerOff()) { return toast->failure("System cannot power off"); }
    if (!pm->provider()->powerOff()) { return toast->failure("Failed to power off"); }
  }
};

class SuspendCommand : public PowerManagementCommand {
  QString id() const override { return "suspend"; }
  QString name() const override { return "Suspend System"; }
  QString description() const override {
    return "Suspend the system to RAM. Unlike hibernation, this does not turn the computer off and will "
           "break on power loss.";
  }
  std::vector<QString> keywords() const override { return {"suspend"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("pause").setBackgroundTint(SemanticColor::Orange);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canSuspend()) { return toast->failure("System cannot suspend"); }
    if (!pm->provider()->suspend()) { return toast->failure("Failed to suspend"); }
  }
};

class SleepCommand : public PowerManagementCommand {
  QString id() const override { return "sleep"; }
  QString name() const override { return "Put System to Sleep"; }
  QString description() const override { return "Put system to sleep"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("moon").setBackgroundTint(SemanticColor::Orange);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canSleep()) { return toast->failure("System can't sleep"); }
    if (!pm->provider()->sleep()) { return toast->failure("Failed to sleep"); }
  }
};

class LogOutCommand : public PowerManagementCommand {
  QString id() const override { return "logout"; }
  QString name() const override { return "Log Out"; }
  QString description() const override {
    return "Terminate the current user session. If you simply want to lock your session you should use 'Lock "
           "Session' instead.";
  }
  std::vector<QString> keywords() const override { return {"logout"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("logout").setBackgroundTint(SemanticColor::Red);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canLogOut()) { return toast->failure("System can't logout"); }
    if (!pm->provider()->logout()) { return toast->failure("Failed to log out"); }
  }
};

PowerManagementExtension::PowerManagementExtension() {
  registerCommand<SuspendCommand>();
  registerCommand<PowerOffCommand>();
  registerCommand<HibernateCommand>();
  registerCommand<RebootCommand>();
  registerCommand<SoftRebootCommand>();
  registerCommand<SleepCommand>();
  registerCommand<LockCommand>();
  registerCommand<LogOutCommand>();
}
