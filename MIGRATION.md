# QWidgets → QML Migration Plan

## Views Still Using QWidgets

### Active Views (need porting)

#### 1. Browse Apps (`SystemBrowseApps`)
- **Command**: `BuiltinViewCommand<BrowseAppsView>` — `src/server/src/extensions/system/system-extension.hpp:76`
- **View**: `BrowseAppsView` — `src/server/src/extensions/system/browse-apps/browse-apps-view.hpp`
- **Base**: `SearchableListView`
- **Type**: Simple fuzzy-filtered list
- **Service**: `AppService`
- **Features**: Preferences for hidden apps filter and alphabetical sort
- **Approach**: `QmlFuzzyListModel<T>` — straightforward, very similar to installed extensions

#### 2. Search Browser Tabs (`SearchBrowserTabsCommand`)
- **Command**: `GuardedBuiltinCallbackCommand` — `src/server/src/extensions/browser/browser-extension.cpp:110`
- **View**: `SearchBrowserTabsView` — `src/server/src/extensions/browser/browser-extension.cpp:44`
- **Base**: `FilteredTypedListView<BrowserTab>`
- **Type**: Simple fuzzy-filtered list
- **Service**: `BrowserExtensionService`
- **Features**: Tabs from connected browsers, focus/close tab actions
- **Approach**: `QmlFuzzyListModel<T>` — simple

#### 3. Manage Fallback Commands (`ManageFallbackCommand`)
- **Command**: `BuiltinViewCommand<ManageFallbackView>` — `src/server/src/extensions/vicinae/configure-fallback-command.hpp:6`
- **View**: `ManageFallbackView` — `src/server/src/extensions/vicinae/manage-fallback/manage-fallback-view.hpp`
- **Base**: `TypedListView<ManageFallbackModel>`
- **Type**: Dual-section list (enabled + available)
- **Service**: `RootItemManager`
- **Features**: Toggle enable/disable for fallback items
- **Approach**: `QmlFuzzyListModel<T>` — add multi-section grouping support to `applyFilter()` so subclasses can split filtered results into sections (enabled/available)

#### 4. System Run Program (`SystemRunCommand`)
- **Command**: `BuiltinCallbackCommand` — `src/server/src/extensions/system/system-extension.hpp:10`
- **View**: `SystemRunView` — `src/server/src/extensions/system/run/system-run-view.hpp`
- **Base**: `TypedListView<RunProgramListModel>`
- **Type**: Dual-section list (execute query + available programs)
- **Service**: `ProgramDb`, `AppService`
- **Features**: Terminal action variants, default action preference, async program scanning
- **Approach**: Custom `QmlCommandListModel` (dual sections + async background scanning)

#### 5. Search Files (`SearchFilesCommand`)
- **Command**: `BuiltinViewCommand<SearchFilesView>` — `src/server/src/extensions/file/file-extension.hpp:12`
- **View**: `SearchFilesView` — `src/server/src/extensions/file/search/search-files-view.hpp`
- **Base**: `TypedListView<FileSearchModel>`
- **Type**: List with detail panel
- **Service**: `FileService`, `AbstractFileIndexer`
- **Features**: Async file search with debouncing, file preview/detail, recent files on empty search
- **Approach**: Custom `QmlCommandListModel` with `QFutureWatcher` pattern, detail panel

#### 6. Create Extension Success View
- **Pushed from**: `QmlCreateExtensionViewHost` — `src/server/src/qml/qml-create-extension-view-host.cpp:135`
- **View**: `CreateExtensionSuccessView` — `src/server/src/extensions/developer/create/create-extension-success-view.hpp`
- **Base**: `DetailView`
- **Type**: Markdown detail with action panel
- **Features**: Shows success markdown, "open in" actions for the created directory
- **Approach**: New `QmlCreateExtensionSuccessViewHost` extending `QmlFormViewBase`, reuses `MarkdownDetailView.qml`

#### 7. Script Executor View
- **Pushed from**: `src/server/src/script/script-actions.cpp:62`
- **View**: `ScriptExecutorView` — `src/server/src/ui/script-output/script-executor-view.hpp`
- **Base**: `BaseView`
- **Type**: Custom view (script output rendering)
- **Features**: Process management, output rendering, kill on pop
- **Approach**: New QML view host with script output component

#### 8. Provider Search View (Extension deeplinks)
- **Pushed from**: `src/server/src/ipc-command-handler.cpp:149`
- **View**: `ProviderSearchView` — `src/server/src/ui/provider-view/provider-view.hpp`
- **Base**: `TypedListView<ProviderSearchModel>`
- **Type**: List (extension-provided search)
- **Notes**: Used for IPC deeplinks into extension search — may need special handling

#### 9. Root Search (`RootCommand`) — CLEANUP ONLY
- **Command**: `BuiltinViewCommand<RootSearchView>` — `src/server/src/extensions/root/root-command.hpp:5`
- **Status**: Already ported — `QmlRootSearchModel` powers the QML launcher. `RootCommand` still creates a `RootSearchView` widget as a navigation stack shim (never rendered). Just needs the widget dependency removed.

### Internal/Debug Views (lower priority)

