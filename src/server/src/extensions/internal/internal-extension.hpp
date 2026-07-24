#pragma once
#include "command-database.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"
#include "qml/bridge-view.hpp"
#include "markdown-showcase-command.hpp"
#include <QCoreApplication>

class UIShowcaseView : public FormViewBase {
public:
  QUrl qmlComponentUrl() const override { return QUrl(QStringLiteral("qrc:/Vicinae/UIShowcase.qml")); }
};

class UIPlayground : public BuiltinViewCommand<UIShowcaseView> {
  Q_DECLARE_TR_FUNCTIONS(UIPlayground)

  QString id() const override { return "playground"; }
  QString name() const override { return tr("UI Showcase"); }
  QString description() const override { return tr("Showcase of vicinae UI elements"); }
  bool isFallback() const override { return false; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::MagnifyingGlass).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  bool isInternal() const override { return true; }
};

class InternalExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(InternalExtension)

  QString id() const override { return "internal"; }
  QString displayName() const override { return tr("Internal Commands"); }
  QString description() const override { return tr("Internal Commands"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::MagnifyingGlass).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  InternalExtension() {
    registerCommand<UIPlayground>();
    registerCommand<MarkdownShowcase>();
  }
};
