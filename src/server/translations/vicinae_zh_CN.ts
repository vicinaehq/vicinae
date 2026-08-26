<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
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
        <translation>文档</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Report a Bug</source>
        <translation>报告问题</translation>
    </message>
</context>
<context>
    <name>ActionListPanel</name>
    <message>
        <location filename="../src/qml/qml/ActionListPanel.qml" line="+113"/>
        <source>No matching actions</source>
        <translation>没有匹配的操作</translation>
    </message>
    <message>
        <location line="+135"/>
        <source>Filter actions...</source>
        <translation>筛选操作…</translation>
    </message>
</context>
<context>
    <name>AdvancedSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AdvancedSettingsPage.qml" line="+29"/>
        <source>Input &amp; Navigation</source>
        <translation>输入与导航</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Pop on backspace</source>
        <translation>退格键返回上一级</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pop back in navigation on backspace when no input is present.</source>
        <translation>没有输入内容时，按退格键返回上一级。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Activate on single click</source>
        <translation>单击即可打开</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activate items with a single click instead of requiring a double click.</source>
        <translation>单击即可打开项目，无需双击。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Wrap navigation</source>
        <translation>循环导航</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Wrap around to the opposite end when moving past the first or last item.</source>
        <translation>移过第一个或最后一个项目时，循环到另一端。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>IME handling</source>
        <translation>输入法处理</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Include IME Preedit strings as part of search queries.</source>
        <translation>将输入法预编辑文本纳入搜索查询。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Keybinding Scheme</source>
        <translation>快捷键方案</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Default uses the standard macOS keys (arrows, Ctrl+N/P); Vim uses Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Opt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>默认方案使用标准 macOS 按键（方向键、Ctrl+N/P）；Vim 使用 Ctrl+J/K 和 Ctrl+H/L；Emacs 使用 Ctrl+N/P 和 Ctrl+Opt+B/F 导航，并在搜索栏中启用 Emacs 编辑方式。</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Default and Vim use Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Alt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>默认和 Vim 方案使用 Ctrl+J/K 与 Ctrl+H/L；Emacs 使用 Ctrl+N/P 和 Ctrl+Alt+B/F 导航，并在搜索栏中启用 Emacs 编辑方式。</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Search</source>
        <translation>搜索</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Root file search</source>
        <translation>根搜索文件结果</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Files are searched asynchronously, so if enabled you should expect a slight delay for file search results to show up.</source>
        <translation>文件采用异步搜索，启用后搜索结果可能会稍有延迟。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Favicon Fetching</source>
        <translation>网站图标获取</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The favicon provider used to load favicons where needed. Select &apos;None&apos; to turn off favicon loading.</source>
        <translation>用于按需加载网站图标的服务。选择“无”可关闭网站图标加载。</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>System</source>
        <translation>系统</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Input server</source>
        <translation>输入服务器</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Whether to spawn the input server at startup. This needs to be enabled in order to support snippets, paste to active window, and other features that require input monitoring or injection.</source>
        <translation>是否在启动时运行输入服务器。片段展开、粘贴到活动窗口及其他需要监听或注入输入的功能必须启用此项。</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Security</source>
        <translation>安全</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Encrypt sensitive data</source>
        <translation>加密敏感数据</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Encrypt sensitive data at rest, such as clipboard history and internal databases (OAuth tokens, extension local storage, API keys). Note that some components, such as on-disk clipboard history, may not be retroactively affected when toggling this option. Turning on this option may ask you to unlock your keychain. Requires a restart in order to apply.</source>
        <translation>加密静态存储的敏感数据，例如剪贴板历史和内部数据库（OAuth 令牌、扩展本地存储、API 密钥）。切换此选项可能不会追溯处理某些组件，例如磁盘上的剪贴板历史。启用时可能会要求解锁钥匙串。重启后生效。</translation>
    </message>
</context>
<context>
    <name>AlertWidget</name>
    <message>
        <location filename="../src/ui/alert/alert.hpp" line="+15"/>
        <source>Are you sure?</source>
        <translation>确定吗？</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>This action cannot be undone</source>
        <translation>此操作无法撤销</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Confirm</source>
        <translation>确认</translation>
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
        <translation>别名</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Additional words to index this item against</source>
        <translation>用于索引此项目的其他词语</translation>
    </message>
</context>
<context>
    <name>AliasFormViewHost</name>
    <message>
        <location filename="../src/qml/alias-form-view-host.cpp" line="+28"/>
        <source>Set alias - %1</source>
        <translation>设置别名 - %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Submit</source>
        <translation>提交</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Alias modified</source>
        <translation>别名已修改</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to modify alias</source>
        <translation>修改别名失败</translation>
    </message>
</context>
<context>
    <name>AppRootItem</name>
    <message>
        <location filename="../src/root-search/apps/app-root-provider.cpp" line="+22"/>
        <location line="+29"/>
        <source>Application</source>
        <translation>应用程序</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Where</source>
        <translation>位置</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Opens in terminal</source>
        <translation>在终端中打开</translation>
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
        <translation>打开应用程序</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy App ID</source>
        <translation>复制应用 ID</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy App Location</source>
        <translation>复制应用位置</translation>
    </message>
</context>
<context>
    <name>AppRootProvider</name>
    <message>
        <location line="+75"/>
        <source>Applications</source>
        <translation>应用程序</translation>
    </message>
</context>
<context>
    <name>AppSelectorModel</name>
    <message>
        <location filename="../src/qml/app-selector-model.cpp" line="+20"/>
        <location line="+65"/>
        <source>%1 (Default)</source>
        <translation>%1（默认）</translation>
    </message>
</context>
<context>
    <name>AppearanceSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AppearanceSettingsPage.qml" line="+29"/>
        <location line="+7"/>
        <source>Theme</source>
        <translation>主题</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Font</source>
        <translation>字体</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Font size</source>
        <translation>字号</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The base point size used to compute font sizes. Fractional values are accepted. Recommended range is [10.0;12.0].</source>
        <translation>用于计算字号的基础点数，可使用小数。建议范围为 [10.0;12.0]。</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>e.g. 11</source>
        <translation>例如 11</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Icon Theme</source>
        <translation>图标主题</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The icon theme used for system icons (applications, mime types, folder icons...). Does not affect builtin Vicinae icons.</source>
        <translation>用于系统图标（应用程序、MIME 类型、文件夹图标等）的图标主题，不影响 Vicinae 内置图标。</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Window</source>
        <translation>窗口</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Window material</source>
        <translation>窗口材质</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Background material applied to the launcher window. Lower the window opacity to see it.</source>
        <translation>应用于启动器窗口的背景材质。降低窗口不透明度即可看到效果。</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Window opacity</source>
        <translation>窗口不透明度</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>e.g. 1.0</source>
        <translation>例如 1.0</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Compact mode</source>
        <translation>紧凑模式</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Show only the search bar at root; expand when a query is entered.</source>
        <translation>在根页面仅显示搜索栏，输入查询后展开。</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Use layer shell</source>
        <translation>使用 layer shell</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Anchor the launcher as a Wayland layer surface (wlr-layer-shell) instead of a regular window. May require reopening Vicinae to fully apply.</source>
        <translation>将启动器固定为 Wayland 层表面（wlr-layer-shell），而非普通窗口。可能需要重新打开 Vicinae 才能完全生效。</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Client-side decorations</source>
        <translation>客户端窗口装饰</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Let Vicinae draw its own rounded borders and shadow instead of relying on the windowing system.</source>
        <translation>由 Vicinae 自行绘制圆角边框和阴影，而非依赖窗口系统。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Corner rounding</source>
        <translation>圆角</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Radius of the launcher window corners, in pixels.</source>
        <translation>启动器窗口的圆角半径，单位为像素。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>e.g. 10</source>
        <translation>例如 10</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Border width</source>
        <translation>边框宽度</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Thickness of the launcher window border, in pixels.</source>
        <translation>启动器窗口的边框宽度，单位为像素。</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>e.g. 3</source>
        <translation>例如 3</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Shadow size</source>
        <translation>阴影大小</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Size of the drop shadow cast by the launcher window, in pixels.</source>
        <translation>启动器窗口投影的大小，单位为像素。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>e.g. 12</source>
        <translation>例如 12</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Native font rendering</source>
        <translation>原生字体渲染</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Use the platform&apos;s native text rendering for system-consistent text. Disable for Qt distance-field rendering (usually faster). May require reopening Vicinae to fully apply.</source>
        <translation>使用平台原生文本渲染，使文字与系统保持一致。关闭后使用 Qt 距离场渲染（通常更快）。可能需要重新打开 Vicinae 才能完全生效。</translation>
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
        <translation>启用后备项</translation>
    </message>
</context>
<context>
    <name>BringToWorkspaceAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="+72"/>
        <source>Bring to current workspace</source>
        <translation>移到当前工作区</translation>
    </message>
</context>
<context>
    <name>BrowseAppsSection</name>
    <message>
        <location filename="../src/qml/browse-apps-model.hpp" line="+32"/>
        <source>Applications ({count})</source>
        <translation>应用程序（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/browse-apps-model.cpp" line="+21"/>
        <source>Hidden</source>
        <translation>已隐藏</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Open Application</source>
        <translation>打开应用程序</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Copy App ID</source>
        <translation>复制应用 ID</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy App Location</source>
        <translation>复制应用位置</translation>
    </message>
</context>
<context>
    <name>BrowseAppsViewHost</name>
    <message>
        <location filename="../src/qml/browse-apps-view-host.cpp" line="+12"/>
        <source>Search apps...</source>
        <translation>搜索应用程序…</translation>
    </message>
</context>
<context>
    <name>BrowseFontsCommand</name>
    <message>
        <location filename="../src/extensions/font/browse-fonts-command.hpp" line="+8"/>
        <source>Search Fonts</source>
        <translation>搜索字体</translation>
    </message>
</context>
<context>
    <name>BrowserExtension</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.hpp" line="+12"/>
        <source>Browser Extension</source>
        <translation>浏览器扩展</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Browser extension related commands.</source>
        <translation>浏览器扩展相关命令。</translation>
    </message>
</context>
<context>
    <name>BrowserTabActionGenerator</name>
    <message>
        <location filename="../src/actions/browser-tab-actions.hpp" line="+24"/>
        <source>Switch to tab</source>
        <translation>切换到标签页</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Convert to shortcut</source>
        <translation>转换为快捷方式</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Convert tab to shortcut</source>
        <translation>将标签页转换为快捷方式</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Close tab</source>
        <translation>关闭标签页</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to close tab: %1</source>
        <translation>关闭标签页失败：%1</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy URL</source>
        <translation>复制 URL</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy Title</source>
        <translation>复制标题</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy ID</source>
        <translation>复制 ID</translation>
    </message>
</context>
<context>
    <name>BrowserTabProvider</name>
    <message>
        <location filename="../src/root-search/browser-tabs/browser-tabs-provider.hpp" line="+70"/>
        <source>Browser Tabs</source>
        <translation>浏览器标签页</translation>
    </message>
</context>
<context>
    <name>BrowserTabRootItem</name>
    <message>
        <location line="-51"/>
        <source>Browser Tab</source>
        <translation>浏览器标签页</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Tab</source>
        <translation>标签页</translation>
    </message>
</context>
<context>
    <name>BrowserTabsSection</name>
    <message>
        <location filename="../src/qml/browser-tabs-model.hpp" line="+17"/>
        <source>Tabs ({count})</source>
        <translation>标签页（{count}）</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Playing Media ({count})</source>
        <translation>正在播放媒体（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/browser-tabs-model.cpp" line="+15"/>
        <source>Muted</source>
        <translation>已静音</translation>
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
        <translation>搜索、聚焦和关闭标签页</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsSection</name>
    <message>
        <location filename="../src/qml/builtin-icons-model.hpp" line="+20"/>
        <source>Icons ({count})</source>
        <translation>图标（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/builtin-icons-model.cpp" line="+15"/>
        <source>Copy Icon Name</source>
        <translation>复制图标名称</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsViewHost</name>
    <message>
        <location filename="../src/qml/builtin-icons-view-host.cpp" line="+10"/>
        <source>Search icons...</source>
        <translation>搜索图标…</translation>
    </message>
</context>
<context>
    <name>CalcHistoryListView</name>
    <message>
        <location filename="../src/qml/qml/CalcHistoryListView.qml" line="+12"/>
        <source>No results</source>
        <translation>无结果</translation>
    </message>
</context>
<context>
    <name>CalcHistorySection</name>
    <message>
        <location filename="../src/qml/calc-history-model.cpp" line="+41"/>
        <source>Copy answer</source>
        <translation>复制答案</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy question</source>
        <translation>复制问题</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy question and answer</source>
        <translation>复制问题和答案</translation>
    </message>
</context>
<context>
    <name>CalcHistoryViewHost</name>
    <message>
        <location filename="../src/qml/calc-history-view-host.cpp" line="+83"/>
        <source>Search past calculations...</source>
        <translation>搜索历史计算…</translation>
    </message>
</context>
<context>
    <name>CalcLiveSection</name>
    <message>
        <location filename="../src/qml/calc-history-view-host.hpp" line="+27"/>
        <source>Calculator</source>
        <translation>计算器</translation>
    </message>
</context>
<context>
    <name>CalculatorExtension</name>
    <message>
        <location filename="../src/extensions/calculator/calculator-extension.hpp" line="+73"/>
        <source>Calculator</source>
        <translation>计算器</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Do maths, convert units or search past calculations...</source>
        <translation>进行计算、单位换算或搜索历史计算…</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Calculator Backend</source>
        <translation>计算器后端</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Which backend to use to perform calculations</source>
        <translation>用于执行计算的后端</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Refresh rates on startup</source>
        <translation>启动时刷新汇率</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Whether exchange rates should be refreshed every time the vicinae server is started. If the current backend does not support it, this is ignored.</source>
        <translation>每次启动 Vicinae 服务器时是否刷新汇率。如果当前后端不支持，此设置将被忽略。</translation>
    </message>
</context>
<context>
    <name>CalculatorHistoryCommand</name>
    <message>
        <location line="-86"/>
        <source>Calculator history</source>
        <translation>计算历史</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse past calculations. You need to copy the result of a calculation for it to be saved in history.</source>
        <translation>浏览历史计算。计算结果需复制后才会保存到历史记录。</translation>
    </message>
