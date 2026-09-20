<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_TW">
<context>
    <name>AboutSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AboutSettingsPage.qml" line="+57"/>
        <source>Version %1 - Commit %2
(%3)</source>
        <translation>版本 %1 - 提交 %2
（%3）</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Documentation</source>
        <translation>文件</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Report a Bug</source>
        <translation>報告問題</translation>
    </message>
</context>
<context>
    <name>ActionListPanel</name>
    <message>
        <location filename="../src/qml/qml/ActionListPanel.qml" line="+113"/>
        <source>No matching actions</source>
        <translation>沒有匹配的操作</translation>
    </message>
    <message>
        <location line="+135"/>
        <source>Filter actions...</source>
        <translation>篩選操作…</translation>
    </message>
</context>
<context>
    <name>AdvancedSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AdvancedSettingsPage.qml" line="+29"/>
        <source>Input &amp; Navigation</source>
        <translation>輸入與導航</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Pop on backspace</source>
        <translation>退格鍵返回上一級</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pop back in navigation on backspace when no input is present.</source>
        <translation>沒有輸入內容時，按退格鍵返回上一級。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Activate on single click</source>
        <translation>單擊即可開啟</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activate items with a single click instead of requiring a double click.</source>
        <translation>單擊即可開啟專案，無需雙擊。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Wrap navigation</source>
        <translation>迴圈導航</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Wrap around to the opposite end when moving past the first or last item.</source>
        <translation>移過第一個或最後一個專案時，迴圈到另一端。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>IME handling</source>
        <translation>輸入法處理</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Include IME Preedit strings as part of search queries.</source>
        <translation>將輸入法預編輯文本納入搜尋查詢。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Keybinding Scheme</source>
        <translation>快捷鍵方案</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Default uses the standard macOS keys (arrows, Ctrl+N/P); Vim uses Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Opt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>預設方案使用標準 macOS 按鍵（方向鍵、Ctrl+N/P）；Vim 使用 Ctrl+J/K 和 Ctrl+H/L；Emacs 使用 Ctrl+N/P 和 Ctrl+Opt+B/F 導航，並在搜尋欄中啟用 Emacs 編輯方式。</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Default and Vim use Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Alt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>預設和 Vim 方案使用 Ctrl+J/K 與 Ctrl+H/L；Emacs 使用 Ctrl+N/P 和 Ctrl+Alt+B/F 導航，並在搜尋欄中啟用 Emacs 編輯方式。</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Search</source>
        <translation>搜尋</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Root file search</source>
        <translation>根搜尋檔案結果</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Files are searched asynchronously, so if enabled you should expect a slight delay for file search results to show up.</source>
        <translation>檔案採用非同步搜尋，啟用後搜尋結果可能會稍有延遲。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Favicon Fetching</source>
        <translation>網站圖示獲取</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The favicon provider used to load favicons where needed. Select &apos;None&apos; to turn off favicon loading.</source>
        <translation>用於按需載入網站圖示的服務。選擇“無”可關閉網站圖示載入。</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>System</source>
        <translation>系統</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Input server</source>
        <translation>輸入伺服器</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Whether to spawn the input server at startup. This needs to be enabled in order to support snippets, paste to active window, and other features that require input monitoring or injection.</source>
        <translation>是否在啟動時執行輸入伺服器。片段展開、貼上到活動視窗及其他需要監聽或注入輸入的功能必須啟用此項。</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Security</source>
        <translation>安全</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Encrypt sensitive data</source>
        <translation>加密敏感資料</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Encrypt sensitive data at rest, such as clipboard history and internal databases (OAuth tokens, extension local storage, API keys). Note that some components, such as on-disk clipboard history, may not be retroactively affected when toggling this option. Turning on this option may ask you to unlock your keychain. Requires a restart in order to apply.</source>
        <translation>加密靜態儲存的敏感資料，例如剪貼簿歷史和內部資料庫（OAuth 令牌、擴充套件本地儲存、API 金鑰）。切換此選項可能不會追溯處理某些元件，例如磁碟上的剪貼簿歷史。啟用時可能會要求解鎖鑰匙串。重啟後生效。</translation>
    </message>
</context>
<context>
    <name>AlertWidget</name>
    <message>
        <location filename="../src/ui/alert/alert.hpp" line="+15"/>
        <source>Are you sure?</source>
        <translation>確定嗎？</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>This action cannot be undone</source>
        <translation>此操作無法撤銷</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Confirm</source>
        <translation>確認</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
</context>
<context>
    <name>AliasFormView</name>
    <message>
        <location filename="../src/qml/qml/AliasFormView.qml" line="+15"/>
        <source>Alias</source>
        <translation>別名</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Additional words to index this item against</source>
        <translation>用於索引此專案的其他詞語</translation>
    </message>
</context>
<context>
    <name>AliasFormViewHost</name>
    <message>
        <location filename="../src/qml/alias-form-view-host.cpp" line="+28"/>
        <source>Set alias - %1</source>
        <translation>設定別名 - %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Submit</source>
        <translation>提交</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Alias modified</source>
        <translation>別名已修改</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to modify alias</source>
        <translation>修改別名失敗</translation>
    </message>
</context>
<context>
    <name>AppRootItem</name>
    <message>
        <location filename="../src/root-search/apps/app-root-provider.cpp" line="+22"/>
        <location line="+29"/>
        <source>Application</source>
        <translation>應用程式</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Where</source>
        <translation>位置</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Opens in terminal</source>
        <translation>在終端中開啟</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Yes</source>
        <translation>是</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No</source>
        <translation>否</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Open Application</source>
        <translation>開啟應用程式</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy App ID</source>
        <translation>複製應用 ID</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy App Location</source>
        <translation>複製應用位置</translation>
    </message>
</context>
<context>
    <name>AppRootProvider</name>
    <message>
        <location line="+75"/>
        <source>Applications</source>
        <translation>應用程式</translation>
    </message>
</context>
<context>
    <name>AppSelectorModel</name>
    <message>
        <location filename="../src/qml/app-selector-model.cpp" line="+20"/>
        <location line="+65"/>
        <source>%1 (Default)</source>
        <translation>%1（預設）</translation>
    </message>
</context>
<context>
    <name>AppearanceSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AppearanceSettingsPage.qml" line="+29"/>
        <location line="+7"/>
        <source>Theme</source>
        <translation>主題</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Font</source>
        <translation>字型</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Font size</source>
        <translation>字號</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The base point size used to compute font sizes. Fractional values are accepted. Recommended range is [10.0;12.0].</source>
        <translation>用於計算字號的基礎點數，可使用小數。建議範圍為 [10.0;12.0]。</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>e.g. 11</source>
        <translation>例如 11</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Icon Theme</source>
        <translation>圖示主題</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The icon theme used for system icons (applications, mime types, folder icons...). Does not affect builtin Vicinae icons.</source>
        <translation>用於系統圖標（應用程式、MIME 型別、資料夾圖示等）的圖示主題，不影響 Vicinae 內建圖示。</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Window</source>
        <translation>視窗</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Window material</source>
        <translation>視窗材質</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Background material applied to the launcher window. Lower the window opacity to see it.</source>
        <translation>應用於啟動器視窗的背景材質。降低視窗不透明度即可看到效果。</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Window opacity</source>
        <translation>視窗不透明度</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>e.g. 1.0</source>
        <translation>例如 1.0</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Compact mode</source>
        <translation>緊湊模式</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Show only the search bar at root; expand when a query is entered.</source>
        <translation>在根頁面僅顯示搜尋欄，輸入查詢後展開。</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Use layer shell</source>
        <translation>使用 layer shell</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Anchor the launcher as a Wayland layer surface (wlr-layer-shell) instead of a regular window. May require reopening Vicinae to fully apply.</source>
        <translation>將啟動器固定為 Wayland 層表面（wlr-layer-shell），而非普通視窗。可能需要重新開啟 Vicinae 才能完全生效。</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Client-side decorations</source>
        <translation>客戶端視窗裝飾</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Let Vicinae draw its own rounded borders and shadow instead of relying on the windowing system.</source>
        <translation>由 Vicinae 自行繪製圓角邊框和陰影，而非依賴視窗系統。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Corner rounding</source>
        <translation>圓角</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Radius of the launcher window corners, in pixels.</source>
        <translation>啟動器視窗的圓角半徑，單位為畫素。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>e.g. 10</source>
        <translation>例如 10</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Border width</source>
        <translation>邊框寬度</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Thickness of the launcher window border, in pixels.</source>
        <translation>啟動器視窗的邊框寬度，單位為畫素。</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>e.g. 3</source>
        <translation>例如 3</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Shadow size</source>
        <translation>陰影大小</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Size of the drop shadow cast by the launcher window, in pixels.</source>
        <translation>啟動器視窗投影的大小，單位為畫素。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>e.g. 12</source>
        <translation>例如 12</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Native font rendering</source>
        <translation>原生字型渲染</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Use the platform&apos;s native text rendering for system-consistent text. Disable for Qt distance-field rendering (usually faster). May require reopening Vicinae to fully apply.</source>
        <translation>使用平臺原生文本渲染，使文字與系統保持一致。關閉後使用 Qt 距離場渲染（通常更快）。可能需要重新開啟 Vicinae 才能完全生效。</translation>
    </message>
</context>
<context>
    <name>AvailableFallbackSection</name>
    <message>
        <location filename="../src/qml/manage-fallback-model.hpp" line="+49"/>
        <source>Available</source>
        <translation>可用</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-fallback-model.cpp" line="+50"/>
        <source>Enable fallback</source>
        <translation>啟用後備項</translation>
    </message>
</context>
<context>
    <name>BringToWorkspaceAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="+72"/>
        <source>Bring to current workspace</source>
        <translation>移到當前工作區</translation>
    </message>
</context>
<context>
    <name>BrowseAppsSection</name>
    <message>
        <location filename="../src/qml/browse-apps-model.hpp" line="+32"/>
        <source>Applications ({count})</source>
        <translation>應用程式（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/browse-apps-model.cpp" line="+21"/>
        <source>Hidden</source>
        <translation>已隱藏</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Open Application</source>
        <translation>開啟應用程式</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Copy App ID</source>
        <translation>複製應用 ID</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy App Location</source>
        <translation>複製應用位置</translation>
    </message>
</context>
<context>
    <name>BrowseAppsViewHost</name>
    <message>
        <location filename="../src/qml/browse-apps-view-host.cpp" line="+12"/>
        <source>Search apps...</source>
        <translation>搜尋應用程式…</translation>
    </message>
</context>
<context>
    <name>BrowseFontsCommand</name>
    <message>
        <location filename="../src/extensions/font/browse-fonts-command.hpp" line="+8"/>
        <source>Search Fonts</source>
        <translation>搜尋字型</translation>
    </message>
</context>
<context>
    <name>BrowserExtension</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.hpp" line="+12"/>
        <source>Browser Extension</source>
        <translation>瀏覽器擴充套件</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Browser extension related commands.</source>
        <translation>瀏覽器擴充套件相關命令。</translation>
    </message>
</context>
<context>
    <name>BrowserTabActionGenerator</name>
    <message>
        <location filename="../src/actions/browser-tab-actions.hpp" line="+24"/>
        <source>Switch to tab</source>
        <translation>切換到標籤頁</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Convert to shortcut</source>
        <translation>轉換為快捷方式</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Convert tab to shortcut</source>
        <translation>將標籤頁轉換為快捷方式</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Close tab</source>
        <translation>關閉標籤頁</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to close tab: %1</source>
        <translation>關閉標籤頁失敗：%1</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy URL</source>
        <translation>複製 URL</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy Title</source>
        <translation>複製標題</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy ID</source>
        <translation>複製 ID</translation>
    </message>
</context>
<context>
    <name>BrowserTabProvider</name>
    <message>
        <location filename="../src/root-search/browser-tabs/browser-tabs-provider.hpp" line="+70"/>
        <source>Browser Tabs</source>
        <translation>瀏覽器標籤頁</translation>
    </message>
</context>
<context>
    <name>BrowserTabRootItem</name>
    <message>
        <location line="-51"/>
        <source>Browser Tab</source>
        <translation>瀏覽器標籤頁</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Tab</source>
        <translation>標籤頁</translation>
    </message>
</context>
<context>
    <name>BrowserTabsSection</name>
    <message>
        <location filename="../src/qml/browser-tabs-model.hpp" line="+17"/>
        <source>Tabs ({count})</source>
        <translation>標籤頁（{count}）</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Playing Media ({count})</source>
        <translation>正在播放媒體（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/browser-tabs-model.cpp" line="+15"/>
        <source>Muted</source>
        <translation>已靜音</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Playing</source>
        <translation>正在播放</translation>
    </message>
</context>
<context>
    <name>BrowserTabsViewHost</name>
    <message>
        <location filename="../src/qml/browser-tabs-view-host.cpp" line="+11"/>
        <source>Search, focus and close tabs</source>
        <translation>搜尋、聚焦和關閉標籤頁</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsSection</name>
    <message>
        <location filename="../src/qml/builtin-icons-model.hpp" line="+20"/>
        <source>Icons ({count})</source>
        <translation>圖示（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/builtin-icons-model.cpp" line="+15"/>
        <source>Copy Icon Name</source>
        <translation>複製圖示名稱</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsViewHost</name>
    <message>
        <location filename="../src/qml/builtin-icons-view-host.cpp" line="+10"/>
        <source>Search icons...</source>
        <translation>搜尋圖示…</translation>
    </message>
</context>
<context>
    <name>CalcHistoryListView</name>
    <message>
        <location filename="../src/qml/qml/CalcHistoryListView.qml" line="+12"/>
        <source>No results</source>
        <translation>無結果</translation>
    </message>
</context>
<context>
    <name>CalcHistorySection</name>
    <message>
        <location filename="../src/qml/calc-history-model.cpp" line="+41"/>
        <source>Copy answer</source>
        <translation>複製答案</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy question</source>
        <translation>複製問題</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy question and answer</source>
        <translation>複製問題和答案</translation>
    </message>
</context>
<context>
    <name>CalcHistoryViewHost</name>
    <message>
        <location filename="../src/qml/calc-history-view-host.cpp" line="+83"/>
        <source>Search past calculations...</source>
        <translation>搜尋歷史計算…</translation>
    </message>
</context>
<context>
    <name>CalcLiveSection</name>
    <message>
        <location filename="../src/qml/calc-history-view-host.hpp" line="+27"/>
        <source>Calculator</source>
        <translation>計算器</translation>
    </message>
</context>
<context>
    <name>CalculatorExtension</name>
    <message>
        <location filename="../src/extensions/calculator/calculator-extension.hpp" line="+73"/>
        <source>Calculator</source>
        <translation>計算器</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Do maths, convert units or search past calculations...</source>
        <translation>進行計算、單位換算或搜尋歷史計算…</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Calculator Backend</source>
        <translation>計算器後端</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Which backend to use to perform calculations</source>
        <translation>用於執行計算的後端</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Refresh rates on startup</source>
        <translation>啟動時重新整理匯率</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Whether exchange rates should be refreshed every time the vicinae server is started. If the current backend does not support it, this is ignored.</source>
        <translation>每次啟動 Vicinae 伺服器時是否重新整理匯率。如果當前後端不支援，此設定將被忽略。</translation>
    </message>
</context>
<context>
    <name>CalculatorHistoryCommand</name>
    <message>
        <location line="-86"/>
        <source>Calculator history</source>
        <translation>計算歷史</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse past calculations. You need to copy the result of a calculation for it to be saved in history.</source>
        <translation>瀏覽歷史計算。計算結果需複製後才會儲存到歷史記錄。</translation>
    </message>
</context>
<context>
    <name>CalculatorRefreshRatesCommand</name>
    <message>
        <location line="+11"/>
        <source>Refresh Exchange Rates</source>
        <translation>重新整理匯率</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Refresh exchange rates used by the calculator to provide currency conversion features. Not all backends may support currency conversions or manually refreshing the rates.</source>
        <translation>重新整理計算器用於貨幣換算的匯率。並非所有後端都支援貨幣換算或手動重新整理匯率。</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>%1 can&apos;t refresh rates</source>
        <translation>%1 無法重新整理匯率</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Refreshing rates...</source>
        <translation>正在重新整理匯率…</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Rates successfully refreshed</source>
        <translation>匯率已重新整理</translation>
    </message>
</context>
<context>
    <name>CalculatorResultDelegate</name>
    <message>
        <location filename="../src/qml/qml/CalculatorResultDelegate.qml" line="+40"/>
        <source>Question</source>
        <translation>問題</translation>
    </message>
    <message>
        <location line="+57"/>
        <source>Answer</source>
        <translation>答案</translation>
    </message>
</context>
<context>
    <name>CalculatorService</name>
    <message>
        <location filename="../src/services/calculator-service/calculator-service.cpp" line="+119"/>
        <source>Pinned</source>
        <translation>已固定</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Today</source>
        <translation>今天</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>This week</source>
        <translation>本週</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>This month</source>
        <translation>本月</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>This year</source>
        <translation>今年</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>A few years ago</source>
        <translation>幾年前</translation>
    </message>
