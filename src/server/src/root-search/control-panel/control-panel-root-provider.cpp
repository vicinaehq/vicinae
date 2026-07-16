#include "root-search/control-panel/control-panel-root-provider.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "clipboard-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include "ui/image/url.hpp"
#include "utils/scoped-com.hpp"

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <wrl/client.h>

#include <algorithm>
#include <set>

using Microsoft::WRL::ComPtr;

namespace {

constexpr const char *CONTROL_PANEL_CLSID = "::{26EE0668-A00A-44D7-9371-BEB064C98683}";
constexpr const wchar_t *CONTROL_PANEL_FOLDER = L"::{26EE0668-A00A-44D7-9371-BEB064C98683}\\0";
constexpr const wchar_t *ALL_TASKS_FOLDER = L"shell:::{ED7BA470-8E54-465E-825C-99712043E01C}";
constexpr const char *ALL_TASKS_ICON_PREFIX = "::{ED7BA470-8E54-465E-825C-99712043E01C}\\";

// Tasks that only duplicate entries the app provider already surfaces.
constexpr const char *SKIPPED_TASK_IDS[] = {
    "{e9c71548-b580-43b2-acdb-1ba924002754}", // Task Manager
};

class OpenControlPanelItemAction : public AbstractAction {
public:
  OpenControlPanelItemAction(const QString &title, const ImageURL &icon, QString target)
      : AbstractAction(title, icon), m_target(std::move(target)) {}

  void execute(ApplicationContext *ctx) override {
    const std::wstring target = m_target.toStdWString();
    const HINSTANCE ret = ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

    if (reinterpret_cast<INT_PTR>(ret) <= 32) {
      ctx->services->toastService()->failure("Failed to open settings");
      return;
    }

    ctx->navigation->closeWindow();
    ctx->navigation->clearSearchText();
  }

private:
  QString m_target;
};

class OpenControlPanelTaskAction : public AbstractAction {
public:
  OpenControlPanelTaskAction(const QString &title, const ImageURL &icon, QByteArray pidl)
      : AbstractAction(title, icon), m_pidl(std::move(pidl)) {}

  void execute(ApplicationContext *ctx) override {
    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_IDLIST;
    sei.nShow = SW_SHOWNORMAL;
    sei.lpIDList = const_cast<char *>(m_pidl.constData());

    if (!ShellExecuteExW(&sei)) {
      ctx->services->toastService()->failure("Failed to open settings");
      return;
    }

    ctx->navigation->closeWindow();
    ctx->navigation->clearSearchText();
  }

private:
  QByteArray m_pidl;
};

QString displayNameOf(IShellFolder *folder, PCUITEMID_CHILD child, SHGDNF flags) {
  STRRET ret;
  if (FAILED(folder->GetDisplayNameOf(child, flags, &ret))) return {};

  wchar_t buf[512];
  if (FAILED(StrRetToBufW(&ret, child, buf, static_cast<UINT>(std::size(buf))))) return {};

  return QString::fromWCharArray(buf);
}

struct BoundShellFolder {
  ComPtr<IShellFolder> folder;
  QByteArray pidl;
};

std::optional<BoundShellFolder> bindShellFolder(const wchar_t *path) {
  PIDLIST_ABSOLUTE folderPidl = nullptr;
  if (FAILED(SHParseDisplayName(path, nullptr, &folderPidl, 0, nullptr))) return std::nullopt;

  BoundShellFolder bound;
  bound.pidl = QByteArray(reinterpret_cast<const char *>(folderPidl), ILGetSize(folderPidl));
  const HRESULT hr = SHBindToObject(nullptr, folderPidl, nullptr, IID_PPV_ARGS(&bound.folder));
  ILFree(folderPidl);
  if (FAILED(hr)) return std::nullopt;
  return bound;
}

std::vector<WinControlPanelApplet> enumerateControlPanelApplets() {
  std::vector<WinControlPanelApplet> applets;
  ScopedCom com;

  auto bound = bindShellFolder(CONTROL_PANEL_FOLDER);
  if (!bound) return applets;

  ComPtr<IEnumIDList> ids;
  if (bound->folder->EnumObjects(nullptr, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &ids) != S_OK || !ids)
    return applets;

  PITEMID_CHILD child = nullptr;
  while (ids->Next(1, &child, nullptr) == S_OK) {
    QString name = displayNameOf(bound->folder.Get(), child, SHGDN_NORMAL);
    QString parsingName = displayNameOf(bound->folder.Get(), child, SHGDN_FORPARSING);
    ILFree(child);

    if (name.isEmpty() || parsingName.isEmpty()) continue;
    // Only CLSID-backed applets round-trip through parsing names; the lone "Fonts" delegate entry
    // yields a path that neither resolves to an icon nor launches.
    if (!parsingName.section('\\', -1).startsWith("::{")) continue;
    applets.emplace_back(WinControlPanelApplet{std::move(name), std::move(parsingName)});
  }

  return applets;
}

std::vector<WinControlPanelTask> enumerateControlPanelTasks() {
  std::vector<WinControlPanelTask> tasks;
  ScopedCom com;

  auto bound = bindShellFolder(ALL_TASKS_FOLDER);
  if (!bound) return tasks;

  ComPtr<IEnumIDList> ids;
  if (bound->folder->EnumObjects(nullptr, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &ids) != S_OK || !ids)
    return tasks;

  const auto *parentPidl = reinterpret_cast<PCIDLIST_ABSOLUTE>(bound->pidl.constData());

  PITEMID_CHILD child = nullptr;
  while (ids->Next(1, &child, nullptr) == S_OK) {
    QString name = displayNameOf(bound->folder.Get(), child, SHGDN_NORMAL);
    QString taskId = displayNameOf(bound->folder.Get(), child, SHGDN_FORPARSING | SHGDN_INFOLDER);

    if (!name.isEmpty() && !taskId.isEmpty()) {
      QByteArray pidl;
      if (PIDLIST_ABSOLUTE full = ILCombine(parentPidl, child)) {
        pidl = QByteArray(reinterpret_cast<const char *>(full), ILGetSize(full));
        ILFree(full);
      }
      if (!pidl.isEmpty()) {
        tasks.emplace_back(WinControlPanelTask{std::move(name), std::move(taskId), std::move(pidl)});
      }
    }
    ILFree(child);
  }

  return tasks;
}

} // namespace

