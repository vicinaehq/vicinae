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
#include <QCoreApplication>

class PowerManagementCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(PowerManagementCommand)

public:
  virtual bool requiresDefaultConfirmation() const { return true; }
  virtual bool supportsCustomProgram() const {
#ifdef Q_OS_MACOS
    return false;
#else
    return true;
#endif
  }

  std::vector<Preference> preferences() const override {
    std::vector<Preference> preferences;

    preferences.reserve(2);

    auto confirm = Preference::makeCheckbox("confirm", tr("Ask for confirmation"));
    confirm.setDefaultValue(requiresDefaultConfirmation());
    preferences.emplace_back(confirm);

    if (supportsCustomProgram()) {
      auto program = Preference::makeText("customProgram");
      program.setRequired(false);
      program.setTitle(tr("Custom program"));
      program.setDescription(tr("Custom POSIX shell command to run instead of the default implementation"));
      preferences.emplace_back(program);
    }

    return preferences;
  }

  void execute(CommandController &controller) const final {
    auto &nav = controller.context()->navigation;
    auto prefs = controller.preferenceValues();
    bool const shouldConfirm = prefs.value("confirm").toBool();
    std::optional<QString> customProgram;

    if (auto prog = prefs.value("customProgram").toString(); !prog.isEmpty()) { customProgram = prog; }

    auto handleConfirm = [this, ctx = controller.context(), customProgram]() {
      auto toast = ctx->services->toastService();

      if (customProgram) {
        QProcess process;
        QStringList args;

        process.setProgram("/bin/sh");
        args << "-c" << *customProgram;
        process.setArguments(std::move(args));
        process.start();
        if (!process.waitForFinished(-1)) {
          toast->failure(tr("Failed to execute custom program %1").arg(*customProgram));
        }
      } else {
        confirm(ctx);
      }

      ctx->navigation->closeWindow({.clearRootSearch = true});
    };

    if (shouldConfirm) {
      nav->confirmAlert(tr("Are you sure"), tr("High-impact operation, please confirm"),
                        [&nav, handleConfirm]() {
                          nav->closeWindow({.clearRootSearch = true});
                          handleConfirm();
                        });
      return;
    }

    nav->closeWindow({.clearRootSearch = true});
    handleConfirm();
  }

  /**
   * Code to be executed by the power management command, after confirmation step
   * confirmed, if any.
   */
  virtual void confirm(const ApplicationContext *ctx) const = 0;
};

class LockCommand : public PowerManagementCommand {
  Q_DECLARE_TR_FUNCTIONS(LockCommand)

  QString id() const override { return "lock"; }
  QString name() const override { return tr("Lock Session"); }
  QString description() const override { return tr("Lock the current user session"); }
  std::vector<QString> keywords() const override { return {"lock"}; }
  bool requiresDefaultConfirmation() const override { return false; }

  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::Lock}.setBackgroundTint(SemanticColor::Accent);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canLock()) {
      toast->failure(tr("System can't lock"));
      return;
    }
    if (!pm->provider()->lock()) {
      toast->failure(tr("Failed to lock"));
      return;
    }
  }
};

class HibernateCommand : public PowerManagementCommand {
  Q_DECLARE_TR_FUNCTIONS(HibernateCommand)

  QString id() const override { return "hibernate"; }
  QString name() const override { return tr("Hibernate System"); }
  QString description() const override {
    return tr("Suspend the system to disk. This turns off the system completely and saves its "
              "state on disk, to be restored on next boot.");
  }
  std::vector<QString> keywords() const override { return {"disk", "suspend"}; }
  bool requiresDefaultConfirmation() const override { return true; }
  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::HardDrive}.setBackgroundTint(SemanticColor::Accent);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canHibernate()) {
      toast->failure(tr("System can't hibernate"));
      return;
    }
    if (!pm->provider()->hibernate()) {
      toast->failure(tr("Failed to hibernate"));
      return;
    }
  }
};

class RebootCommand : public PowerManagementCommand {
  Q_DECLARE_TR_FUNCTIONS(RebootCommand)

  QString id() const override { return "reboot"; }
  QString name() const override { return tr("Reboot System"); }
  QString description() const override { return tr("Reboot the system"); }
  std::vector<QString> keywords() const override { return {"restart"}; }
  bool requiresDefaultConfirmation() const override { return true; }
  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::RotateAntiClockwise}.setBackgroundTint(SemanticColor::Accent);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canReboot()) {
      toast->failure(tr("System can't reboot"));
      return;
    }
    if (!pm->provider()->reboot()) {
      toast->failure(tr("Failed to reboot"));
      return;
    }
  }
};

