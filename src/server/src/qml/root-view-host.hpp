#pragma once
#include "bridge-view.hpp"

/// Minimal view host for the root navigation frame.
/// The actual root search UI is provided by RootSearchModel inside the QML launcher.
/// This host only exists so that the navigation stack has a valid BaseView at the bottom.
class RootViewHost : public ViewHostBase {
public:
  QUrl qmlComponentUrl() const override { return {}; }
  QString initialSearchPlaceholderText() const override { return QStringLiteral("Search for anything..."); }
};
