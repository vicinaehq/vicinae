#pragma once
#include "command-database.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"
#include "qml/bridge-view.hpp"
#include "markdown-showcase-command.hpp"

class ButtonShowcaseView : public FormViewBase {
public:
  QUrl qmlComponentUrl() const override { return QUrl(QStringLiteral("qrc:/Vicinae/ButtonShowcase.qml")); }
};

class UIPlayground : public BuiltinViewCommand<ButtonShowcaseView> {
  QString id() const override { return "playground"; }
  QString name() const override { return "UI Showcase"; }
  QString description() const override { return "Showcase of vicinae UI elements"; }
  bool isFallback() const override { return false; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::MagnifyingGlass).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  bool isInternal() const override { return true; }
};

class InternalExtension : public BuiltinCommandRepository {
  QString id() const override { return "internal"; }
  QString displayName() const override { return "Internal Commands"; }
  QString description() const override { return "Internal Commands"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::MagnifyingGlass).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  InternalExtension() {
    registerCommand<UIPlayground>();
    registerCommand<MarkdownShowcase>();
  }
};
