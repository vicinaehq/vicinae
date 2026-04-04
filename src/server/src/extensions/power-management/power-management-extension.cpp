#include "power-management-extension.hpp"
#include "command-controller.hpp"
#include <qcontainerfwd.h>
#include <qprocess.h>
#include "common/context.hpp"
#include "preference.hpp"
#include "single-view-command-context.hpp"
#include "service-registry.hpp"
#include "services/power-manager/power-manager.hpp"
#include "services/toast/toast-service.hpp"

class PowerManagementCommand : public BuiltinCallbackCommand {
public:
  virtual bool requiresDefaultConfirmation() const { return true; }
  virtual bool supportsCustomProgram() const { return true; }

  std::vector<Preference> preferences() const override {
    std::vector<Preference> preferences;

    preferences.reserve(2);

    auto confirm = Preference::makeCheckbox("confirm", "Ask for confirmation");
    confirm.setDefaultValue(requiresDefaultConfirmation());
    preferences.emplace_back(confirm);

    if (supportsCustomProgram()) {
      auto program = Preference::makeText("customProgram");
      program.setRequired(false);
      program.setTitle("Custom program");
      program.setDescription("Custom POSIX shell command to run instead of the default implementation");
      preferences.emplace_back(program);
    }

    return preferences;
  }

  void execute(CommandController *controller) const final {
    auto &nav = controller->context()->navigation;
    auto prefs = controller->preferenceValues();
    bool const shouldConfirm = prefs.value("confirm").toBool();
    std::optional<QString> customProgram;

    if (auto prog = prefs.value("customProgram").toString(); !prog.isEmpty()) { customProgram = prog; }

    auto handleConfirm = [this, ctx = controller->context(), customProgram]() {
      auto toast = ctx->services->toastService();

      if (customProgram) {
        QProcess process;
        QStringList args;

        process.setProgram("/bin/sh");
        args << "-c" << *customProgram;
        process.setArguments(std::move(args));
        process.start();
        if (!process.waitForFinished(-1)) {
          toast->failure("Failed to execute custom program " + *customProgram);
        }
      } else {
        confirm(ctx);
      }

      ctx->navigation->closeWindow({.clearRootSearch = true});
    };

    if (shouldConfirm) {
      nav->confirmAlert("Are you sure", "High-impact operation, please confirm", [&nav, handleConfirm]() {
        handleConfirm();
        nav->closeWindow({.clearRootSearch = true});
      });
      return;
    }

    handleConfirm();
    nav->closeWindow({.clearRootSearch = true});
  }

  /**
   * Code to be executed by the power management command, after confirmation step
   * confirmed, if any.
   */
  virtual void confirm(const ApplicationContext *ctx) const = 0;
};

class LockCommand : public PowerManagementCommand {
  QString id() const override { return "lock"; }
  QString name() const override { return "Lock Session"; }
  QString description() const override { return "Lock the current user session"; }
  std::vector<QString> keywords() const override { return {"lock"}; }
  bool requiresDefaultConfirmation() const override { return false; }

  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::Lock}.setBackgroundTint(SemanticColor::Orange);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canLock()) {
      toast->failure("System can't lock");
      return;
    }
    if (!pm->provider()->lock()) {
      toast->failure("Failed to lock");
      return;
    }
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
    return ImageURL{BuiltinIcon::HardDrive}.setBackgroundTint(SemanticColor::Orange);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canHibernate()) {
      toast->failure("System can't hibernate");
      return;
    }
    if (!pm->provider()->hibernate()) {
      toast->failure("Failed to hibernate");
      return;
    }
  }
};

class RebootCommand : public PowerManagementCommand {
  QString id() const override { return "reboot"; }
  QString name() const override { return "Reboot System"; }
  QString description() const override { return "Reboot the system"; }
  std::vector<QString> keywords() const override { return {"restart"}; }
  bool requiresDefaultConfirmation() const override { return true; }
  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::RotateAntiClockwise}.setBackgroundTint(SemanticColor::Orange);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canReboot()) {
      toast->failure("System can't reboot");
      return;
    }
    if (!pm->provider()->reboot()) {
      toast->failure("Failed to reboot");
      return;
    }
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

    if (!pm->provider()->canSoftReboot()) {
      toast->failure("System can't soft reboot");
      return;
    }
    if (!pm->provider()->softReboot()) {
      toast->failure("Failed to soft reboot");
      return;
    }
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

    if (!pm->provider()->canPowerOff()) {
      toast->failure("System cannot power off");
      return;
    }
    if (!pm->provider()->powerOff()) {
      toast->failure("Failed to power off");
      return;
    }
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

    if (!pm->provider()->canSuspend()) {
      toast->failure("System cannot suspend");
      return;
    }
    if (!pm->provider()->suspend()) {
      toast->failure("Failed to suspend");
      return;
    }
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

    if (!pm->provider()->canSleep()) {
      toast->failure("System can't sleep");
      return;
    }
    if (!pm->provider()->sleep()) {
      toast->failure("Failed to sleep");
      return;
    }
  }
};

class LogOutCommand : public PowerManagementCommand {
  QString id() const override { return "logout"; }
  QString name() const override { return "Log Out"; }
  QString description() const override {
    return "Terminate the current user session. If you simply want to lock your session you should use 'Lock "
           "Session' instead.";
  }
  bool supportsCustomProgram() const override { return true; }

  std::vector<QString> keywords() const override { return {"logout"}; }

  ImageURL iconUrl() const override {
    return ImageURL::builtin("logout").setBackgroundTint(SemanticColor::Red);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canLogOut()) {
      toast->failure("System can't logout");
      return;
    }
    if (!pm->provider()->logout()) {
      toast->failure("Failed to log out");
      return;
    }
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