QString WinControlPanelRootItem::title() const { return m_applet.displayName; }

QString WinControlPanelRootItem::typeDisplayName() const { return "Control Panel"; }

ImageURL WinControlPanelRootItem::iconUrl() const { return ImageURL::winShellIcon(m_applet.parsingName); }

EntrypointId WinControlPanelRootItem::uniqueId() const {
  const QString id = "cpl." + m_applet.parsingName.section('\\', -1);
  return EntrypointId("control-panel", id.toStdString());
}

AccessoryList WinControlPanelRootItem::accessories() const {
  return {{.text = "Control Panel", .color = SemanticColor::TextMuted}};
}

std::vector<std::pair<QString, QString>> WinControlPanelRootItem::settingsMetadata() const {
  return {{"Name", m_applet.displayName}, {"Where", "shell:" + m_applet.parsingName}};
}

std::unique_ptr<ActionPanelState>
WinControlPanelRootItem::newActionPanel(ApplicationContext *ctx, const RootItemMetadata &metadata) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto mainSection = panel->createSection();
  auto utils = panel->createSection();
  auto itemSection = panel->createSection();

  const QString target = "shell:" + m_applet.parsingName;
  auto open = new OpenControlPanelItemAction(QString("Open %1").arg(m_applet.displayName), iconUrl(), target);
  mainSection->addAction(new DefaultActionWrapper(uniqueId(), open));

  utils->addAction(new CopyToClipboardAction(Clipboard::Text(target), "Copy Path"));

  for (const auto &action : RootSearchActionGenerator::generateActions(*this, metadata)) {
    itemSection->addAction(action);
  }

  panel->setTitle(m_applet.displayName);
  return panel;
}

QString WinControlPanelTaskRootItem::title() const { return m_task.displayName; }

QString WinControlPanelTaskRootItem::typeDisplayName() const { return "Control Panel"; }

ImageURL WinControlPanelTaskRootItem::iconUrl() const {
  return ImageURL::winShellIcon(ALL_TASKS_ICON_PREFIX + m_task.taskId);
}

EntrypointId WinControlPanelTaskRootItem::uniqueId() const {
  return EntrypointId("control-panel", ("task." + m_task.taskId).toStdString());
}

AccessoryList WinControlPanelTaskRootItem::accessories() const {
  return {{.text = "Control Panel", .color = SemanticColor::TextMuted}};
}

std::vector<std::pair<QString, QString>> WinControlPanelTaskRootItem::settingsMetadata() const {
  return {{"Name", m_task.displayName}, {"Task ID", m_task.taskId}};
}

std::unique_ptr<ActionPanelState>
WinControlPanelTaskRootItem::newActionPanel(ApplicationContext *ctx, const RootItemMetadata &metadata) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto mainSection = panel->createSection();
  auto itemSection = panel->createSection();

  auto open =
      new OpenControlPanelTaskAction(QString("Open %1").arg(m_task.displayName), iconUrl(), m_task.pidl);
  mainSection->addAction(new DefaultActionWrapper(uniqueId(), open));

  for (const auto &action : RootSearchActionGenerator::generateActions(*this, metadata)) {
    itemSection->addAction(action);
  }

  panel->setTitle(m_task.displayName);
  return panel;
}

QString WinControlPanelRootProvider::uniqueId() const { return "control-panel"; }

QString WinControlPanelRootProvider::displayName() const { return "Control Panel"; }

QString WinControlPanelRootProvider::description() const { return "Control Panel applets and system tasks."; }

ImageURL WinControlPanelRootProvider::icon() const { return ImageURL::winShellIcon(CONTROL_PANEL_CLSID); }

RootProvider::Type WinControlPanelRootProvider::type() const { return RootProvider::Type::GroupProvider; }

std::vector<std::shared_ptr<RootItem>> WinControlPanelRootProvider::loadItems() const {
  auto applets = enumerateControlPanelApplets();
  auto tasks = enumerateControlPanelTasks();

  std::vector<std::shared_ptr<RootItem>> items;
  items.reserve(applets.size() + tasks.size());

  std::set<QString> seenTitles;

  for (auto &applet : applets) {
    seenTitles.insert(applet.displayName.toLower());
    items.emplace_back(std::make_shared<WinControlPanelRootItem>(std::move(applet)));
  }
  // Tasks are the noisiest source: drop the ones whose title an applet already covers.
  for (auto &task : tasks) {
    if (seenTitles.contains(task.displayName.toLower())) continue;
    if (std::ranges::contains(SKIPPED_TASK_IDS, task.taskId.toLower())) continue;
    items.emplace_back(std::make_shared<WinControlPanelTaskRootItem>(std::move(task)));
  }

  return items;
}
