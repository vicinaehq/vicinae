#include "entry.hpp"
#include "action.hpp"
#include "reader.hpp"
#include "../utils/utils.hpp"
#include "../env/env.hpp"
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <ostream>
#include <system_error>
#include "exec.hpp"

namespace fs = std::filesystem;

namespace xdgpp {
DesktopEntry DesktopEntry::fromFile(const fs::path &path, const ParseOptions &opts) {
  return DesktopEntry(path, opts);
}

DesktopEntry DesktopEntry::fromData(std::string_view data, const ParseOptions &opts) {
  return DesktopEntry(data, opts);
}

DesktopEntry::Type DesktopEntry::type() const { return m_type; }

bool DesktopEntry::isApplication() const { return m_type == Type::Application; }

bool DesktopEntry::isLink() const { return m_type == Type::Link; }

bool DesktopEntry::isDirectory() const { return m_type == Type::Directory; }

std::optional<std::string> DesktopEntry::version() const { return m_version; }

std::string DesktopEntry::name() const { return m_name; }

std::optional<std::string> DesktopEntry::genericName() const { return m_genericName; }

bool DesktopEntry::noDisplay() const { return m_noDisplay; }

std::optional<std::string> DesktopEntry::comment() const { return m_comment; }

std::optional<std::string> DesktopEntry::icon() const { return m_icon; }

bool DesktopEntry::deleted() const { return m_hidden; }

std::optional<std::string> DesktopEntry::exec() const { return m_exec; }

std::optional<std::string_view> DesktopEntry::tryExec() const { return m_tryExec; }

std::vector<std::string> DesktopEntry::parseExec() const { return parseExec({}); }

std::vector<std::string> DesktopEntry::parseExec(const std::vector<std::string> &uris) const {
  if (!m_exec) return {};

  ExecParser parser(m_name);

  if (m_icon) { parser.setIcon(*m_icon); }

  return parser.parse(m_exec.value(), uris);
}

std::optional<std::string> DesktopEntry::startupWMClass() const { return m_wmClass; }

const std::vector<std::string> &DesktopEntry::mimes() const { return m_mimes; }

const std::vector<std::string> &DesktopEntry::categories() const { return m_categories; }

bool DesktopEntry::hasCategory(std::string_view category) const {
  return std::ranges::find(categories(), category) != categories().end();
}

bool DesktopEntry::supportsMime(std::string_view mime) const {
  return std::ranges::find(mimes(), mime) != mimes().end();
}

const std::vector<std::string> &DesktopEntry::keywords() const { return m_keywords; }

const std::vector<std::string> &DesktopEntry::onlyShowIn() const { return m_onlyShowIn; }

const std::vector<std::string> &DesktopEntry::notShowIn() const { return m_notShowIn; }

bool DesktopEntry::singleMainWindow() const { return m_singleMainWindow; }

std::optional<std::filesystem::path> DesktopEntry::workingDirectory() const { return m_path; }

bool DesktopEntry::terminal() const { return m_terminal; }

bool DesktopEntry::isValid() const { return !m_error.has_value(); }

bool DesktopEntry::shouldBeShownInCurrentContext() const {
  if (deleted() || noDisplay()) return false;

  {
    auto &in = onlyShowIn();
    if (!in.empty() && std::ranges::find(in, xdgpp::currentDesktop()) == in.end()) { return false; }
  }

  {
    auto &notIn = notShowIn();
    if (!notIn.empty() && std::ranges::find(notIn, xdgpp::currentDesktop()) != notIn.end()) { return false; }
  }

  return true;
}

std::optional<std::string> DesktopEntry::errorMessage() const { return m_error; }

const std::vector<DesktopEntryAction> &DesktopEntry::actions() const { return m_actions; }

DesktopEntry::DesktopEntry(const fs::path &path, const ParseOptions &opts) {
  std::error_code ec;

  if (!fs::is_regular_file(path, ec)) {
    m_error = std::string("Could not read file at ") + path.c_str();
    return;
  }

  *this = DesktopEntry(std::string_view(slurp(path)), opts);
}

DesktopEntry::DesktopEntry(std::string_view data, const ParseOptions &opts) {
  DesktopEntryReader entry(data, {.locale = opts.locale});
  auto group = entry.group("Desktop Entry");

  if (!group) {
    m_error = "No Desktop Entry group was found";
    return;
  }

  if (auto type = group->key("Type")) {
    auto str = type->asString();
    if (str == "Application") {
      m_type = Type::Application;
    } else if (str == "Link") {
      m_type = Type::Link;
    } else if (str == "Directory") {
      m_type = Type::Directory;
    }
  }

  if (auto version = group->key("Version")) { m_version = version->asString(); }

  if (auto name = group->key("Name")) {
    m_name = name->asString();
  } else {
    m_error = "Name key is always required";
  }

  if (auto icon = group->key("Icon")) { m_icon = icon->asString(); }
  if (auto genericName = group->key("GenericName")) { m_genericName = genericName->asString(); }
  if (auto comment = group->key("Comment")) { m_comment = comment->asString(); }
  if (auto noDisplay = group->key("NoDisplay")) { m_noDisplay = noDisplay->asBoolean(); }
  if (auto hidden = group->key("Hidden")) { m_hidden = hidden->asBoolean(); }
  if (auto terminal = group->key("Terminal")) { m_terminal = terminal->asBoolean(); }
  if (auto exec = group->key("Exec")) { m_exec = exec->asString(); }
  if (auto exec = group->key("TryExec")) { m_tryExec = exec->asString(); }
  if (auto wmClass = group->key("StartupWMClass")) { m_wmClass = wmClass->asString(); }
  if (auto mimes = group->key("MimeType")) { m_mimes = mimes->asStringList(); }
  if (auto mimes = group->key("Categories")) { m_categories = mimes->asStringList(); }
  if (auto kws = group->key("Keywords")) { m_keywords = kws->asStringList(); }
  if (auto onlyShowIn = group->key("OnlyShowIn")) { m_onlyShowIn = onlyShowIn->asStringList(); }
  if (auto notShowIn = group->key("NotShowIn")) { m_notShowIn = notShowIn->asStringList(); }
  if (auto path = group->key("Path")) { m_path = path->asString(); }
  if (auto smw = group->key("SingleMainWindow")) { m_singleMainWindow = smw->asBoolean(); }

  if (auto actionList = group->key("Actions")) {
    auto actions = actionList->asStringList();
    m_actions.reserve(actions.size());

    for (const auto &actionId : actions) {
      if (auto group = entry.group("Desktop Action " + actionId)) {
        m_actions.emplace_back(DesktopEntryAction::fromGroup(*group));
      }
    }
  }
}
}; // namespace xdgpp

std::ostream &operator<<(std::ostream &os, const xdgpp::DesktopEntry &entry) {
  os << "DesktopEntry(" << std::quoted(entry.name()) << ")";
  return os;
}