#### 10. Search Builtin Icons (`IconBrowserCommand`)
- **Command**: `BuiltinViewCommand<SearchBuiltinIconView>` — `src/server/src/extensions/vicinae/vicinae-extension.cpp:192`
- **View**: `SearchBuiltinIconView` — `src/server/src/extensions/vicinae/browse-icons/search-builtin-icons-view.hpp`
- **Base**: `FilteredTypedListView<SearchIconDataType>`
- **Type**: Filtered list
- **Approach**: `QmlFuzzyListModel<T>`

#### 11. Inspect Local Storage (`InspectLocalStorage`)
- **Command**: `BuiltinViewCommand<BrowseLocalStorageView>` — `src/server/src/extensions/vicinae/vicinae-extension.cpp:203`
- **View**: `BrowseLocalStorageView` + `BrowseLocalStorageNamespaceView` — `src/server/src/extensions/vicinae/local-storage/browse-local-storage.hpp`
- **Base**: `FilteredTypedListView<T>`
- **Type**: Two-level list browser with markdown detail
- **Approach**: Two `QmlFuzzyListModel<T>` views or single view with navigation

#### 12. UI Playground (`UIPlayground`)
- **Command**: `BuiltinViewCommand<PlaygroundView>` — `src/server/src/extensions/internal/internal-extension.hpp:8`
- **View**: `PlaygroundView` — `src/server/src/extensions/internal/playground/playground-view.hpp`
- **Base**: `BaseView` (custom grid)
- **Type**: Custom emoji grid with keyboard navigation
- **Notes**: Internal debug tool — could be removed entirely since Search Emojis is already QML

### Dead Code (already replaced, can delete)

#### Old widget views superseded by QML:
- `ClipboardHistoryView` — `src/server/src/extensions/clipboard/history/clipboard-history-view.{hpp,cpp}` (replaced by `QmlClipboardHistoryViewHost`)
- `OAuthTokenStoreView` — `src/server/src/extensions/vicinae/oauth-token-store/oauth-token-store-view.{hpp,cpp}` (replaced by `QmlOAuthTokenStoreViewHost`)
- `VicinaeStoreListingView` — `src/server/src/extensions/vicinae/store/store-listing-view.{hpp,cpp}` (replaced by `QmlVicinaeStoreViewHost`)
- `VicinaeStoreDetailView` — `src/server/src/extensions/vicinae/store/store-detail-view.hpp` (replaced by `QmlVicinaeStoreDetailHost`)
- `RaycastStoreListingView` — `src/server/src/extensions/raycast/store/store-listing-view.{hpp,cpp}` (replaced by `QmlRaycastStoreViewHost`)
- `RaycastStoreDetailView` — `src/server/src/extensions/raycast/store/store-detail-view.hpp` (replaced by `QmlRaycastStoreDetailHost`)
- `ManageThemesView` — `src/server/src/extensions/theme/manage/manage-themes-view.{hpp,cpp}` (never instantiated — appears superseded by `QmlThemeListModel`/`SetThemeCommand`)

### Manage Snippets (old widget view)
- `ManageSnippetsView` — `src/server/src/extensions/snippet/manage-snippets-view.hpp` (replaced by `QmlManageSnippetsViewHost`, but file still exists)

## Widget Compatibility Path to Remove

After all views are ported, remove the widget embedding code from the QML launcher:

**`src/server/src/qml/qml-launcher-window.hpp`:**
- `#include` for `BaseView` (line 17)
- `embedWidget(BaseView *view)` method (line 97)
- `removeWidget()` method (line 98)
- `repositionWidget()` method (line 99)
- `BaseView *m_activeWidget` member (line 109)

**`src/server/src/qml/qml-launcher-window.cpp`:**
- `repositionWidget()` connections in constructor (lines 62-65)
- The `else if` branch in `handleCurrentViewChanged()` for widget views (lines 316-318)
- `embedWidget()` implementation (lines 327-345)
- `removeWidget()` implementation (lines 348-357)
- `repositionWidget()` implementation (lines 363-370)
- Widget input filter forwarding (lines 407-408)
- Widget cleanup in `handleCurrentViewChanged()` back-to-root path (line 261)
- Widget cleanup in the QML bridge path (lines 281-284)

**`src/server/src/cli/server.cpp`:**
- Remove the "widget-based RootSearchView is created but never rendered" shim pattern (line 327-329)
- Eventually: remove the old `LauncherWindow` (QWidgets) path entirely (lines 340-351)

## Suggested Migration Order

1. **Delete dead code** — remove already-superseded widget views
2. **Browse Apps** — simplest, good warm-up (`QmlFuzzyListModel`)
3. **Search Browser Tabs** — also simple (`QmlFuzzyListModel`)
4. **Search Builtin Icons** — simple, internal
5. **Manage Fallback Commands** — dual-section, moderate
6. **System Run Program** — dual-section + async, moderate
7. **Create Extension Success** — markdown detail, small scope
8. **Search Files** — async + detail panel, more complex
9. **Script Executor** — custom output view
10. **Provider Search** — extension deeplink handler
11. **Root Search decoupling** — decouple `RootCommand` from `RootSearchView`
12. **Local Storage / Playground** — internal debug views, lowest priority
13. **Remove widget compatibility path** — final step after all views are ported