</context>
<context>
    <name>CategoryFilterAccessory</name>
    <message>
        <location filename="../src/qml/qml/CategoryFilterAccessory.qml" line="+8"/>
        <source>All</source>
        <translation>全部</translation>
    </message>
</context>
<context>
    <name>ChangeEmojiSkinToneAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+99"/>
        <source>%1 skin tone</source>
        <translation>%1 膚色</translation>
    </message>
</context>
<context>
    <name>ClearClipboardHistoryCommand</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.cpp" line="+35"/>
        <source>Clear Clipboard History</source>
        <translation>清除剪貼簿歷史</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clear the clipboard history</source>
        <translation>清除剪貼簿歷史</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Are you sure?</source>
        <translation>確定嗎？</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Your clipboard history will be gone forever :(</source>
        <translation>剪貼簿歷史會永遠消失 :(</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to clear clipboard history</source>
        <translation>清除剪貼簿歷史失敗</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard history cleared</source>
        <translation>剪貼簿歷史已清除</translation>
    </message>
</context>
<context>
    <name>ClipboardClearCommand</name>
    <message>
        <location line="-39"/>
        <source>Clear Current Clipboard Data</source>
        <translation>清除當前剪貼簿資料</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clear the current content of the clipboard</source>
        <translation>清除剪貼簿當前內容</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Failed to clear clipboard</source>
        <translation>清除剪貼簿失敗</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard cleared</source>
        <translation>剪貼簿已清除</translation>
    </message>
</context>
<context>
    <name>ClipboardExtension</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.hpp" line="+13"/>
        <source>Clipboard</source>
        <translation>剪貼簿</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>System clipboard integration</source>
        <translation>系統剪貼簿整合</translation>
    </message>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.cpp" line="+55"/>
        <source>Erase on startup</source>
        <translation>啟動時清除</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Erase clipboard history every time the vicinae server is started</source>
        <translation>每次啟動 Vicinae 伺服器時清除剪貼簿歷史</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Clipboard monitoring</source>
        <translation>剪貼簿監控</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Whether clipboard activity is recorded in the history. Every clipboard action performed while this is turned off will not be recorded.</source>
        <translation>是否將剪貼簿活動記錄到歷史中。關閉期間的所有剪貼簿操作都不會被記錄。</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Ignore Passwords</source>
        <translation>忽略密碼</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Ignore selections that can be identified as a password. This relies on the application providing an explicit hint that the selection is a password. While most password managers and private browser windows do, some might not implement this properly.</source>
        <translation>忽略可識別為密碼的選中內容。此功能依賴應用明確標記該內容為密碼。大多數密碼管理器和瀏覽器隱私視窗會提供此標記，但部分應用可能未正確實現。</translation>
    </message>
</context>
<context>
    <name>ClipboardFilterAccessory</name>
    <message>
        <location filename="../src/qml/qml/ClipboardFilterAccessory.qml" line="+8"/>
        <source>All</source>
        <translation>全部</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Text</source>
        <translation>文本</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Images</source>
        <translation>圖片</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Links</source>
        <translation>連結</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Files</source>
        <translation>檔案</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryCommand</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-history-command.cpp" line="+16"/>
        <source>Paste</source>
        <translation>貼上</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy</source>
        <translation>複製</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Default Action</source>
        <translation>預設操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to perform on pressing return. Paste is only available if your environment supports it.</source>
        <translation>按回車鍵時執行的預設操作。僅噹噹前環境支援時才能貼上。</translation>
    </message>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-history-command.hpp" line="+11"/>
        <source>Clipboard History</source>
        <translation>剪貼簿歷史</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse your clipboard&apos;s history, pin, edit and remove entries.</source>
        <translation>瀏覽剪貼簿歷史，並固定、編輯或刪除條目。</translation>
    </message>
</context>
<context>
    <name>ClipboardHistorySection</name>
    <message>
        <location filename="../src/qml/clipboard-history-model.cpp" line="+58"/>
        <source>Open Settings</source>
        <translation>開啟設定</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryView</name>
    <message>
        <location filename="../src/qml/qml/ClipboardHistoryView.qml" line="+187"/>
        <source>Type</source>
        <translation>型別</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Size</source>
        <translation>大小</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copied at</source>
        <translation>複製時間</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>Preview not available for this content type</source>
        <translation>無法預覽此型別的內容</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryViewHost</name>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.hpp" line="+84"/>
        <source>Loading...</source>
        <translation>正在載入…</translation>
    </message>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.cpp" line="+101"/>
        <source>Browse clipboard history...</source>
        <translation>瀏覽剪貼簿歷史…</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard monitoring unavailable</source>
        <translation>剪貼簿監控不可用</translation>
    </message>
    <message>
        <location line="+67"/>
        <source>Pause clipboard</source>
        <translation>暫停剪貼簿監控</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Resume clipboard</source>
        <translation>恢復剪貼簿監控</translation>
    </message>
    <message numerus="yes">
        <location line="+8"/>
        <source>%n Items</source>
        <translation>
            <numerusform>%n 項</numerusform>
        </translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Decryption failed</source>
        <translation>解密失敗</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Vicinae could not decrypt the data for this selection. It was most likely encrypted with a different key and cannot be recovered. You can remove this entry from the history.</source>
        <translation>Vicinae 無法解密此選中項的資料。該資料很可能使用了其他金鑰加密，無法恢復。你可以從歷史記錄中刪除此條目。</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Data unavailable</source>
        <translation>資料不可用</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The data for this selection could not be found on disk.</source>
        <translation>無法在磁碟上找到此選中項的資料。</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Data is encrypted</source>
        <translation>資料已加密</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Data for this selection was previously encrypted but the clipboard is not currently configured to use encryption. You should be able to fix this by enabling it in the settings.</source>
        <translation>此選中項的資料之前已加密，但剪貼簿目前未配置為使用加密。在設定中啟用加密應該可以解決此問題。</translation>
    </message>
</context>
<context>
    <name>ClipboardService</name>
    <message>
        <location filename="../src/services/clipboard/clipboard-service.cpp" line="+316"/>
        <source>Image (%1x%2)</source>
        <translation>圖片（%1x%2）</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Image</source>
        <translation>圖片</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Unknown</source>
        <translation>未知</translation>
    </message>
</context>
<context>
    <name>CloseWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="-37"/>
        <source>Close window</source>
        <translation>關閉視窗</translation>
    </message>
</context>
<context>
    <name>CommandLineSection</name>
    <message>
        <location filename="../src/qml/system-run-model.hpp" line="+19"/>
        <source>Execute query</source>
        <translation>執行查詢</translation>
    </message>
    <message>
        <location filename="../src/qml/system-run-model.cpp" line="+44"/>
        <source>Open in %1 (hold)</source>
        <translation>在 %1 中開啟（保持視窗）</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open in %1</source>
        <translation>在 %1 中開啟</translation>
    </message>
</context>
<context>
    <name>CommandListView</name>
    <message>
        <location filename="../src/qml/qml/CommandListView.qml" line="+12"/>
        <source>No results</source>
        <translation>無結果</translation>
    </message>
</context>
<context>
    <name>CommandRootItem</name>
    <message>
        <location filename="../src/root-search/extensions/extension-root-provider.cpp" line="+28"/>
        <location line="+48"/>
        <source>Command</source>
        <translation>命令</translation>
    </message>
    <message>
        <location line="-43"/>
        <location line="+27"/>
        <source>Open command</source>
        <translation>開啟命令</translation>
    </message>
    <message>
        <location line="-13"/>
        <source>Copy extension path</source>
        <translation>複製擴充套件路徑</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Internal Command</source>
        <translation>內部命令</translation>
    </message>
</context>
<context>
    <name>CompletionPopup</name>
    <message>
        <location filename="../src/qml/qml/CompletionPopup.qml" line="+11"/>
        <source>Filter...</source>
        <translation>篩選…</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorAnswerAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+29"/>
        <source>Answer copied to clipboard</source>
        <translation>答案已複製到剪貼簿</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy answer</source>
        <translation>複製答案失敗</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy Result</source>
        <translation>複製結果</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorQuestionAndAnswerAction</name>
    <message>
        <location line="+18"/>
        <source>Answer copied to clipboard</source>
        <translation>答案已複製到剪貼簿</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy answer</source>
        <translation>複製答案失敗</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy Question And Answer</source>
        <translation>複製問題和答案</translation>
    </message>
</context>
<context>
    <name>CopyClipboardSelection</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+29"/>
        <source>Selection copied to clipboard</source>
        <translation>選中內容已複製到剪貼簿</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to copy to clipboard</source>
        <translation>複製到剪貼簿失敗</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy to clipboard</source>
        <translation>複製到剪貼簿</translation>
    </message>
</context>
<context>
    <name>CopyItemDeeplink</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+75"/>
        <source>Deeplink copied in clipboard</source>
        <translation>深層連結已複製到剪貼簿</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Deeplink</source>
        <translation>複製深層連結</translation>
    </message>
</context>
<context>
    <name>CopyShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="+230"/>
        <source>Copied to clipboard</source>
        <translation>已複製到剪貼簿</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy shortcut</source>
        <translation>複製快捷方式</translation>
    </message>
</context>
<context>
    <name>CopyToClipboardAction</name>
    <message>
        <location filename="../src/clipboard-actions.hpp" line="+23"/>
        <source>Copied to clipboard</source>
        <translation>已複製到剪貼簿</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy to clipboard</source>
        <translation>複製到剪貼簿</translation>
    </message>
</context>
<context>
    <name>CreateExtensionCommand</name>
    <message>
        <location filename="../src/extensions/developer/developer-extension.hpp" line="+10"/>
        <source>Create Extension</source>
        <translation>建立擴充套件</translation>
    </message>
</context>
<context>
    <name>CreateExtensionFormView</name>
    <message>
        <location filename="../src/qml/qml/CreateExtensionFormView.qml" line="+15"/>
        <source>Author</source>
        <translation>作者</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>If you plan on submitting your extension to the &lt;a href=&quot;vicinae://launch/core/store&quot;&gt;Vicinae store&lt;/a&gt;, this must exactly match your GitHub handle. Otherwise, you can set it to anything.</source>
        <translation>如果要將擴充套件提交到 &lt;a href=&quot;vicinae://launch/core/store&quot;&gt;Vicinae Store&lt;/a&gt;，此項必須與 GitHub 使用者名稱完全一致。否則可任意填寫。</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Username</source>
        <translation>使用者名稱</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Extension Title</source>
        <translation>擴充套件標題</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>My Extension</source>
        <translation>我的擴充套件</translation>
    </message>
    <message>
        <location line="+8"/>
        <location line="+42"/>
        <source>Description</source>
        <translation>描述</translation>
    </message>
    <message>
        <location line="-36"/>
        <source>An extension that does super cool things</source>
        <translation>一個能完成超酷功能的擴充套件</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Location</source>
        <translation>位置</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Command Title</source>
        <translation>命令標題</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>My Wonderful Command</source>
        <translation>我的超棒命令</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>My command does this, and that...</source>
        <translation>我的命令可以完成這項和那項功能…</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Template</source>
        <translation>模板</translation>
    </message>
</context>
<context>
    <name>CreateExtensionSuccessViewHost</name>
    <message>
        <location filename="../src/qml/create-extension-success-view-host.cpp" line="+7"/>
        <source>
# Extension successfully created

Your new extension %1 has been succesfully created at `%2`.

For commands from this extension to be picked up by Vicinae, you need to run your extension in development mode at least once:

```bash
cd %2
npm install
npm run dev
```

You can learn more about extension development in the [Vicinae documentation](https://docs.vicinae.com/).
</source>
        <translation>
# 擴充套件建立成功

新擴充套件 %1 已成功創建於 `%2`。

要讓 Vicinae 識別此擴充套件中的命令，需要至少以開發模式執行一次：

```bash
cd %2
npm install
npm run dev
```

有關擴充套件開發的更多資訊，請參閱 [Vicinae 文件](https://docs.vicinae.com/)。
</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Open in %1</source>
        <translation>在 %1 中開啟</translation>
    </message>
</context>
<context>
    <name>CreateExtensionViewHost</name>
    <message>
        <location filename="../src/qml/create-extension-view-host.cpp" line="+44"/>
        <source>Create extension</source>
        <translation>建立擴充套件</translation>
    </message>
    <message>
        <location line="+18"/>
        <location line="+4"/>
        <location line="+19"/>
        <location line="+5"/>
        <source>Min. 3 chars</source>
        <translation>至少 3 個字元</translation>
    </message>
    <message>
        <location line="-20"/>
        <source>Min. 16 chars</source>
        <translation>至少 16 個字元</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Must exist</source>
        <translation>必須存在</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Form has errors</source>
        <translation>表單有錯誤</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Failed to create extension</source>
        <translation>建立擴充套件失敗</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Extension created!</source>
        <translation>擴充套件已建立！</translation>
    </message>
</context>
<context>
    <name>CreateShortcutCommand</name>
    <message>
        <location filename="../src/extensions/shortcut/create-shortcut-command.hpp" line="+10"/>
        <source>Create Shortcut</source>
        <translation>建立快捷方式</translation>
    </message>
</context>
<context>
    <name>CreateShortcutFromActiveBrowserTabCommand</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="+44"/>
        <source>Create Shortcut from Active Tab</source>
        <translation>從當前標籤頁建立快捷方式</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Create a vicinae shortcut from the currently active browser tab. May yield unexpected results if many browsers are connected at once.</source>
        <translation>從當前活動的瀏覽器標籤頁建立 Vicinae 快捷方式。如果同時連線了多個瀏覽器，結果可能不符合預期。</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>No active tab!</source>
        <translation>沒有活動標籤頁！</translation>
    </message>
</context>
<context>
    <name>CreateSnippetCommand</name>
    <message>
        <location filename="../src/extensions/snippet/create-snippet-command.hpp" line="+10"/>
        <source>Create Snippet</source>
        <translation>建立片段</translation>
    </message>
</context>
<context>
    <name>DMenuSection</name>
    <message>
        <location filename="../src/qml/dmenu-model.cpp" line="+93"/>
        <source>Select entry</source>
        <translation>選擇條目</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pass search text</source>
        <translation>傳遞搜尋文本</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Select and copy entry</source>
        <translation>選擇並複製條目</translation>
    </message>
</context>
<context>
    <name>DMenuView</name>
    <message>
        <location filename="../src/qml/qml/DMenuView.qml" line="+79"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Path</source>
        <translation>路徑</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>型別</translation>
    </message>
</context>
<context>
    <name>DMenuViewHost</name>
    <message>
        <location filename="../src/qml/dmenu-view-host.cpp" line="+34"/>
        <source>Search entries...</source>
        <translation>搜尋條目…</translation>
    </message>
    <message>
        <location line="+73"/>
        <source>Pass search text</source>
        <translation>傳遞搜尋文本</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Pass and copy search text</source>
        <translation>傳遞並複製搜尋文本</translation>
    </message>
</context>
<context>
    <name>DetailListView</name>
    <message>
        <location filename="../src/qml/qml/DetailListView.qml" line="+32"/>
        <source>No results</source>
        <translation>無結果</translation>
    </message>
</context>
<context>
    <name>DeveloperExtension</name>
    <message>
        <location filename="../src/extensions/developer/developer-extension.hpp" line="+10"/>
        <source>Developer</source>
        <translation>開發者</translation>
    </message>
</context>
<context>
    <name>DisableApplication</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+8"/>
        <source>Disable item</source>
        <translation>停用專案</translation>
    </message>
</context>
<context>
    <name>DisableItemAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="+88"/>
        <source>Are you sure?</source>
        <translation>確定嗎？</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You will need to go in the settings to manually re-enable it.</source>
        <translation>需要前往設定手動重新啟用。</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Disable</source>
        <translation>停用</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Item disabled</source>
        <translation>專案已停用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to disable</source>
        <translation>停用失敗</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Disable item</source>
        <translation>停用專案</translation>
    </message>
</context>
<context>
    <name>DismissNewsAction</name>
    <message>
        <location filename="../src/services/news/news-service.cpp" line="+34"/>
        <source>Dismiss</source>
        <translation>忽略</translation>
    </message>
</context>
<context>
    <name>DuplicateShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-50"/>
        <source>Duplicate link</source>
        <translation>建立連結副本</translation>
    </message>
</context>
<context>
    <name>EditClipboardKeywordsAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+49"/>
        <source>Additional keywords that will be used to index this selection.</source>
        <translation>用於索引此選中項的其他關鍵詞。</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit keywords</source>
        <translation>編輯關鍵詞</translation>
    </message>
</context>
<context>
    <name>EditEmojiKeywordsAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+35"/>
        <source>Additional keywords that will be used to index this glyph</source>
        <translation>用於索引此字元的其他關鍵詞</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Edit keyword</source>
        <translation>編輯關鍵詞</translation>
    </message>
</context>
<context>
    <name>EditKeywordsFormView</name>
    <message>
        <location filename="../src/qml/qml/EditKeywordsFormView.qml" line="+19"/>
        <source>Keywords</source>
        <translation>關鍵詞</translation>
    </message>
</context>
<context>
    <name>EditKeywordsViewHost</name>
    <message>
        <location filename="../src/qml/edit-keywords-view-host.cpp" line="+26"/>
        <source>Submit</source>
        <translation>提交</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Keywords edited</source>
        <translation>關鍵詞已編輯</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to edit keywords</source>
        <translation>編輯關鍵詞失敗</translation>
    </message>
</context>
<context>
    <name>EditShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-41"/>
        <source>Edit shortcut</source>
        <translation>編輯快捷方式</translation>
    </message>
</context>
<context>
    <name>EmojiCategoryFilterAccessory</name>
    <message>
        <location filename="../src/qml/qml/EmojiCategoryFilterAccessory.qml" line="+8"/>
        <source>All</source>
        <translation>全部</translation>
    </message>
</context>
<context>
    <name>EmojiGridModel</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.hpp" line="+58"/>
        <source>Search for emojis and symbols...</source>
        <translation>搜尋表情符號和符號…</translation>
    </message>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+241"/>
        <source>Pinned</source>
        <translation>已固定</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Recently used</source>
        <translation>最近使用</translation>
    </message>
</context>
<context>
    <name>EmojiGridViewHost</name>
    <message>
        <location filename="../src/qml/emoji-grid-view-host.hpp" line="+40"/>
        <source>All</source>
        <translation>全部</translation>
    </message>
</context>
<context>
    <name>EmptyView</name>
    <message>
        <location filename="../src/qml/qml/EmptyView.qml" line="+7"/>
        <source>No results</source>
        <translation>無結果</translation>
    </message>
</context>
<context>
    <name>EnabledFallbackSection</name>
    <message>
        <location filename="../src/qml/manage-fallback-model.hpp" line="-19"/>
        <source>Enabled</source>
        <translation>已啟用</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-fallback-model.cpp" line="-22"/>
        <source>Disable fallback</source>
        <translation>停用後備項</translation>
    </message>
</context>
<context>
    <name>Expansion</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.hpp" line="+31"/>
        <source>Keyword cannot be empty</source>
        <translation>關鍵詞不能為空</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Keyword exceeds maximum length of %1</source>
        <translation>關鍵詞超過最大長度 %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keyword must only contain printable ASCII characters (no spaces)</source>
        <translation>關鍵詞只能包含可列印的 ASCII 字元（不含空格）</translation>
    </message>
</context>
<context>
    <name>ExtensionBoilerplateGenerator</name>
    <message>
        <location filename="../src/services/extension-boilerplate-generator/extension-boilerplate-generator.cpp" line="+24"/>
        <source>Simple List</source>
        <translation>簡單列表</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>List with Detail</source>
        <translation>帶詳情的列表</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Controlled List</source>
        <translation>受控列表</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Simple Detail</source>
        <translation>簡單詳情</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>No View</source>
        <translation>無檢視</translation>
    </message>
</context>
<context>
    <name>ExtensionErrorViewHost</name>
    <message>
        <location filename="../src/qml/extension-error-view-host.cpp" line="+6"/>
        <source># Extension crashed 💥!

This extension threw an uncaught exception and crashed as a result.

Find the full stacktrace below. You can also directly copy it from the action menu.

```
%1
```</source>
        <translation># 擴充套件崩潰了 💥！

此擴充套件丟擲了未捕獲的異常並因此崩潰。

完整堆疊跟蹤如下，也可直接從操作選單複製。

```
%1
```</translation>
    </message>
</context>
<context>
    <name>ExtensionFormModel</name>
    <message>
        <location filename="../src/qml/extension-form-model.cpp" line="+200"/>
        <source>One or more fields have errors</source>
        <translation>一個或多個欄位有錯誤</translation>
    </message>
</context>
<context>
    <name>ExtensionGridModel</name>
    <message>
        <location filename="../src/qml/extension-grid-model.cpp" line="+186"/>
        <source>Search...</source>
        <translation>搜尋…</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>No results</source>
        <translation>無結果</translation>
    </message>
</context>
<context>
    <name>ExtensionGridView</name>
    <message>
        <location filename="../src/qml/qml/ExtensionGridView.qml" line="+9"/>
        <source>No results</source>
        <translation>無結果</translation>
    </message>
</context>
<context>
    <name>ExtensionListModel</name>
    <message>
        <location filename="../src/qml/extension-list-model.cpp" line="+189"/>
        <source>Search...</source>
        <translation>搜尋…</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No results</source>
        <translation>無結果</translation>
    </message>
</context>
<context>
    <name>ExtensionSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/ExtensionSettingsPage.qml" line="+94"/>
        <source>Description</source>
        <translation>描述</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Preferences</source>
        <translation>偏好設定</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Commands</source>
        <translation>命令</translation>
    </message>
    <message>
        <location line="+105"/>
        <source>Shortcut</source>
        <translation>快捷方式</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Add Alias</source>
        <translation>新增別名</translation>
    </message>
</context>
<context>
    <name>ExtensionView</name>
    <message>
        <location filename="../src/qml/qml/ExtensionView.qml" line="+99"/>
        <source>No results</source>
        <translation>無結果</translation>
    </message>
</context>
<context>
    <name>FileExtension</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="+65"/>
        <source>System files</source>
        <translation>系統檔案</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Integrate with system files</source>
        <translation>與系統檔案整合</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Enabled</source>
        <translation>已啟用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Whether to run the file indexer in the background. When turned off, the indexer process is stopped entirely and file search becomes unavailable until it is turned back on.</source>
        <translation>是否在後臺執行檔案索引器。關閉後索引器程序將完全停止，重新啟用前無法搜尋檔案。</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Search paths</source>
        <translation>搜尋路徑</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Directories that Vicinae will search</source>
        <translation>Vicinae 要搜尋的目錄</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Excluded search paths</source>
        <translation>排除的搜尋路徑</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Directories to exclude from file indexing</source>
        <translation>不納入檔案索引的目錄</translation>
    </message>
</context>
<context>
    <name>FilePreview</name>
    <message>
        <location filename="../src/qml/qml/FilePreview.qml" line="+42"/>
        <source>Preview not available for this file type</source>
        <translation>無法預覽此檔案型別</translation>
    </message>
</context>
<context>
    <name>FocusWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="-17"/>
        <source>Focus window</source>
        <translation>聚焦視窗</translation>
    </message>
</context>
<context>
    <name>FontBrowserViewHost</name>
    <message>
        <location filename="../src/qml/font-browser-view-host.hpp" line="+38"/>
        <source>All</source>
        <translation>全部</translation>
    </message>
</context>
<context>
    <name>FontExtension</name>
    <message>
        <location filename="../src/extensions/font/font-extension.hpp" line="+9"/>
        <source>Font</source>
        <translation>字型</translation>
    </message>
</context>
<context>
    <name>FontGridModel</name>
    <message>
        <location filename="../src/qml/font-grid-model.hpp" line="+47"/>
        <source>Search fonts...</source>
        <translation>搜尋字型…</translation>
    </message>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="+133"/>
        <source>All Fonts (%1)</source>
        <translation>所有字型（%1）</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Results (%1)</source>
        <translation>結果（%1）</translation>
    </message>
</context>
<context>
    <name>Footer</name>
    <message>
        <location filename="../src/qml/qml/Footer.qml" line="+60"/>
        <source>Actions</source>
        <translation>操作</translation>
    </message>
</context>
<context>
    <name>ForceQuitAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+95"/>
        <source>Force Quit Application</source>
        <translation>強制退出應用程式</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to force quit %1</source>
        <translation>強制退出 %1 失敗</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Force quit %1</source>
        <translation>強制退出 %1</translation>
    </message>
</context>
<context>
    <name>ForgetTelemetryCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+210"/>
        <source>Forget Past Vicinae Telemetry</source>
        <translation>清除以往的 Vicinae 遙測關聯</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Asks the vicinae server to anonymize telemetry data that was sent with your vicinae instance ID attached. The ID is only linked to your vicinae install, which has no direct relationship with your system.</source>
        <translation>要求 Vicinae 伺服器匿名化附帶 Vicinae 例項 ID 傳送的遙測資料。該 ID 僅與此次 Vicinae 安裝關聯，與系統沒有直接關係。</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Processing...</source>
        <translation>正在處理…</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Past telemetry was successfully detached from your vicinae user ID.</source>
        <translation>以往遙測資料已與 Vicinae 使用者 ID 解除關聯。</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to forget past telemetry data</source>
        <translation>清除以往遙測資料關聯失敗</translation>
    </message>
</context>
<context>
    <name>FormAppSelector</name>
    <message>
        <location filename="../src/qml/qml/FormAppSelector.qml" line="+50"/>
        <source>All applications</source>
        <translation>所有應用程式</translation>
    </message>
    <message>
        <location line="+70"/>
        <source>+ Restrict to app…</source>
        <translation>+ 限定到應用…</translation>
    </message>
</context>
<context>
    <name>FormFilePicker</name>
    <message>
        <location filename="../src/qml/qml/FormFilePicker.qml" line="+84"/>
        <source>Select files</source>
        <translation>選擇檔案</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Select a file</source>
        <translation>選擇檔案</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Select a directory</source>
        <translation>選擇目錄</translation>
    </message>
    <message>
        <location line="+46"/>
        <source>No directory selected</source>
        <translation>未選擇目錄</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No file selected</source>
        <translation>未選擇檔案</translation>
    </message>
    <message>
        <location line="+115"/>
        <source>+ Add folder…</source>
        <translation>+ 新增資料夾…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>+ Add file…</source>
        <translation>+ 新增檔案…</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsModel</name>
    <message>
        <location filename="../src/qml/general-settings-model.cpp" line="+176"/>
        <location line="+11"/>
        <source>None</source>
        <translation>無</translation>
    </message>
    <message>
        <location line="-10"/>
        <location line="+10"/>
        <source>Blurred</source>
        <translation>模糊</translation>
    </message>
    <message>
        <location line="-8"/>
        <location line="+8"/>
        <source>Liquid Glass</source>
        <translation>液態玻璃</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Window material</source>
        <translation>視窗材質</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Themes</source>
        <translation>主題</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Fonts</source>
        <translation>字型</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Icon Themes</source>
        <translation>圖示主題</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Favicon Services</source>
        <translation>網站圖示服務</translation>
    </message>
    <message>
        <location line="+13"/>
        <location line="+10"/>
        <source>Default</source>
        <translation>預設</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Keybinding Schemes</source>
        <translation>快捷鍵方案</translation>
    </message>
    <message>
        <location line="+48"/>
        <location line="+10"/>
        <source>System default</source>
        <translation>系統預設</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>Languages</source>
        <translation>語言</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/GeneralSettingsPage.qml" line="+29"/>
        <source>Behavior</source>
        <translation>行為</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Launcher hotkey</source>
        <translation>啟動器快捷鍵</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Global shortcut to toggle the Vicinae launcher.</source>
        <translation>用於顯示或隱藏 Vicinae 啟動器的全域性快捷鍵。</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Close on focus loss</source>
        <translation>失去焦點時關閉</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Close on Escape</source>
        <translation>按 Escape 鍵關閉</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pressing Escape closes the launcher instead of navigating one view back.</source>
        <translation>按 Escape 鍵時關閉啟動器，而不是返回上一級檢視。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Pop to root on close</source>
        <translation>關閉時返回根檢視</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reset the navigation state when the launcher window is closed.</source>
        <translation>關閉啟動器視窗時重置導航狀態。</translation>
    </message>
    <message>
        <location line="+10"/>
        <location line="+7"/>
        <source>Language</source>
        <translation>語言</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Requires restarting Vicinae to take effect.</source>
        <translation>重啟 Vicinae 後生效。</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Privacy</source>
        <translation>隱私</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Basic usage statistics</source>
        <translation>基本使用情況統計</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Send basic system and vicinae installation information on startup to help improve Vicinae.</source>
        <translation>啟動時傳送基本系統和 Vicinae 安裝資訊，幫助改進 Vicinae。</translation>
    </message>
</context>
<context>
    <name>GenericGridView</name>
    <message>
        <location filename="../src/qml/qml/GenericGridView.qml" line="+33"/>
        <source>No results</source>
        <translation>無結果</translation>
    </message>
</context>
<context>
    <name>GenericListView</name>
    <message>
        <location filename="../src/qml/qml/GenericListView.qml" line="+25"/>
        <source>No results</source>
        <translation>無結果</translation>
    </message>
</context>
<context>
    <name>GlobalShortcutService</name>
    <message>
        <location filename="../src/services/global-shortcuts/global-shortcut-service.cpp" line="+43"/>
        <source>Toggle Vicinae</source>
        <translation>顯示或隱藏 Vicinae</translation>
    </message>
    <message>
        <location line="+84"/>
        <source>the launcher hotkey</source>
        <translation>啟動器快捷鍵</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>another command</source>
        <translation>其他命令</translation>
    </message>
</context>
<context>
    <name>Gnome::Workspace</name>
    <message>
        <location filename="../src/services/window-manager/gnome/gnome-workspace.cpp" line="+18"/>
        <source>Workspace %1</source>
        <translation>工作區 %1</translation>
    </message>
</context>
<context>
    <name>HibernateCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+127"/>
        <source>Hibernate System</source>
        <translation>休眠系統</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Suspend the system to disk. This turns off the system completely and saves its state on disk, to be restored on next boot.</source>
        <translation>將系統狀態儲存到磁碟後完全關機，並在下次啟動時恢復。</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>System can&apos;t hibernate</source>
        <translation>系統無法休眠</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to hibernate</source>
        <translation>休眠失敗</translation>
    </message>
</context>
<context>
    <name>IconBrowserCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+25"/>
        <source>Search Builtin Icons</source>
        <translation>搜尋內建圖示</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search Vicinae builtin set of icons</source>
        <translation>搜尋 Vicinae 內建圖示集</translation>
    </message>
</context>
<context>
    <name>ImageViewer</name>
    <message>
        <location filename="../src/qml/qml/ImageViewer.qml" line="+159"/>
        <source>%1 / %2</source>
        <translation>%1 / %2</translation>
    </message>
</context>
<context>
    <name>InspectLocalStorage</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+12"/>
        <source>Inspect Local Storage</source>
        <translation>檢視本地儲存</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Browse data stored in Vicinae&apos;s local storage. This includes data stored for builtin extensions as well as third-party extensions making use of the LocalStorage API.</source>
        <translation>瀏覽 Vicinae 本地儲存中的資料，包括內建擴充套件的資料，以及使用 LocalStorage API 的第三方擴充套件資料。</translation>
    </message>
</context>
<context>
    <name>InstallUpdateAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+188"/>
        <source>Install Update</source>
        <translation>安裝更新</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>An update is already in progress</source>
        <translation>已有更新正在進行</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsSection</name>
    <message>
        <location filename="../src/qml/installed-extensions-model.hpp" line="+18"/>
        <source>Installed Extensions ({count})</source>
        <translation>已安裝的擴充套件（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/installed-extensions-model.cpp" line="+36"/>
        <source>Local</source>
        <translation>本地</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Copy</source>
        <translation>複製</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy Name</source>
        <translation>複製名稱</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy ID</source>
        <translation>複製 ID</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy Path</source>
        <translation>複製路徑</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy Author</source>
        <translation>複製作者</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsViewHost</name>
    <message>
        <location filename="../src/qml/installed-extensions-view-host.cpp" line="+12"/>
        <source>Search extensions...</source>
        <translation>搜尋擴充套件…</translation>
    </message>
</context>
<context>
    <name>InternalExtension</name>
    <message>
        <location filename="../src/extensions/internal/internal-extension.hpp" line="+32"/>
        <location line="+1"/>
        <source>Internal Commands</source>
        <translation>內部命令</translation>
    </message>
</context>
<context>
    <name>KeyboardBridge</name>
    <message>
        <location filename="../src/qml/keyboard-bridge.hpp" line="+51"/>
        <source>Modifier required</source>
        <translation>需要修飾鍵</translation>
    </message>
</context>
<context>
    <name>LauncherWindow</name>
    <message>
        <location filename="../src/qml/qml/LauncherWindow.qml" line="+29"/>
        <source>Vicinae Launcher</source>
        <translation>Vicinae 啟動器</translation>
    </message>
    <message>
        <location filename="../src/qml/launcher-window.cpp" line="+571"/>
        <source>Open Settings</source>
        <translation>開啟設定</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keyboard Shortcuts</source>
        <translation>鍵盤快捷鍵</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Documentation</source>
        <translation>文件</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+5"/>
        <source>Opened in browser</source>
        <translation>已在瀏覽器中開啟</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Report a Bug</source>
        <translation>報告問題</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>About Vicinae</source>
        <translation>關於 Vicinae</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemSection</name>
    <message>
        <location filename="../src/qml/local-storage-model.hpp" line="+19"/>
        <source>Items ({count})</source>
        <translation>專案（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/local-storage-model.cpp" line="+31"/>
        <source>Show value</source>
        <translation>顯示值</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemViewHost</name>
    <message>
        <location filename="../src/qml/local-storage-view-host.cpp" line="+23"/>
        <source>Search items...</source>
        <translation>搜尋專案…</translation>
    </message>
</context>
<context>
    <name>LocalStorageNamespaceSection</name>
    <message>
        <location filename="../src/qml/local-storage-model.hpp" line="-11"/>
        <source>Namespaces ({count})</source>
        <translation>名稱空間（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/local-storage-model.cpp" line="-16"/>
        <source>Browse namespace</source>
        <translation>瀏覽名稱空間</translation>
    </message>
</context>
<context>
    <name>LocalStorageViewHost</name>
    <message>
        <location filename="../src/qml/local-storage-view-host.cpp" line="-15"/>
        <source>Search namespaces...</source>
        <translation>搜尋名稱空間…</translation>
    </message>
</context>
<context>
    <name>LockCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-48"/>
        <source>Lock Session</source>
        <translation>鎖定會話</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lock the current user session</source>
        <translation>鎖定當前使用者會話</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System can&apos;t lock</source>
        <translation>系統無法鎖定</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to lock</source>
        <translation>鎖定失敗</translation>
    </message>
</context>
<context>
    <name>LogOutCommand</name>
    <message>
        <location line="+176"/>
        <source>Log Out</source>
        <translation>退出登入</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Terminate the current user session. If you simply want to lock your session you should use &apos;Lock Session&apos; instead.</source>
        <translation>終止當前使用者會話。如果只是想鎖定會話，請改用“鎖定會話”。</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>System can&apos;t logout</source>
        <translation>系統無法退出登入</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to log out</source>
        <translation>退出登入失敗</translation>
    </message>
</context>
<context>
    <name>MacOSGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/macos-global-shortcut-backend.cpp" line="+238"/>
        <source>unsupported or invalid trigger</source>
        <translation>不支援或無效的觸發鍵</translation>
    </message>
</context>
<context>
    <name>MacSettingsRootItem</name>
    <message>
        <location filename="../src/root-search/macos-settings/macos-settings-root-provider.mm" line="+132"/>
        <location line="+9"/>
        <source>System Settings</source>
        <translation>系統設定</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Bundle ID</source>
        <translation>Bundle ID</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Legacy ID</source>
        <translation>舊版 ID</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Where</source>
        <translation>位置</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Open %1 Settings</source>
        <translation>開啟“%1”設定</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy URL</source>
        <translation>複製 URL</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy Bundle ID</source>
        <translation>複製 Bundle ID</translation>
    </message>
</context>
<context>
    <name>MacSettingsRootProvider</name>
    <message>
        <location line="+13"/>
        <source>System Settings</source>
        <translation>系統設定</translation>
    </message>
</context>
<context>
    <name>MacosUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/macos-update-installer.mm" line="+208"/>
        <source>This installation cannot update itself</source>
        <translation>此安裝無法自行更新</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Mounting update image…</source>
        <translation>正在掛載更新映像…</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Failed to mount the update image</source>
        <translation>掛載更新映像失敗</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Could not find the update image mount point</source>
        <translation>找不到更新映像的掛載點</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Verifying update…</source>
        <translation>正在驗證更新…</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Installing update…</source>
        <translation>正在安裝更新…</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to stage update: %1</source>
        <translation>暫存更新失敗：%1</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Failed to move the current app aside: %1</source>
        <translation>移開當前應用失敗：%1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to install the new app: %1</source>
        <translation>安裝新應用失敗：%1</translation>
    </message>
</context>
<context>
    <name>ManageFallbackActions</name>
    <message>
        <location filename="../src/actions/fallback-actions.hpp" line="+15"/>
        <source>Manage Fallback Actions</source>
        <translation>管理後備操作</translation>
    </message>
</context>
<context>
    <name>ManageFallbackCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/configure-fallback-command.hpp" line="+11"/>
        <source>Configure Fallback Commands</source>
        <translation>配置後備命令</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Configure what commands are to be presented as fallback options when nothing matches the search in the root search.</source>
        <translation>配置根搜尋無匹配項時顯示的後備命令。</translation>
    </message>
</context>
<context>
    <name>ManageFallbackViewHost</name>
    <message>
        <location filename="../src/qml/manage-fallback-view-host.cpp" line="+12"/>
        <source>Search commands...</source>
        <translation>搜尋命令…</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsCommand</name>
    <message>
        <location filename="../src/extensions/shortcut/manage-shortcuts-command.hpp" line="+9"/>
        <source>Manage Shortcuts</source>
        <translation>管理快捷方式</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsSection</name>
    <message>
        <location filename="../src/qml/manage-shortcuts-model.hpp" line="+17"/>
        <source>Shortcuts ({count})</source>
        <translation>快捷方式（{count}）</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsViewHost</name>
    <message>
        <location filename="../src/qml/manage-shortcuts-view-host.cpp" line="+28"/>
        <source>Search shortcuts...</source>
        <translation>搜尋快捷方式…</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Application</source>
        <translation>應用程式</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Opened</source>
        <translation>開啟次數</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Last Opened</source>
        <translation>上次開啟</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Never</source>
        <translation>從未</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Created at</source>
        <translation>建立時間</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsCommand</name>
    <message>
        <location filename="../src/extensions/snippet/manage-snippets-command.hpp" line="+10"/>
        <source>Manage Snippets</source>
        <translation>管理片段</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsSection</name>
    <message>
        <location filename="../src/qml/manage-snippets-model.hpp" line="+18"/>
        <source>Snippets ({count})</source>
        <translation>片段（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-snippets-model.cpp" line="+32"/>
        <source>Copy to clipboard</source>
        <translation>複製到剪貼簿</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copied to clipboard</source>
        <translation>已複製到剪貼簿</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy to clipboard</source>
        <translation>複製到剪貼簿失敗</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Edit snippet</source>
        <translation>編輯片段</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Duplicate snippet</source>
        <translation>建立片段副本</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Remove snippet</source>
        <translation>移除片段</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove snippet</source>
        <translation>移除片段失敗</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsViewHost</name>
    <message>
        <location filename="../src/qml/manage-snippets-view-host.hpp" line="+54"/>
        <source>No snippets</source>
        <translation>沒有片段</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Create a snippet to get started</source>
        <translation>建立一個片段即可開始</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-snippets-view-host.cpp" line="+28"/>
        <source>Search for snippets...</source>
        <translation>搜尋片段…</translation>
    </message>
    <message>
        <location line="+25"/>
        <source>Text</source>
        <translation>文本</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>File</source>
        <translation>檔案</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>型別</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Created at</source>
        <translation>建立時間</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Updated at</source>
        <translation>更新時間</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Keyword</source>
        <translation>關鍵詞</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Apps</source>
        <translation>應用程式</translation>
    </message>
    <message>
        <location line="+68"/>
        <source>Create snippet</source>
        <translation>建立片段</translation>
    </message>
</context>
<context>
    <name>MarkItemAsFavorite</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="-60"/>
        <source>Mark as favorite</source>
        <translation>標為收藏</translation>
    </message>
</context>
<context>
    <name>MarkdownShowcase</name>
    <message>
        <location filename="../src/extensions/internal/markdown-showcase-command.hpp" line="+172"/>
        <source>Markdown Showcase</source>
        <translation>Markdown 功能展示</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Preview all supported markdown features</source>
        <translation>預覽所有支援的 Markdown 功能</translation>
    </message>
</context>
<context>
    <name>MarkdownView</name>
    <message>
        <location filename="../src/qml/qml/markdown/MarkdownView.qml" line="+265"/>
        <source>Copy</source>
        <translation>複製</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Select All</source>
        <translation>全選</translation>
    </message>
</context>
<context>
    <name>MdCallout</name>
    <message>
        <location filename="../src/qml/qml/markdown/MdCallout.qml" line="+35"/>
        <source>Caution</source>
        <translation>危險</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Warning</source>
        <translation>警告</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Important</source>
        <translation>重要</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tip</source>
        <translation>提示</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Note</source>
        <translation>備註</translation>
    </message>
</context>
<context>
    <name>MdCodeBlock</name>
    <message>
        <location filename="../src/qml/qml/markdown/MdCodeBlock.qml" line="+58"/>
        <source>Copied!</source>
        <translation>已複製！</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Copy</source>
        <translation>複製</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceView</name>
    <message>
        <location filename="../src/qml/qml/MissingPreferenceView.qml" line="+28"/>
        <source>Welcome to %1</source>
        <translation>歡迎使用 %1</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Before you can use this command, you need to fill in the required preference fields below.</source>
        <translation>使用此命令前，需要填寫以下必填偏好設定。</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceViewHost</name>
    <message>
        <location filename="../src/qml/missing-preference-view-host.cpp" line="+202"/>
        <source>Save preferences</source>
        <translation>儲存偏好設定</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Please fill in all required fields</source>
        <translation>請填寫所有必填欄位</translation>
    </message>
</context>
<context>
    <name>NavigationController</name>
    <message>
        <location filename="../src/navigation-controller.cpp" line="+654"/>
        <source>Extension manager is not running</source>
        <translation>擴充套件管理器未執行</translation>
    </message>
</context>
<context>
    <name>NewsService</name>
    <message>
        <location filename="../src/services/news/news-service.cpp" line="+68"/>
        <source>Telemetry</source>
        <translation>遙測</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>We now collect basic usage statistics on startup</source>
        <translation>現在會在啟動時收集基本使用情況統計</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Learn more</source>
        <translation>瞭解更多</translation>
    </message>
</context>
<context>
    <name>NullUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/null-update-installer.hpp" line="+14"/>
        <source>Self update is not supported on this platform</source>
        <translation>此平臺不支援自動更新</translation>
    </message>
</context>
<context>
    <name>OAuthOverlayView</name>
    <message>
        <location filename="../src/qml/qml/OAuthOverlayView.qml" line="+91"/>
        <source>Continue with %1</source>
        <translation>使用 %1 繼續</translation>
    </message>
    <message>
        <location line="+33"/>
        <source>You&apos;re in!</source>
        <translation>連線成功！</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Successfully connected to %1.
Back to command in an instant...</source>
        <translation>已成功連線到 %1。
即將返回命令…</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-31"/>
        <source>Manage OAuth Token Sets</source>
        <translation>管理 OAuth 令牌組</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Manage OAuth token sets that have been saved by extensions providing OAuth integrations.</source>
        <translation>管理由提供 OAuth 整合的擴充套件儲存的 OAuth 令牌組。</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreSection</name>
    <message>
        <location filename="../src/qml/oauth-token-store-model.hpp" line="+17"/>
        <source>OAuth Token Sets ({count})</source>
        <translation>OAuth 令牌組（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/oauth-token-store-model.cpp" line="+20"/>
        <source>Expired</source>
        <translation>已過期</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Remove token set</source>
        <translation>移除令牌組</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Are you sure?</source>
        <translation>確定嗎？</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You will need to go through the OAuth login flow again the next time you want to use this service</source>
        <translation>下次使用此服務時，需要重新完成 OAuth 登入流程</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to remove token set</source>
        <translation>移除令牌組失敗</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Token set removed</source>
        <translation>已移除令牌組</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy</source>
        <translation>複製</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy Access Token</source>
        <translation>複製訪問令牌</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Refresh Token</source>
        <translation>複製重新整理令牌</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy ID Token</source>
        <translation>複製 ID 令牌</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy Scopes</source>
        <translation>複製作用域</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Expiration Date</source>
        <translation>複製過期時間</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreViewHost</name>
    <message>
        <location filename="../src/qml/oauth-token-store-view-host.cpp" line="+12"/>
        <source>Search token sets...</source>
        <translation>搜尋令牌組…</translation>
    </message>
</context>
<context>
    <name>OnboardingWindow</name>
    <message>
        <location filename="../src/qml/qml/OnboardingWindow.qml" line="+39"/>
        <source>Grant Access</source>
        <translation>授予訪問許可權</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Granted</source>
        <translation>已授予</translation>
    </message>
    <message>
        <location line="+16"/>
        <location line="+39"/>
        <source>Welcome to Vicinae</source>
        <translation>歡迎使用 Vicinae</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Let&apos;s set it up. It only takes a minute.</source>
        <translation>來完成初始設定吧，只需一分鐘。</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Permissions</source>
        <translation>許可權</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Vicinae needs additional permissions in order to make the best of your Mac.</source>
        <translation>Vicinae 需要額外許可權才能充分利用你的 Mac。</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Accessibility</source>
        <translation>輔助功能</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Used to paste, expand snippets, and move windows.</source>
        <translation>用於貼上、展開片段和移動視窗。</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Full Disk Access</source>
        <translation>完全磁碟訪問許可權</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Allows file search to cover your entire disk.</source>
        <translation>讓檔案搜尋覆蓋整個磁碟。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Notifications</source>
        <translation>通知</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Allows extensions to send desktop notifications.</source>
        <translation>允許擴充套件傳送桌面通知。</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Accessibility is required: global shortcuts, paste, and snippet expansion cannot work without it.</source>
        <translation>必須授予輔助功能許可權，否則全域性快捷鍵、貼上和片段展開將無法使用。</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Full disk access needs to be explicitly enabled if you want file search to cover all your files.</source>
        <translation>若要搜尋所有檔案，需要明確啟用完全磁碟訪問許可權。</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Make it your own</source>
        <translation>按你的喜好設定</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>You will be able to change these settings later.</source>
        <translation>這些設定稍後仍可更改。</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Theme</source>
        <translation>主題</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Shared across the entire app.</source>
        <translation>在整個應用中共用。</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Global hotkey</source>
        <translation>全域性快捷鍵</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Opens the launcher from anywhere.</source>
        <translation>可在任意位置開啟啟動器。</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Launch at login</source>
        <translation>登入時啟動</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Starts Vicinae in the background at login.</source>
        <translation>登入時在後臺啟動 Vicinae。</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Setup complete</source>
        <translation>設定完成</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Vicinae is running. Open the launcher with:</source>
        <translation>Vicinae 已在執行。使用以下快捷鍵開啟啟動器：</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Vicinae is open source software.</source>
        <translation>Vicinae 是開源軟體。</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Sponsor</source>
        <translation>贊助</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Back</source>
        <translation>返回</translation>
    </message>
    <message>
        <location line="+44"/>
        <source>Finish</source>
        <translation>完成</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Continue</source>
        <translation>繼續</translation>
    </message>
</context>
<context>
    <name>OpenAboutCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/open-about-command.hpp" line="+13"/>
        <source>About</source>
        <translation>關於</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the &quot;About&quot; tab of the vicinae settings.</source>
        <translation>開啟 Vicinae 設定中的“關於”標籤頁。</translation>
    </message>
</context>
<context>
    <name>OpenAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-56"/>
        <source>Failed to start app</source>
        <translation>啟動應用失敗</translation>
    </message>
</context>
<context>
    <name>OpenAppLocationAction</name>
    <message>
        <location line="-36"/>
        <source>Open Location</source>
        <translation>開啟所在位置</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to open app location</source>
        <translation>開啟應用所在位置失敗</translation>
    </message>
</context>
<context>
    <name>OpenBuiltinCommandAction</name>
    <message>
        <location filename="../src/command-actions.hpp" line="+17"/>
        <source>Open command</source>
        <translation>開啟命令</translation>
    </message>
</context>
<context>
    <name>OpenCalculatorHistoryAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+9"/>
        <source>Open Calculator History</source>
        <translation>開啟計算器歷史記錄</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-32"/>
        <source>Open shortcut</source>
        <translation>開啟快捷方式</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutWithAction</name>
    <message>
        <location line="+85"/>
        <source>Open with...</source>
        <translation>開啟方式…</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelItemAction</name>
    <message>
        <location filename="../src/root-search/control-panel/control-panel-root-provider.cpp" line="+45"/>
        <source>Failed to open settings</source>
        <translation>開啟設定失敗</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelTaskAction</name>
    <message>
        <location line="+26"/>
        <source>Failed to open settings</source>
        <translation>開啟設定失敗</translation>
    </message>
</context>
<context>
    <name>OpenDefaultVicinaeConfig</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-157"/>
        <source>Open Default Config File</source>
        <translation>開啟預設配置檔案</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the default vicinae configuration file</source>
        <translation>開啟 Vicinae 預設配置檔案</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Failed to open temporary file</source>
        <translation>開啟臨時檔案失敗</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Failed to open default config file</source>
        <translation>開啟預設配置檔案失敗</translation>
    </message>
</context>
<context>
    <name>OpenDiscordCommand</name>
    <message>
        <location line="-66"/>
        <source>Join the Discord Server</source>
        <translation>加入 Discord 伺服器</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open link to join the official Vicinae discord server.</source>
        <translation>開啟連結以加入 Vicinae 官方 Discord 伺服器。</translation>
    </message>
</context>
<context>
    <name>OpenDocumentationCommand</name>
    <message>
        <location line="-16"/>
        <source>Open Online Documentation</source>
        <translation>開啟線上文件</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Navigate to the official vicinae documentation website.</source>
        <translation>前往 Vicinae 官方文件網站。</translation>
    </message>
</context>
<context>
    <name>OpenFileAction</name>
    <message>
        <location filename="../src/actions/files/file-actions.hpp" line="+18"/>
        <source>Open with %1</source>
        <translation>使用 %1 開啟</translation>
    </message>
</context>
<context>
    <name>OpenInBrowserAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="+105"/>
        <source>Open in browser</source>
        <translation>在瀏覽器中開啟</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+95"/>
        <source>Failed to open in browser</source>
        <translation>無法在瀏覽器中開啟</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Opened in browser</source>
        <translation>已在瀏覽器中開啟</translation>
    </message>
</context>
<context>
    <name>OpenInTerminalAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="-62"/>
        <source>Open in %1</source>
        <translation>在 %1 中開啟</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-85"/>
        <source>Failed to start app</source>
        <translation>啟動應用失敗</translation>
    </message>
</context>
<context>
    <name>OpenItemPreferencesAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="-28"/>
        <source>Open Preferences</source>
        <translation>開啟偏好設定</translation>
    </message>
</context>
<context>
    <name>OpenKeybindSettingsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+156"/>
        <source>Open Vicinae Keybind Settings</source>
        <translation>開啟 Vicinae 快捷鍵設定</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the vicinae keybind settings window</source>
        <translation>開啟 Vicinae 快捷鍵設定視窗</translation>
    </message>
</context>
<context>
    <name>OpenRawProgramAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="+25"/>
        <source>Execute program</source>
        <translation>執行程式</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+35"/>
        <source>Failed to start app</source>
        <translation>啟動應用失敗</translation>
    </message>
</context>
<context>
    <name>OpenSettingsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-40"/>
        <source>Open Vicinae Settings</source>
        <translation>開啟 Vicinae 設定</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open the vicinae settings window, which is an independent floating window.</source>
        <translation>開啟獨立浮動的 Vicinae 設定視窗。</translation>
    </message>
</context>
<context>
    <name>OpenSettingsPaneAction</name>
    <message>
        <location filename="../src/root-search/macos-settings/macos-settings-root-provider.mm" line="-135"/>
        <source>Failed to open System Settings</source>
        <translation>開啟系統設定失敗</translation>
    </message>
</context>
<context>
    <name>OpenShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-128"/>
        <source>No app with id %1</source>
        <translation>沒有 ID 為 %1 的應用</translation>
    </message>
    <message>
        <location line="+9"/>
        <location line="+7"/>
        <source>Open shortcut</source>
        <translation>開啟快捷方式</translation>
    </message>
</context>
<context>
    <name>OpenShortcutFromSearchText</name>
    <message>
        <location line="+43"/>
        <source>Open shortcut</source>
        <translation>開啟快捷方式</translation>
    </message>
</context>
<context>
    <name>OpenVicinaeConfig</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-81"/>
        <source>Open Config File</source>
        <translation>開啟配置檔案</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the main vicinae configuration file</source>
        <translation>開啟 Vicinae 主配置檔案</translation>
    </message>
</context>
<context>
    <name>OpenWindowsSettingAction</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="+147"/>
        <source>Failed to open settings</source>
        <translation>開啟設定失敗</translation>
    </message>
</context>
<context>
    <name>OpenWithAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+54"/>
        <source>Open with...</source>
        <translation>開啟方式…</translation>
    </message>
</context>
<context>
    <name>PasteToFocusedWindowAction</name>
    <message>
        <location filename="../src/clipboard-actions.hpp" line="+11"/>
        <source>Paste to active window</source>
        <translation>貼上到當前視窗</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy to focused window</source>
        <translation>複製到當前視窗</translation>
    </message>
</context>
<context>
    <name>PinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+28"/>
        <source>Entry pinned</source>
        <translation>已固定條目</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pin entry</source>
        <translation>固定條目</translation>
    </message>
</context>
<context>
    <name>PinClipboardAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="-26"/>
        <source>Selection pinned</source>
        <translation>已固定所選內容</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Selection unpinned</source>
        <translation>已取消固定所選內容</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to change pin status</source>
        <translation>更改固定狀態失敗</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Pin</source>
        <translation>固定</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unpin</source>
        <translation>取消固定</translation>
    </message>
</context>
<context>
    <name>PinEmojiAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-322"/>
        <source>Pin emoji</source>
        <translation>固定表情符號</translation>
    </message>
</context>
<context>
    <name>PinWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="+37"/>
        <source>Unpin from all workspaces</source>
        <translation>從所有工作區取消固定</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Pin to all workspaces</source>
        <translation>固定到所有工作區</translation>
    </message>
</context>
<context>
    <name>PowerManagementCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-283"/>
        <source>Ask for confirmation</source>
        <translation>要求確認</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Custom program</source>
        <translation>自定義程式</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Custom POSIX shell command to run instead of the default implementation</source>
        <translation>代替預設實現執行的自定義 POSIX shell 命令</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Failed to execute custom program %1</source>
        <translation>執行自定義程式 %1 失敗</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Are you sure</source>
        <translation>確定嗎</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>High-impact operation, please confirm</source>
        <translation>此操作影響較大，請確認</translation>
    </message>
</context>
<context>
    <name>PowerManagementExtension</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.hpp" line="+8"/>
        <source>Power Management</source>
        <translation>電源管理</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power off, suspend, sleep, hibernate your computer.</source>
        <translation>關閉、掛起、睡眠或休眠電腦。</translation>
    </message>
</context>
<context>
    <name>PowerOffCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+137"/>
        <source>Power Off System</source>
        <translation>關閉系統</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power off the system</source>
        <translation>關閉系統</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>System cannot power off</source>
        <translation>系統無法關機</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to power off</source>
        <translation>關機失敗</translation>
    </message>
</context>
<context>
    <name>PreviewFontAction</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="-105"/>
        <source>Preview font</source>
        <translation>預覽字型</translation>
    </message>
</context>
<context>
    <name>ProgramsSection</name>
    <message>
        <location filename="../src/qml/system-run-model.hpp" line="+21"/>
        <source>Programs (%1)</source>
        <translation>程式（%1）</translation>
    </message>
    <message>
        <location filename="../src/qml/system-run-model.cpp" line="+52"/>
        <source>Open in %1 (hold)</source>
        <translation>在 %1 中開啟（保持視窗）</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open in %1</source>
        <translation>在 %1 中開啟</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Copy exec path</source>
        <translation>複製執行檔路徑</translation>
    </message>
</context>
<context>
    <name>ProviderSearchSection</name>
    <message>
        <location filename="../src/qml/provider-search-model.hpp" line="+11"/>
        <source>Results ({count})</source>
        <translation>結果（{count}）</translation>
    </message>
</context>
<context>
    <name>ProviderSearchViewHost</name>
    <message>
        <location filename="../src/qml/provider-search-view-host.cpp" line="+15"/>
        <source>Search %1</source>
        <translation>搜尋 %1</translation>
    </message>
</context>
<context>
    <name>PruneMemoryCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+56"/>
        <source>Prune Vicinae Memory Usage</source>
        <translation>清理 Vicinae 記憶體佔用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Try pruning vicinae&apos;s memory usage by clearing pixmap cache and calling malloc_trim(). Mostly provided for internal testing.</source>
        <translation>嘗試清除畫素圖快取並呼叫 malloc_trim() 來降低 Vicinae 的記憶體佔用。主要用於內部測試。</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Pruned 🥊</source>
        <translation>已清理 🥊</translation>
    </message>
</context>
<context>
    <name>PutCalculatorAnswerInSearchBar</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-20"/>
        <source>Put answer in search bar</source>
        <translation>將答案放入搜尋欄</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/utils/utils.cpp" line="+50"/>
        <source>in the future</source>
        <translation>未來</translation>
    </message>
    <message numerus="yes">
        <location line="+8"/>
        <source>%n year(s) ago</source>
        <translation>
            <numerusform>%n 年前</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+3"/>
        <source>%n month(s) ago</source>
        <translation>
            <numerusform>%n 個月前</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n day(s) ago</source>
        <translation>
            <numerusform>%n 天前</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n hour(s) ago</source>
        <translation>
            <numerusform>%n 小時前</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n minute(s) ago</source>
        <translation>
            <numerusform>%n 分鐘前</numerusform>
        </translation>
    </message>
    <message>
        <location line="+2"/>
        <source>just now</source>
        <translation>剛剛</translation>
    </message>
</context>
<context>
    <name>QuitAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-43"/>
        <source>Quit Application</source>
        <translation>退出應用</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to quit %1</source>
        <translation>退出 %1 失敗</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Quit %1</source>
        <translation>退出 %1</translation>
    </message>
</context>
<context>
    <name>RaycastCompatExtension</name>
    <message>
        <location filename="../src/extensions/raycast/raycast-compat-extension.hpp" line="+15"/>
        <source>Raycast compatibility features</source>
        <translation>Raycast 相容功能</translation>
    </message>
</context>
<context>
    <name>RaycastStoreCommand</name>
    <message>
        <location filename="../src/extensions/raycast/raycast-store-command.hpp" line="+13"/>
        <source>Install compatible extensions from the Raycast store</source>
        <translation>從 Raycast Store 安裝相容擴充套件</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Always show intro</source>
        <translation>始終顯示介紹</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>
# Welcome to the Raycast Extension Store

Vicinae provides direct integration with the official [Raycast store](https://www.raycast.com/store), allowing you to search and install Raycast extensions directly from Vicinae.
</source>
        <translation>
# 歡迎使用 Raycast Extension Store

Vicinae 與官方 [Raycast Store](https://www.raycast.com/store) 直接整合，可直接在 Vicinae 中搜索並安裝 Raycast 擴充套件。
</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>
Each extension has a colored compatibility indicator showing how well it works on Linux.

Vicinae also has its own [extension store](vicinae://launch/core/store), which does not suffer from these limitations.
</source>
        <translation>
每個擴充套件都有彩色相容性標記，表示它在 Linux 上的執行情況。

Vicinae 也有自己的 [Extension Store](vicinae://launch/core/store)，不受這些限制。
</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>
Vicinae also has its own [extension store](vicinae://launch/core/store).
</source>
        <translation>
Vicinae 也有自己的 [Extension Store](vicinae://launch/core/store)。
</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Continue to store</source>
        <translation>繼續前往商店</translation>
    </message>
</context>
<context>
    <name>RaycastStoreDetailHost</name>
    <message>
        <location filename="../src/qml/raycast-store-detail-host.cpp" line="+43"/>
        <source>Failed to load extension</source>
        <translation>載入擴充套件失敗</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The extension &quot;%1&quot; could not be loaded. It may not exist or the store may be unreachable.</source>
        <translation>無法載入擴充套件“%1”。它可能不存在，或商店暫時無法訪問。</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Extension Store - %1</source>
        <translation>Extension Store - %1</translation>
    </message>
    <message>
        <location line="+31"/>
        <source>This extension should be fully compatible.</source>
        <translation>此擴充套件應該完全相容。</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>This extension works but has a few quirks.</source>
        <translation>此擴充套件可以執行，但有少量問題。</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>This extension is not compatible.</source>
        <translation>此擴充套件不相容。</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No compatibility data is available for this extension.</source>
        <translation>沒有此擴充套件的相容性資料。</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>No compatibility data is available — this extension may or may not work.</source>
        <translation>沒有相容性資料——此擴充套件可能可用，也可能不可用。</translation>
    </message>
    <message>
        <location line="+83"/>
        <source>Install extension</source>
        <translation>安裝擴充套件</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading extension...</source>
        <translation>正在下載擴充套件…</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to download extension</source>
        <translation>下載擴充套件失敗</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to extract extension archive</source>
        <translation>解壓擴充套件包失敗</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Extension installed</source>
        <translation>擴充套件已安裝</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Report issue</source>
        <translation>報告問題</translation>
    </message>
</context>
<context>
    <name>RaycastStoreSection</name>
    <message>
        <location filename="../src/qml/raycast-store-model.cpp" line="+45"/>
        <source>Show details</source>
        <translation>顯示詳情</translation>
    </message>
</context>
<context>
    <name>RaycastStoreViewHost</name>
    <message>
        <location filename="../src/qml/raycast-store-view-host.cpp" line="+37"/>
        <source>Browse Raycast extensions</source>
        <translation>瀏覽 Raycast 擴充套件</translation>
    </message>
    <message>
        <location line="+32"/>
        <source>Failed to fetch extensions</source>
        <translation>獲取擴充套件失敗</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Extensions</source>
        <translation>擴充套件</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to search extensions</source>
        <translation>搜尋擴充套件失敗</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Results</source>
        <translation>結果</translation>
    </message>
</context>
<context>
    <name>RebootCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-72"/>
        <source>Reboot System</source>
        <translation>重啟系統</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reboot the system</source>
        <translation>重啟系統</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>System can&apos;t reboot</source>
        <translation>系統無法重啟</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to reboot</source>
        <translation>重啟失敗</translation>
    </message>
</context>
<context>
    <name>RebuildFileIndexCommand</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="-72"/>
        <source>Rebuild File Index</source>
        <translation>重建檔案索引</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Fully rebuild the file index. Running this manually can be useful if the file search feels particularly out of date.</source>
        <translation>完全重建檔案索引。如果檔案搜尋結果明顯過時，可手動執行此操作。</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Are you sure?</source>
        <translation>確定嗎？</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Rebuilding the entire index can be time consuming and CPU intensive, depending on the number of files present in your home directory.</source>
        <translation>重建整個索引可能耗時較長並大量佔用 CPU，具體取決於主目錄中的檔案數量。</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Reset</source>
        <translation>重置</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Index rebuild started...</source>
        <translation>已開始重建索引…</translation>
    </message>
</context>
<context>
    <name>RefreshAppsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/refresh-apps-command.hpp" line="+12"/>
        <source>Refresh Apps</source>
        <translation>重新整理應用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Force a refresh of the application database. The database should normally automatically update itself on changes, but this can help working around some edge cases.</source>
        <translation>強制重新整理應用程式資料庫。資料庫通常會在發生變化時自動更新，但此操作可用於處理某些特殊情況。</translation>
    </message>
    <message>
        <location filename="../src/extensions/vicinae/refresh-apps-command.cpp" line="+15"/>
        <source>Apps successfully refreshed</source>
        <translation>應用重新整理成功</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to refresh apps</source>
        <translation>重新整理應用失敗</translation>
    </message>
</context>
<context>
    <name>ReloadScriptDirectoriesCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+29"/>
        <source>Reload Script Directories</source>
        <translation>重新載入指令碼目錄</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reload script directories</source>
        <translation>重新載入指令碼目錄</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>New scan triggered, index will update shortly</source>
        <translation>已觸發新一輪掃描，索引很快會更新</translation>
    </message>
</context>
<context>
    <name>RemoveAllCalculatorHistoryRecordsAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+97"/>
        <source>Delete all entries</source>
        <translation>刪除所有條目</translation>
    </message>
</context>
<context>
    <name>RemoveAllSelectionsAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+27"/>
        <source>Are you sure?</source>
        <translation>確定嗎？</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>All your clipboard history will be lost forever</source>
        <translation>所有剪貼簿歷史記錄將永久丟失</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Delete all</source>
        <translation>全部刪除</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>All selections were removed</source>
        <translation>已移除所有所選內容</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove all selections</source>
        <translation>移除所有所選內容失敗</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Remove all</source>
        <translation>全部移除</translation>
    </message>
</context>
<context>
    <name>RemoveCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-43"/>
        <source>Entry removed</source>
        <translation>已移除條目</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove entry</source>
        <translation>移除條目失敗</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Delete entry</source>
        <translation>刪除條目</translation>
    </message>
</context>
<context>
    <name>RemoveSelectionAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="-70"/>
        <source>Entry removed</source>
        <translation>已移除條目</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove entry</source>
        <translation>移除條目失敗</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Remove entry</source>
        <translation>移除條目</translation>
    </message>
</context>
<context>
    <name>RemoveShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="+33"/>
        <source>Removed link</source>
        <translation>已移除連結</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove link</source>
        <translation>移除連結失敗</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Remove link</source>
        <translation>移除連結</translation>
    </message>
</context>
<context>
    <name>ReportVicinaeBugCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/report-bug-command.hpp" line="+10"/>
        <source>Report a Vicinae Bug</source>
        <translation>報告 Vicinae 錯誤</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Navigate to Vicinae issue creation page with all relevant informations pre-filled.</source>
        <translation>前往 Vicinae 問題建立頁面，並預先填寫所有相關資訊。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Title</source>
        <translation>標題</translation>
    </message>
</context>
<context>
    <name>ResetEmojiRankingAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+21"/>
        <source>Reset ranking</source>
        <translation>重置排序</translation>
    </message>
</context>
<context>
    <name>ResetEmojiSkinToneAction</name>
    <message>
        <location line="+32"/>
        <source>Reset to preference</source>
        <translation>重置為偏好設定</translation>
    </message>
</context>
<context>
    <name>ResetItemRanking</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="-25"/>
        <source>Ranking was successfully reset</source>
        <translation>已成功重置排序</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Unable to reset ranking</source>
        <translation>無法重置排序</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Are you sure?</source>
        <translation>確定嗎？</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You will have to rebuild search history for this item in order for it to reappear on top of the root search results.</source>
        <translation>需要重新積累此專案的搜尋歷史，它才會再次出現在根搜尋結果頂部。</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Reset</source>
        <translation>重置</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Reset ranking</source>
        <translation>重置排序</translation>
    </message>
</context>
<context>
    <name>RevealFileInFolderAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+26"/>
        <source>Show in file browser</source>
        <translation>在檔案瀏覽器中顯示</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to open folder</source>
        <translation>開啟資料夾失敗</translation>
    </message>
</context>
<context>
    <name>RootCalculatorSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+79"/>
        <source>Calculator</source>
        <translation>計算器</translation>
    </message>
</context>
<context>
    <name>RootFallbackSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="+407"/>
        <source>Use &quot;%1&quot; with...</source>
        <translation>用“%1”開啟…</translation>
    </message>
</context>
<context>
    <name>RootFavoritesSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+67"/>
        <source>Favorites</source>
        <translation>收藏</translation>
    </message>
</context>
<context>
    <name>RootFilesSection</name>
    <message>
        <location line="+50"/>
        <source>Files</source>
        <translation>檔案</translation>
    </message>
</context>
<context>
    <name>RootLinkSection</name>
    <message>
        <location line="-137"/>
        <source>Link</source>
        <translation>連結</translation>
    </message>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="-295"/>
        <source>Open in %1</source>
        <translation>在 %1 中開啟</translation>
    </message>
</context>
<context>
    <name>RootNewsSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+64"/>
        <source>What&apos;s New</source>
        <translation>新功能</translation>
    </message>
</context>
<context>
    <name>RootResultsSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="+199"/>
        <source>Suggestions</source>
        <translation>建議</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Results (%1)</source>
        <translation>結果（%1）</translation>
    </message>
</context>
<context>
    <name>RootSearchActionGenerator</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+55"/>
        <source>Copy ID</source>
        <translation>複製 ID</translation>
    </message>
</context>
<context>
    <name>RootShortcutItem</name>
    <message>
        <location filename="../src/root-search/shortcuts/shortcut-root-provider.cpp" line="+67"/>
        <location line="+11"/>
        <source>Shortcut</source>
        <translation>快捷方式</translation>
    </message>
</context>
<context>
    <name>RootUpdateSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="-21"/>
        <location filename="../src/qml/root-search-sources.cpp" line="-125"/>
        <source>Update</source>
        <translation>更新</translation>
    </message>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="-14"/>
        <source>Vicinae %1 is available</source>
        <translation>Vicinae %1 已釋出</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>You are running %1</source>
        <translation>當前版本：%1</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>View Release Notes</source>
        <translation>檢視發行說明</translation>
    </message>
</context>
<context>
    <name>RootViewHost</name>
    <message>
        <location filename="../src/qml/root-view-host.hpp" line="+15"/>
        <source>Search for anything...</source>
        <translation>搜尋任何內容…</translation>
    </message>
</context>
<context>
    <name>ScriptExecutorViewHost</name>
    <message>
        <location filename="../src/qml/script-executor-view-host.cpp" line="+76"/>
        <source>Script execution failed: %1</source>
        <translation>指令碼執行失敗：%1</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Running... (%1s ago)</source>
        <translation>執行中…（%1 秒前）</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Done in %1s (exit=%2)</source>
        <translation>已完成，用時 %1 秒（退出碼=%2）</translation>
    </message>
    <message>
        <location line="+12"/>
        <location line="+9"/>
        <source>Script process killed</source>
        <translation>指令碼程序已終止</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Running...</source>
        <translation>執行中…</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Kill process</source>
        <translation>終止程序</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Run script again</source>
        <translation>再次執行指令碼</translation>
    </message>
</context>
<context>
    <name>ScriptRootItem</name>
    <message>
        <location filename="../src/root-search/scripts/script-root-provider.hpp" line="+27"/>
        <location line="+86"/>
        <source>Script</source>
        <translation>指令碼</translation>
    </message>
    <message>
        <location line="-43"/>
        <source>Mode</source>
        <translation>模式</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Path</source>
        <translation>路徑</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Author</source>
        <translation>作者</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Open script directory</source>
        <translation>開啟指令碼目錄</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy path to script</source>
        <translation>複製指令碼路徑</translation>
    </message>
</context>
<context>
    <name>ScriptRootProvider</name>
    <message>
        <location line="+47"/>
        <source>Script Commands</source>
        <translation>指令碼命令</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Custom directories</source>
        <translation>自定義目錄</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Additional list of directories to source scripts from. These directories always take precedence over the default system ones</source>
        <translation>用於載入指令碼的其他目錄。這些目錄始終優先於系統預設目錄</translation>
    </message>
</context>
<context>
    <name>SearchBrowserTabsCommand</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="+15"/>
        <source>Search Browser Tabs</source>
        <translation>搜尋瀏覽器標籤頁</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search tabs from all connected browsers</source>
        <translation>搜尋所有已連線瀏覽器中的標籤頁</translation>
    </message>
</context>
<context>
    <name>SearchEmojiCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/search-emoji-command.hpp" line="+15"/>
        <source>Search Emojis &amp; Symbols</source>
        <translation>搜尋表情與符號</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search for any emoji or symbol</source>
        <translation>搜尋任意表情或符號</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Paste</source>
        <translation>貼上</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy</source>
        <translation>複製</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Default Action</source>
        <translation>預設操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to perform on pressing return. Paste is only available if your environment supports it.</source>
        <translation>按回車鍵時執行的預設操作。僅噹噹前環境支援時才能貼上。</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Skin tone</source>
        <translation>膚色</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Skin tone to use for relevant emojis.</source>
        <translation>為適用的表情選擇膚色。</translation>
    </message>
</context>
<context>
    <name>SearchEmojiGridSource</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.hpp" line="-22"/>
        <source>Results (%1)</source>
        <translation>結果（%1）</translation>
    </message>
</context>
<context>
    <name>SearchFilesCommand</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="-38"/>
        <source>Search Files</source>
        <translation>搜尋檔案</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search files on your system</source>
        <translation>搜尋系統中的檔案</translation>
    </message>
</context>
<context>
    <name>SearchFilesView</name>
    <message>
        <location filename="../src/qml/qml/SearchFilesView.qml" line="+37"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Path</source>
        <translation>路徑</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>型別</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Last modified</source>
        <translation>上次修改</translation>
    </message>
</context>
<context>
    <name>SearchFilesViewHost</name>
    <message>
        <location filename="../src/qml/search-files-view-host.cpp" line="+59"/>
        <source>Search for files...</source>
        <translation>搜尋檔案…</translation>
    </message>
    <message>
        <location line="+30"/>
        <location line="+4"/>
        <source>Direct file path</source>
        <translation>直接輸入檔案路徑</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Recently Accessed</source>
        <translation>最近訪問</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Results</source>
        <translation>結果</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>All</source>
        <translation>全部</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Other</source>
        <translation>其他</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Directories</source>
        <translation>目錄</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Images</source>
        <translation>圖片</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Videos</source>
        <translation>影片</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Audio</source>
        <translation>音訊</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Documents</source>
        <translation>文件</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Archives</source>
        <translation>壓縮包</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Applications</source>
        <translation>應用程式</translation>
    </message>
</context>
<context>
    <name>SetAppFont</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="-15"/>
        <source>Set as vicinae font</source>
        <translation>設為 Vicinae 字型</translation>
    </message>
</context>
<context>
    <name>SetRootItemAliasAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="-18"/>
        <source>Set alias</source>
        <translation>設定別名</translation>
    </message>
</context>
<context>
    <name>SetThemeAction</name>
    <message>
        <location filename="../src/actions/theme/theme-actions.cpp" line="+11"/>
        <source>Theme successfully updated</source>
        <translation>主題已更新</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Set theme</source>
        <translation>設定主題</translation>
    </message>
</context>
<context>
    <name>SetThemeCommand</name>
    <message>
        <location filename="../src/extensions/theme/set-theme-command.hpp" line="+9"/>
        <source>Set Theme</source>
        <translation>設定主題</translation>
    </message>
</context>
<context>
    <name>SetVolumeCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="+213"/>
        <source>Set Volume to %1%</source>
        <translation>將音量設為 %1%</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Set system volume to %1%</source>
        <translation>將系統音量設為 %1%</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to set volume</source>
        <translation>設定音量失敗</translation>
    </message>
</context>
<context>
    <name>SetWallpaperAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+17"/>
        <source>Set as wallpaper</source>
        <translation>設為桌布</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Wallpaper set</source>
        <translation>桌布已設定</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to set wallpaper</source>
        <translation>設定桌布失敗</translation>
    </message>
</context>
<context>
    <name>SettingsSidebar</name>
    <message>
        <location filename="../src/qml/qml/SettingsSidebar.qml" line="+99"/>
        <source>Search...</source>
        <translation>搜尋…</translation>
    </message>
</context>
<context>
    <name>SettingsSidebarModel</name>
    <message>
        <location filename="../src/qml/settings-sidebar-model.cpp" line="+90"/>
        <source>General</source>
        <translation>通用</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Appearance</source>
        <translation>外觀</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Keybindings</source>
        <translation>快捷鍵</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced</source>
        <translation>高階</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>About</source>
        <translation>關於</translation>
    </message>
</context>
<context>
    <name>SettingsWindow</name>
    <message>
        <location filename="../src/qml/qml/SettingsWindow.qml" line="+10"/>
        <source>General</source>
        <translation>通用</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Appearance</source>
        <translation>外觀</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keybindings</source>
        <translation>快捷鍵</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Advanced</source>
        <translation>高階</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>About</source>
        <translation>關於</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Vicinae Settings</source>
        <translation>Vicinae 設定</translation>
    </message>
    <message>
        <location line="+88"/>
        <source>Imported from Raycast</source>
        <translation>從 Raycast 匯入</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>From the Vicinae store</source>
        <translation>來自 Vicinae Store</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Locally installed extension</source>
        <translation>本地安裝的擴充套件</translation>
    </message>
</context>
<context>
    <name>ShortcutExtension</name>
    <message>
        <location filename="../src/extensions/shortcut/shortcut-extension.hpp" line="+11"/>
        <source>Manage Shortcuts</source>
        <translation>管理快捷方式</translation>
    </message>
</context>
<context>
    <name>ShortcutField</name>
    <message>
        <location filename="../src/qml/qml/ShortcutField.qml" line="+14"/>
        <source>Record shortcut</source>
        <translation>錄製快捷鍵</translation>
    </message>
</context>
<context>
    <name>ShortcutFormView</name>
    <message>
        <location filename="../src/qml/qml/ShortcutFormView.qml" line="+14"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Shortcut Name</source>
        <translation>快捷方式名稱</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>The URL that will be opened by the specified app. You can make it dynamic by using placeholders such as {argument}.</source>
        <translation>由指定應用開啟的 URL。可使用 {argument} 等佔位符使其動態變化。</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Open with</source>
        <translation>開啟方式</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Icon</source>
        <translation>圖示</translation>
    </message>
</context>
<context>
    <name>ShortcutFormViewHost</name>
    <message>
        <location filename="../src/qml/shortcut-form-view-host.cpp" line="+47"/>
        <source>Submit</source>
        <translation>提交</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Copy of %1</source>
        <translation>%1 的副本</translation>
    </message>
    <message>
        <location line="+67"/>
        <source>Edit &quot;%1&quot;</source>
        <translation>編輯“%1”</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Duplicate &quot;%1&quot;</source>
        <translation>建立“%1”副本</translation>
    </message>
    <message>
        <location line="+11"/>
        <location line="+131"/>
        <location line="+50"/>
        <source>Default</source>
        <translation>預設</translation>
    </message>
    <message>
        <location line="-157"/>
        <source>Selected Text</source>
        <translation>所選文本</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clipboard Text</source>
        <translation>剪貼簿文本</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Argument</source>
        <translation>引數</translation>
    </message>
    <message>
        <location line="+35"/>
        <location line="+5"/>
        <location line="+5"/>
        <source>Required</source>
        <translation>必填</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Validation failed</source>
        <translation>驗證失敗</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Failed to update shortcut</source>
        <translation>更新快捷方式失敗</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Shortcut updated</source>
        <translation>快捷方式已更新</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to create shortcut</source>
        <translation>建立快捷方式失敗</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Shortcut created</source>
        <translation>快捷方式已建立</translation>
    </message>
</context>
<context>
    <name>ShortcutRecorderField</name>
    <message>
        <location filename="../src/qml/qml/ShortcutRecorderField.qml" line="+24"/>
        <location line="+22"/>
        <location line="+62"/>
        <source>Recording...</source>
        <translation>錄製中…</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Keybind updated</source>
        <translation>快捷鍵已更新</translation>
    </message>
</context>
<context>
    <name>ShortcutRootProvider</name>
    <message>
        <location filename="../src/root-search/shortcuts/shortcut-root-provider.cpp" line="+42"/>
        <source>Shortcuts</source>
        <translation>快捷方式</translation>
    </message>
</context>
<context>
    <name>ShortcutsSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/ShortcutsSettingsPage.qml" line="+55"/>
        <source>Keybindings</source>
        <translation>快捷鍵</translation>
    </message>
    <message>
        <location line="+78"/>
        <source>Record Shortcut</source>
        <translation>錄製快捷鍵</translation>
    </message>
</context>
<context>
    <name>SkipUpdateVersionAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+8"/>
        <source>Skip This Version</source>
        <translation>跳過此版本</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Skipped %1</source>
        <translation>已跳過 %1</translation>
    </message>
</context>
<context>
    <name>SleepCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+94"/>
        <source>Put System to Sleep</source>
        <translation>使系統進入睡眠</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Put system to sleep</source>
        <translation>使系統進入睡眠</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>System can&apos;t sleep</source>
        <translation>系統無法進入睡眠</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to sleep</source>
        <translation>進入睡眠失敗</translation>
    </message>
</context>
<context>
    <name>SnippetDatabase</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.cpp" line="+42"/>
        <location line="+56"/>
        <source>keyword already assigned to &quot;%1&quot;</source>
        <translation>關鍵詞已分配給“%1”</translation>
    </message>
    <message>
        <location line="-43"/>
        <source>No snippet with that ID</source>
        <translation>沒有具有該 ID 的片段</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>No such snippet</source>
        <translation>找不到該片段</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Snippet limit reached (%1)</source>
        <translation>已達到片段數量上限（%1）</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Failed to save snippets on disk: %1</source>
        <translation>無法將片段儲存到磁碟：%1</translation>
    </message>
</context>
<context>
    <name>SnippetExtension</name>
    <message>
        <location filename="../src/extensions/snippet/snippet-extension.cpp" line="+38"/>
        <source>Expansion</source>
        <translation>展開</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Enable automatic snippet expansion when triggers are typed</source>
        <translation>輸入觸發詞時自動展開片段</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Undo</source>
        <translation>撤銷</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Press backspace immediately after expansion to undo and restore the trigger text</source>
        <translation>展開後立即按退格鍵可撤銷並恢復觸發文本</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Keyboard layout</source>
        <translation>鍵盤佈局</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>XKB layout used for trigger detection (e.g. &quot;us&quot;, &quot;fr&quot;). Leave empty for system default.</source>
        <translation>用於檢測觸發詞的 XKB 佈局（如“us”“fr”）。留空則使用系統預設值。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Pre-paste delay (ms)</source>
        <translation>貼上前延遲（ms）</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Delay between setting clipboard and injecting paste shortcut. Increase if expansions paste empty on slow compositors.</source>
        <translation>設定剪貼簿與注入貼上快捷鍵之間的延遲。如果在較慢的合成器上展開後貼上為空，請增大此值。</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Key injection delay (ms)</source>
        <translation>按鍵注入延遲（ms）</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Delay between injected key events. Increase if expansions produce missing or garbled characters on slow compositors.</source>
        <translation>注入按鍵事件之間的延遲。如果在較慢的合成器上展開時缺字或亂碼，請增大此值。</translation>
    </message>
    <message>
        <location filename="../src/extensions/snippet/snippet-extension.hpp" line="+11"/>
        <source>Snippets</source>
        <translation>片段</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Text expansion and snippet management</source>
        <translation>文本展開與片段管理</translation>
    </message>
</context>
<context>
    <name>SnippetFormView</name>
    <message>
        <location filename="../src/qml/qml/SnippetFormView.qml" line="+15"/>
        <source>Title</source>
        <translation>標題</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Euro symbol</source>
        <translation>歐元符號</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Content</source>
        <translation>內容</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You can use {dynamic placeholders} to make the content dynamic: &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;learn more&lt;/a&gt;.</source>
        <translation>可使用 {dynamic placeholders} 建立動態內容：&lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;瞭解詳情&lt;/a&gt;。</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Keyword</source>
        <translation>關鍵詞</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Typing this keyword anywhere will result in it being replaced by the content of the snippet.</source>
        <translation>在任意位置輸入此關鍵詞都會將其替換為片段內容。</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The snippet server is not running. Keyword expansion is unavailable. &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;Learn more&lt;/a&gt;.</source>
        <translation>片段伺服器未執行，關鍵詞展開不可用。&lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;瞭解詳情&lt;/a&gt;。</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Applications</source>
        <translation>應用程式</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Restrict expansion to specific applications. By default, it works everywhere.</source>
        <translation>僅在指定應用中展開。預設在所有應用中生效。</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Expand as word</source>
        <translation>按單詞展開</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>If a keyword is typed, it will only be expanded after space or punctuation.</source>
        <translation>輸入關鍵詞後，只有再輸入空格或標點才會展開。</translation>
    </message>
</context>
<context>
    <name>SnippetFormViewHost</name>
    <message>
        <location filename="../src/qml/snippet-form-view-host.cpp" line="+49"/>
        <source>Submit</source>
        <translation>提交</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Copy of %1</source>
        <translation>%1 的副本</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Edit &quot;%1&quot;</source>
        <translation>編輯“%1”</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Duplicate &quot;%1&quot;</source>
        <translation>建立“%1”副本</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>2 chars min.</source>
        <translation>至少 2 個字元。</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Content should not be empty</source>
        <translation>內容不能為空</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Only one {cursor} placeholder is allowed</source>
        <translation>只能使用一個 {cursor} 佔位符</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Validation failed</source>
        <translation>驗證失敗</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Snippet updated</source>
        <translation>片段已更新</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Snippet successfully created</source>
        <translation>片段建立成功</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Cursor Position</source>
        <translation>游標位置</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clipboard Text</source>
        <translation>剪貼簿文本</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Date</source>
        <translation>日期</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Argument</source>
        <translation>引數</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Shell Command</source>
        <translation>Shell 命令</translation>
    </message>
</context>
<context>
    <name>SoftRebootCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-99"/>
        <source>Soft Reboot System</source>
        <translation>軟重啟系統</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Soft reboot the system, which usually means only userspace is rebooted.</source>
        <translation>軟重啟系統，通常僅重啟使用者空間。</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System can&apos;t soft reboot</source>
        <translation>系統無法軟重啟</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to soft reboot</source>
        <translation>軟重啟失敗</translation>
    </message>
</context>
<context>
    <name>SponsorVicinaeCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-121"/>
        <source>Donate to Vicinae</source>
        <translation>贊助 Vicinae</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open link to Vicinae&apos;s GitHub sponsor page</source>
        <translation>開啟 Vicinae 的 GitHub 贊助頁面</translation>
    </message>
</context>
<context>
    <name>StoreDetailView</name>
    <message>
        <location filename="../src/qml/qml/StoreDetailView.qml" line="+196"/>
        <source>Installed</source>
        <translation>已安裝</translation>
    </message>
    <message>
        <location line="+166"/>
        <source>Description</source>
        <translation>描述</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Commands</source>
        <translation>命令</translation>
    </message>
    <message>
        <location line="+74"/>
        <source>Open README</source>
        <translation>開啟 README</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Last update</source>
        <translation>上次更新</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Contributors</source>
        <translation>貢獻者</translation>
    </message>
    <message>
        <location line="+29"/>
        <source>Categories</source>
        <translation>分類</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Source Code</source>
        <translation>原始碼</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>View Code</source>
        <translation>檢視程式碼</translation>
    </message>
</context>
<context>
    <name>SuspendCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+36"/>
        <source>Suspend System</source>
        <translation>掛起系統</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Suspend the system to RAM. Unlike hibernation, this does not turn the computer off and will break on power loss.</source>
        <translation>將系統掛起到 RAM。與休眠不同，這不會關閉計算機，斷電後狀態會丟失。</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System cannot suspend</source>
        <translation>系統無法掛起</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to suspend</source>
        <translation>掛起失敗</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsCommand</name>
    <message>
        <location filename="../src/extensions/wm/switch-windows-command.hpp" line="+10"/>
        <source>Switch Windows</source>
        <translation>切換視窗</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsSection</name>
    <message>
        <location filename="../src/qml/switch-windows-model.hpp" line="+28"/>
        <source>Open Windows</source>
        <translation>開啟的視窗</translation>
    </message>
    <message>
        <location filename="../src/qml/switch-windows-model.cpp" line="+19"/>
        <source>WS %1</source>
        <translation>工作區 %1</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Window Actions</source>
        <translation>視窗操作</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsViewHost</name>
    <message>
        <location filename="../src/qml/switch-windows-view-host.cpp" line="+12"/>
        <source>Search open window...</source>
        <translation>搜尋已開啟的視窗…</translation>
    </message>
</context>
<context>
    <name>SystemBrowseApps</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-123"/>
        <source>Browse Apps</source>
        <translation>瀏覽應用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse all applications that are installed on the system</source>
        <translation>瀏覽系統中已安裝的所有應用</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Show hidden apps</source>
        <translation>顯示隱藏應用</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sort alphabetically</source>
        <translation>按字母排序</translation>
    </message>
</context>
<context>
    <name>SystemExtension</name>
    <message>
        <location line="+149"/>
        <source>System</source>
        <translation>系統</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>System-related commands</source>
        <translation>系統相關命令</translation>
    </message>
</context>
<context>
    <name>SystemRunCommand</name>
    <message>
        <location line="-239"/>
        <source>Run Terminal Program</source>
        <translation>在終端中執行程式</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run a program in a terminal window</source>
        <translation>在終端視窗中執行程式</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>command</source>
        <translation>命令</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Run in terminal</source>
        <translation>在終端中執行</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run in terminal (hold)</source>
        <translation>在終端中執行（保持視窗）</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run directly</source>
        <translation>直接執行</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Default Action</source>
        <translation>預設操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to run on pressing return</source>
        <translation>按回車時執行的預設操作</translation>
    </message>
    <message>
        <location line="+29"/>
        <source>Not a valid executable</source>
        <translation>不是有效的執行檔</translation>
    </message>
</context>
<context>
    <name>SystemRunViewHost</name>
    <message>
        <location filename="../src/qml/system-run-view-host.cpp" line="+20"/>
        <source>Search for a program to execute...</source>
        <translation>搜尋要執行的程式…</translation>
    </message>
</context>
<context>
    <name>ThemeExtension</name>
    <message>
        <location filename="../src/extensions/theme/theme-extension.hpp" line="+9"/>
        <source>Theme</source>
        <translation>主題</translation>
    </message>
</context>
<context>
    <name>ThemeSection</name>
    <message>
        <location filename="../src/qml/theme-list-model.cpp" line="+22"/>
        <source>Default theme description</source>
        <translation>預設主題說明</translation>
    </message>
    <message>
        <location line="+64"/>
        <source>Open theme file</source>
        <translation>開啟主題檔案</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy ID</source>
        <translation>複製 ID</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy path</source>
        <translation>複製路徑</translation>
    </message>
</context>
<context>
    <name>ThemeViewHost</name>
    <message>
        <location filename="../src/qml/theme-view-host.cpp" line="+22"/>
        <source>Search for a theme...</source>
        <translation>搜尋主題…</translation>
    </message>
    <message>
        <location line="+51"/>
        <source>Current Theme</source>
        <translation>當前主題</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Available Themes</source>
        <translation>可用主題</translation>
    </message>
</context>
<context>
    <name>ToggleItemAsFavorite</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="+17"/>
        <source>Remove from favorites</source>
        <translation>取消收藏</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Add to favorites</source>
        <translation>新增到收藏</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Successfuly added to favorites</source>
        <translation>已新增到收藏</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Successfuly removed from favorites</source>
        <translation>已取消收藏</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to add to favorites</source>
        <translation>新增到收藏失敗</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove from favorites</source>
        <translation>取消收藏失敗</translation>
    </message>
</context>
<context>
    <name>ToggleMuteCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="+163"/>
        <source>Toggle Mute</source>
        <translation>切換靜音</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mute or unmute system audio</source>
        <translation>將系統音訊靜音或取消靜音</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Failed to toggle mute</source>
        <translation>切換靜音失敗</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Muted</source>
        <translation>已靜音</translation>
    </message>
</context>
<context>
    <name>UIPlayground</name>
    <message>
        <location filename="../src/extensions/internal/internal-extension.hpp" line="-15"/>
        <source>UI Showcase</source>
        <translation>UI 展示</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Showcase of vicinae UI elements</source>
        <translation>展示 Vicinae UI 元素</translation>
    </message>
</context>
<context>
    <name>UIShowcase</name>
    <message>
        <location filename="../src/qml/qml/UIShowcase.qml" line="+26"/>
        <source>Buttons</source>
        <translation>按鈕</translation>
    </message>
</context>
<context>
    <name>UninstallExtensionAction</name>
    <message>
        <location filename="../src/actions/extension/extension-actions.cpp" line="+11"/>
        <source>Are you sure?</source>
        <translation>確定嗎？</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>All this extension data will be permanently lost. If you just want the extension to not appear in the root search anymore, consider disabling it instead.</source>
        <translation>此擴充套件的所有資料都將永久丟失。如果只是不想讓它出現在根搜尋中，可考慮將其停用。</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Uninstall</source>
        <translation>解除安裝</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Extension uninstalled</source>
        <translation>擴充套件已解除安裝</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to uninstall extension</source>
        <translation>解除安裝擴充套件失敗</translation>
    </message>
    <message>
        <location filename="../src/actions/extension/extension-actions.hpp" line="+14"/>
        <source>Uninstall Extension</source>
        <translation>解除安裝擴充套件</translation>
    </message>
</context>
<context>
    <name>UnpinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-23"/>
        <source>Entry unpinned</source>
        <translation>已取消固定條目</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Unpin entry</source>
        <translation>取消固定條目</translation>
    </message>
</context>
<context>
    <name>UnpinEmojiAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-43"/>
        <source>Unpin emoji</source>
        <translation>取消固定表情符號</translation>
    </message>
</context>
<context>
    <name>UpdateService</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="-167"/>
        <source>Update installed</source>
        <translation>更新已安裝</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Restarting…</source>
        <translation>正在重啟…</translation>
    </message>
    <message>
        <location line="+88"/>
        <source>Downloading Vicinae %1…</source>
        <translation>正在下載 Vicinae %1…</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading Vicinae %1… %2%</source>
        <translation>正在下載 Vicinae %1… %2%</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Installing update…</source>
        <translation>正在安裝更新…</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Update failed</source>
        <translation>更新失敗</translation>
    </message>
</context>
<context>
    <name>VicinaeExtension</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.hpp" line="+11"/>
        <source>General vicinae-related commands.</source>
        <translation>Vicinae 通用命令。</translation>
    </message>
</context>
<context>
    <name>VicinaeHotkeyGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/vicinae-hotkey-global-shortcut-backend.cpp" line="+48"/>
        <source>Unsupported trigger key</source>
        <translation>不支援的觸發鍵</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Hotkey binding was lost</source>
        <translation>熱鍵繫結已丟失</translation>
    </message>
</context>
<context>
    <name>VicinaeListInstalledExtensionsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/list-installed-extensions-command.hpp" line="+11"/>
        <source>Show Installed Extensions</source>
        <translation>顯示已安裝的擴充套件</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Show all third-party extensions that have been installed. This includes local extensions as well as extensions downloaded from the stores (vicinae and raycast).</source>
        <translation>顯示所有已安裝的第三方擴充套件，包括本地擴充套件以及從 Vicinae 和 Raycast Store 下載的擴充套件。</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-store-command.hpp" line="+13"/>
        <source>Install extensions from the Vicinae store</source>
        <translation>從 Vicinae Store 安裝擴充套件</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Always show intro</source>
        <translation>始終顯示介紹</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>
# Welcome to the vicinae extension store

The vicinae extension store features community-built extensions that have been approved by our core contributors.

Every extension listed here has its source code available in the [vicinaehq/extensions](https://github.com/vicinaehq/extensions) repository.

If you&apos;re looking to build your own extension, take a look at the [documentation](https://docs.vicinae.com/extensions/introduction). If you think your extension would be a good fit for the store, feel free to submit it!
</source>
        <translation>
# 歡迎來到 Vicinae Store

Vicinae Store 收錄了由社群開發並經核心貢獻者批准的擴充套件。

這裡列出的每個擴充套件都可在 [vicinaehq/extensions](https://github.com/vicinaehq/extensions) 倉庫中檢視原始碼。

如果你想開發自己的擴充套件，請檢視[文件](https://docs.vicinae.com/extensions/introduction)。如果你認為自己的擴充套件適合上架，歡迎提交！
</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Continue to store</source>
        <translation>繼續前往商店</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreDetailHost</name>
    <message>
        <location filename="../src/qml/vicinae-store-detail-host.cpp" line="+38"/>
        <source>Failed to load extension</source>
        <translation>載入擴充套件失敗</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Could not fetch extension data from the store.</source>
        <translation>無法從商店獲取擴充套件資料。</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Extension not found</source>
        <translation>找不到擴充套件</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The extension &quot;%1&quot; could not be found in the store.</source>
        <translation>在商店中找不到擴充套件“%1”。</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Extension Store - %1</source>
        <translation>Extension Store - %1</translation>
    </message>
    <message>
        <location line="+81"/>
        <source>Install extension</source>
        <translation>安裝擴充套件</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading extension...</source>
        <translation>正在下載擴充套件…</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to download extension</source>
        <translation>下載擴充套件失敗</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to extract extension archive</source>
        <translation>解壓擴充套件包失敗</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Extension installed</source>
        <translation>擴充套件已安裝</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Report issue</source>
        <translation>報告問題</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreSection</name>
    <message>
        <location filename="../src/qml/vicinae-store-model.cpp" line="+41"/>
        <source>Show details</source>
        <translation>顯示詳情</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreViewHost</name>
    <message>
        <location filename="../src/qml/vicinae-store-view-host.cpp" line="+27"/>
        <source>Browse Vicinae extensions</source>
        <translation>瀏覽 Vicinae 擴充套件</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Failed to fetch extensions</source>
        <translation>獲取擴充套件失敗</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Extensions</source>
        <translation>擴充套件</translation>
    </message>
</context>
<context>
    <name>VolumeDownCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-74"/>
        <source>Turn Volume Down</source>
        <translation>調低音量</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Decrease system volume</source>
        <translation>降低系統音量</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid step value</source>
        <translation>無效的步長值</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to adjust volume</source>
        <translation>調節音量失敗</translation>
    </message>
</context>
<context>
    <name>VolumeUpCommand</name>
    <message>
        <location line="-61"/>
        <source>Turn Volume Up</source>
        <translation>調高音量</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Increase system volume</source>
        <translation>提高系統音量</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid step value</source>
        <translation>無效的步長值</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to adjust volume</source>
        <translation>調節音量失敗</translation>
    </message>
</context>
<context>
    <name>WallpaperManager</name>
    <message>
        <location filename="../src/services/wallpaper/wallpaper-manager.cpp" line="+68"/>
        <source>Setting the wallpaper is not supported in the current environment</source>
        <translation>當前環境不支援設定桌布</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No such file: %1</source>
        <translation>檔案不存在：%1</translation>
    </message>
</context>
<context>
    <name>WinControlPanelRootItem</name>
    <message>
        <location filename="../src/root-search/control-panel/control-panel-root-provider.cpp" line="+104"/>
        <location line="+10"/>
        <source>Control Panel</source>
        <translation>控制面板</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Where</source>
        <translation>位置</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Open Applet</source>
        <translation>開啟控制面板項</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Path</source>
        <translation>複製路徑</translation>
    </message>
</context>
<context>
    <name>WinControlPanelRootProvider</name>
    <message>
        <location line="+49"/>
        <source>Control Panel</source>
        <translation>控制面板</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Control Panel applets and system tasks.</source>
        <translation>控制面板小程式和系統任務。</translation>
    </message>
</context>
<context>
    <name>WinControlPanelTaskRootItem</name>
    <message>
        <location line="-40"/>
        <location line="+11"/>
        <source>Control Panel</source>
        <translation>控制面板</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Task ID</source>
        <translation>任務 ID</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Open</source>
        <translation>開啟</translation>
    </message>
</context>
<context>
    <name>WinSettingsPage</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="-114"/>
        <source>Display</source>
        <translation>螢幕</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>System</source>
        <translation>系統</translation>
    </message>
    <message>
        <location line="-17"/>
        <source>Night Light</source>
        <translation>夜間模式</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sound</source>
        <translation>聲音</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Volume Mixer</source>
        <translation>音量混合器</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Notifications</source>
        <translation>通知</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Focus</source>
        <translation>專注</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power &amp; Battery</source>
        <translation>電源和電池</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Storage</source>
        <translation>儲存</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Nearby Sharing</source>
        <translation>就近共享</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Multitasking</source>
        <translation>多工處理</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activation</source>
        <translation>啟用</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Troubleshoot</source>
        <translation>疑難解答</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Recovery</source>
        <translation>恢復</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Projecting to This PC</source>
        <translation>投影到此電腦</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Remote Desktop</source>
        <translation>遠端桌面</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clipboard</source>
        <translation>剪貼簿</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>About</source>
        <translation>關於</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Optional Features</source>
        <translation>可選功能</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>For Developers</source>
        <translation>開發者選項</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Bluetooth &amp; Devices</source>
        <translation>藍牙和裝置</translation>
    </message>
    <message>
        <location line="-8"/>
        <source>Devices</source>
        <translation>裝置</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Printers &amp; Scanners</source>
        <translation>印表機和掃描器</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mobile Devices</source>
        <translation>移動裝置</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Cameras</source>
        <translation>相機</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mouse</source>
        <translation>滑鼠</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Touchpad</source>
        <translation>觸控板</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pen &amp; Windows Ink</source>
        <translation>筆和 Windows Ink</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>AutoPlay</source>
        <translation>自動播放</translation>
    </message>
    <message>
        <location line="+3"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Network &amp; Internet</source>
        <translation>網路和 Internet</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Wi-Fi</source>
        <translation>Wi-Fi</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ethernet</source>
        <translation>乙太網路</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Mobile Hotspot</source>
        <translation>移動熱點</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Airplane Mode</source>
        <translation>飛航模式</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Proxy</source>
        <translation>代理</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dial-up</source>
        <translation>撥號</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced Network Settings</source>
        <translation>高階網路設定</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Background</source>
        <translation>背景</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Personalization</source>
        <translation>個性化</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Colors</source>
        <translation>顏色</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Themes</source>
        <translation>主題</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lock Screen</source>
        <translation>鎖屏介面</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Touch Keyboard</source>
        <translation>觸控鍵盤</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Start</source>
        <translation>開始</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Taskbar</source>
        <translation>工作列</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Fonts</source>
        <translation>字型</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dynamic Lighting</source>
        <translation>動態光效</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Installed Apps</source>
        <translation>安裝的應用</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Apps</source>
        <translation>應用程式</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>Default Apps</source>
        <translation>預設應用</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Offline Maps</source>
        <translation>離線地圖</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Apps for Websites</source>
        <translation>網站應用</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Video Playback</source>
        <translation>影片播放</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Startup Apps</source>
        <translation>啟動應用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Your Info</source>
        <translation>你的資訊</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Accounts</source>
        <translation>賬戶</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>Email &amp; Accounts</source>
        <translation>電子郵件和賬戶</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sign-in Options</source>
        <translation>登入選項</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Access Work or School</source>
        <translation>訪問工作或學校</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Family &amp; Other Users</source>
        <translation>家庭和其他使用者</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Windows Backup</source>
        <translation>Windows 備份</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Date &amp; Time</source>
        <translation>日期和時間</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Time &amp; Language</source>
        <translation>時間和語言</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Language &amp; Region</source>
        <translation>語言和區域</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Typing</source>
        <translation>輸入</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Speech</source>
        <translation>語音</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Game Bar</source>
        <translation>Game Bar</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Gaming</source>
        <translation>遊戲</translation>
    </message>
    <message>
        <location line="-1"/>
        <source>Captures</source>
        <translation>捕獲</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Game Mode</source>
        <translation>遊戲模式</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Text Size</source>
        <translation>文本大小</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Accessibility</source>
        <translation>輔助功能</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Visual Effects</source>
        <translation>視覺效果</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Magnifier</source>
        <translation>放大鏡</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Color Filters</source>
        <translation>顏色篩選器</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Contrast Themes</source>
        <translation>對比度主題</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Narrator</source>
        <translation>講述人</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Audio</source>
        <translation>音訊</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Captions</source>
        <translation>字幕</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Keyboard</source>
        <translation>鍵盤</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Mouse</source>
        <translation>滑鼠</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Eye Control</source>
        <translation>眼球控制</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Windows Security</source>
        <translation>Windows 安全中心</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Privacy &amp; Security</source>
        <translation>隱私和安全性</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Find My Device</source>
        <translation>查詢我的裝置</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Privacy</source>
        <translation>隱私</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Location</source>
        <translation>位置</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Camera Access</source>
        <translation>相機訪問許可權</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Microphone Access</source>
        <translation>麥克風訪問許可權</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activity History</source>
        <translation>活動歷史記錄</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Diagnostics &amp; Feedback</source>
        <translation>診斷和反饋</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search Permissions</source>
        <translation>搜尋許可權</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Windows Update</source>
        <translation>Windows 更新</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Update History</source>
        <translation>更新歷史記錄</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced Update Options</source>
        <translation>進階選項</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Windows Insider Program</source>
        <translation>Windows 預覽體驗計劃</translation>
    </message>
</context>
<context>
    <name>WinSettingsPageRootItem</name>
    <message>
        <location line="+37"/>
        <source>System Settings</source>
        <translation>系統設定</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Settings</source>
        <translation>設定</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Category</source>
        <translation>類別</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Open %1 Settings</source>
        <translation>開啟“%1”設定</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy URL</source>
        <translation>複製 URL</translation>
    </message>
</context>
<context>
    <name>WinSettingsRootProvider</name>
    <message>
        <location line="+12"/>
        <source>Windows Settings</source>
        <translation>Windows 設定</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Pages of the Windows Settings app.</source>
        <translation>Windows 設定應用中的頁面。</translation>
    </message>
</context>
<context>
    <name>WindowManagementExtension</name>
    <message>
        <location filename="../src/extensions/wm/wm-extension.hpp" line="+12"/>
        <source>Window Management</source>
        <translation>視窗管理</translation>
    </message>
</context>
<context>
    <name>WindowsAppDatabase</name>
    <message>
        <location filename="../src/services/app-service/windows/win-app-database.cpp" line="+993"/>
        <source>Focus window</source>
        <translation>聚焦視窗</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Launch app</source>
        <translation>啟動應用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Default action</source>
        <translation>預設操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Action to perform when the return key is pressed. Always default to &apos;launch&apos; if the app has no open window.</source>
        <translation>按回車時執行的操作。如果應用沒有開啟的視窗，則始終預設為“啟動”。</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>File Explorer</source>
        <translation>檔案資源管理器</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Terminal</source>
        <translation>終端</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Command Prompt</source>
        <translation>命令提示符</translation>
    </message>
</context>
<context>
    <name>WindowsApplication</name>
    <message>
        <location filename="../src/services/app-service/windows/win-app.hpp" line="+70"/>
        <source>%1: Run as Administrator</source>
        <translation>%1：以管理員身份執行</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run as Administrator</source>
        <translation>以管理員身份執行</translation>
    </message>
</context>
<context>
    <name>WindowsGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/windows-global-shortcut-backend.cpp" line="+215"/>
        <source>unsupported or invalid trigger</source>
        <translation>不支援或無效的觸發鍵</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>already registered by another application</source>
        <translation>已被其他應用註冊</translation>
    </message>
</context>
<context>
    <name>X11GlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/x11-global-shortcut-backend.cpp" line="+122"/>
        <source>This shortcut is already in use by another application</source>
        <translation>此快捷鍵已被其他應用佔用</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Unsupported trigger key</source>
        <translation>不支援的觸發鍵</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Trigger key is not present on this keyboard</source>
        <translation>此鍵盤上沒有該觸發鍵</translation>
    </message>
</context>
<context>
    <name>X11Workspace</name>
    <message>
        <location filename="../src/services/window-manager/x11/x11-window-manager.cpp" line="+426"/>
        <source>Desktop %1</source>
        <translation>桌面 %1</translation>
    </message>
</context>
<context>
    <name>XdgAppDatabase</name>
    <message>
        <location filename="../src/services/app-service/xdg/xdg-app-database.cpp" line="+564"/>
        <source>Focus window</source>
        <translation>聚焦視窗</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Launch app</source>
        <translation>啟動應用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Default action</source>
        <translation>預設操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Action to perform when the return key is pressed. Always default to &apos;launch&apos; if the app has no open window.</source>
        <translation>按回車時執行的操作。如果應用沒有開啟的視窗，則始終預設為“啟動”。</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Launch Prefix</source>
        <translation>啟動字首</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Custom app launcher to use. Affects applications as well as their sub-actions.</source>
        <translation>要使用的自定義應用啟動器，會影響應用及其子操作。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Application directories</source>
        <translation>應用目錄</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Directories applications are sourced from. The list cannot be modified directly. In order to do so, you need to append additonal paths to the &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt; environment variables.</source>
        <translation>用於查詢應用的目錄。此列表無法直接修改；如需新增目錄，請將其他路徑追加到 &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt; 環境變數。</translation>
    </message>
</context>
<context>
    <name>XdpFileChooser</name>
    <message>
        <location filename="../src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.cpp" line="+39"/>
        <source>Open Directory</source>
        <translation>開啟目錄</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Open File</source>
        <translation>開啟檔案</translation>
    </message>
</context>
<context>
    <name>browser-extension</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="-58"/>
        <source>No browser connected</source>
        <translation>未連線瀏覽器</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You need to connect at least one browser to vicinae using the browser extension in order to use this command.</source>
        <translation>需要通過瀏覽器擴充套件將至少一個瀏覽器連線到 Vicinae，才能使用此命令。</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Open documentation</source>
        <translation>開啟文件</translation>
    </message>
</context>
<context>
    <name>clipboard-history-view-host</name>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.cpp" line="-207"/>
        <source>Text</source>
        <translation>文本</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Link</source>
        <translation>連結</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Image</source>
        <translation>圖片</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>File</source>
        <translation>檔案</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Unknown</source>
        <translation>未知</translation>
    </message>
</context>
<context>
    <name>emoji-categories</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-52"/>
        <source>Smileys &amp; Emotion</source>
        <translation>笑臉與情感</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>People &amp; Body</source>
        <translation>人物與身體</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Animals &amp; Nature</source>
        <translation>動物與自然</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Food &amp; Drink</source>
        <translation>食物與飲品</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Travel &amp; Places</source>
        <translation>旅行與地點</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activities</source>
        <translation>活動</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Objects</source>
        <translation>物品</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Symbols</source>
        <translation>符號</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Flags</source>
        <translation>旗幟</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Math</source>
        <translation>數學</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Arrows</source>
        <translation>箭頭</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Currency</source>
        <translation>貨幣</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Punctuation</source>
        <translation>標點</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Shapes</source>
        <translation>形狀</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Misc Symbols</source>
        <translation>其他符號</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Greek</source>
        <translation>希臘文</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Number Forms</source>
        <translation>數字形式</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Fancy Letters</source>
        <translation>花式字母</translation>
    </message>
</context>
<context>
    <name>emoji-grid-model</name>
    <message>
        <location line="+129"/>
        <source>Copy</source>
        <translation>複製</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy name</source>
        <translation>複製名稱</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy unicode codepoint</source>
        <translation>複製 Unicode 碼位</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy category</source>
        <translation>複製分類</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>Skin tones</source>
        <translation>膚色</translation>
    </message>
</context>
<context>
    <name>file-list-item</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+35"/>
        <source>Copy file</source>
        <translation>複製檔案</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy file path</source>
        <translation>複製檔案路徑</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy file name</source>
        <translation>複製檔名</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Copy mime type</source>
        <translation>複製 MIME 型別</translation>
    </message>
</context>
<context>
    <name>font-categories</name>
    <message>
        <location filename="../src/font-service.cpp" line="+127"/>
        <source>Latin</source>
        <translation>拉丁文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Cyrillic</source>
        <translation>西里爾文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Greek</source>
        <translation>希臘文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Monospace</source>
        <translation>等寬</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Emoji</source>
        <translation>表情符號</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Japanese</source>
        <translation>日文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Korean</source>
        <translation>韓文</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Simplified Chinese</source>
        <translation>簡體中文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Traditional Chinese</source>
        <translation>繁體中文</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Arabic</source>
        <translation>阿拉伯文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Hebrew</source>
        <translation>希伯來文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Thai</source>
        <translation>泰文</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lao</source>
        <translation>寮國文</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Devanagari</source>
        <translation>天城文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Bengali</source>
        <translation>孟加拉文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Gurmukhi</source>
        <translation>古木基文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Gujarati</source>
        <translation>古吉拉特文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tamil</source>
        <translation>泰米爾文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Telugu</source>
        <translation>泰盧固文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Kannada</source>
        <translation>卡納達文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Malayalam</source>
        <translation>馬拉雅拉姆文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Sinhala</source>
        <translation>僧伽羅文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Armenian</source>
        <translation>亞美尼亞文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Georgian</source>
        <translation>喬治亞文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Thaana</source>
        <translation>塔安那文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tibetan</source>
        <translation>藏文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Myanmar</source>
        <translation>緬甸文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Khmer</source>
        <translation>高棉文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Syriac</source>
        <translation>敘利亞文</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ogham</source>
        <translation>歐甘文</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Runic</source>
        <translation>盧恩文</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>N&apos;Ko</source>
        <translation>西非書面文字</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Symbols</source>
        <translation>符號</translation>
    </message>
</context>
<context>
    <name>font-grid-model</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="+26"/>
        <source>Copy font family</source>
        <translation>複製字型族</translation>
    </message>
</context>
<context>
    <name>keybind-manager</name>
    <message>
        <location filename="../src/internal/keyboard/keybind-manager.cpp" line="+9"/>
        <source>Toggle action panel</source>
        <translation>切換操作面板</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Toggle the action panel to access and filter through the list of available actions for the currently selected item</source>
        <translation>切換操作面板，以檢視並篩選當前所選專案的可用操作</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Open Search Filter</source>
        <translation>開啟搜尋篩選器</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the search filter selector if present</source>
        <translation>開啟搜尋篩選器選擇器（如果有）</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Open settings window</source>
        <translation>開啟設定視窗</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open this settings window from the launcher window</source>
        <translation>從啟動器視窗開啟此設定視窗</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Open Action</source>
        <translation>通用開啟操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can open the selected item</source>
        <translation>供可開啟所選專案的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Copy Action</source>
        <translation>通用複製操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the selected item</source>
        <translation>供可複製所選專案的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Name Action</source>
        <translation>複製名稱操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the name of the selected item</source>
        <translation>供可複製所選專案名稱的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Path Action</source>
        <translation>複製路徑操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the path of the selected item</source>
        <translation>供可複製所選專案路徑的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Save Action</source>
        <translation>儲存操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can save the selected item</source>
        <translation>供可儲存所選專案的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Duplicate Action</source>
        <translation>建立副本操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can duplicate the selected item</source>
        <translation>供可建立所選專案副本的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic New Action</source>
        <translation>通用新建操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that create something</source>
        <translation>供建立內容的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Move Up Action</source>
        <translation>通用上移操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can move up the selected item. This does not affect list navigation controls.</source>
        <translation>供可上移所選專案的操作使用，不影響列表導航控制。</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Move Down Action</source>
        <translation>通用下移操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can move down the selected item. This does not affect list navigation controls.</source>
        <translation>供可下移所選專案的操作使用，不影響列表導航控制。</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Refresh Action</source>
        <translation>通用重新整理操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can refresh the selected item</source>
        <translation>供可重新整理所選專案的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Pin Action</source>
        <translation>通用固定操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can pin the selected item</source>
        <translation>供可固定所選專案的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Remove Action</source>
        <translation>移除操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can remove the selected item. This is normally used for small, not too impactful removals.</source>
        <translation>供可移除所選專案的操作使用，通常用於影響較小的移除。</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Dangerous Remove Action</source>
        <translation>危險移除操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that perform an impactful removal, generally accompanied by a confirmation dialog.</source>
        <translation>供影響較大的移除操作使用，通常會顯示確認對話方塊。</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit Action</source>
        <translation>編輯操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can edit the currently selected item</source>
        <translation>供可編輯當前所選專案的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit Secondary Action</source>
        <translation>編輯次要屬性操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can edit a secondary characteristic of the currently selected item</source>
        <translation>供可編輯當前所選專案次要屬性的操作使用</translation>
    </message>
</context>
<context>
    <name>macos-update-installer</name>
    <message>
        <location filename="../src/services/update/macos-update-installer.mm" line="-184"/>
        <source>Update image contains more than one app</source>
        <translation>更新映像包含多個應用</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to list update image: %1</source>
        <translation>無法列出更新映像內容：%1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No app found in update image</source>
        <translation>更新映像中未找到應用</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to read the update&apos;s code signature</source>
        <translation>無法讀取更新的程式碼簽名</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to build the signature requirement</source>
        <translation>無法構建簽名要求</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Update signature verification failed (%1)</source>
        <translation>更新簽名驗證失敗（%1）</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Update has no CFBundleShortVersionString</source>
        <translation>更新沒有 CFBundleShortVersionString</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Update version mismatch: expected %1, found %2</source>
        <translation>更新版本不匹配：應為 %1，實際為 %2</translation>
    </message>
</context>
<context>
    <name>shortcut-conflict</name>
    <message>
        <location filename="../src/qml/shortcut-conflict.cpp" line="+10"/>
        <source>Modifier required</source>
        <translation>需要修飾鍵</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+5"/>
        <source>Already bound to &quot;%1&quot;</source>
        <translation>已繫結到“%1”</translation>
    </message>
</context>
<context>
    <name>system-extension</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-34"/>
        <source>Volume %1%</source>
        <translation>音量 %1%</translation>
    </message>
</context>
<context>
    <name>utils</name>
    <message>
        <location filename="../src/utils/utils.cpp" line="+88"/>
        <source>0 bytes</source>
        <translation>0 位元組</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>bytes</source>
        <translation>位元組</translation>
    </message>
</context>
<context>
    <name>virtual-desktops</name>
    <message>
        <location filename="../src/services/window-manager/windows/virtual-desktops.cpp" line="+67"/>
        <source>Desktop %1</source>
        <translation>桌面 %1</translation>
    </message>
</context>
</TS>
