#pragma once
#include <xdgpp/xdgpp.hpp>
#include "services/app-service/abstract-app-db.hpp"
#include "utils.hpp"

class XdgApplication : public AbstractApplication {
  xdgpp::DesktopFile m_entry;

  QString simplifiedId() const { return id().remove(".desktop"); }

public:
  QString id() const override { return QString::fromStdString(std::string(m_entry.id())); }
  QString displayName() const override { return QString::fromStdString(m_entry.name()); }

  const xdgpp::DesktopFile &data() const { return m_entry; }

  bool displayable() const override { return m_entry.shouldBeShownInCurrentContext(); }

  bool isTerminalApp() const override { return m_entry.terminal(); }
  // bool isTerminalEmulator() const override { return _data.categories.contains("TerminalEmulator"); }
  //
  bool isTerminalEmulator() const override { return false; }
  std::filesystem::path path() const override { return m_entry.path(); };
  QString description() const override { return QString::fromStdString(m_entry.comment().value_or("")); }

  QString program() const override {
    auto ss = parseExec({});
    return ss.empty() ? QString() : ss.at(0);
  }

  std::vector<QString> windowClasses() const override {
    std::vector<QString> classes;

    if (auto wmClass = m_entry.startupWMClass()) {
      classes.emplace_back(QString::fromStdString(wmClass.value()));
    }

    classes.emplace_back(program());
    classes.emplace_back(simplifiedId());

    return classes;
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

  virtual std::vector<QString> parseExec(const std::vector<QString> &args) const {
    return Utils::toQStringVec(m_entry.parseExec(Utils::toStdStringVec(args)));
  }

  XdgApplication(const xdgpp::DesktopFile &file) : m_entry(file) {}
};
