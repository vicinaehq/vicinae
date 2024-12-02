#pragma once
#include "app-database.hpp"
#include "calculator-database.hpp"
#include "command-object.hpp"
#include "common.hpp"
#include "quicklist-database.hpp"
#include "ui/toast.hpp"
#include <memory>
#include <qicon.h>
#include <qlocale.h>
#include <qmimedatabase.h>
#include <qobject.h>

struct CopyTextToClipboardAction : public IAction {
  QString text;
  QString nm;

public:
  QString name() const override { return nm; }
  QIcon icon() const override { return QIcon::fromTheme("clipboard"); }
  void exec(ExecutionContext ctx) override {
    auto &clipboard = ctx.service<ClipboardService>();

    clipboard.copyText(text);
    ctx.hideWindow();
    ctx.setSearch("");
  }

  CopyTextToClipboardAction(const QString &s,
                            const QString &name = "Copy to clipboard")
      : text(s), nm(name) {}
};

struct OpenInAppAction : public IAction {
  std::shared_ptr<DesktopExecutable> app;

  QString name() const override { return app->name; }

  QIcon icon() const override { return app->icon(); }

  void exec(ExecutionContext ctx) override {
    app->launch({ctx.query()});
    ctx.hideWindow();
    ctx.setSearch("");
  }

  OpenInAppAction(std::shared_ptr<DesktopExecutable> &app) : app(app) {}
};

struct OpenInDefaultAppAction : public IAction {
  std::shared_ptr<DesktopExecutable> app;
  QString file;
  QString actionName;

  QIcon icon() const override { return app->icon(); }
  QString name() const override { return actionName; }
  void exec(ExecutionContext ctx) override {
    if (!app) {
      ctx.setToast("No app to open" + file, ToastPriority::Danger);
      return;
    }

    app->launch({file});
    ctx.hideWindow();
    ctx.setSearch("");
  }

  OpenInDefaultAppAction(Service<AppDatabase> appDb, const QString &fileName,
                         const QString &actionName = "Open")
      : file(fileName), actionName(actionName) {
    QMimeDatabase mimeDb;
    auto mime = mimeDb.mimeTypeForFile(fileName);

    app = appDb.findBestOpenerForMime(mime);
  }
};

class ActionnableQuicklink : public IActionnable {

public:
  const Quicklink &link;
  struct OpenQuicklinkAction : public IAction {
    const Quicklink &link;
    QString name() const override { return "Open link"; }
    QIcon icon() const override { return QIcon::fromTheme(link.iconName); }
    void exec(ExecutionContext ctx) override {
      auto &appDb = ctx.service<AppDatabase>();
      auto app = appDb.getById(link.app);

      if (!app) {
        ctx.setToast("No app to open link", ToastPriority::Danger);
        return;
      }

      QString url = link.url;
      auto completions = ctx.completions();

      for (const auto &arg : ctx.completions()) {
        url = url.arg(arg);
      }

      if (completions.isEmpty() && link.placeholders.size() == 1) {
        url = url.arg(ctx.query());
      }

      app->launch({url});
      ctx.hideWindow();
      ctx.setSearch("");
    }

    OpenQuicklinkAction(const Quicklink &link) : link(link) {}
  };

  struct DeleteQuicklinkAction : public IAction {
    const Quicklink &link;
    QString name() const override { return "Delete link"; }
    QIcon icon() const override { return QIcon::fromTheme("node-delete"); };
    void exec(ExecutionContext ctx) override {
      auto db = ctx.service<QuicklistDatabase>();

      if (db.removeOne(link.id)) {
        ctx.setToast("Link deleted", ToastPriority::Success);
      } else {
        ctx.setToast("Failed to delete link", ToastPriority::Danger);
      }

      ctx.reloadSearch();
    }

    DeleteQuicklinkAction(const Quicklink &link) : link(link) {}
  };

  ActionnableQuicklink(ExecutionContext ctx, const Quicklink &link)
      : link(link) {}

  ActionList generateActions() const override {
    return {std::make_shared<OpenQuicklinkAction>(link),
            std::make_shared<DeleteQuicklinkAction>(link)};
  }
};

class ActionnableCalculator : public IActionnable {
  QString expression;
  QString result;

  struct CopyResultAction : public IAction {
    const ActionnableCalculator &calc;

    QString name() const override { return "Copy result"; }
    QIcon icon() const override { return QIcon::fromTheme("clipboard"); }

    void exec(ExecutionContext ctx) override {
      auto &calcDb = ctx.service<CalculatorDatabase>();
      auto &clip = ctx.service<ClipboardService>();

      clip.copyText(calc.result);
      calcDb.saveComputation(calc.expression, calc.result);
      ctx.hideWindow();
      ctx.setSearch("");
    }

    CopyResultAction(const ActionnableCalculator &calc) : calc(calc) {}
  };

  struct CopyExpressionAction : public IAction {
    const ActionnableCalculator &calc;

    QString name() const override { return "Copy expression"; }
    QIcon icon() const override { return QIcon::fromTheme("clipboard"); }

    void exec(ExecutionContext ctx) override {
      auto &calcDb = ctx.service<CalculatorDatabase>();
      auto &clip = ctx.service<ClipboardService>();

      clip.copyText(calc.expression + " = " + calc.result);
      calcDb.saveComputation(calc.expression, calc.result);
      ctx.hideWindow();
      ctx.setSearch("");
    }

    CopyExpressionAction(const ActionnableCalculator &calc) : calc(calc) {}
  };

public:
  ActionnableCalculator(ExecutionContext ctx, const QString &expression,
                        const QString &result)
      : expression(expression), result(result) {}

  ActionList generateActions() const override {
    return {std::make_shared<CopyResultAction>(*this),
            std::make_shared<CopyExpressionAction>(*this)};
  }
};
