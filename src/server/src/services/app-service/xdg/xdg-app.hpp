#pragma once
#include <xdgpp/xdgpp.hpp>
#include "services/app-service/abstract-app-db.hpp"
#include "utils.hpp"
#include "xdgpp/desktop-entry/exec.hpp"

class XdgApplication : public AbstractApplication {
  xdgpp::DesktopFile m_entry;

  QString simplifiedId() const { return id().remove(".desktop"); }

public:
  QString id() const override { return QString::fromStdString(std::string(m_entry.id())); }
  QString displayName() const override { return QString::fromStdString(m_entry.name()); }

  const xdgpp::DesktopFile &data() const { return m_entry; }

  bool displayable() const override { return m_entry.shouldBeShownInCurrentContext(); }

  bool isTerminalApp() const override { return m_entry.terminal(); }
  bool isTerminalEmulator() const override { return m_entry.hasCategory("TerminalEmulator"); }
  std::filesystem::path path() const override { return m_entry.path(); };
  QString description() const override { return QString::fromStdString(m_entry.comment().value_or("")); }

  QString program() const override {
    auto ss = parseExec({});
    return ss.empty() ? QString() : ss.at(0);
  }

  bool matchesWindowClass(const QString &target) const override {
    auto normalizeClass = [](const QString &s) { return s.toLower().remove(".desktop"); };
    QString normalizedTarget = normalizeClass(target);

    if (auto cl = windowClass(); cl && normalizeClass(cl.value()) == normalizedTarget) { return true; }

    return normalizeClass(id()) == normalizedTarget;
  }

  std::optional<QString> windowClass() const override {
    return m_entry.startupWMClass().transform(
        [](const std::string &str) { return QString::fromStdString(str); });
  }

  std::vector<QString> keywords() const override {
    std::vector<QString> strs;

    for (const auto &kw : m_entry.keywords()) {
      strs.emplace_back(QString::fromStdString(kw));
    }

    return strs;
  }

  ImageURL iconUrl() const override {
    auto icon = QString::fromStdString(m_entry.icon().value_or(""));
    return ImageURL::system(icon);
  }

  std::vector<std::shared_ptr<AbstractApplication>> actions() const override;

  virtual std::vector<QString> parseExec(const std::vector<QString> &args,
                                         const std::optional<QString> &prefix = {}) const {
    auto launchPrefix = prefix.transform([](const QString &str) { return str.toStdString(); });
    auto parsed = m_entry.parseExec(Utils::toStdStringVec(args), true, launchPrefix);
    return Utils::toQStringVec(parsed);
  }

  XdgApplication(const xdgpp::DesktopFile &file) : m_entry(file) {}
};
