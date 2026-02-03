#pragma once
#include "utils.hpp"
#include "xdg-app.hpp"

/**
 * An action has to be an application object of its own, so we just make it
 * inherit its parent application, overriding only the relevant fields.
 */
class XdgApplicationAction : public XdgApplication {

public:
  XdgApplicationAction(const xdgpp::DesktopFile &file, const xdgpp::DesktopEntryAction &action)
      : XdgApplication(file), m_data(action) {}

  bool isAction() const override { return true; }

  ImageURL iconUrl() const override {
    if (auto icon = m_data.icon()) { return ImageURL::system(icon.value().c_str()); }

    return XdgApplication::iconUrl();
  }

  std::vector<QString> parseExec(const std::vector<QString> &args,
                                 const std::optional<QString> &prefix) const override {
    auto launchPrefix = prefix.transform([](const QString &str) { return str.toStdString(); });
    return Utils::toQStringVec(m_data.parseExec(Utils::toStdStringVec(args), true, launchPrefix));
  }

  QString id() const override {
    return QString::fromStdString(XdgApplication::id().toStdString() + "." + m_data.id());
  };

  QString fullyQualifiedName() const override {
    return (XdgApplication::displayName().toStdString() + ": " + m_data.name()).c_str();
  }

  QString displayName() const override { return m_data.name().c_str(); }

private:
  xdgpp::DesktopEntryAction m_data;
};