</context>
<context>
    <name>CalculatorRefreshRatesCommand</name>
    <message>
        <location line="+11"/>
        <source>Refresh Exchange Rates</source>
        <translation>刷新汇率</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Refresh exchange rates used by the calculator to provide currency conversion features. Not all backends may support currency conversions or manually refreshing the rates.</source>
        <translation>刷新计算器用于货币换算的汇率。并非所有后端都支持货币换算或手动刷新汇率。</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>%1 can&apos;t refresh rates</source>
        <translation>%1 无法刷新汇率</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Refreshing rates...</source>
        <translation>正在刷新汇率…</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Rates successfully refreshed</source>
        <translation>汇率已刷新</translation>
    </message>
</context>
<context>
    <name>CalculatorResultDelegate</name>
    <message>
        <location filename="../src/qml/qml/CalculatorResultDelegate.qml" line="+40"/>
        <source>Question</source>
        <translation>问题</translation>
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
        <translation>本周</translation>
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
        <translation>几年前</translation>
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
        <translation>%1 肤色</translation>
    </message>
</context>
<context>
    <name>ClearClipboardHistoryCommand</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.cpp" line="+35"/>
        <source>Clear Clipboard History</source>
        <translation>清除剪贴板历史</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clear the clipboard history</source>
        <translation>清除剪贴板历史</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Are you sure?</source>
        <translation>确定吗？</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Your clipboard history will be gone forever :(</source>
        <translation>剪贴板历史会永远消失 :(</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to clear clipboard history</source>
        <translation>清除剪贴板历史失败</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard history cleared</source>
        <translation>剪贴板历史已清除</translation>
    </message>
</context>
<context>
    <name>ClipboardClearCommand</name>
    <message>
        <location line="-39"/>
        <source>Clear Current Clipboard Data</source>
        <translation>清除当前剪贴板数据</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clear the current content of the clipboard</source>
        <translation>清除剪贴板当前内容</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Failed to clear clipboard</source>
        <translation>清除剪贴板失败</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard cleared</source>
        <translation>剪贴板已清除</translation>
    </message>
</context>
<context>
    <name>ClipboardExtension</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.hpp" line="+13"/>
        <source>Clipboard</source>
        <translation>剪贴板</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>System clipboard integration</source>
        <translation>系统剪贴板集成</translation>
    </message>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.cpp" line="+55"/>
        <source>Erase on startup</source>
        <translation>启动时清除</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Erase clipboard history every time the vicinae server is started</source>
        <translation>每次启动 Vicinae 服务器时清除剪贴板历史</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Clipboard monitoring</source>
        <translation>剪贴板监控</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Whether clipboard activity is recorded in the history. Every clipboard action performed while this is turned off will not be recorded.</source>
        <translation>是否将剪贴板活动记录到历史中。关闭期间的所有剪贴板操作都不会被记录。</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Ignore Passwords</source>
        <translation>忽略密码</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Ignore selections that can be identified as a password. This relies on the application providing an explicit hint that the selection is a password. While most password managers and private browser windows do, some might not implement this properly.</source>
        <translation>忽略可识别为密码的选中内容。此功能依赖应用明确标记该内容为密码。大多数密码管理器和浏览器隐私窗口会提供此标记，但部分应用可能未正确实现。</translation>
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
        <translation>图片</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Links</source>
        <translation>链接</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Files</source>
        <translation>文件</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryCommand</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-history-command.cpp" line="+16"/>
        <source>Paste</source>
        <translation>粘贴</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy</source>
        <translation>复制</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Default Action</source>
        <translation>默认操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to perform on pressing return. Paste is only available if your environment supports it.</source>
        <translation>按回车键时执行的默认操作。仅当当前环境支持时才能粘贴。</translation>
    </message>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-history-command.hpp" line="+11"/>
        <source>Clipboard History</source>
        <translation>剪贴板历史</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse your clipboard&apos;s history, pin, edit and remove entries.</source>
        <translation>浏览剪贴板历史，并固定、编辑或删除条目。</translation>
    </message>
</context>
<context>
    <name>ClipboardHistorySection</name>
    <message>
        <location filename="../src/qml/clipboard-history-model.cpp" line="+58"/>
        <source>Open Settings</source>
        <translation>打开设置</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryView</name>
    <message>
        <location filename="../src/qml/qml/ClipboardHistoryView.qml" line="+187"/>
        <source>Type</source>
        <translation>类型</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Size</source>
        <translation>大小</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copied at</source>
        <translation>复制时间</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>Preview not available for this content type</source>
        <translation>无法预览此类型的内容</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryViewHost</name>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.hpp" line="+84"/>
        <source>Loading...</source>
        <translation>正在加载…</translation>
    </message>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.cpp" line="+101"/>
        <source>Browse clipboard history...</source>
        <translation>浏览剪贴板历史…</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard monitoring unavailable</source>
        <translation>剪贴板监控不可用</translation>
    </message>
    <message>
        <location line="+67"/>
        <source>Pause clipboard</source>
        <translation>暂停剪贴板监控</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Resume clipboard</source>
        <translation>恢复剪贴板监控</translation>
    </message>
    <message numerus="yes">
        <location line="+8"/>
        <source>%n Items</source>
        <translation>
            <numerusform>%n 项</numerusform>
        </translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Decryption failed</source>
        <translation>解密失败</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Vicinae could not decrypt the data for this selection. It was most likely encrypted with a different key and cannot be recovered. You can remove this entry from the history.</source>
        <translation>Vicinae 无法解密此选中项的数据。该数据很可能使用了其他密钥加密，无法恢复。你可以从历史记录中删除此条目。</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Data unavailable</source>
        <translation>数据不可用</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The data for this selection could not be found on disk.</source>
        <translation>无法在磁盘上找到此选中项的数据。</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Data is encrypted</source>
        <translation>数据已加密</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Data for this selection was previously encrypted but the clipboard is not currently configured to use encryption. You should be able to fix this by enabling it in the settings.</source>
        <translation>此选中项的数据之前已加密，但剪贴板目前未配置为使用加密。在设置中启用加密应该可以解决此问题。</translation>
    </message>
</context>
<context>
    <name>ClipboardService</name>
    <message>
        <location filename="../src/services/clipboard/clipboard-service.cpp" line="+316"/>
        <source>Image (%1x%2)</source>
        <translation>图片（%1x%2）</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Image</source>
        <translation>图片</translation>
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
        <translation>关闭窗口</translation>
    </message>
</context>
<context>
    <name>CommandLineSection</name>
    <message>
        <location filename="../src/qml/system-run-model.hpp" line="+19"/>
        <source>Execute query</source>
        <translation>执行查询</translation>
    </message>
    <message>
        <location filename="../src/qml/system-run-model.cpp" line="+44"/>
        <source>Open in %1 (hold)</source>
        <translation>在 %1 中打开（保持窗口）</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open in %1</source>
        <translation>在 %1 中打开</translation>
    </message>
</context>
<context>
    <name>CommandListView</name>
    <message>
        <location filename="../src/qml/qml/CommandListView.qml" line="+12"/>
        <source>No results</source>
        <translation>无结果</translation>
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
        <translation>打开命令</translation>
    </message>
    <message>
        <location line="-13"/>
        <source>Copy extension path</source>
        <translation>复制扩展路径</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Internal Command</source>
        <translation>内部命令</translation>
    </message>
</context>
<context>
    <name>CompletionPopup</name>
    <message>
        <location filename="../src/qml/qml/CompletionPopup.qml" line="+11"/>
        <source>Filter...</source>
        <translation>筛选…</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorAnswerAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+29"/>
        <source>Answer copied to clipboard</source>
        <translation>答案已复制到剪贴板</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy answer</source>
        <translation>复制答案失败</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy Result</source>
        <translation>复制结果</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorQuestionAndAnswerAction</name>
    <message>
        <location line="+18"/>
        <source>Answer copied to clipboard</source>
        <translation>答案已复制到剪贴板</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy answer</source>
        <translation>复制答案失败</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy Question And Answer</source>
        <translation>复制问题和答案</translation>
    </message>
</context>
<context>
    <name>CopyClipboardSelection</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+29"/>
        <source>Selection copied to clipboard</source>
        <translation>选中内容已复制到剪贴板</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to copy to clipboard</source>
        <translation>复制到剪贴板失败</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy to clipboard</source>
        <translation>复制到剪贴板</translation>
    </message>
</context>
<context>
    <name>CopyItemDeeplink</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+75"/>
        <source>Deeplink copied in clipboard</source>
        <translation>深层链接已复制到剪贴板</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Deeplink</source>
        <translation>复制深层链接</translation>
    </message>
</context>
<context>
    <name>CopyShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="+230"/>
        <source>Copied to clipboard</source>
        <translation>已复制到剪贴板</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy shortcut</source>
        <translation>复制快捷方式</translation>
    </message>
</context>
<context>
    <name>CopyToClipboardAction</name>
    <message>
        <location filename="../src/clipboard-actions.hpp" line="+23"/>
        <source>Copied to clipboard</source>
        <translation>已复制到剪贴板</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy to clipboard</source>
        <translation>复制到剪贴板</translation>
    </message>
</context>
<context>
    <name>CreateExtensionCommand</name>
    <message>
        <location filename="../src/extensions/developer/developer-extension.hpp" line="+10"/>
        <source>Create Extension</source>
        <translation>创建扩展</translation>
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
        <translation>如果要将扩展提交到 &lt;a href=&quot;vicinae://launch/core/store&quot;&gt;Vicinae Store&lt;/a&gt;，此项必须与 GitHub 用户名完全一致。否则可任意填写。</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Username</source>
        <translation>用户名</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Extension Title</source>
        <translation>扩展标题</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>My Extension</source>
        <translation>我的扩展</translation>
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
        <translation>一个能完成超酷功能的扩展</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Location</source>
        <translation>位置</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Command Title</source>
        <translation>命令标题</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>My Wonderful Command</source>
        <translation>我的超棒命令</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>My command does this, and that...</source>
        <translation>我的命令可以完成这项和那项功能…</translation>
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
# 扩展创建成功

新扩展 %1 已成功创建于 `%2`。

要让 Vicinae 识别此扩展中的命令，需要至少以开发模式运行一次：

```bash
cd %2
npm install
npm run dev
```

有关扩展开发的更多信息，请参阅 [Vicinae 文档](https://docs.vicinae.com/)。
</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Open in %1</source>
        <translation>在 %1 中打开</translation>
    </message>
</context>
<context>
    <name>CreateExtensionViewHost</name>
    <message>
        <location filename="../src/qml/create-extension-view-host.cpp" line="+44"/>
        <source>Create extension</source>
        <translation>创建扩展</translation>
    </message>
    <message>
        <location line="+18"/>
        <location line="+4"/>
        <location line="+19"/>
        <location line="+5"/>
        <source>Min. 3 chars</source>
        <translation>至少 3 个字符</translation>
    </message>
    <message>
        <location line="-20"/>
        <source>Min. 16 chars</source>
        <translation>至少 16 个字符</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Must exist</source>
        <translation>必须存在</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Form has errors</source>
        <translation>表单有错误</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Failed to create extension</source>
        <translation>创建扩展失败</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Extension created!</source>
        <translation>扩展已创建！</translation>
    </message>
</context>
<context>
    <name>CreateShortcutCommand</name>
    <message>
        <location filename="../src/extensions/shortcut/create-shortcut-command.hpp" line="+10"/>
        <source>Create Shortcut</source>
        <translation>创建快捷方式</translation>
    </message>
</context>
<context>
    <name>CreateShortcutFromActiveBrowserTabCommand</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="+44"/>
        <source>Create Shortcut from Active Tab</source>
        <translation>从当前标签页创建快捷方式</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Create a vicinae shortcut from the currently active browser tab. May yield unexpected results if many browsers are connected at once.</source>
        <translation>从当前活动的浏览器标签页创建 Vicinae 快捷方式。如果同时连接了多个浏览器，结果可能不符合预期。</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>No active tab!</source>
        <translation>没有活动标签页！</translation>
    </message>
</context>
<context>
    <name>CreateSnippetCommand</name>
    <message>
        <location filename="../src/extensions/snippet/create-snippet-command.hpp" line="+10"/>
        <source>Create Snippet</source>
        <translation>创建片段</translation>
    </message>
</context>
<context>
    <name>DMenuSection</name>
    <message>
        <location filename="../src/qml/dmenu-model.cpp" line="+93"/>
        <source>Select entry</source>
        <translation>选择条目</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pass search text</source>
        <translation>传递搜索文本</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Select and copy entry</source>
        <translation>选择并复制条目</translation>
    </message>
</context>
<context>
    <name>DMenuView</name>
    <message>
        <location filename="../src/qml/qml/DMenuView.qml" line="+79"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Path</source>
        <translation>路径</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>类型</translation>
    </message>
</context>
<context>
    <name>DMenuViewHost</name>
    <message>
        <location filename="../src/qml/dmenu-view-host.cpp" line="+34"/>
        <source>Search entries...</source>
        <translation>搜索条目…</translation>
    </message>
    <message>
        <location line="+73"/>
        <source>Pass search text</source>
        <translation>传递搜索文本</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Pass and copy search text</source>
        <translation>传递并复制搜索文本</translation>
    </message>
</context>
<context>
    <name>DetailListView</name>
    <message>
        <location filename="../src/qml/qml/DetailListView.qml" line="+32"/>
        <source>No results</source>
        <translation>无结果</translation>
    </message>
</context>
<context>
    <name>DeveloperExtension</name>
    <message>
        <location filename="../src/extensions/developer/developer-extension.hpp" line="+10"/>
        <source>Developer</source>
        <translation>开发者</translation>
    </message>
</context>
<context>
    <name>DisableApplication</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+8"/>
        <source>Disable item</source>
        <translation>禁用项目</translation>
    </message>
</context>
<context>
    <name>DisableItemAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="+88"/>
        <source>Are you sure?</source>
        <translation>确定吗？</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You will need to go in the settings to manually re-enable it.</source>
        <translation>需要前往设置手动重新启用。</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Disable</source>
        <translation>禁用</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Item disabled</source>
        <translation>项目已禁用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to disable</source>
        <translation>禁用失败</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Disable item</source>
        <translation>禁用项目</translation>
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
        <translation>创建链接副本</translation>
    </message>
</context>
<context>
    <name>EditClipboardKeywordsAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+49"/>
        <source>Additional keywords that will be used to index this selection.</source>
        <translation>用于索引此选中项的其他关键词。</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit keywords</source>
        <translation>编辑关键词</translation>
    </message>
</context>
<context>
    <name>EditEmojiKeywordsAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+35"/>
        <source>Additional keywords that will be used to index this glyph</source>
        <translation>用于索引此字符的其他关键词</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Edit keyword</source>
        <translation>编辑关键词</translation>
    </message>
</context>
<context>
    <name>EditKeywordsFormView</name>
    <message>
        <location filename="../src/qml/qml/EditKeywordsFormView.qml" line="+19"/>
        <source>Keywords</source>
        <translation>关键词</translation>
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
        <translation>关键词已编辑</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to edit keywords</source>
        <translation>编辑关键词失败</translation>
    </message>
</context>
<context>
    <name>EditShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-41"/>
        <source>Edit shortcut</source>
        <translation>编辑快捷方式</translation>
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
        <translation>搜索表情符号和符号…</translation>
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
        <translation>无结果</translation>
    </message>
</context>
<context>
    <name>EnabledFallbackSection</name>
    <message>
        <location filename="../src/qml/manage-fallback-model.hpp" line="-19"/>
        <source>Enabled</source>
        <translation>已启用</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-fallback-model.cpp" line="-22"/>
        <source>Disable fallback</source>
        <translation>禁用后备项</translation>
    </message>
</context>
<context>
    <name>Expansion</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.hpp" line="+31"/>
        <source>Keyword cannot be empty</source>
        <translation>关键词不能为空</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Keyword exceeds maximum length of %1</source>
        <translation>关键词超过最大长度 %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keyword must only contain printable ASCII characters (no spaces)</source>
        <translation>关键词只能包含可打印的 ASCII 字符（不含空格）</translation>
    </message>
</context>
<context>
    <name>ExtensionBoilerplateGenerator</name>
    <message>
        <location filename="../src/services/extension-boilerplate-generator/extension-boilerplate-generator.cpp" line="+24"/>
        <source>Simple List</source>
        <translation>简单列表</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>List with Detail</source>
        <translation>带详情的列表</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Controlled List</source>
        <translation>受控列表</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Simple Detail</source>
        <translation>简单详情</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>No View</source>
        <translation>无视图</translation>
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
        <translation># 扩展崩溃了 💥！

此扩展抛出了未捕获的异常并因此崩溃。

完整堆栈跟踪如下，也可直接从操作菜单复制。

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
        <translation>一个或多个字段有错误</translation>
    </message>
</context>
<context>
    <name>ExtensionGridModel</name>
    <message>
        <location filename="../src/qml/extension-grid-model.cpp" line="+186"/>
        <source>Search...</source>
        <translation>搜索…</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>No results</source>
        <translation>无结果</translation>
    </message>
</context>
<context>
    <name>ExtensionGridView</name>
    <message>
        <location filename="../src/qml/qml/ExtensionGridView.qml" line="+9"/>
        <source>No results</source>
        <translation>无结果</translation>
    </message>
</context>
<context>
    <name>ExtensionListModel</name>
    <message>
        <location filename="../src/qml/extension-list-model.cpp" line="+189"/>
        <source>Search...</source>
        <translation>搜索…</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No results</source>
        <translation>无结果</translation>
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
        <translation>偏好设置</translation>
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
        <translation>添加别名</translation>
    </message>
</context>
<context>
    <name>ExtensionView</name>
    <message>
        <location filename="../src/qml/qml/ExtensionView.qml" line="+99"/>
        <source>No results</source>
        <translation>无结果</translation>
    </message>
</context>
<context>
    <name>FileExtension</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="+65"/>
        <source>System files</source>
        <translation>系统文件</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Integrate with system files</source>
        <translation>与系统文件集成</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Enabled</source>
        <translation>已启用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Whether to run the file indexer in the background. When turned off, the indexer process is stopped entirely and file search becomes unavailable until it is turned back on.</source>
        <translation>是否在后台运行文件索引器。关闭后索引器进程将完全停止，重新启用前无法搜索文件。</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Search paths</source>
        <translation>搜索路径</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Directories that Vicinae will search</source>
        <translation>Vicinae 要搜索的目录</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Excluded search paths</source>
        <translation>排除的搜索路径</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Directories to exclude from file indexing</source>
        <translation>不纳入文件索引的目录</translation>
    </message>
</context>
<context>
    <name>FilePreview</name>
    <message>
        <location filename="../src/qml/qml/FilePreview.qml" line="+42"/>
        <source>Preview not available for this file type</source>
        <translation>无法预览此文件类型</translation>
    </message>
</context>
<context>
    <name>FocusWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="-17"/>
        <source>Focus window</source>
        <translation>聚焦窗口</translation>
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
        <translation>字体</translation>
    </message>
</context>
<context>
    <name>FontGridModel</name>
    <message>
        <location filename="../src/qml/font-grid-model.hpp" line="+47"/>
        <source>Search fonts...</source>
        <translation>搜索字体…</translation>
    </message>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="+133"/>
        <source>All Fonts (%1)</source>
        <translation>所有字体（%1）</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Results (%1)</source>
        <translation>结果（%1）</translation>
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
        <translation>强制退出应用程序</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to force quit %1</source>
        <translation>强制退出 %1 失败</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Force quit %1</source>
        <translation>强制退出 %1</translation>
    </message>
</context>
<context>
    <name>ForgetTelemetryCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+210"/>
        <source>Forget Past Vicinae Telemetry</source>
        <translation>清除以往的 Vicinae 遥测关联</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Asks the vicinae server to anonymize telemetry data that was sent with your vicinae instance ID attached. The ID is only linked to your vicinae install, which has no direct relationship with your system.</source>
        <translation>要求 Vicinae 服务器匿名化附带 Vicinae 实例 ID 发送的遥测数据。该 ID 仅与此次 Vicinae 安装关联，与系统没有直接关系。</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Processing...</source>
        <translation>正在处理…</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Past telemetry was successfully detached from your vicinae user ID.</source>
        <translation>以往遥测数据已与 Vicinae 用户 ID 解除关联。</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to forget past telemetry data</source>
        <translation>清除以往遥测数据关联失败</translation>
    </message>
</context>
<context>
    <name>FormAppSelector</name>
    <message>
        <location filename="../src/qml/qml/FormAppSelector.qml" line="+50"/>
        <source>All applications</source>
        <translation>所有应用程序</translation>
    </message>
    <message>
        <location line="+70"/>
        <source>+ Restrict to app…</source>
        <translation>+ 限定到应用…</translation>
    </message>
</context>
<context>
    <name>FormFilePicker</name>
    <message>
        <location filename="../src/qml/qml/FormFilePicker.qml" line="+84"/>
        <source>Select files</source>
        <translation>选择文件</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Select a file</source>
        <translation>选择文件</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Select a directory</source>
        <translation>选择目录</translation>
    </message>
    <message>
        <location line="+46"/>
        <source>No directory selected</source>
        <translation>未选择目录</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No file selected</source>
        <translation>未选择文件</translation>
    </message>
    <message>
        <location line="+115"/>
        <source>+ Add folder…</source>
        <translation>+ 添加文件夹…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>+ Add file…</source>
        <translation>+ 添加文件…</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsModel</name>
    <message>
        <location filename="../src/qml/general-settings-model.cpp" line="+176"/>
        <location line="+11"/>
        <source>None</source>
        <translation>无</translation>
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
        <translation>液态玻璃</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Window material</source>
        <translation>窗口材质</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Themes</source>
        <translation>主题</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Fonts</source>
        <translation>字体</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Icon Themes</source>
        <translation>图标主题</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Favicon Services</source>
        <translation>网站图标服务</translation>
    </message>
    <message>
        <location line="+13"/>
        <location line="+10"/>
        <source>Default</source>
        <translation>默认</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Keybinding Schemes</source>
        <translation>快捷键方案</translation>
    </message>
    <message>
        <location line="+48"/>
        <location line="+10"/>
        <source>System default</source>
        <translation>系统默认</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>Languages</source>
        <translation>语言</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/GeneralSettingsPage.qml" line="+29"/>
        <source>Behavior</source>
        <translation>行为</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Launcher hotkey</source>
        <translation>启动器快捷键</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Global shortcut to toggle the Vicinae launcher.</source>
        <translation>用于显示或隐藏 Vicinae 启动器的全局快捷键。</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Close on focus loss</source>
        <translation>失去焦点时关闭</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Close on Escape</source>
        <translation>按 Escape 键关闭</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pressing Escape closes the launcher instead of navigating one view back.</source>
        <translation>按 Escape 键时关闭启动器，而不是返回上一级视图。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Pop to root on close</source>
        <translation>关闭时返回根视图</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reset the navigation state when the launcher window is closed.</source>
        <translation>关闭启动器窗口时重置导航状态。</translation>
    </message>
    <message>
        <location line="+10"/>
        <location line="+7"/>
        <source>Language</source>
        <translation>语言</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Requires restarting Vicinae to take effect.</source>
        <translation>重启 Vicinae 后生效。</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Privacy</source>
        <translation>隐私</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Basic usage statistics</source>
        <translation>基本使用情况统计</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Send basic system and vicinae installation information on startup to help improve Vicinae.</source>
        <translation>启动时发送基本系统和 Vicinae 安装信息，帮助改进 Vicinae。</translation>
    </message>
</context>
<context>
    <name>GenericGridView</name>
    <message>
        <location filename="../src/qml/qml/GenericGridView.qml" line="+33"/>
        <source>No results</source>
        <translation>无结果</translation>
    </message>
</context>
<context>
    <name>GenericListView</name>
    <message>
        <location filename="../src/qml/qml/GenericListView.qml" line="+25"/>
        <source>No results</source>
        <translation>无结果</translation>
    </message>
</context>
<context>
    <name>GlobalShortcutService</name>
    <message>
        <location filename="../src/services/global-shortcuts/global-shortcut-service.cpp" line="+43"/>
        <source>Toggle Vicinae</source>
        <translation>显示或隐藏 Vicinae</translation>
    </message>
    <message>
        <location line="+84"/>
        <source>the launcher hotkey</source>
        <translation>启动器快捷键</translation>
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
        <translation>工作区 %1</translation>
    </message>
</context>
<context>
    <name>HibernateCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+127"/>
        <source>Hibernate System</source>
        <translation>休眠系统</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Suspend the system to disk. This turns off the system completely and saves its state on disk, to be restored on next boot.</source>
        <translation>将系统状态保存到磁盘后完全关机，并在下次启动时恢复。</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>System can&apos;t hibernate</source>
        <translation>系统无法休眠</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to hibernate</source>
        <translation>休眠失败</translation>
    </message>
</context>
<context>
    <name>IconBrowserCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+25"/>
        <source>Search Builtin Icons</source>
        <translation>搜索内置图标</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search Vicinae builtin set of icons</source>
        <translation>搜索 Vicinae 内置图标集</translation>
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
        <translation>查看本地存储</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Browse data stored in Vicinae&apos;s local storage. This includes data stored for builtin extensions as well as third-party extensions making use of the LocalStorage API.</source>
        <translation>浏览 Vicinae 本地存储中的数据，包括内置扩展的数据，以及使用 LocalStorage API 的第三方扩展数据。</translation>
    </message>
</context>
<context>
    <name>InstallUpdateAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+188"/>
        <source>Install Update</source>
        <translation>安装更新</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>An update is already in progress</source>
        <translation>已有更新正在进行</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsSection</name>
    <message>
        <location filename="../src/qml/installed-extensions-model.hpp" line="+18"/>
        <source>Installed Extensions ({count})</source>
        <translation>已安装的扩展（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/installed-extensions-model.cpp" line="+36"/>
        <source>Local</source>
        <translation>本地</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Copy</source>
        <translation>复制</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy Name</source>
        <translation>复制名称</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy ID</source>
        <translation>复制 ID</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy Path</source>
        <translation>复制路径</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy Author</source>
        <translation>复制作者</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsViewHost</name>
    <message>
        <location filename="../src/qml/installed-extensions-view-host.cpp" line="+12"/>
        <source>Search extensions...</source>
        <translation>搜索扩展…</translation>
    </message>
</context>
<context>
    <name>InternalExtension</name>
    <message>
        <location filename="../src/extensions/internal/internal-extension.hpp" line="+32"/>
        <location line="+1"/>
        <source>Internal Commands</source>
        <translation>内部命令</translation>
    </message>
</context>
<context>
    <name>KeyboardBridge</name>
    <message>
        <location filename="../src/qml/keyboard-bridge.hpp" line="+51"/>
        <source>Modifier required</source>
        <translation>需要修饰键</translation>
    </message>
</context>
<context>
    <name>LauncherWindow</name>
    <message>
        <location filename="../src/qml/qml/LauncherWindow.qml" line="+29"/>
        <source>Vicinae Launcher</source>
        <translation>Vicinae 启动器</translation>
    </message>
    <message>
        <location filename="../src/qml/launcher-window.cpp" line="+571"/>
        <source>Open Settings</source>
        <translation>打开设置</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keyboard Shortcuts</source>
        <translation>键盘快捷键</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Documentation</source>
        <translation>文档</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+5"/>
        <source>Opened in browser</source>
        <translation>已在浏览器中打开</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Report a Bug</source>
        <translation>报告问题</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>About Vicinae</source>
        <translation>关于 Vicinae</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemSection</name>
    <message>
        <location filename="../src/qml/local-storage-model.hpp" line="+19"/>
        <source>Items ({count})</source>
        <translation>项目（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/local-storage-model.cpp" line="+31"/>
        <source>Show value</source>
        <translation>显示值</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemViewHost</name>
    <message>
        <location filename="../src/qml/local-storage-view-host.cpp" line="+23"/>
        <source>Search items...</source>
        <translation>搜索项目…</translation>
    </message>
</context>
<context>
    <name>LocalStorageNamespaceSection</name>
    <message>
        <location filename="../src/qml/local-storage-model.hpp" line="-11"/>
        <source>Namespaces ({count})</source>
        <translation>命名空间（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/local-storage-model.cpp" line="-16"/>
        <source>Browse namespace</source>
        <translation>浏览命名空间</translation>
    </message>
</context>
<context>
    <name>LocalStorageViewHost</name>
    <message>
        <location filename="../src/qml/local-storage-view-host.cpp" line="-15"/>
        <source>Search namespaces...</source>
        <translation>搜索命名空间…</translation>
    </message>
</context>
<context>
    <name>LockCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-48"/>
        <source>Lock Session</source>
        <translation>锁定会话</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lock the current user session</source>
        <translation>锁定当前用户会话</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System can&apos;t lock</source>
        <translation>系统无法锁定</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to lock</source>
        <translation>锁定失败</translation>
    </message>
</context>
<context>
    <name>LogOutCommand</name>
    <message>
        <location line="+176"/>
        <source>Log Out</source>
        <translation>退出登录</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Terminate the current user session. If you simply want to lock your session you should use &apos;Lock Session&apos; instead.</source>
        <translation>终止当前用户会话。如果只是想锁定会话，请改用“锁定会话”。</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>System can&apos;t logout</source>
        <translation>系统无法退出登录</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to log out</source>
        <translation>退出登录失败</translation>
    </message>
</context>
<context>
    <name>MacOSGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/macos-global-shortcut-backend.cpp" line="+238"/>
        <source>unsupported or invalid trigger</source>
        <translation>不支持或无效的触发键</translation>
    </message>
</context>
<context>
    <name>MacSettingsRootItem</name>
    <message>
        <location filename="../src/root-search/macos-settings/macos-settings-root-provider.mm" line="+132"/>
        <location line="+9"/>
        <source>System Settings</source>
        <translation>系统设置</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Bundle ID</source>
        <translation>Bundle ID</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Legacy ID</source>
        <translation>旧版 ID</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Where</source>
        <translation>位置</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Open %1 Settings</source>
        <translation>打开“%1”设置</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy URL</source>
        <translation>复制 URL</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy Bundle ID</source>
        <translation>复制 Bundle ID</translation>
    </message>
</context>
<context>
    <name>MacSettingsRootProvider</name>
    <message>
        <location line="+13"/>
        <source>System Settings</source>
        <translation>系统设置</translation>
    </message>
</context>
<context>
    <name>MacosUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/macos-update-installer.mm" line="+208"/>
        <source>This installation cannot update itself</source>
        <translation>此安装无法自行更新</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Mounting update image…</source>
        <translation>正在挂载更新映像…</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Failed to mount the update image</source>
        <translation>挂载更新映像失败</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Could not find the update image mount point</source>
        <translation>找不到更新映像的挂载点</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Verifying update…</source>
        <translation>正在验证更新…</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Installing update…</source>
        <translation>正在安装更新…</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to stage update: %1</source>
        <translation>暂存更新失败：%1</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Failed to move the current app aside: %1</source>
        <translation>移开当前应用失败：%1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to install the new app: %1</source>
        <translation>安装新应用失败：%1</translation>
    </message>
</context>
<context>
    <name>ManageFallbackActions</name>
    <message>
        <location filename="../src/actions/fallback-actions.hpp" line="+15"/>
        <source>Manage Fallback Actions</source>
        <translation>管理后备操作</translation>
    </message>
</context>
<context>
    <name>ManageFallbackCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/configure-fallback-command.hpp" line="+11"/>
        <source>Configure Fallback Commands</source>
        <translation>配置后备命令</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Configure what commands are to be presented as fallback options when nothing matches the search in the root search.</source>
        <translation>配置根搜索无匹配项时显示的后备命令。</translation>
    </message>
</context>
<context>
    <name>ManageFallbackViewHost</name>
    <message>
        <location filename="../src/qml/manage-fallback-view-host.cpp" line="+12"/>
        <source>Search commands...</source>
        <translation>搜索命令…</translation>
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
        <translation>搜索快捷方式…</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Application</source>
        <translation>应用程序</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Opened</source>
        <translation>打开次数</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Last Opened</source>
        <translation>上次打开</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Never</source>
        <translation>从未</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Created at</source>
        <translation>创建时间</translation>
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
        <translation>复制到剪贴板</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copied to clipboard</source>
        <translation>已复制到剪贴板</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy to clipboard</source>
        <translation>复制到剪贴板失败</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Edit snippet</source>
        <translation>编辑片段</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Duplicate snippet</source>
        <translation>创建片段副本</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Remove snippet</source>
        <translation>移除片段</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove snippet</source>
        <translation>移除片段失败</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsViewHost</name>
    <message>
        <location filename="../src/qml/manage-snippets-view-host.hpp" line="+54"/>
        <source>No snippets</source>
        <translation>没有片段</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Create a snippet to get started</source>
        <translation>创建一个片段即可开始</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-snippets-view-host.cpp" line="+28"/>
        <source>Search for snippets...</source>
        <translation>搜索片段…</translation>
    </message>
    <message>
        <location line="+25"/>
        <source>Text</source>
        <translation>文本</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>File</source>
        <translation>文件</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>类型</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Created at</source>
        <translation>创建时间</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Updated at</source>
        <translation>更新时间</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Keyword</source>
        <translation>关键词</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Apps</source>
        <translation>应用程序</translation>
    </message>
    <message>
        <location line="+68"/>
        <source>Create snippet</source>
        <translation>创建片段</translation>
    </message>
</context>
<context>
    <name>MarkItemAsFavorite</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="-60"/>
        <source>Mark as favorite</source>
        <translation>标为收藏</translation>
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
        <translation>预览所有支持的 Markdown 功能</translation>
    </message>
</context>
<context>
    <name>MarkdownView</name>
    <message>
        <location filename="../src/qml/qml/markdown/MarkdownView.qml" line="+265"/>
        <source>Copy</source>
        <translation>复制</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Select All</source>
        <translation>全选</translation>
    </message>
</context>
<context>
    <name>MdCallout</name>
    <message>
        <location filename="../src/qml/qml/markdown/MdCallout.qml" line="+35"/>
        <source>Caution</source>
        <translation>危险</translation>
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
        <translation>备注</translation>
    </message>
</context>
<context>
    <name>MdCodeBlock</name>
    <message>
        <location filename="../src/qml/qml/markdown/MdCodeBlock.qml" line="+58"/>
        <source>Copied!</source>
        <translation>已复制！</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Copy</source>
        <translation>复制</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceView</name>
    <message>
        <location filename="../src/qml/qml/MissingPreferenceView.qml" line="+28"/>
        <source>Welcome to %1</source>
        <translation>欢迎使用 %1</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Before you can use this command, you need to fill in the required preference fields below.</source>
        <translation>使用此命令前，需要填写以下必填偏好设置。</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceViewHost</name>
    <message>
        <location filename="../src/qml/missing-preference-view-host.cpp" line="+202"/>
        <source>Save preferences</source>
        <translation>保存偏好设置</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Please fill in all required fields</source>
        <translation>请填写所有必填字段</translation>
    </message>
</context>
<context>
    <name>NavigationController</name>
    <message>
        <location filename="../src/navigation-controller.cpp" line="+654"/>
        <source>Extension manager is not running</source>
        <translation>扩展管理器未运行</translation>
    </message>
</context>
<context>
    <name>NewsService</name>
    <message>
        <location filename="../src/services/news/news-service.cpp" line="+68"/>
        <source>Telemetry</source>
        <translation>遥测</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>We now collect basic usage statistics on startup</source>
        <translation>现在会在启动时收集基本使用情况统计</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Learn more</source>
        <translation>了解更多</translation>
    </message>
</context>
<context>
    <name>NullUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/null-update-installer.hpp" line="+14"/>
        <source>Self update is not supported on this platform</source>
        <translation>此平台不支持自动更新</translation>
    </message>
</context>
<context>
    <name>OAuthOverlayView</name>
    <message>
        <location filename="../src/qml/qml/OAuthOverlayView.qml" line="+91"/>
        <source>Continue with %1</source>
        <translation>使用 %1 继续</translation>
    </message>
    <message>
        <location line="+33"/>
        <source>You&apos;re in!</source>
        <translation>连接成功！</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Successfully connected to %1.
Back to command in an instant...</source>
        <translation>已成功连接到 %1。
即将返回命令…</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-31"/>
        <source>Manage OAuth Token Sets</source>
        <translation>管理 OAuth 令牌组</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Manage OAuth token sets that have been saved by extensions providing OAuth integrations.</source>
        <translation>管理由提供 OAuth 集成的扩展保存的 OAuth 令牌组。</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreSection</name>
    <message>
        <location filename="../src/qml/oauth-token-store-model.hpp" line="+17"/>
        <source>OAuth Token Sets ({count})</source>
        <translation>OAuth 令牌组（{count}）</translation>
    </message>
    <message>
        <location filename="../src/qml/oauth-token-store-model.cpp" line="+20"/>
        <source>Expired</source>
        <translation>已过期</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Remove token set</source>
        <translation>移除令牌组</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Are you sure?</source>
        <translation>确定吗？</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You will need to go through the OAuth login flow again the next time you want to use this service</source>
        <translation>下次使用此服务时，需要重新完成 OAuth 登录流程</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to remove token set</source>
        <translation>移除令牌组失败</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Token set removed</source>
        <translation>已移除令牌组</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy</source>
        <translation>复制</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy Access Token</source>
        <translation>复制访问令牌</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Refresh Token</source>
        <translation>复制刷新令牌</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy ID Token</source>
        <translation>复制 ID 令牌</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy Scopes</source>
        <translation>复制作用域</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Expiration Date</source>
        <translation>复制过期时间</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreViewHost</name>
    <message>
        <location filename="../src/qml/oauth-token-store-view-host.cpp" line="+12"/>
        <source>Search token sets...</source>
        <translation>搜索令牌组…</translation>
    </message>
</context>
<context>
    <name>OnboardingWindow</name>
    <message>
        <location filename="../src/qml/qml/OnboardingWindow.qml" line="+39"/>
        <source>Grant Access</source>
        <translation>授予访问权限</translation>
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
        <translation>欢迎使用 Vicinae</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Let&apos;s set it up. It only takes a minute.</source>
        <translation>来完成初始设置吧，只需一分钟。</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Permissions</source>
        <translation>权限</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Vicinae needs additional permissions in order to make the best of your Mac.</source>
        <translation>Vicinae 需要额外权限才能充分利用你的 Mac。</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Accessibility</source>
        <translation>辅助功能</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Used to paste, expand snippets, and move windows.</source>
        <translation>用于粘贴、展开片段和移动窗口。</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Full Disk Access</source>
        <translation>完全磁盘访问权限</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Allows file search to cover your entire disk.</source>
        <translation>让文件搜索覆盖整个磁盘。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Notifications</source>
        <translation>通知</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Allows extensions to send desktop notifications.</source>
        <translation>允许扩展发送桌面通知。</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Accessibility is required: global shortcuts, paste, and snippet expansion cannot work without it.</source>
        <translation>必须授予辅助功能权限，否则全局快捷键、粘贴和片段展开将无法使用。</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Full disk access needs to be explicitly enabled if you want file search to cover all your files.</source>
        <translation>若要搜索所有文件，需要明确启用完全磁盘访问权限。</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Make it your own</source>
        <translation>按你的喜好设置</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>You will be able to change these settings later.</source>
        <translation>这些设置稍后仍可更改。</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Theme</source>
        <translation>主题</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Shared across the entire app.</source>
        <translation>在整个应用中共用。</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Global hotkey</source>
        <translation>全局快捷键</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Opens the launcher from anywhere.</source>
        <translation>可在任意位置打开启动器。</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Launch at login</source>
        <translation>登录时启动</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Starts Vicinae in the background at login.</source>
        <translation>登录时在后台启动 Vicinae。</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Setup complete</source>
        <translation>设置完成</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Vicinae is running. Open the launcher with:</source>
        <translation>Vicinae 已在运行。使用以下快捷键打开启动器：</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Vicinae is open source software.</source>
        <translation>Vicinae 是开源软件。</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Sponsor</source>
        <translation>赞助</translation>
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
        <translation>继续</translation>
    </message>
</context>
<context>
    <name>OpenAboutCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/open-about-command.hpp" line="+13"/>
        <source>About</source>
        <translation>关于</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the &quot;About&quot; tab of the vicinae settings.</source>
        <translation>打开 Vicinae 设置中的“关于”标签页。</translation>
    </message>
</context>
<context>
    <name>OpenAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-56"/>
        <source>Failed to start app</source>
        <translation>启动应用失败</translation>
    </message>
</context>
<context>
    <name>OpenAppLocationAction</name>
    <message>
        <location line="-36"/>
        <source>Open Location</source>
        <translation>打开所在位置</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to open app location</source>
        <translation>打开应用所在位置失败</translation>
    </message>
</context>
<context>
    <name>OpenBuiltinCommandAction</name>
    <message>
        <location filename="../src/command-actions.hpp" line="+17"/>
        <source>Open command</source>
        <translation>打开命令</translation>
    </message>
</context>
<context>
    <name>OpenCalculatorHistoryAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+9"/>
        <source>Open Calculator History</source>
        <translation>打开计算器历史记录</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-32"/>
        <source>Open shortcut</source>
        <translation>打开快捷方式</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutWithAction</name>
    <message>
        <location line="+85"/>
        <source>Open with...</source>
        <translation>打开方式…</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelItemAction</name>
    <message>
        <location filename="../src/root-search/control-panel/control-panel-root-provider.cpp" line="+45"/>
        <source>Failed to open settings</source>
        <translation>打开设置失败</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelTaskAction</name>
    <message>
        <location line="+26"/>
        <source>Failed to open settings</source>
        <translation>打开设置失败</translation>
    </message>
</context>
<context>
    <name>OpenDefaultVicinaeConfig</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-157"/>
        <source>Open Default Config File</source>
        <translation>打开默认配置文件</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the default vicinae configuration file</source>
        <translation>打开 Vicinae 默认配置文件</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Failed to open temporary file</source>
        <translation>打开临时文件失败</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Failed to open default config file</source>
        <translation>打开默认配置文件失败</translation>
    </message>
</context>
<context>
    <name>OpenDiscordCommand</name>
    <message>
        <location line="-66"/>
        <source>Join the Discord Server</source>
        <translation>加入 Discord 服务器</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open link to join the official Vicinae discord server.</source>
        <translation>打开链接以加入 Vicinae 官方 Discord 服务器。</translation>
    </message>
</context>
<context>
    <name>OpenDocumentationCommand</name>
    <message>
        <location line="-16"/>
        <source>Open Online Documentation</source>
        <translation>打开在线文档</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Navigate to the official vicinae documentation website.</source>
        <translation>前往 Vicinae 官方文档网站。</translation>
    </message>
</context>
<context>
    <name>OpenFileAction</name>
    <message>
        <location filename="../src/actions/files/file-actions.hpp" line="+18"/>
        <source>Open with %1</source>
        <translation>使用 %1 打开</translation>
    </message>
</context>
<context>
    <name>OpenInBrowserAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="+105"/>
        <source>Open in browser</source>
        <translation>在浏览器中打开</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+95"/>
        <source>Failed to open in browser</source>
        <translation>无法在浏览器中打开</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Opened in browser</source>
        <translation>已在浏览器中打开</translation>
    </message>
</context>
<context>
    <name>OpenInTerminalAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="-62"/>
        <source>Open in %1</source>
        <translation>在 %1 中打开</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-85"/>
        <source>Failed to start app</source>
        <translation>启动应用失败</translation>
    </message>
</context>
<context>
    <name>OpenItemPreferencesAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="-28"/>
        <source>Open Preferences</source>
        <translation>打开偏好设置</translation>
    </message>
</context>
<context>
    <name>OpenKeybindSettingsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+156"/>
        <source>Open Vicinae Keybind Settings</source>
        <translation>打开 Vicinae 快捷键设置</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the vicinae keybind settings window</source>
        <translation>打开 Vicinae 快捷键设置窗口</translation>
    </message>
</context>
<context>
    <name>OpenRawProgramAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="+25"/>
        <source>Execute program</source>
        <translation>执行程序</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+35"/>
        <source>Failed to start app</source>
        <translation>启动应用失败</translation>
    </message>
</context>
<context>
    <name>OpenSettingsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-40"/>
        <source>Open Vicinae Settings</source>
        <translation>打开 Vicinae 设置</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open the vicinae settings window, which is an independent floating window.</source>
        <translation>打开独立浮动的 Vicinae 设置窗口。</translation>
    </message>
</context>
<context>
    <name>OpenSettingsPaneAction</name>
    <message>
        <location filename="../src/root-search/macos-settings/macos-settings-root-provider.mm" line="-135"/>
        <source>Failed to open System Settings</source>
        <translation>打开系统设置失败</translation>
    </message>
</context>
<context>
    <name>OpenShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-128"/>
        <source>No app with id %1</source>
        <translation>没有 ID 为 %1 的应用</translation>
    </message>
    <message>
        <location line="+9"/>
        <location line="+7"/>
        <source>Open shortcut</source>
        <translation>打开快捷方式</translation>
    </message>
</context>
<context>
    <name>OpenShortcutFromSearchText</name>
    <message>
        <location line="+43"/>
        <source>Open shortcut</source>
        <translation>打开快捷方式</translation>
    </message>
</context>
<context>
    <name>OpenVicinaeConfig</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-81"/>
        <source>Open Config File</source>
        <translation>打开配置文件</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the main vicinae configuration file</source>
        <translation>打开 Vicinae 主配置文件</translation>
    </message>
</context>
<context>
    <name>OpenWindowsSettingAction</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="+147"/>
        <source>Failed to open settings</source>
        <translation>打开设置失败</translation>
    </message>
</context>
<context>
    <name>OpenWithAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+54"/>
        <source>Open with...</source>
        <translation>打开方式…</translation>
    </message>
</context>
<context>
    <name>PasteToFocusedWindowAction</name>
    <message>
        <location filename="../src/clipboard-actions.hpp" line="+11"/>
        <source>Paste to active window</source>
        <translation>粘贴到当前窗口</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy to focused window</source>
        <translation>复制到当前窗口</translation>
    </message>
</context>
<context>
    <name>PinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+28"/>
        <source>Entry pinned</source>
        <translation>已固定条目</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pin entry</source>
        <translation>固定条目</translation>
    </message>
</context>
<context>
    <name>PinClipboardAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="-26"/>
        <source>Selection pinned</source>
        <translation>已固定所选内容</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Selection unpinned</source>
        <translation>已取消固定所选内容</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to change pin status</source>
        <translation>更改固定状态失败</translation>
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
        <translation>固定表情符号</translation>
    </message>
</context>
<context>
    <name>PinWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="+37"/>
        <source>Unpin from all workspaces</source>
        <translation>从所有工作区取消固定</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Pin to all workspaces</source>
        <translation>固定到所有工作区</translation>
    </message>
</context>
<context>
    <name>PowerManagementCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-283"/>
        <source>Ask for confirmation</source>
        <translation>要求确认</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Custom program</source>
        <translation>自定义程序</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Custom POSIX shell command to run instead of the default implementation</source>
        <translation>代替默认实现运行的自定义 POSIX shell 命令</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Failed to execute custom program %1</source>
        <translation>执行自定义程序 %1 失败</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Are you sure</source>
        <translation>确定吗</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>High-impact operation, please confirm</source>
        <translation>此操作影响较大，请确认</translation>
    </message>
</context>
<context>
    <name>PowerManagementExtension</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.hpp" line="+8"/>
        <source>Power Management</source>
        <translation>电源管理</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power off, suspend, sleep, hibernate your computer.</source>
        <translation>关闭、挂起、睡眠或休眠电脑。</translation>
    </message>
</context>
<context>
    <name>PowerOffCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+137"/>
        <source>Power Off System</source>
        <translation>关闭系统</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power off the system</source>
        <translation>关闭系统</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>System cannot power off</source>
        <translation>系统无法关机</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to power off</source>
        <translation>关机失败</translation>
    </message>
</context>
<context>
    <name>PreviewFontAction</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="-105"/>
        <source>Preview font</source>
        <translation>预览字体</translation>
    </message>
</context>
<context>
    <name>ProgramsSection</name>
    <message>
        <location filename="../src/qml/system-run-model.hpp" line="+21"/>
        <source>Programs (%1)</source>
        <translation>程序（%1）</translation>
    </message>
    <message>
        <location filename="../src/qml/system-run-model.cpp" line="+52"/>
        <source>Open in %1 (hold)</source>
        <translation>在 %1 中打开（保持窗口）</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open in %1</source>
        <translation>在 %1 中打开</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Copy exec path</source>
        <translation>复制可执行文件路径</translation>
    </message>
</context>
<context>
    <name>ProviderSearchSection</name>
    <message>
        <location filename="../src/qml/provider-search-model.hpp" line="+11"/>
        <source>Results ({count})</source>
        <translation>结果（{count}）</translation>
    </message>
</context>
<context>
    <name>ProviderSearchViewHost</name>
    <message>
        <location filename="../src/qml/provider-search-view-host.cpp" line="+15"/>
        <source>Search %1</source>
        <translation>搜索 %1</translation>
    </message>
</context>
<context>
    <name>PruneMemoryCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+56"/>
        <source>Prune Vicinae Memory Usage</source>
        <translation>清理 Vicinae 内存占用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Try pruning vicinae&apos;s memory usage by clearing pixmap cache and calling malloc_trim(). Mostly provided for internal testing.</source>
        <translation>尝试清除像素图缓存并调用 malloc_trim() 来降低 Vicinae 的内存占用。主要用于内部测试。</translation>
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
        <translation>将答案放入搜索栏</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/utils/utils.cpp" line="+50"/>
        <source>in the future</source>
        <translation>未来</translation>
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
            <numerusform>%n 个月前</numerusform>
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
            <numerusform>%n 小时前</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n minute(s) ago</source>
        <translation>
            <numerusform>%n 分钟前</numerusform>
        </translation>
    </message>
    <message>
        <location line="+2"/>
        <source>just now</source>
        <translation>刚刚</translation>
    </message>
</context>
<context>
    <name>QuitAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-43"/>
        <source>Quit Application</source>
        <translation>退出应用</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to quit %1</source>
        <translation>退出 %1 失败</translation>
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
        <translation>Raycast 兼容功能</translation>
    </message>
</context>
<context>
    <name>RaycastStoreCommand</name>
    <message>
        <location filename="../src/extensions/raycast/raycast-store-command.hpp" line="+13"/>
        <source>Install compatible extensions from the Raycast store</source>
        <translation>从 Raycast Store 安装兼容扩展</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Always show intro</source>
        <translation>始终显示介绍</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>
# Welcome to the Raycast Extension Store

Vicinae provides direct integration with the official [Raycast store](https://www.raycast.com/store), allowing you to search and install Raycast extensions directly from Vicinae.
</source>
        <translation>
# 欢迎使用 Raycast Extension Store

Vicinae 与官方 [Raycast Store](https://www.raycast.com/store) 直接集成，可直接在 Vicinae 中搜索并安装 Raycast 扩展。
</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>
Each extension has a colored compatibility indicator showing how well it works on Linux.

Vicinae also has its own [extension store](vicinae://launch/core/store), which does not suffer from these limitations.
</source>
        <translation>
每个扩展都有彩色兼容性标记，表示它在 Linux 上的运行情况。

Vicinae 也有自己的 [Extension Store](vicinae://launch/core/store)，不受这些限制。
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
        <translation>继续前往商店</translation>
    </message>
</context>
<context>
    <name>RaycastStoreDetailHost</name>
    <message>
        <location filename="../src/qml/raycast-store-detail-host.cpp" line="+43"/>
        <source>Failed to load extension</source>
        <translation>加载扩展失败</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The extension &quot;%1&quot; could not be loaded. It may not exist or the store may be unreachable.</source>
        <translation>无法加载扩展“%1”。它可能不存在，或商店暂时无法访问。</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Extension Store - %1</source>
        <translation>Extension Store - %1</translation>
    </message>
    <message>
        <location line="+31"/>
        <source>This extension should be fully compatible.</source>
        <translation>此扩展应该完全兼容。</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>This extension works but has a few quirks.</source>
        <translation>此扩展可以运行，但有少量问题。</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>This extension is not compatible.</source>
        <translation>此扩展不兼容。</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No compatibility data is available for this extension.</source>
        <translation>没有此扩展的兼容性数据。</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>No compatibility data is available — this extension may or may not work.</source>
        <translation>没有兼容性数据——此扩展可能可用，也可能不可用。</translation>
    </message>
    <message>
        <location line="+83"/>
        <source>Install extension</source>
        <translation>安装扩展</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading extension...</source>
        <translation>正在下载扩展…</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to download extension</source>
        <translation>下载扩展失败</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to extract extension archive</source>
        <translation>解压扩展包失败</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Extension installed</source>
        <translation>扩展已安装</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Report issue</source>
        <translation>报告问题</translation>
    </message>
</context>
<context>
    <name>RaycastStoreSection</name>
    <message>
        <location filename="../src/qml/raycast-store-model.cpp" line="+45"/>
        <source>Show details</source>
        <translation>显示详情</translation>
    </message>
</context>
<context>
    <name>RaycastStoreViewHost</name>
    <message>
        <location filename="../src/qml/raycast-store-view-host.cpp" line="+37"/>
        <source>Browse Raycast extensions</source>
        <translation>浏览 Raycast 扩展</translation>
    </message>
    <message>
        <location line="+32"/>
        <source>Failed to fetch extensions</source>
        <translation>获取扩展失败</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Extensions</source>
        <translation>扩展</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to search extensions</source>
        <translation>搜索扩展失败</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Results</source>
        <translation>结果</translation>
    </message>
</context>
<context>
    <name>RebootCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-72"/>
        <source>Reboot System</source>
        <translation>重启系统</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reboot the system</source>
        <translation>重启系统</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>System can&apos;t reboot</source>
        <translation>系统无法重启</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to reboot</source>
        <translation>重启失败</translation>
    </message>
</context>
<context>
    <name>RebuildFileIndexCommand</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="-72"/>
        <source>Rebuild File Index</source>
        <translation>重建文件索引</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Fully rebuild the file index. Running this manually can be useful if the file search feels particularly out of date.</source>
        <translation>完全重建文件索引。如果文件搜索结果明显过时，可手动运行此操作。</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Are you sure?</source>
        <translation>确定吗？</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Rebuilding the entire index can be time consuming and CPU intensive, depending on the number of files present in your home directory.</source>
        <translation>重建整个索引可能耗时较长并大量占用 CPU，具体取决于主目录中的文件数量。</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Reset</source>
        <translation>重置</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Index rebuild started...</source>
        <translation>已开始重建索引…</translation>
    </message>
</context>
<context>
    <name>RefreshAppsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/refresh-apps-command.hpp" line="+12"/>
        <source>Refresh Apps</source>
        <translation>刷新应用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Force a refresh of the application database. The database should normally automatically update itself on changes, but this can help working around some edge cases.</source>
        <translation>强制刷新应用程序数据库。数据库通常会在发生变化时自动更新，但此操作可用于处理某些特殊情况。</translation>
    </message>
    <message>
        <location filename="../src/extensions/vicinae/refresh-apps-command.cpp" line="+15"/>
        <source>Apps successfully refreshed</source>
        <translation>应用刷新成功</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to refresh apps</source>
        <translation>刷新应用失败</translation>
    </message>
</context>
<context>
    <name>ReloadScriptDirectoriesCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+29"/>
        <source>Reload Script Directories</source>
        <translation>重新加载脚本目录</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reload script directories</source>
        <translation>重新加载脚本目录</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>New scan triggered, index will update shortly</source>
        <translation>已触发新一轮扫描，索引很快会更新</translation>
    </message>
</context>
<context>
    <name>RemoveAllCalculatorHistoryRecordsAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+97"/>
        <source>Delete all entries</source>
        <translation>删除所有条目</translation>
    </message>
</context>
<context>
    <name>RemoveAllSelectionsAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+27"/>
        <source>Are you sure?</source>
        <translation>确定吗？</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>All your clipboard history will be lost forever</source>
        <translation>所有剪贴板历史记录将永久丢失</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Delete all</source>
        <translation>全部删除</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>All selections were removed</source>
        <translation>已移除所有所选内容</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove all selections</source>
        <translation>移除所有所选内容失败</translation>
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
        <translation>已移除条目</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove entry</source>
        <translation>移除条目失败</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Delete entry</source>
        <translation>删除条目</translation>
    </message>
</context>
<context>
    <name>RemoveSelectionAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="-70"/>
        <source>Entry removed</source>
        <translation>已移除条目</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove entry</source>
        <translation>移除条目失败</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Remove entry</source>
        <translation>移除条目</translation>
    </message>
</context>
<context>
    <name>RemoveShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="+33"/>
        <source>Removed link</source>
        <translation>已移除链接</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove link</source>
        <translation>移除链接失败</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Remove link</source>
        <translation>移除链接</translation>
    </message>
</context>
<context>
    <name>ReportVicinaeBugCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/report-bug-command.hpp" line="+10"/>
        <source>Report a Vicinae Bug</source>
        <translation>报告 Vicinae 错误</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Navigate to Vicinae issue creation page with all relevant informations pre-filled.</source>
        <translation>前往 Vicinae 问题创建页面，并预先填写所有相关信息。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Title</source>
        <translation>标题</translation>
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
        <translation>重置为偏好设置</translation>
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
        <translation>无法重置排序</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Are you sure?</source>
        <translation>确定吗？</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You will have to rebuild search history for this item in order for it to reappear on top of the root search results.</source>
        <translation>需要重新积累此项目的搜索历史，它才会再次出现在根搜索结果顶部。</translation>
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
        <translation>在文件浏览器中显示</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to open folder</source>
        <translation>打开文件夹失败</translation>
    </message>
</context>
<context>
    <name>RootCalculatorSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+79"/>
        <source>Calculator</source>
        <translation>计算器</translation>
    </message>
</context>
<context>
    <name>RootFallbackSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="+407"/>
        <source>Use &quot;%1&quot; with...</source>
        <translation>用“%1”打开…</translation>
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
        <translation>文件</translation>
    </message>
</context>
<context>
    <name>RootLinkSection</name>
    <message>
        <location line="-137"/>
        <source>Link</source>
        <translation>链接</translation>
    </message>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="-295"/>
        <source>Open in %1</source>
        <translation>在 %1 中打开</translation>
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
        <translation>建议</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Results (%1)</source>
        <translation>结果（%1）</translation>
    </message>
</context>
<context>
    <name>RootSearchActionGenerator</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+55"/>
        <source>Copy ID</source>
        <translation>复制 ID</translation>
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
        <translation>Vicinae %1 已发布</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>You are running %1</source>
        <translation>当前版本：%1</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>View Release Notes</source>
        <translation>查看发行说明</translation>
    </message>
</context>
<context>
    <name>RootViewHost</name>
    <message>
        <location filename="../src/qml/root-view-host.hpp" line="+15"/>
        <source>Search for anything...</source>
        <translation>搜索任何内容…</translation>
    </message>
</context>
<context>
    <name>ScriptExecutorViewHost</name>
    <message>
        <location filename="../src/qml/script-executor-view-host.cpp" line="+76"/>
        <source>Script execution failed: %1</source>
        <translation>脚本执行失败：%1</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Running... (%1s ago)</source>
        <translation>运行中…（%1 秒前）</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Done in %1s (exit=%2)</source>
        <translation>已完成，用时 %1 秒（退出码=%2）</translation>
    </message>
    <message>
        <location line="+12"/>
        <location line="+9"/>
        <source>Script process killed</source>
        <translation>脚本进程已终止</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Running...</source>
        <translation>运行中…</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Kill process</source>
        <translation>终止进程</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Run script again</source>
        <translation>再次运行脚本</translation>
    </message>
</context>
<context>
    <name>ScriptRootItem</name>
    <message>
        <location filename="../src/root-search/scripts/script-root-provider.hpp" line="+27"/>
        <location line="+86"/>
        <source>Script</source>
        <translation>脚本</translation>
    </message>
    <message>
        <location line="-43"/>
        <source>Mode</source>
        <translation>模式</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Path</source>
        <translation>路径</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Author</source>
        <translation>作者</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Open script directory</source>
        <translation>打开脚本目录</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy path to script</source>
        <translation>复制脚本路径</translation>
    </message>
</context>
<context>
    <name>ScriptRootProvider</name>
    <message>
        <location line="+47"/>
        <source>Script Commands</source>
        <translation>脚本命令</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Custom directories</source>
        <translation>自定义目录</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Additional list of directories to source scripts from. These directories always take precedence over the default system ones</source>
        <translation>用于加载脚本的其他目录。这些目录始终优先于系统默认目录</translation>
    </message>
</context>
<context>
    <name>SearchBrowserTabsCommand</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="+15"/>
        <source>Search Browser Tabs</source>
        <translation>搜索浏览器标签页</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search tabs from all connected browsers</source>
        <translation>搜索所有已连接浏览器中的标签页</translation>
    </message>
</context>
<context>
    <name>SearchEmojiCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/search-emoji-command.hpp" line="+15"/>
        <source>Search Emojis &amp; Symbols</source>
        <translation>搜索表情与符号</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search for any emoji or symbol</source>
        <translation>搜索任意表情或符号</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Paste</source>
        <translation>粘贴</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy</source>
        <translation>复制</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Default Action</source>
        <translation>默认操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to perform on pressing return. Paste is only available if your environment supports it.</source>
        <translation>按回车键时执行的默认操作。仅当当前环境支持时才能粘贴。</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Skin tone</source>
        <translation>肤色</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Skin tone to use for relevant emojis.</source>
        <translation>为适用的表情选择肤色。</translation>
    </message>
</context>
<context>
    <name>SearchEmojiGridSource</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.hpp" line="-22"/>
        <source>Results (%1)</source>
        <translation>结果（%1）</translation>
    </message>
</context>
<context>
    <name>SearchFilesCommand</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="-38"/>
        <source>Search Files</source>
        <translation>搜索文件</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search files on your system</source>
        <translation>搜索系统中的文件</translation>
    </message>
</context>
<context>
    <name>SearchFilesView</name>
    <message>
        <location filename="../src/qml/qml/SearchFilesView.qml" line="+37"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Path</source>
        <translation>路径</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>类型</translation>
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
        <translation>搜索文件…</translation>
    </message>
    <message>
        <location line="+30"/>
        <location line="+4"/>
        <source>Direct file path</source>
        <translation>直接输入文件路径</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Recently Accessed</source>
        <translation>最近访问</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Results</source>
        <translation>结果</translation>
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
        <translation>目录</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Images</source>
        <translation>图片</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Videos</source>
        <translation>视频</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Audio</source>
        <translation>音频</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Documents</source>
        <translation>文档</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Archives</source>
        <translation>压缩包</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Applications</source>
        <translation>应用程序</translation>
    </message>
</context>
<context>
    <name>SetAppFont</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="-15"/>
        <source>Set as vicinae font</source>
        <translation>设为 Vicinae 字体</translation>
    </message>
</context>
<context>
    <name>SetRootItemAliasAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="-18"/>
        <source>Set alias</source>
        <translation>设置别名</translation>
    </message>
</context>
<context>
    <name>SetThemeAction</name>
    <message>
        <location filename="../src/actions/theme/theme-actions.cpp" line="+11"/>
        <source>Theme successfully updated</source>
        <translation>主题已更新</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Set theme</source>
        <translation>设置主题</translation>
    </message>
</context>
<context>
    <name>SetThemeCommand</name>
    <message>
        <location filename="../src/extensions/theme/set-theme-command.hpp" line="+9"/>
        <source>Set Theme</source>
        <translation>设置主题</translation>
    </message>
</context>
<context>
    <name>SetVolumeCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="+213"/>
        <source>Set Volume to %1%</source>
        <translation>将音量设为 %1%</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Set system volume to %1%</source>
        <translation>将系统音量设为 %1%</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to set volume</source>
        <translation>设置音量失败</translation>
    </message>
</context>
<context>
    <name>SetWallpaperAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+17"/>
        <source>Set as wallpaper</source>
        <translation>设为壁纸</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Wallpaper set</source>
        <translation>壁纸已设置</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to set wallpaper</source>
        <translation>设置壁纸失败</translation>
    </message>
</context>
<context>
    <name>SettingsSidebar</name>
    <message>
        <location filename="../src/qml/qml/SettingsSidebar.qml" line="+99"/>
        <source>Search...</source>
        <translation>搜索…</translation>
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
        <translation>外观</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Keybindings</source>
        <translation>快捷键</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced</source>
        <translation>高级</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>About</source>
        <translation>关于</translation>
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
        <translation>外观</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keybindings</source>
        <translation>快捷键</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Advanced</source>
        <translation>高级</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>About</source>
        <translation>关于</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Vicinae Settings</source>
        <translation>Vicinae 设置</translation>
    </message>
    <message>
        <location line="+88"/>
        <source>Imported from Raycast</source>
        <translation>从 Raycast 导入</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>From the Vicinae store</source>
        <translation>来自 Vicinae Store</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Locally installed extension</source>
        <translation>本地安装的扩展</translation>
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
        <translation>录制快捷键</translation>
    </message>
</context>
<context>
    <name>ShortcutFormView</name>
    <message>
        <location filename="../src/qml/qml/ShortcutFormView.qml" line="+14"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Shortcut Name</source>
        <translation>快捷方式名称</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>The URL that will be opened by the specified app. You can make it dynamic by using placeholders such as {argument}.</source>
        <translation>由指定应用打开的 URL。可使用 {argument} 等占位符使其动态变化。</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Open with</source>
        <translation>打开方式</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Icon</source>
        <translation>图标</translation>
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
        <translation>编辑“%1”</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Duplicate &quot;%1&quot;</source>
        <translation>创建“%1”副本</translation>
    </message>
    <message>
        <location line="+11"/>
        <location line="+131"/>
        <location line="+50"/>
        <source>Default</source>
        <translation>默认</translation>
    </message>
    <message>
        <location line="-157"/>
        <source>Selected Text</source>
        <translation>所选文本</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clipboard Text</source>
        <translation>剪贴板文本</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Argument</source>
        <translation>参数</translation>
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
        <translation>验证失败</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Failed to update shortcut</source>
        <translation>更新快捷方式失败</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Shortcut updated</source>
        <translation>快捷方式已更新</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to create shortcut</source>
        <translation>创建快捷方式失败</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Shortcut created</source>
        <translation>快捷方式已创建</translation>
    </message>
</context>
<context>
    <name>ShortcutRecorderField</name>
    <message>
        <location filename="../src/qml/qml/ShortcutRecorderField.qml" line="+24"/>
        <location line="+22"/>
        <location line="+62"/>
        <source>Recording...</source>
        <translation>录制中…</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Keybind updated</source>
        <translation>快捷键已更新</translation>
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
        <translation>快捷键</translation>
    </message>
    <message>
        <location line="+78"/>
        <source>Record Shortcut</source>
        <translation>录制快捷键</translation>
    </message>
</context>
<context>
    <name>SkipUpdateVersionAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+8"/>
        <source>Skip This Version</source>
        <translation>跳过此版本</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Skipped %1</source>
        <translation>已跳过 %1</translation>
    </message>
</context>
<context>
    <name>SleepCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+94"/>
        <source>Put System to Sleep</source>
        <translation>使系统进入睡眠</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Put system to sleep</source>
        <translation>使系统进入睡眠</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>System can&apos;t sleep</source>
        <translation>系统无法进入睡眠</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to sleep</source>
        <translation>进入睡眠失败</translation>
    </message>
</context>
<context>
    <name>SnippetDatabase</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.cpp" line="+42"/>
        <location line="+56"/>
        <source>keyword already assigned to &quot;%1&quot;</source>
        <translation>关键词已分配给“%1”</translation>
    </message>
    <message>
        <location line="-43"/>
        <source>No snippet with that ID</source>
        <translation>没有具有该 ID 的片段</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>No such snippet</source>
        <translation>找不到该片段</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Snippet limit reached (%1)</source>
        <translation>已达到片段数量上限（%1）</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Failed to save snippets on disk: %1</source>
        <translation>无法将片段保存到磁盘：%1</translation>
    </message>
</context>
<context>
    <name>SnippetExtension</name>
    <message>
        <location filename="../src/extensions/snippet/snippet-extension.cpp" line="+38"/>
        <source>Expansion</source>
        <translation>展开</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Enable automatic snippet expansion when triggers are typed</source>
        <translation>输入触发词时自动展开片段</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Undo</source>
        <translation>撤销</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Press backspace immediately after expansion to undo and restore the trigger text</source>
        <translation>展开后立即按退格键可撤销并恢复触发文本</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Keyboard layout</source>
        <translation>键盘布局</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>XKB layout used for trigger detection (e.g. &quot;us&quot;, &quot;fr&quot;). Leave empty for system default.</source>
        <translation>用于检测触发词的 XKB 布局（如“us”“fr”）。留空则使用系统默认值。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Pre-paste delay (ms)</source>
        <translation>粘贴前延迟（ms）</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Delay between setting clipboard and injecting paste shortcut. Increase if expansions paste empty on slow compositors.</source>
        <translation>设置剪贴板与注入粘贴快捷键之间的延迟。如果在较慢的合成器上展开后粘贴为空，请增大此值。</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Key injection delay (ms)</source>
        <translation>按键注入延迟（ms）</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Delay between injected key events. Increase if expansions produce missing or garbled characters on slow compositors.</source>
        <translation>注入按键事件之间的延迟。如果在较慢的合成器上展开时缺字或乱码，请增大此值。</translation>
    </message>
    <message>
        <location filename="../src/extensions/snippet/snippet-extension.hpp" line="+11"/>
        <source>Snippets</source>
        <translation>片段</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Text expansion and snippet management</source>
        <translation>文本展开与片段管理</translation>
    </message>
</context>
<context>
    <name>SnippetFormView</name>
    <message>
        <location filename="../src/qml/qml/SnippetFormView.qml" line="+15"/>
        <source>Title</source>
        <translation>标题</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Euro symbol</source>
        <translation>欧元符号</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Content</source>
        <translation>内容</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You can use {dynamic placeholders} to make the content dynamic: &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;learn more&lt;/a&gt;.</source>
        <translation>可使用 {dynamic placeholders} 创建动态内容：&lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;了解详情&lt;/a&gt;。</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Keyword</source>
        <translation>关键词</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Typing this keyword anywhere will result in it being replaced by the content of the snippet.</source>
        <translation>在任意位置输入此关键词都会将其替换为片段内容。</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The snippet server is not running. Keyword expansion is unavailable. &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;Learn more&lt;/a&gt;.</source>
        <translation>片段服务器未运行，关键词展开不可用。&lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;了解详情&lt;/a&gt;。</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Applications</source>
        <translation>应用程序</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Restrict expansion to specific applications. By default, it works everywhere.</source>
        <translation>仅在指定应用中展开。默认在所有应用中生效。</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Expand as word</source>
        <translation>按单词展开</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>If a keyword is typed, it will only be expanded after space or punctuation.</source>
        <translation>输入关键词后，只有再输入空格或标点才会展开。</translation>
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
        <translation>编辑“%1”</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Duplicate &quot;%1&quot;</source>
        <translation>创建“%1”副本</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>2 chars min.</source>
        <translation>至少 2 个字符。</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Content should not be empty</source>
        <translation>内容不能为空</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Only one {cursor} placeholder is allowed</source>
        <translation>只能使用一个 {cursor} 占位符</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Validation failed</source>
        <translation>验证失败</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Snippet updated</source>
        <translation>片段已更新</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Snippet successfully created</source>
        <translation>片段创建成功</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Cursor Position</source>
        <translation>光标位置</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clipboard Text</source>
        <translation>剪贴板文本</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Date</source>
        <translation>日期</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Argument</source>
        <translation>参数</translation>
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
        <translation>软重启系统</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Soft reboot the system, which usually means only userspace is rebooted.</source>
        <translation>软重启系统，通常仅重启用户空间。</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System can&apos;t soft reboot</source>
        <translation>系统无法软重启</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to soft reboot</source>
        <translation>软重启失败</translation>
    </message>
</context>
<context>
    <name>SponsorVicinaeCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-121"/>
        <source>Donate to Vicinae</source>
        <translation>赞助 Vicinae</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open link to Vicinae&apos;s GitHub sponsor page</source>
        <translation>打开 Vicinae 的 GitHub 赞助页面</translation>
    </message>
</context>
<context>
    <name>StoreDetailView</name>
    <message>
        <location filename="../src/qml/qml/StoreDetailView.qml" line="+196"/>
        <source>Installed</source>
        <translation>已安装</translation>
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
        <translation>打开 README</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Last update</source>
        <translation>上次更新</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Contributors</source>
        <translation>贡献者</translation>
    </message>
    <message>
        <location line="+29"/>
        <source>Categories</source>
        <translation>分类</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Source Code</source>
        <translation>源代码</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>View Code</source>
        <translation>查看代码</translation>
    </message>
</context>
<context>
    <name>SuspendCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+36"/>
        <source>Suspend System</source>
        <translation>挂起系统</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Suspend the system to RAM. Unlike hibernation, this does not turn the computer off and will break on power loss.</source>
        <translation>将系统挂起到 RAM。与休眠不同，这不会关闭计算机，断电后状态会丢失。</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System cannot suspend</source>
        <translation>系统无法挂起</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to suspend</source>
        <translation>挂起失败</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsCommand</name>
    <message>
        <location filename="../src/extensions/wm/switch-windows-command.hpp" line="+10"/>
        <source>Switch Windows</source>
        <translation>切换窗口</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsSection</name>
    <message>
        <location filename="../src/qml/switch-windows-model.hpp" line="+28"/>
        <source>Open Windows</source>
        <translation>打开的窗口</translation>
    </message>
    <message>
        <location filename="../src/qml/switch-windows-model.cpp" line="+19"/>
        <source>WS %1</source>
        <translation>工作区 %1</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Window Actions</source>
        <translation>窗口操作</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsViewHost</name>
    <message>
        <location filename="../src/qml/switch-windows-view-host.cpp" line="+12"/>
        <source>Search open window...</source>
        <translation>搜索已打开的窗口…</translation>
    </message>
</context>
<context>
    <name>SystemBrowseApps</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-123"/>
        <source>Browse Apps</source>
        <translation>浏览应用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse all applications that are installed on the system</source>
        <translation>浏览系统中已安装的所有应用</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Show hidden apps</source>
        <translation>显示隐藏应用</translation>
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
        <translation>系统</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>System-related commands</source>
        <translation>系统相关命令</translation>
    </message>
</context>
<context>
    <name>SystemRunCommand</name>
    <message>
        <location line="-239"/>
        <source>Run Terminal Program</source>
        <translation>在终端中运行程序</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run a program in a terminal window</source>
        <translation>在终端窗口中运行程序</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>command</source>
        <translation>命令</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Run in terminal</source>
        <translation>在终端中运行</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run in terminal (hold)</source>
        <translation>在终端中运行（保持窗口）</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run directly</source>
        <translation>直接运行</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Default Action</source>
        <translation>默认操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to run on pressing return</source>
        <translation>按回车时运行的默认操作</translation>
    </message>
    <message>
        <location line="+29"/>
        <source>Not a valid executable</source>
        <translation>不是有效的可执行文件</translation>
    </message>
</context>
<context>
    <name>SystemRunViewHost</name>
    <message>
        <location filename="../src/qml/system-run-view-host.cpp" line="+20"/>
        <source>Search for a program to execute...</source>
        <translation>搜索要执行的程序…</translation>
    </message>
</context>
<context>
    <name>ThemeExtension</name>
    <message>
        <location filename="../src/extensions/theme/theme-extension.hpp" line="+9"/>
        <source>Theme</source>
        <translation>主题</translation>
    </message>
</context>
<context>
    <name>ThemeSection</name>
    <message>
        <location filename="../src/qml/theme-list-model.cpp" line="+22"/>
        <source>Default theme description</source>
        <translation>默认主题说明</translation>
    </message>
    <message>
        <location line="+64"/>
        <source>Open theme file</source>
        <translation>打开主题文件</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy ID</source>
        <translation>复制 ID</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy path</source>
        <translation>复制路径</translation>
    </message>
</context>
<context>
    <name>ThemeViewHost</name>
    <message>
        <location filename="../src/qml/theme-view-host.cpp" line="+22"/>
        <source>Search for a theme...</source>
        <translation>搜索主题…</translation>
    </message>
    <message>
        <location line="+51"/>
        <source>Current Theme</source>
        <translation>当前主题</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Available Themes</source>
        <translation>可用主题</translation>
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
        <translation>添加到收藏</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Successfuly added to favorites</source>
        <translation>已添加到收藏</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Successfuly removed from favorites</source>
        <translation>已取消收藏</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to add to favorites</source>
        <translation>添加到收藏失败</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove from favorites</source>
        <translation>取消收藏失败</translation>
    </message>
</context>
<context>
    <name>ToggleMuteCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="+163"/>
        <source>Toggle Mute</source>
        <translation>切换静音</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mute or unmute system audio</source>
        <translation>将系统音频静音或取消静音</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Failed to toggle mute</source>
        <translation>切换静音失败</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Muted</source>
        <translation>已静音</translation>
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
        <translation>按钮</translation>
    </message>
</context>
<context>
    <name>UninstallExtensionAction</name>
    <message>
        <location filename="../src/actions/extension/extension-actions.cpp" line="+11"/>
        <source>Are you sure?</source>
        <translation>确定吗？</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>All this extension data will be permanently lost. If you just want the extension to not appear in the root search anymore, consider disabling it instead.</source>
        <translation>此扩展的所有数据都将永久丢失。如果只是不想让它出现在根搜索中，可考虑将其禁用。</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Uninstall</source>
        <translation>卸载</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Extension uninstalled</source>
        <translation>扩展已卸载</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to uninstall extension</source>
        <translation>卸载扩展失败</translation>
    </message>
    <message>
        <location filename="../src/actions/extension/extension-actions.hpp" line="+14"/>
        <source>Uninstall Extension</source>
        <translation>卸载扩展</translation>
    </message>
</context>
<context>
    <name>UnpinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-23"/>
        <source>Entry unpinned</source>
        <translation>已取消固定条目</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Unpin entry</source>
        <translation>取消固定条目</translation>
    </message>
</context>
<context>
    <name>UnpinEmojiAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-43"/>
        <source>Unpin emoji</source>
        <translation>取消固定表情符号</translation>
    </message>
</context>
<context>
    <name>UpdateService</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="-167"/>
        <source>Update installed</source>
        <translation>更新已安装</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Restarting…</source>
        <translation>正在重启…</translation>
    </message>
    <message>
        <location line="+88"/>
        <source>Downloading Vicinae %1…</source>
        <translation>正在下载 Vicinae %1…</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading Vicinae %1… %2%</source>
        <translation>正在下载 Vicinae %1… %2%</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Installing update…</source>
        <translation>正在安装更新…</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Update failed</source>
        <translation>更新失败</translation>
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
        <translation>不支持的触发键</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Hotkey binding was lost</source>
        <translation>热键绑定已丢失</translation>
    </message>
</context>
<context>
    <name>VicinaeListInstalledExtensionsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/list-installed-extensions-command.hpp" line="+11"/>
        <source>Show Installed Extensions</source>
        <translation>显示已安装的扩展</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Show all third-party extensions that have been installed. This includes local extensions as well as extensions downloaded from the stores (vicinae and raycast).</source>
        <translation>显示所有已安装的第三方扩展，包括本地扩展以及从 Vicinae 和 Raycast Store 下载的扩展。</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-store-command.hpp" line="+13"/>
        <source>Install extensions from the Vicinae store</source>
        <translation>从 Vicinae Store 安装扩展</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Always show intro</source>
        <translation>始终显示介绍</translation>
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
# 欢迎来到 Vicinae Store

Vicinae Store 收录了由社区开发并经核心贡献者批准的扩展。

这里列出的每个扩展都可在 [vicinaehq/extensions](https://github.com/vicinaehq/extensions) 仓库中查看源代码。

如果你想开发自己的扩展，请查看[文档](https://docs.vicinae.com/extensions/introduction)。如果你认为自己的扩展适合上架，欢迎提交！
</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Continue to store</source>
        <translation>继续前往商店</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreDetailHost</name>
    <message>
        <location filename="../src/qml/vicinae-store-detail-host.cpp" line="+38"/>
        <source>Failed to load extension</source>
        <translation>加载扩展失败</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Could not fetch extension data from the store.</source>
        <translation>无法从商店获取扩展数据。</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Extension not found</source>
        <translation>找不到扩展</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The extension &quot;%1&quot; could not be found in the store.</source>
        <translation>在商店中找不到扩展“%1”。</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Extension Store - %1</source>
        <translation>Extension Store - %1</translation>
    </message>
    <message>
        <location line="+81"/>
        <source>Install extension</source>
        <translation>安装扩展</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading extension...</source>
        <translation>正在下载扩展…</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to download extension</source>
        <translation>下载扩展失败</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to extract extension archive</source>
        <translation>解压扩展包失败</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Extension installed</source>
        <translation>扩展已安装</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Report issue</source>
        <translation>报告问题</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreSection</name>
    <message>
        <location filename="../src/qml/vicinae-store-model.cpp" line="+41"/>
        <source>Show details</source>
        <translation>显示详情</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreViewHost</name>
    <message>
        <location filename="../src/qml/vicinae-store-view-host.cpp" line="+27"/>
        <source>Browse Vicinae extensions</source>
        <translation>浏览 Vicinae 扩展</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Failed to fetch extensions</source>
        <translation>获取扩展失败</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Extensions</source>
        <translation>扩展</translation>
    </message>
</context>
<context>
    <name>VolumeDownCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-74"/>
        <source>Turn Volume Down</source>
        <translation>调低音量</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Decrease system volume</source>
        <translation>降低系统音量</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid step value</source>
        <translation>无效的步长值</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to adjust volume</source>
        <translation>调节音量失败</translation>
    </message>
</context>
<context>
    <name>VolumeUpCommand</name>
    <message>
        <location line="-61"/>
        <source>Turn Volume Up</source>
        <translation>调高音量</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Increase system volume</source>
        <translation>提高系统音量</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid step value</source>
        <translation>无效的步长值</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to adjust volume</source>
        <translation>调节音量失败</translation>
    </message>
</context>
<context>
    <name>WallpaperManager</name>
    <message>
        <location filename="../src/services/wallpaper/wallpaper-manager.cpp" line="+68"/>
        <source>Setting the wallpaper is not supported in the current environment</source>
        <translation>当前环境不支持设置壁纸</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No such file: %1</source>
        <translation>文件不存在：%1</translation>
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
        <translation>名称</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Where</source>
        <translation>位置</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Open Applet</source>
        <translation>打开控制面板项</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Path</source>
        <translation>复制路径</translation>
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
        <translation>控制面板小程序和系统任务。</translation>
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
        <translation>名称</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Task ID</source>
        <translation>任务 ID</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Open</source>
        <translation>打开</translation>
    </message>
</context>
<context>
    <name>WinSettingsPage</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="-114"/>
        <source>Display</source>
        <translation>屏幕</translation>
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
        <translation>系统</translation>
    </message>
    <message>
        <location line="-17"/>
        <source>Night Light</source>
        <translation>夜间模式</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sound</source>
        <translation>声音</translation>
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
        <translation>专注</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power &amp; Battery</source>
        <translation>电源和电池</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Storage</source>
        <translation>存储</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Nearby Sharing</source>
        <translation>就近共享</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Multitasking</source>
        <translation>多任务处理</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activation</source>
        <translation>激活</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Troubleshoot</source>
        <translation>疑难解答</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Recovery</source>
        <translation>恢复</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Projecting to This PC</source>
        <translation>投影到此电脑</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Remote Desktop</source>
        <translation>远程桌面</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clipboard</source>
        <translation>剪贴板</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>About</source>
        <translation>关于</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Optional Features</source>
        <translation>可选功能</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>For Developers</source>
        <translation>开发者选项</translation>
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
        <translation>蓝牙和设备</translation>
    </message>
    <message>
        <location line="-8"/>
        <source>Devices</source>
        <translation>设备</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Printers &amp; Scanners</source>
        <translation>打印机和扫描仪</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mobile Devices</source>
        <translation>移动设备</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Cameras</source>
        <translation>相机</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mouse</source>
        <translation>鼠标</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Touchpad</source>
        <translation>触摸板</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pen &amp; Windows Ink</source>
        <translation>笔和 Windows Ink</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>AutoPlay</source>
        <translation>自动播放</translation>
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
        <translation>网络和 Internet</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Wi-Fi</source>
        <translation>Wi-Fi</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ethernet</source>
        <translation>以太网</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Mobile Hotspot</source>
        <translation>移动热点</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Airplane Mode</source>
        <translation>飞行模式</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Proxy</source>
        <translation>代理</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dial-up</source>
        <translation>拨号</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced Network Settings</source>
        <translation>高级网络设置</translation>
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
        <translation>个性化</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Colors</source>
        <translation>颜色</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Themes</source>
        <translation>主题</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lock Screen</source>
        <translation>锁屏界面</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Touch Keyboard</source>
        <translation>触摸键盘</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Start</source>
        <translation>开始</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Taskbar</source>
        <translation>任务栏</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Fonts</source>
        <translation>字体</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dynamic Lighting</source>
        <translation>动态光效</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Installed Apps</source>
        <translation>安装的应用</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Apps</source>
        <translation>应用程序</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>Default Apps</source>
        <translation>默认应用</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Offline Maps</source>
        <translation>脱机地图</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Apps for Websites</source>
        <translation>网站应用</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Video Playback</source>
        <translation>视频播放</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Startup Apps</source>
        <translation>启动应用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Your Info</source>
        <translation>你的信息</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Accounts</source>
        <translation>账户</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>Email &amp; Accounts</source>
        <translation>电子邮件和账户</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sign-in Options</source>
        <translation>登录选项</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Access Work or School</source>
        <translation>访问工作或学校</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Family &amp; Other Users</source>
        <translation>家庭和其他用户</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Windows Backup</source>
        <translation>Windows 备份</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Date &amp; Time</source>
        <translation>日期和时间</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Time &amp; Language</source>
        <translation>时间和语言</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Language &amp; Region</source>
        <translation>语言和区域</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Typing</source>
        <translation>输入</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Speech</source>
        <translation>语音</translation>
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
        <translation>游戏</translation>
    </message>
    <message>
        <location line="-1"/>
        <source>Captures</source>
        <translation>捕获</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Game Mode</source>
        <translation>游戏模式</translation>
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
        <translation>辅助功能</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Visual Effects</source>
        <translation>视觉效果</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Magnifier</source>
        <translation>放大镜</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Color Filters</source>
        <translation>颜色筛选器</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Contrast Themes</source>
        <translation>对比度主题</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Narrator</source>
        <translation>讲述人</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Audio</source>
        <translation>音频</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Captions</source>
        <translation>字幕</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Keyboard</source>
        <translation>键盘</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Mouse</source>
        <translation>鼠标</translation>
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
        <translation>隐私和安全性</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Find My Device</source>
        <translation>查找我的设备</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Privacy</source>
        <translation>隐私</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Location</source>
        <translation>位置</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Camera Access</source>
        <translation>相机访问权限</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Microphone Access</source>
        <translation>麦克风访问权限</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activity History</source>
        <translation>活动历史记录</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Diagnostics &amp; Feedback</source>
        <translation>诊断和反馈</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search Permissions</source>
        <translation>搜索权限</translation>
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
        <translation>更新历史记录</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced Update Options</source>
        <translation>高级选项</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Windows Insider Program</source>
        <translation>Windows 预览体验计划</translation>
    </message>
</context>
<context>
    <name>WinSettingsPageRootItem</name>
    <message>
        <location line="+37"/>
        <source>System Settings</source>
        <translation>系统设置</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Settings</source>
        <translation>设置</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Category</source>
        <translation>类别</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Open %1 Settings</source>
        <translation>打开“%1”设置</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy URL</source>
        <translation>复制 URL</translation>
    </message>
</context>
<context>
    <name>WinSettingsRootProvider</name>
    <message>
        <location line="+12"/>
        <source>Windows Settings</source>
        <translation>Windows 设置</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Pages of the Windows Settings app.</source>
        <translation>Windows 设置应用中的页面。</translation>
    </message>
</context>
<context>
    <name>WindowManagementExtension</name>
    <message>
        <location filename="../src/extensions/wm/wm-extension.hpp" line="+12"/>
        <source>Window Management</source>
        <translation>窗口管理</translation>
    </message>
</context>
<context>
    <name>WindowsAppDatabase</name>
    <message>
        <location filename="../src/services/app-service/windows/win-app-database.cpp" line="+993"/>
        <source>Focus window</source>
        <translation>聚焦窗口</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Launch app</source>
        <translation>启动应用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Default action</source>
        <translation>默认操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Action to perform when the return key is pressed. Always default to &apos;launch&apos; if the app has no open window.</source>
        <translation>按回车时执行的操作。如果应用没有打开的窗口，则始终默认为“启动”。</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>File Explorer</source>
        <translation>文件资源管理器</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Terminal</source>
        <translation>终端</translation>
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
        <translation>%1：以管理员身份运行</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run as Administrator</source>
        <translation>以管理员身份运行</translation>
    </message>
</context>
<context>
    <name>WindowsGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/windows-global-shortcut-backend.cpp" line="+215"/>
        <source>unsupported or invalid trigger</source>
        <translation>不支持或无效的触发键</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>already registered by another application</source>
        <translation>已被其他应用注册</translation>
    </message>
</context>
<context>
    <name>X11GlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/x11-global-shortcut-backend.cpp" line="+122"/>
        <source>This shortcut is already in use by another application</source>
        <translation>此快捷键已被其他应用占用</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Unsupported trigger key</source>
        <translation>不支持的触发键</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Trigger key is not present on this keyboard</source>
        <translation>此键盘上没有该触发键</translation>
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
        <translation>聚焦窗口</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Launch app</source>
        <translation>启动应用</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Default action</source>
        <translation>默认操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Action to perform when the return key is pressed. Always default to &apos;launch&apos; if the app has no open window.</source>
        <translation>按回车时执行的操作。如果应用没有打开的窗口，则始终默认为“启动”。</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Launch Prefix</source>
        <translation>启动前缀</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Custom app launcher to use. Affects applications as well as their sub-actions.</source>
        <translation>要使用的自定义应用启动器，会影响应用及其子操作。</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Application directories</source>
        <translation>应用目录</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Directories applications are sourced from. The list cannot be modified directly. In order to do so, you need to append additonal paths to the &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt; environment variables.</source>
        <translation>用于查找应用的目录。此列表无法直接修改；如需添加目录，请将其他路径追加到 &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt; 环境变量。</translation>
    </message>
</context>
<context>
    <name>XdpFileChooser</name>
    <message>
        <location filename="../src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.cpp" line="+39"/>
        <source>Open Directory</source>
        <translation>打开目录</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Open File</source>
        <translation>打开文件</translation>
    </message>
</context>
<context>
    <name>browser-extension</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="-58"/>
        <source>No browser connected</source>
        <translation>未连接浏览器</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You need to connect at least one browser to vicinae using the browser extension in order to use this command.</source>
        <translation>需要通过浏览器扩展将至少一个浏览器连接到 Vicinae，才能使用此命令。</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Open documentation</source>
        <translation>打开文档</translation>
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
        <translation>链接</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Image</source>
        <translation>图片</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>File</source>
        <translation>文件</translation>
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
        <translation>笑脸与情感</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>People &amp; Body</source>
        <translation>人物与身体</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Animals &amp; Nature</source>
        <translation>动物与自然</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Food &amp; Drink</source>
        <translation>食物与饮品</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Travel &amp; Places</source>
        <translation>旅行与地点</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activities</source>
        <translation>活动</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Objects</source>
        <translation>物品</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Symbols</source>
        <translation>符号</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Flags</source>
        <translation>旗帜</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Math</source>
        <translation>数学</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Arrows</source>
        <translation>箭头</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Currency</source>
        <translation>货币</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Punctuation</source>
        <translation>标点</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Shapes</source>
        <translation>形状</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Misc Symbols</source>
        <translation>其他符号</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Greek</source>
        <translation>希腊文</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Number Forms</source>
        <translation>数字形式</translation>
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
        <translation>复制</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy name</source>
        <translation>复制名称</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy unicode codepoint</source>
        <translation>复制 Unicode 码位</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy category</source>
        <translation>复制分类</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>Skin tones</source>
        <translation>肤色</translation>
    </message>
</context>
<context>
    <name>file-list-item</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+35"/>
        <source>Copy file</source>
        <translation>复制文件</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy file path</source>
        <translation>复制文件路径</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy file name</source>
        <translation>复制文件名</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Copy mime type</source>
        <translation>复制 MIME 类型</translation>
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
        <translation>西里尔文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Greek</source>
        <translation>希腊文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Monospace</source>
        <translation>等宽</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Emoji</source>
        <translation>表情符号</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Japanese</source>
        <translation>日文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Korean</source>
        <translation>韩文</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Simplified Chinese</source>
        <translation>简体中文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Traditional Chinese</source>
        <translation>繁体中文</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Arabic</source>
        <translation>阿拉伯文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Hebrew</source>
        <translation>希伯来文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Thai</source>
        <translation>泰文</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lao</source>
        <translation>老挝文</translation>
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
        <translation>泰米尔文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Telugu</source>
        <translation>泰卢固文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Kannada</source>
        <translation>卡纳达文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Malayalam</source>
        <translation>马拉雅拉姆文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Sinhala</source>
        <translation>僧伽罗文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Armenian</source>
        <translation>亚美尼亚文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Georgian</source>
        <translation>格鲁吉亚文</translation>
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
        <translation>缅甸文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Khmer</source>
        <translation>高棉文</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Syriac</source>
        <translation>叙利亚文</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ogham</source>
        <translation>欧甘文</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Runic</source>
        <translation>卢恩文</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>N&apos;Ko</source>
        <translation>西非书面文字</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Symbols</source>
        <translation>符号</translation>
    </message>
</context>
<context>
    <name>font-grid-model</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="+26"/>
        <source>Copy font family</source>
        <translation>复制字体族</translation>
    </message>
</context>
<context>
    <name>keybind-manager</name>
    <message>
        <location filename="../src/internal/keyboard/keybind-manager.cpp" line="+9"/>
        <source>Toggle action panel</source>
        <translation>切换操作面板</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Toggle the action panel to access and filter through the list of available actions for the currently selected item</source>
        <translation>切换操作面板，以查看并筛选当前所选项目的可用操作</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Open Search Filter</source>
        <translation>打开搜索筛选器</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the search filter selector if present</source>
        <translation>打开搜索筛选器选择器（如果有）</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Open settings window</source>
        <translation>打开设置窗口</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open this settings window from the launcher window</source>
        <translation>从启动器窗口打开此设置窗口</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Open Action</source>
        <translation>通用打开操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can open the selected item</source>
        <translation>供可打开所选项目的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Copy Action</source>
        <translation>通用复制操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the selected item</source>
        <translation>供可复制所选项目的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Name Action</source>
        <translation>复制名称操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the name of the selected item</source>
        <translation>供可复制所选项目名称的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Path Action</source>
        <translation>复制路径操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the path of the selected item</source>
        <translation>供可复制所选项目路径的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Save Action</source>
        <translation>保存操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can save the selected item</source>
        <translation>供可保存所选项目的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Duplicate Action</source>
        <translation>创建副本操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can duplicate the selected item</source>
        <translation>供可创建所选项目副本的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic New Action</source>
        <translation>通用新建操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that create something</source>
        <translation>供创建内容的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Move Up Action</source>
        <translation>通用上移操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can move up the selected item. This does not affect list navigation controls.</source>
        <translation>供可上移所选项目的操作使用，不影响列表导航控制。</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Move Down Action</source>
        <translation>通用下移操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can move down the selected item. This does not affect list navigation controls.</source>
        <translation>供可下移所选项目的操作使用，不影响列表导航控制。</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Refresh Action</source>
        <translation>通用刷新操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can refresh the selected item</source>
        <translation>供可刷新所选项目的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Pin Action</source>
        <translation>通用固定操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can pin the selected item</source>
        <translation>供可固定所选项目的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Remove Action</source>
        <translation>移除操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can remove the selected item. This is normally used for small, not too impactful removals.</source>
        <translation>供可移除所选项目的操作使用，通常用于影响较小的移除。</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Dangerous Remove Action</source>
        <translation>危险移除操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that perform an impactful removal, generally accompanied by a confirmation dialog.</source>
        <translation>供影响较大的移除操作使用，通常会显示确认对话框。</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit Action</source>
        <translation>编辑操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can edit the currently selected item</source>
        <translation>供可编辑当前所选项目的操作使用</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit Secondary Action</source>
        <translation>编辑次要属性操作</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can edit a secondary characteristic of the currently selected item</source>
        <translation>供可编辑当前所选项目次要属性的操作使用</translation>
    </message>
</context>
<context>
    <name>macos-update-installer</name>
    <message>
        <location filename="../src/services/update/macos-update-installer.mm" line="-184"/>
        <source>Update image contains more than one app</source>
        <translation>更新映像包含多个应用</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to list update image: %1</source>
        <translation>无法列出更新映像内容：%1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No app found in update image</source>
        <translation>更新映像中未找到应用</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to read the update&apos;s code signature</source>
        <translation>无法读取更新的代码签名</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to build the signature requirement</source>
        <translation>无法构建签名要求</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Update signature verification failed (%1)</source>
        <translation>更新签名验证失败（%1）</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Update has no CFBundleShortVersionString</source>
        <translation>更新没有 CFBundleShortVersionString</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Update version mismatch: expected %1, found %2</source>
        <translation>更新版本不匹配：应为 %1，实际为 %2</translation>
    </message>
</context>
<context>
    <name>shortcut-conflict</name>
    <message>
        <location filename="../src/qml/shortcut-conflict.cpp" line="+10"/>
        <source>Modifier required</source>
        <translation>需要修饰键</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+5"/>
        <source>Already bound to &quot;%1&quot;</source>
        <translation>已绑定到“%1”</translation>
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
        <translation>0 字节</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>bytes</source>
        <translation>字节</translation>
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