class SoftRebootCommand : public PowerManagementCommand {
  Q_DECLARE_TR_FUNCTIONS(SoftRebootCommand)

  QString id() const override { return "soft-reboot"; }
  QString name() const override { return tr("Soft Reboot System"); }
  QString description() const override {
    return tr("Soft reboot the system, which usually means only userspace is rebooted.");
  }
  std::vector<QString> keywords() const override { return {"restart"}; }
  bool requiresDefaultConfirmation() const override { return true; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("rotate-anti-clockwise").setBackgroundTint(SemanticColor::Cyan);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canSoftReboot()) {
      toast->failure(tr("System can't soft reboot"));
      return;
    }
    if (!pm->provider()->softReboot()) {
      toast->failure(tr("Failed to soft reboot"));
      return;
    }
  }
};

class PowerOffCommand : public PowerManagementCommand {
  Q_DECLARE_TR_FUNCTIONS(PowerOffCommand)

  QString id() const override { return "power-off"; }
  QString name() const override { return tr("Power Off System"); }
  QString description() const override { return tr("Power off the system"); }
  std::vector<QString> keywords() const override { return {"shutdown"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("power").setBackgroundTint(SemanticColor::Red);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canPowerOff()) {
      toast->failure(tr("System cannot power off"));
      return;
    }
    if (!pm->provider()->powerOff()) {
      toast->failure(tr("Failed to power off"));
      return;
    }
  }
};

class SuspendCommand : public PowerManagementCommand {
  Q_DECLARE_TR_FUNCTIONS(SuspendCommand)

  QString id() const override { return "suspend"; }
  QString name() const override { return tr("Suspend System"); }
  QString description() const override {
    return tr("Suspend the system to RAM. Unlike hibernation, this does not turn the computer off and will "
              "break on power loss.");
  }
  std::vector<QString> keywords() const override { return {"suspend"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("pause").setBackgroundTint(SemanticColor::Accent);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canSuspend()) {
      toast->failure(tr("System cannot suspend"));
      return;
    }
    if (!pm->provider()->suspend()) {
      toast->failure(tr("Failed to suspend"));
      return;
    }
  }
};

class SleepCommand : public PowerManagementCommand {
  Q_DECLARE_TR_FUNCTIONS(SleepCommand)

  QString id() const override { return "sleep"; }
  QString name() const override { return tr("Put System to Sleep"); }
  QString description() const override { return tr("Put system to sleep"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("moon").setBackgroundTint(SemanticColor::Accent);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canSleep()) {
      toast->failure(tr("System can't sleep"));
      return;
    }
    if (!pm->provider()->sleep()) {
      toast->failure(tr("Failed to sleep"));
      return;
    }
  }
};

class LogOutCommand : public PowerManagementCommand {
  Q_DECLARE_TR_FUNCTIONS(LogOutCommand)

  QString id() const override { return "logout"; }
  QString name() const override { return tr("Log Out"); }
  QString description() const override {
    return tr("Terminate the current user session. If you simply want to lock your session you should use "
              "'Lock Session' instead.");
  }

  std::vector<QString> keywords() const override { return {"logout"}; }

  ImageURL iconUrl() const override {
    return ImageURL::builtin("logout").setBackgroundTint(SemanticColor::Red);
  }

  void confirm(const ApplicationContext *ctx) const override {
    auto pm = ctx->services->powerManager();
    auto toast = ctx->services->toastService();

    if (!pm->provider()->canLogOut()) {
      toast->failure(tr("System can't logout"));
      return;
    }
    if (!pm->provider()->logout()) {
      toast->failure(tr("Failed to log out"));
      return;
    }
  }
};

PowerManagementExtension::PowerManagementExtension() {
  registerCommand<PowerOffCommand>();
  registerCommand<RebootCommand>();
  registerCommand<SleepCommand>();
  registerCommand<LockCommand>();
  registerCommand<LogOutCommand>();
#ifndef Q_OS_MACOS
  registerCommand<SuspendCommand>();
  registerCommand<HibernateCommand>();
  registerCommand<SoftRebootCommand>();
#endif
}
