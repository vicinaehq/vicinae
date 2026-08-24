<?xml version='1.0' encoding='utf-8'?>
<TS version="2.1" language="ru">
<context>
    <name>AboutSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AboutSettingsPage.qml" line="+59" />
        <source>Version %1 - Commit %2
(%3)</source>
        <translation>Версия %1 - Коммит %2
(%3)</translation>
    </message>
    <message>
        <location line="+24" />
        <source>Documentation</source>
        <translation>Документация</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Report a Bug</source>
        <translation>Сообщить об ошибке</translation>
    </message>
</context>
<context>
    <name>ActionListPanel</name>
    <message>
        <location filename="../src/qml/qml/ActionListPanel.qml" line="+113" />
        <source>No matching actions</source>
        <translation>Нет подходящих действий</translation>
    </message>
    <message>
        <location line="+135" />
        <source>Filter actions...</source>
        <translation>Фильтровать действия...</translation>
    </message>
</context>
<context>
    <name>AdvancedSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AdvancedSettingsPage.qml" line="+33" />
        <source>Input &amp; Navigation</source>
        <translation>Ввод и навигация</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Pop on backspace</source>
        <translation>Назад по Backspace</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Pop back in navigation on backspace when no input is present.</source>
        <translation>Возвращаться на шаг назад по клавише Backspace, когда поле ввода пустое.</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Activate on single click</source>
        <translation>Активировать одним кликом</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Activate items with a single click instead of requiring a double click.</source>
        <translation>Открывать элементы одним кликом вместо двойного клика.</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Wrap navigation</source>
        <translation>Циклическая навигация</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Wrap around to the opposite end when moving past the first or last item.</source>
        <translation>Переходить к противоположному концу при перемещении за первый или последний элемент.</translation>
    </message>
    <message>
        <location line="+8" />
        <source>IME handling</source>
        <translation>Обработка IME</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Include IME Preedit strings as part of search queries.</source>
        <translation>Включать строки прередуктирования IME в поисковые запросы.</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Keybinding Scheme</source>
        <translation>Схема горячих клавиш</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Default uses the standard macOS keys (arrows, Ctrl+N/P); Vim uses Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Opt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>По умолчанию используются стандартные клавиши macOS (стрелки, Ctrl+N/P); Vim — Ctrl+J/K и Ctrl+H/L; Emacs — Ctrl+N/P и Ctrl+Opt+B/F для навигации, а также редактирование в стиле Emacs в строке поиска.</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Default and Vim use Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Alt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>По умолчанию и в Vim используются Ctrl+J/K и Ctrl+H/L; в Emacs — Ctrl+N/P и Ctrl+Alt+B/F для навигации, а также редактирование в стиле Emacs в строке поиска.</translation>
    </message>
    <message>
        <location line="+12" />
        <source>Search</source>
        <translation>Поиск</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Root file search</source>
        <translation>Поиск файлов в корневом уровне</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Files are searched asynchronously, so if enabled you should expect a slight delay for file search results to show up.</source>
        <translation>Файлы ищутся асинхронно, поэтому при включении возможна небольшая задержка перед появлением результатов поиска файлов.</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Favicon Fetching</source>
        <translation>Загрузка favicon</translation>
    </message>
    <message>
        <location line="+1" />
        <source>The favicon provider used to load favicons where needed. Select 'None' to turn off favicon loading.</source>
        <translation>Провайдер favicon, используемый для их загрузки при необходимости. Выберите «None», чтобы отключить загрузку favicon.</translation>
    </message>
    <message>
        <location line="+12" />
        <source>System</source>
        <translation>Система</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Input server</source>
        <translation>Сервер ввода</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Whether to spawn the input server at startup. This needs to be enabled in order to support snippets, paste to active window, and other features that require input monitoring or injection.</source>
        <translation>Запускать ли сервер ввода при старте. Это необходимо для работы сниппетов, вставки в активное окно и других функций, требующих мониторинга или перехвата ввода.</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Security</source>
        <translation>Безопасность</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Encrypt sensitive data</source>
        <translation>Шифровать конфиденциальные данные</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Encrypt sensitive data at rest, such as clipboard history and internal databases (OAuth tokens, extension local storage, API keys). Note that some components, such as on-disk clipboard history, may not be retroactively affected when toggling this option. Turning on this option may ask you to unlock your keychain. Requires a restart in order to apply.</source>
        <translation>Шифровать хранящиеся конфиденциальные данные, например историю буфера обмена и внутренние базы данных (OAuth-токены, локальное хранилище расширений, API-ключи). Учтите, что некоторые компоненты, такие как история буфера обмена на диске, могут не затронуться при переключении этой опции. При включении может потребоваться разблокировать вашу связку ключей. Для применения требуется перезапуск.</translation>
    </message>
</context>
<context>
    <name>AlertWidget</name>
    <message>
        <location filename="../src/ui/alert/alert.hpp" line="+15" />
        <source>Are you sure?</source>
        <translation>Вы уверены?</translation>
    </message>
    <message>
        <location line="+1" />
        <source>This action cannot be undone</source>
        <translation>Это действие нельзя отменить</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Confirm</source>
        <translation>Подтвердить</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Cancel</source>
        <translation>Отмена</translation>
    </message>
</context>
<context>
    <name>AliasFormView</name>
    <message>
        <location filename="../src/qml/qml/AliasFormView.qml" line="+15" />
        <source>Alias</source>
        <translation>Алиас</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Additional words to index this item against</source>
        <translation>Дополнительные слова для индексации этого элемента</translation>
    </message>
</context>
<context>
    <name>AliasFormViewHost</name>
    <message>
        <location filename="../src/qml/alias-form-view-host.cpp" line="+28" />
        <source>Set alias - %1</source>
        <translation>Задать алиас - %1</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Submit</source>
        <translation>Отправить</translation>
    </message>
    <message>
        <location line="+13" />
        <source>Alias modified</source>
        <translation>Алиас изменён</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Failed to modify alias</source>
        <translation>Не удалось изменить алиас</translation>
    </message>
</context>
<context>
    <name>AppRootItem</name>
    <message>
        <location filename="../src/root-search/apps/app-root-provider.cpp" line="+18" />
        <location line="+28" />
        <source>Application</source>
        <translation>Приложение</translation>
    </message>
    <message>
        <location line="-9" />
        <source>Name</source>
        <translation>Название</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Where</source>
        <translation>Расположение</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Opens in terminal</source>
        <translation>Открывается в терминале</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Yes</source>
        <translation>Да</translation>
    </message>
    <message>
        <location line="+0" />
        <source>No</source>
        <translation>Нет</translation>
    </message>
    <message>
        <location line="+20" />
        <source>Open Application</source>
        <translation>Открыть приложение</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Copy App ID</source>
        <translation>Копировать ID приложения</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Copy App Location</source>
        <translation>Копировать путь приложения</translation>
    </message>
</context>
<context>
    <name>AppRootProvider</name>
    <message>
        <location line="+77" />
        <source>Applications</source>
        <translation>Приложения</translation>
    </message>
</context>
<context>
    <name>AppSelectorModel</name>
    <message>
        <location filename="../src/qml/app-selector-model.cpp" line="+17" />
        <location line="+44" />
        <source>%1 (Default)</source>
        <translation>%1 (По умолчанию)</translation>
    </message>
</context>
<context>
    <name>AppearanceSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AppearanceSettingsPage.qml" line="+33" />
        <location line="+7" />
        <source>Theme</source>
        <translation>Тема</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Font</source>
        <translation>Шрифт</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Font size</source>
        <translation>Размер шрифта</translation>
    </message>
    <message>
        <location line="+1" />
        <source>The base point size used to compute font sizes. Fractional values are accepted. Recommended range is [10.0;12.0].</source>
        <translation>Базовый кегль, на основе которого вычисляются размеры шрифтов. Допускаются дробные значения. Рекомендуемый диапазон — [10.0;12.0].</translation>
    </message>
    <message>
        <location line="+6" />
        <source>e.g. 11</source>
        <translation>например, 11</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Icon Theme</source>
        <translation>Тема значков</translation>
    </message>
    <message>
        <location line="+1" />
        <source>The icon theme used for system icons (applications, mime types, folder icons...). Does not affect builtin Vicinae icons.</source>
        <translation>Тема значков для системных иконок (приложения, типы MIME, папки...). Не влияет на встроенные значки Vicinae.</translation>
    </message>
    <message>
        <location line="+12" />
        <source>Window</source>
        <translation>Окно</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Window material</source>
        <translation>Материал окна</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Background material applied to the launcher window. Lower the window opacity to see it.</source>
        <translation>Материал фона, применяемый к окну лаунчера. Уменьшите прозрачность окна, чтобы увидеть его.</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Window opacity</source>
        <translation>Прозрачность окна</translation>
    </message>
    <message>
        <location line="+5" />
        <source>e.g. 1.0</source>
        <translation>например, 1.0</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Compact mode</source>
        <translation>Компактный режим</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Show only the search bar at root; expand when a query is entered.</source>
        <translation>Показывать только строку поиска в корневом уровне; раскрываться при вводе запроса.</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Use layer shell</source>
        <translation>Использовать layer shell</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Anchor the launcher as a Wayland layer surface (wlr-layer-shell) instead of a regular window. May require reopening Vicinae to fully apply.</source>
        <translation>Закрепить лаунчер как поверхность слоя Wayland (wlr-layer-shell) вместо обычного окна. Для полного применения может потребоваться перезапуск Vicinae.</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Client-side decorations</source>
        <translation>Декорации на стороне клиента</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Let Vicinae draw its own rounded borders and shadow instead of relying on the windowing system.</source>
        <translation>Разрешить Vicinae рисовать собственные скруглённые рамки и тень, а не полагаться на оконную систему.</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Corner rounding</source>
        <translation>Скругление углов</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Radius of the launcher window corners, in pixels.</source>
        <translation>Радиус скругления углов окна лаунчера в пикселях.</translation>
    </message>
    <message>
        <location line="+8" />
        <source>e.g. 10</source>
        <translation>например, 10</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Border width</source>
        <translation>Ширина рамки</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Thickness of the launcher window border, in pixels.</source>
        <translation>Толщина рамки окна лаунчера в пикселях.</translation>
    </message>
    <message>
        <location line="+7" />
        <source>e.g. 3</source>
        <translation>например, 3</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Shadow size</source>
        <translation>Размер тени</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Size of the drop shadow cast by the launcher window, in pixels.</source>
        <translation>Размер падающей тени окна лаунчера в пикселях.</translation>
    </message>
    <message>
        <location line="+8" />
        <source>e.g. 12</source>
        <translation>например, 12</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Native font rendering</source>
        <translation>Системный рендеринг шрифта</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Use the platform's native text rendering for system-consistent text. Disable for Qt distance-field rendering (usually faster). May require reopening Vicinae to fully apply.</source>
        <translation>Использовать системный рендеринг текста платформы для единообразного отображения. Отключите, чтобы использовать Qt distance-field-рендеринг (обычно быстрее). Для полного применения может потребоваться перезапуск Vicinae.</translation>
    </message>
</context>
<context>
    <name>AvailableFallbackSection</name>
    <message>
        <location filename="../src/qml/manage-fallback-model.hpp" line="+49" />
        <source>Available</source>
        <translation>Доступно</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-fallback-model.cpp" line="+50" />
        <source>Enable fallback</source>
        <translation>Включить резервный вариант</translation>
    </message>
</context>
<context>
    <name>BringToWorkspaceAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="+72" />
        <source>Bring to current workspace</source>
        <translation>Перенести в текущее рабочее пространство</translation>
    </message>
</context>
<context>
    <name>BrowseAppsSection</name>
    <message>
        <location filename="../src/qml/browse-apps-model.hpp" line="+32" />
        <source>Applications ({count})</source>
        <translation>Приложения ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/browse-apps-model.cpp" line="+19" />
        <source>Hidden</source>
        <translation>Скрытые</translation>
    </message>
    <message>
        <location line="+16" />
        <source>Open Application</source>
        <translation>Открыть приложение</translation>
    </message>
    <message>
        <location line="+17" />
        <source>Copy App ID</source>
        <translation>Копировать ID приложения</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Copy App Location</source>
        <translation>Копировать путь приложения</translation>
    </message>
</context>
<context>
    <name>BrowseAppsViewHost</name>
    <message>
        <location filename="../src/qml/browse-apps-view-host.cpp" line="+12" />
        <source>Search apps...</source>
        <translation>Поиск приложений...</translation>
    </message>
</context>
<context>
    <name>BrowseFontsCommand</name>
    <message>
        <location filename="../src/extensions/font/browse-fonts-command.hpp" line="+8" />
        <source>Search Fonts</source>
        <translation>Поиск шрифтов</translation>
    </message>
</context>
<context>
    <name>BrowserExtension</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.hpp" line="+12" />
        <source>Browser Extension</source>
        <translation>Браузерное расширение</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Browser extension related commands.</source>
        <translation>Команды, связанные с браузерным расширением.</translation>
    </message>
</context>
<context>
    <name>BrowserTabActionGenerator</name>
    <message>
        <location filename="../src/actions/browser-tab-actions.hpp" line="+24" />
        <source>Switch to tab</source>
        <translation>Перейти к вкладке</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Convert to shortcut</source>
        <translation>Преобразовать в ярлык</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Convert tab to shortcut</source>
        <translation>Преобразовать вкладку в ярлык</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Close tab</source>
        <translation>Закрыть вкладку</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to close tab: %1</source>
        <translation>Не удалось закрыть вкладку: %1</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Copy URL</source>
        <translation>Копировать URL</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Copy Title</source>
        <translation>Копировать заголовок</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Copy ID</source>
        <translation>Копировать ID</translation>
    </message>
</context>
<context>
    <name>BrowserTabProvider</name>
    <message>
        <location filename="../src/root-search/browser-tabs/browser-tabs-provider.hpp" line="+70" />
        <source>Browser Tabs</source>
        <translation>Вкладки браузера</translation>
    </message>
</context>
<context>
    <name>BrowserTabRootItem</name>
    <message>
        <location line="-51" />
        <source>Browser Tab</source>
        <translation>Вкладка браузера</translation>
    </message>
    <message>
        <location line="+14" />
        <source>Tab</source>
        <translation>Вкладка</translation>
    </message>
</context>
<context>
    <name>BrowserTabsSection</name>
    <message>
        <location filename="../src/qml/browser-tabs-model.hpp" line="+22" />
        <source>Tabs ({count})</source>
        <translation>Вкладки ({count})</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Playing Media ({count})</source>
        <translation>Воспроизводимое медиа ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/browser-tabs-model.cpp" line="+13" />
        <source>Muted</source>
        <translation>Без звука</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Playing</source>
        <translation>Воспроизводится</translation>
    </message>
</context>
<context>
    <name>BrowserTabsViewHost</name>
    <message>
        <location filename="../src/qml/browser-tabs-view-host.cpp" line="+12" />
        <source>Search, focus and close tabs</source>
        <translation>Поиск, фокус и закрытие вкладок</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsSection</name>
    <message>
        <location filename="../src/qml/builtin-icons-model.hpp" line="+20" />
        <source>Icons ({count})</source>
        <translation>Значки ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/builtin-icons-model.cpp" line="+15" />
        <source>Copy Icon Name</source>
        <translation>Копировать имя значка</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsViewHost</name>
    <message>
        <location filename="../src/qml/builtin-icons-view-host.cpp" line="+10" />
        <source>Search icons...</source>
        <translation>Поиск значков...</translation>
    </message>
</context>
<context>
    <name>CalcHistoryListView</name>
    <message>
        <location filename="../src/qml/qml/CalcHistoryListView.qml" line="+12" />
        <source>No results</source>
        <translation>Нет результатов</translation>
    </message>
</context>
<context>
    <name>CalcHistorySection</name>
    <message>
        <location filename="../src/qml/calc-history-model.cpp" line="+39" />
        <source>Copy answer</source>
        <translation>Копировать ответ</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Copy question</source>
        <translation>Копировать вопрос</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Copy question and answer</source>
        <translation>Копировать вопрос и ответ</translation>
    </message>
</context>
<context>
    <name>CalcHistoryViewHost</name>
    <message>
        <location filename="../src/qml/calc-history-view-host.cpp" line="+92" />
        <source>Search past calculations...</source>
        <translation>Поиск по прошлым вычислениям...</translation>
    </message>
</context>
<context>
    <name>CalcLiveSection</name>
    <message>
        <location filename="../src/qml/calc-history-view-host.hpp" line="+27" />
        <source>Calculator</source>
        <translation>Калькулятор</translation>
    </message>
    <message>
        <location filename="../src/qml/calc-history-view-host.cpp" line="-14" />
        <source>Copy unformatted answer</source>
        <translation>Копировать ответ без форматирования</translation>
    </message>
</context>
<context>
    <name>CalculatorExtension</name>
    <message>
        <location filename="../src/extensions/calculator/calculator-extension.hpp" line="+73" />
        <source>Calculator</source>
        <translation>Калькулятор</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Do maths, convert units or search past calculations...</source>
        <translation>Выполняйте вычисления, переводите единицы измерения или ищите прошлые расчёты...</translation>
    </message>
    <message>
        <location line="+22" />
        <source>Calculator Backend</source>
        <translation>Бэкенд калькулятора</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Which backend to use to perform calculations</source>
        <translation>Какой бэкенд использовать для вычислений</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Refresh rates on startup</source>
        <translation>Обновлять курсы при запуске</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Whether exchange rates should be refreshed every time the vicinae server is started. If the current backend does not support it, this is ignored.</source>
        <translation>Обновлять ли курсы валют при каждом запуске сервера vicinae. Если текущий бэкенд это не поддерживает, параметр игнорируется.</translation>
    </message>
</context>
<context>
    <name>CalculatorHistoryCommand</name>
    <message>
        <location line="-87" />
        <source>Calculator history</source>
        <translation>История калькулятора</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Browse past calculations. You need to copy the result of a calculation for it to be saved in history.</source>
        <translation>Просмотр прошлых вычислений. Чтобы результат попал в историю, его нужно скопировать.</translation>
    </message>
</context>
<context>
    <name>CalculatorRefreshRatesCommand</name>
    <message>
        <location line="+11" />
        <source>Refresh Exchange Rates</source>
        <translation>Обновить курсы валют</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Refresh exchange rates used by the calculator to provide currency conversion features. Not all backends may support currency conversions or manually refreshing the rates.</source>
        <translation>Обновить курсы валют, используемые калькулятором для конвертации валют. Не все бэкенды поддерживают конвертацию или ручное обновление курсов.</translation>
    </message>
    <message>
        <location line="+17" />
        <source>%1 can't refresh rates</source>
        <translation>%1 не может обновить курсы</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Refreshing rates...</source>
        <translation>Обновление курсов...</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Rates successfully refreshed</source>
        <translation>Курсы успешно обновлены</translation>
    </message>
</context>
<context>
    <name>CalculatorResultDelegate</name>
    <message>
        <location filename="../src/qml/qml/CalculatorResultDelegate.qml" line="+53" />
        <source>Expression</source>
        <translation>Выражение</translation>
    </message>
    <message>
        <location line="+58" />
        <source>Result</source>
        <translation>Результат</translation>
    </message>
</context>
<context>
    <name>CalculatorService</name>
    <message>
        <location filename="../src/services/calculator-service/calculator-service.cpp" line="+122" />
        <source>Pinned</source>
        <translation>Закреплено</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Today</source>
        <translation>Сегодня</translation>
    </message>
    <message>
        <location line="+12" />
        <source>This week</source>
        <translation>На этой неделе</translation>
    </message>
    <message>
        <location line="+13" />
        <source>This month</source>
        <translation>В этом месяце</translation>
    </message>
    <message>
        <location line="+13" />
        <source>This year</source>
        <translation>В этом году</translation>
    </message>
    <message>
        <location line="+9" />
        <source>A few years ago</source>
        <translation>Несколько лет назад</translation>
    </message>
</context>
<context>
    <name>ChangeEmojiSkinToneAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+101" />
        <source>%1 skin tone</source>
        <translation>Тон кожи %1</translation>
    </message>
</context>
<context>
    <name>ClearClipboardHistoryCommand</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.cpp" line="+35" />
        <source>Clear Clipboard History</source>
        <translation>Очистить историю буфера обмена</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Clear the clipboard history</source>
        <translation>Очистить историю буфера обмена</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Are you sure?</source>
        <translation>Вы уверены?</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Your clipboard history will be gone forever :(</source>
        <translation>Ваша история буфера обмена будет удалена навсегда :(</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Failed to clear clipboard history</source>
        <translation>Не удалось очистить историю буфера обмена</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Clipboard history cleared</source>
        <translation>История буфера обмена очищена</translation>
    </message>
</context>
<context>
    <name>ClipboardClearCommand</name>
    <message>
        <location line="-39" />
        <source>Clear Current Clipboard Data</source>
        <translation>Очистить текущие данные буфера обмена</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Clear the current content of the clipboard</source>
        <translation>Очистить текущее содержимое буфера обмена</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Failed to clear clipboard</source>
        <translation>Не удалось очистить буфер обмена</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Clipboard cleared</source>
        <translation>Буфер обмена очищен</translation>
    </message>
</context>
<context>
    <name>ClipboardExtension</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.hpp" line="+13" />
        <source>Clipboard</source>
        <translation>Буфер обмена</translation>
    </message>
    <message>
        <location line="+4" />
        <source>System clipboard integration</source>
        <translation>Интеграция с системным буфером обмена</translation>
    </message>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.cpp" line="+55" />
        <source>Erase on startup</source>
        <translation>Очищать при запуске</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Erase clipboard history every time the vicinae server is started</source>
        <translation>Очищать историю буфера обмена при каждом запуске сервера vicinae</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Clipboard monitoring</source>
        <translation>Отслеживание буфера обмена</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Whether clipboard activity is recorded in the history. Every clipboard action performed while this is turned off will not be recorded.</source>
        <translation>Записывать ли действия с буфером обмена в историю. Каждое действие, выполненное при выключенной опции, записано не будет.</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Ignore Passwords</source>
        <translation>Игнорировать пароли</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Ignore selections that can be identified as a password. This relies on the application providing an explicit hint that the selection is a password. While most password managers and private browser windows do, some might not implement this properly.</source>
        <translation>Игнорировать выделения, которые можно распознать как пароль. Это зависит от того, даёт ли приложение явную подсказку, что выделение является паролем. Большинство менеджеров паролей и приватных окон браузера это делают, но некоторые могут реализовать это некорректно.</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryCommand</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-history-command.cpp" line="+16" />
        <source>Paste</source>
        <translation>Вставить</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Copy</source>
        <translation>Копировать</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Default Action</source>
        <translation>Действие по умолчанию</translation>
    </message>
    <message>
        <location line="+1" />
        <source>The default action to perform on pressing return. Paste is only available if your environment supports it.</source>
        <translation>Действие, выполняемое по нажатию Enter. Вставка доступна только при поддержке её вашим окружением.</translation>
    </message>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-history-command.hpp" line="+11" />
        <source>Clipboard History</source>
        <translation>История буфера обмена</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Browse your clipboard's history, pin, edit and remove entries.</source>
        <translation>Просматривайте историю буфера обмена: закрепляйте, редактируйте и удаляйте записи.</translation>
    </message>
</context>
<context>
    <name>ClipboardHistorySection</name>
    <message>
        <location filename="../src/qml/clipboard-history-model.cpp" line="+70" />
        <source>Open Settings</source>
        <translation>Открыть настройки</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryView</name>
    <message>
        <location filename="../src/qml/qml/ClipboardHistoryView.qml" line="+193" />
        <source>Type</source>
        <translation>Тип</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Size</source>
        <translation>Размер</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Copied at</source>
        <translation>Скопировано</translation>
    </message>
    <message>
        <location line="+52" />
        <source>Preview not available for this content type</source>
        <translation>Предпросмотр недоступен для этого типа содержимого</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryViewHost</name>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.hpp" line="+88" />
        <source>Loading...</source>
        <translation>Загрузка...</translation>
    </message>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.cpp" line="+72" />
        <source>All</source>
        <translation>Все</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Text</source>
        <translation>Текст</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Images</source>
        <translation>Изображения</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Links</source>
        <translation>Ссылки</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Files</source>
        <translation>Файлы</translation>
    </message>
    <message>
        <location line="+31" />
        <source>Browse clipboard history...</source>
        <translation>Просмотр истории буфера обмена...</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Clipboard monitoring unavailable</source>
        <translation>Отслеживание буфера обмена недоступно</translation>
    </message>
    <message>
        <location line="+68" />
        <source>Pause clipboard</source>
        <translation>Приостановить буфер обмена</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Resume clipboard</source>
        <translation>Возобновить буфер обмена</translation>
    </message>
    <message numerus="yes">
        <location line="+8" />
        <source>%n Items</source>
        <translation>
            <numerusform>%n элемент</numerusform>
            <numerusform>%n элемента</numerusform>
            <numerusform>%n элементов</numerusform>
        </translation>
    </message>
    <message>
        <location line="+28" />
        <source>Decryption failed</source>
        <translation>Ошибка расшифровки</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Vicinae could not decrypt the data for this selection. It was most likely encrypted with a different key and cannot be recovered. You can remove this entry from the history.</source>
        <translation>Vicinae не удалось расшифровать данные этого выделения. Скорее всего, они были зашифрованы другим ключом и восстановлению не подлежат. Вы можете удалить эту запись из истории.</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Data unavailable</source>
        <translation>Данные недоступны</translation>
    </message>
    <message>
        <location line="+1" />
        <source>The data for this selection could not be found on disk.</source>
        <translation>Данные для этого выделения не найдены на диске.</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Data is encrypted</source>
        <translation>Данные зашифрованы</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Data for this selection was previously encrypted but the clipboard is not currently configured to use encryption. You should be able to fix this by enabling it in the settings.</source>
        <translation>Данные этого выделения ранее были зашифрованы, но сейчас буфер обмена не настроен на шифрование. Обычно это исправляется включением шифрования в настройках.</translation>
    </message>
</context>
<context>
    <name>ClipboardService</name>
    <message>
        <location filename="../src/services/clipboard/clipboard-service.cpp" line="+316" />
        <source>Image (%1x%2)</source>
        <translation>Изображение (%1x%2)</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Image</source>
        <translation>Изображение</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Unknown</source>
        <translation>Неизвестно</translation>
    </message>
</context>
<context>
    <name>CloseWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="-37" />
        <source>Close window</source>
        <translation>Закрыть окно</translation>
    </message>
</context>
<context>
    <name>CommandLineSection</name>
    <message>
        <location filename="../src/qml/system-run-model.hpp" line="+19" />
        <source>Execute query</source>
        <translation>Выполнить запрос</translation>
    </message>
    <message>
        <location filename="../src/qml/system-run-model.cpp" line="+44" />
        <source>Open in %1 (hold)</source>
        <translation>Открыть в %1 (удерживать)</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Open in %1</source>
        <translation>Открыть в %1</translation>
    </message>
</context>
<context>
    <name>CommandListView</name>
    <message>
        <location filename="../src/qml/qml/CommandListView.qml" line="+12" />
        <source>No results</source>
        <translation>Нет результатов</translation>
    </message>
</context>
<context>
    <name>CommandRootItem</name>
    <message>
        <location filename="../src/root-search/extensions/extension-root-provider.cpp" line="+28" />
        <location line="+48" />
        <source>Command</source>
        <translation>Команда</translation>
    </message>
    <message>
        <location line="-43" />
        <location line="+27" />
        <source>Open command</source>
        <translation>Открыть команду</translation>
    </message>
    <message>
        <location line="-13" />
        <source>Copy extension path</source>
        <translation>Копировать путь к расширению</translation>
    </message>
    <message>
        <location line="+28" />
        <source>Internal Command</source>
        <translation>Внутренняя команда</translation>
    </message>
</context>
<context>
    <name>CompletionPopup</name>
    <message>
        <location filename="../src/qml/qml/CompletionPopup.qml" line="+13" />
        <source>Filter...</source>
        <translation>Фильтр...</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorAnswerAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+29" />
        <source>Answer copied to clipboard</source>
        <translation>Ответ скопирован в буфер обмена</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Failed to copy answer</source>
        <translation>Не удалось скопировать ответ</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Copy Result</source>
        <translation>Копировать результат</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorQuestionAndAnswerAction</name>
    <message>
        <location line="+18" />
        <source>Answer copied to clipboard</source>
        <translation>Ответ скопирован в буфер обмена</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Failed to copy answer</source>
        <translation>Не удалось скопировать ответ</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Copy Question And Answer</source>
        <translation>Копировать вопрос и ответ</translation>
    </message>
</context>
<context>
    <name>CopyClipboardSelection</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+29" />
        <source>Selection copied to clipboard</source>
        <translation>Выделение скопировано в буфер обмена</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Failed to copy to clipboard</source>
        <translation>Не удалось скопировать в буфер обмена</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Copy to clipboard</source>
        <translation>Копировать в буфер обмена</translation>
    </message>
</context>
<context>
    <name>CopyItemDeeplink</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+76" />
        <source>Deeplink copied in clipboard</source>
        <translation>Диплинк скопирован в буфер обмена</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Copy Deeplink</source>
        <translation>Копировать диплинк</translation>
    </message>
</context>
<context>
    <name>CopyShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="+232" />
        <source>Copied to clipboard</source>
        <translation>Скопировано в буфер обмена</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Copy shortcut</source>
        <translation>Копировать ярлык</translation>
    </message>
</context>
<context>
    <name>CopyToClipboardAction</name>
    <message>
        <location filename="../src/clipboard-actions.hpp" line="+20" />
        <source>Copied to clipboard</source>
        <translation>Скопировано в буфер обмена</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Copy to clipboard</source>
        <translation>Копировать в буфер обмена</translation>
    </message>
</context>
<context>
    <name>CreateExtensionCommand</name>
    <message>
        <location filename="../src/extensions/developer/developer-extension.hpp" line="+10" />
        <source>Create Extension</source>
        <translation>Создать расширение</translation>
    </message>
</context>
<context>
    <name>CreateExtensionFormView</name>
    <message>
        <location filename="../src/qml/qml/CreateExtensionFormView.qml" line="+15" />
        <source>Author</source>
        <translation>Автор</translation>
    </message>
    <message>
        <location line="+2" />
        <source>If you plan on submitting your extension to the &lt;a href="vicinae://launch/core/store"&gt;Vicinae store&lt;/a&gt;, this must exactly match your GitHub handle. Otherwise, you can set it to anything.</source>
        <translation>Если вы планируете отправить расширение в &lt;a href="vicinae://launch/core/store"&gt;магазин Vicinae&lt;/a&gt;, это значение должно точно совпадать с вашим именем на GitHub. В противном случае можно указать что угодно.</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Username</source>
        <translation>Имя пользователя</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Extension Title</source>
        <translation>Название расширения</translation>
    </message>
    <message>
        <location line="+5" />
        <source>My Extension</source>
        <translation>Моё расширение</translation>
    </message>
    <message>
        <location line="+8" />
        <location line="+42" />
        <source>Description</source>
        <translation>Описание</translation>
    </message>
    <message>
        <location line="-36" />
        <source>An extension that does super cool things</source>
        <translation>Расширение, которое делает суперкрутые вещи</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Location</source>
        <translation>Расположение</translation>
    </message>
    <message>
        <location line="+15" />
        <source>Command Title</source>
        <translation>Название команды</translation>
    </message>
    <message>
        <location line="+5" />
        <source>My Wonderful Command</source>
        <translation>Моя великолепная команда</translation>
    </message>
    <message>
        <location line="+14" />
        <source>My command does this, and that...</source>
        <translation>Моя команда делает то и это...</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Template</source>
        <translation>Шаблон</translation>
    </message>
</context>
<context>
    <name>CreateExtensionSuccessViewHost</name>
    <message>
        <location filename="../src/qml/create-extension-success-view-host.cpp" line="+7" />
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
# Расширение успешно создано

Ваше новое расширение %1 успешно создано в `%2`.

Чтобы команды этого расширения были доступны в Vicinae, необходимо хотя бы раз запустить его в режиме разработки:

```bash
cd %2
npm install
npm run dev
```

Подробнее о разработке расширений — в [документации Vicinae](https://docs.vicinae.com/).
</translation>
    </message>
    <message>
        <location line="+37" />
        <source>Open in %1</source>
        <translation>Открыть в %1</translation>
    </message>
</context>
<context>
    <name>CreateExtensionViewHost</name>
    <message>
        <location filename="../src/qml/create-extension-view-host.cpp" line="+37" />
        <source>Create extension</source>
        <translation>Создать расширение</translation>
    </message>
    <message>
        <location line="+19" />
        <location line="+4" />
        <location line="+19" />
        <location line="+5" />
        <source>Min. 3 chars</source>
        <translation>Мин. 3 символа</translation>
    </message>
    <message>
        <location line="-20" />
        <source>Min. 16 chars</source>
        <translation>Мин. 16 символов</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Must exist</source>
        <translation>Обязательное поле</translation>
    </message>
    <message>
        <location line="+18" />
        <source>Form has errors</source>
        <translation>В форме есть ошибки</translation>
    </message>
    <message>
        <location line="+21" />
        <source>Failed to create extension</source>
        <translation>Не удалось создать расширение</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Extension created!</source>
        <translation>Расширение создано!</translation>
    </message>
</context>
<context>
    <name>CreateShortcutCommand</name>
    <message>
        <location filename="../src/extensions/shortcut/shortcut-extension.hpp" line="+13" />
        <source>Create Shortcut</source>
        <translation>Создать ярлык</translation>
    </message>
</context>
<context>
    <name>CreateShortcutFromActiveBrowserTabCommand</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="+44" />
        <source>Create Shortcut from Active Tab</source>
        <translation>Создать ярлык из активной вкладки</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Create a vicinae shortcut from the currently active browser tab. May yield unexpected results if many browsers are connected at once.</source>
        <translation>Создать ярлык vicinae из активной вкладки браузера. Если подключено сразу несколько браузеров, результат может быть непредсказуемым.</translation>
    </message>
    <message>
        <location line="+15" />
        <source>No active tab!</source>
        <translation>Нет активной вкладки!</translation>
    </message>
</context>
<context>
    <name>CreateSnippetCommand</name>
    <message>
        <location filename="../src/extensions/snippet/create-snippet-command.hpp" line="+10" />
        <source>Create Snippet</source>
        <translation>Создать сниппет</translation>
    </message>
</context>
<context>
    <name>DMenuSection</name>
    <message>
        <location filename="../src/qml/dmenu-model.cpp" line="+96" />
        <source>Select entry</source>
        <translation>Выбрать элемент</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Select entry (index)</source>
        <translation>Выбрать элемент (по индексу)</translation>
    </message>
    <message>
        <location line="+14" />
        <source>Pass search text</source>
        <translation>Передать текст поиска</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Select and copy entry</source>
        <translation>Выбрать и скопировать элемент</translation>
    </message>
</context>
<context>
    <name>DMenuView</name>
    <message>
        <location filename="../src/qml/qml/DMenuView.qml" line="+79" />
        <source>Name</source>
        <translation>Название</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Path</source>
        <translation>Путь</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Type</source>
        <translation>Тип</translation>
    </message>
</context>
<context>
    <name>DMenuViewHost</name>
    <message>
        <location filename="../src/qml/dmenu-view-host.cpp" line="+35" />
        <source>Search entries...</source>
        <translation>Поиск элементов...</translation>
    </message>
    <message>
        <location line="+73" />
        <source>Pass search text</source>
        <translation>Передать текст поиска</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Pass and copy search text</source>
        <translation>Передать и скопировать текст поиска</translation>
    </message>
</context>
<context>
    <name>DetailListView</name>
    <message>
        <location filename="../src/qml/qml/DetailListView.qml" line="+32" />
        <source>No results</source>
        <translation>Нет результатов</translation>
    </message>
</context>
<context>
    <name>DeveloperExtension</name>
    <message>
        <location filename="../src/extensions/developer/developer-extension.hpp" line="+10" />
        <source>Developer</source>
        <translation>Разработка</translation>
    </message>
</context>
<context>
    <name>DisableApplication</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+8" />
        <source>Disable item</source>
        <translation>Отключить элемент</translation>
    </message>
</context>
<context>
    <name>DisableItemAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="+89" />
        <source>Are you sure?</source>
        <translation>Вы уверены?</translation>
    </message>
    <message>
        <location line="+1" />
        <source>You will need to go in the settings to manually re-enable it.</source>
        <translation>Чтобы снова включить его, вам придётся зайти в настройки.</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Disable</source>
        <translation>Отключить</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Item disabled</source>
        <translation>Элемент отключён</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to disable</source>
        <translation>Не удалось отключить</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Disable item</source>
        <translation>Отключить элемент</translation>
    </message>
</context>
<context>
    <name>DismissNewsAction</name>
    <message>
        <location filename="../src/services/news/news-service.cpp" line="+34" />
        <source>Dismiss</source>
        <translation>Скрыть</translation>
    </message>
</context>
<context>
    <name>DuplicateShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-50" />
        <source>Duplicate link</source>
        <translation>Дублировать ссылку</translation>
    </message>
</context>
<context>
    <name>EditClipboardKeywordsAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+50" />
        <source>Additional keywords that will be used to index this selection.</source>
        <translation>Дополнительные ключевые слова для индексации этого выделения.</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Edit keywords</source>
        <translation>Изменить ключевые слова</translation>
    </message>
</context>
<context>
    <name>EditEmojiKeywordsAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+35" />
        <source>Additional keywords that will be used to index this glyph</source>
        <translation>Дополнительные ключевые слова для индексации этого символа</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Edit keyword</source>
        <translation>Изменить ключевое слово</translation>
    </message>
</context>
<context>
    <name>EditKeywordsFormView</name>
    <message>
        <location filename="../src/qml/qml/EditKeywordsFormView.qml" line="+19" />
        <source>Keywords</source>
        <translation>Ключевые слова</translation>
    </message>
</context>
<context>
    <name>EditKeywordsViewHost</name>
    <message>
        <location filename="../src/qml/edit-keywords-view-host.cpp" line="+27" />
        <source>Submit</source>
        <translation>Отправить</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Keywords edited</source>
        <translation>Ключевые слова изменены</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Failed to edit keywords</source>
        <translation>Не удалось изменить ключевые слова</translation>
    </message>
</context>
<context>
    <name>EditShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-41" />
        <source>Edit shortcut</source>
        <translation>Изменить ярлык</translation>
    </message>
</context>
<context>
    <name>EmojiGridModel</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.hpp" line="+58" />
        <source>Search for emojis and symbols...</source>
        <translation>Поиск эмодзи и символов...</translation>
    </message>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+241" />
        <source>Pinned</source>
        <translation>Закреплённые</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Recently used</source>
        <translation>Недавние</translation>
    </message>
</context>
<context>
    <name>EmojiGridViewHost</name>
    <message>
        <location filename="../src/qml/emoji-grid-view-host.hpp" line="+64" />
        <source>All</source>
        <translation>Все</translation>
    </message>
</context>
<context>
    <name>EmptyView</name>
    <message>
        <location filename="../src/qml/qml/EmptyView.qml" line="+7" />
        <source>No results</source>
        <translation>Нет результатов</translation>
    </message>
</context>
<context>
    <name>EnabledFallbackSection</name>
    <message>
        <location filename="../src/qml/manage-fallback-model.hpp" line="-19" />
        <source>Enabled</source>
        <translation>Включены</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-fallback-model.cpp" line="-22" />
        <source>Disable fallback</source>
        <translation>Отключить резервный вариант</translation>
    </message>
</context>
<context>
    <name>Expansion</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.hpp" line="+31" />
        <source>Keyword cannot be empty</source>
        <translation>Ключевое слово не может быть пустым</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Keyword exceeds maximum length of %1</source>
        <translation>Ключевое слово превышает максимальную длину %1</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Keyword must only contain printable ASCII characters (no spaces)</source>
        <translation>Ключевое слово должно содержать только печатные ASCII-символы (без пробелов)</translation>
    </message>
</context>
<context>
    <name>ExtensionBoilerplateGenerator</name>
    <message>
        <location filename="../src/services/extension-boilerplate-generator/extension-boilerplate-generator.cpp" line="+24" />
        <source>Simple List</source>
        <translation>Простой список</translation>
    </message>
    <message>
        <location line="+2" />
        <source>List with Detail</source>
        <translation>Список с деталями</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Controlled List</source>
        <translation>Управляемый список</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Simple Detail</source>
        <translation>Простая деталь</translation>
    </message>
    <message>
        <location line="+3" />
        <source>No View</source>
        <translation>Без интерфейса</translation>
    </message>
</context>
<context>
    <name>ExtensionErrorViewHost</name>
    <message>
        <location filename="../src/qml/extension-error-view-host.cpp" line="+6" />
        <source># Extension crashed 💥!

This extension threw an uncaught exception and crashed as a result.

Find the full stacktrace below. You can also directly copy it from the action menu.

```
%1
```</source>
        <translation># Расширение аварийно завершилось 💥!

Это расширение вызвало необработанное исключение и из-за этого завершилось.

Полный стектрейс — ниже. Его также можно скопировать из меню действий.

```
%1
```</translation>
    </message>
</context>
<context>
    <name>ExtensionFormModel</name>
    <message>
        <location filename="../src/qml/extension-form-model.cpp" line="+229" />
        <source>One or more fields have errors</source>
        <translation>Одно или несколько полей содержат ошибки</translation>
    </message>
</context>
<context>
    <name>ExtensionGridModel</name>
    <message>
        <location filename="../src/qml/extension-grid-model.cpp" line="+204" />
        <source>Search...</source>
        <translation>Поиск...</translation>
    </message>
    <message>
        <location line="+52" />
        <source>No results</source>
        <translation>Нет результатов</translation>
    </message>
</context>
<context>
    <name>ExtensionGridView</name>
    <message>
        <location filename="../src/qml/qml/ExtensionGridView.qml" line="+9" />
        <source>No results</source>
        <translation>Нет результатов</translation>
    </message>
</context>
<context>
    <name>ExtensionListModel</name>
    <message>
        <location filename="../src/qml/extension-list-model.cpp" line="+197" />
        <source>Search...</source>
        <translation>Поиск...</translation>
    </message>
    <message>
        <location line="+5" />
        <source>No results</source>
        <translation>Нет результатов</translation>
    </message>
</context>
<context>
    <name>ExtensionSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/ExtensionSettingsPage.qml" line="+113" />
        <source>Description</source>
        <translation>Описание</translation>
    </message>
    <message>
        <location line="+23" />
        <source>Preferences</source>
        <translation>Настройки</translation>
    </message>
    <message>
        <location line="+14" />
        <source>Commands</source>
        <translation>Команды</translation>
    </message>
    <message>
        <location line="+118" />
        <source>Shortcut</source>
        <translation>Ярлык</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Add Alias</source>
        <translation>Добавить алиас</translation>
    </message>
    <message>
        <location line="+96" />
        <source>Nothing to configure</source>
        <translation>Нечего настраивать</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Commands and preferences will show up here once available.</source>
        <translation>Команды и настройки появятся здесь, как только станут доступны.</translation>
    </message>
</context>
<context>
    <name>ExtensionView</name>
    <message>
        <location filename="../src/qml/qml/ExtensionView.qml" line="+99" />
        <source>No results</source>
        <translation>Нет результатов</translation>
    </message>
</context>
<context>
    <name>FileExtension</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="+65" />
        <source>System files</source>
        <translation>Системные файлы</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Integrate with system files</source>
        <translation>Интеграция с системными файлами</translation>
    </message>
    <message>
        <location line="+24" />
        <source>Enabled</source>
        <translation>Включено</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Whether to run the file indexer in the background. When turned off, the indexer process is stopped entirely and file search becomes unavailable until it is turned back on.</source>
        <translation>Запускать ли индексатор файлов в фоне. При выключении процесс индексатора полностью останавливается, и поиск файлов становится недоступен, пока его снова не включат.</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Search paths</source>
        <translation>Пути поиска</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Directories that Vicinae will search</source>
        <translation>Каталоги, которые будет искать Vicinae</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Excluded search paths</source>
        <translation>Исключённые пути поиска</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Directories to exclude from file indexing</source>
        <translation>Каталоги, исключаемые из индексации файлов</translation>
    </message>
</context>
<context>
    <name>FilePreview</name>
    <message>
        <location filename="../src/qml/qml/FilePreview.qml" line="+42" />
        <source>Preview not available for this file type</source>
        <translation>Предпросмотр недоступен для этого типа файла</translation>
    </message>
</context>
<context>
    <name>FocusWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="-17" />
        <source>Focus window</source>
        <translation>Сфокусировать окно</translation>
    </message>
</context>
<context>
    <name>FontBrowserViewHost</name>
    <message>
        <location filename="../src/qml/font-browser-view-host.hpp" line="+56" />
        <source>All</source>
        <translation>Все</translation>
    </message>
</context>
<context>
    <name>FontExtension</name>
    <message>
        <location filename="../src/extensions/font/font-extension.hpp" line="+9" />
        <source>Font</source>
        <translation>Шрифт</translation>
    </message>
</context>
<context>
    <name>FontGridModel</name>
    <message>
        <location filename="../src/qml/font-grid-model.hpp" line="+47" />
        <source>Search fonts...</source>
        <translation>Поиск шрифтов...</translation>
    </message>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="+133" />
        <source>All Fonts (%1)</source>
        <translation>Все шрифты (%1)</translation>
    </message>
    <message>
        <location line="+15" />
        <source>Results (%1)</source>
        <translation>Результаты (%1)</translation>
    </message>
</context>
<context>
    <name>Footer</name>
    <message>
        <location filename="../src/qml/qml/Footer.qml" line="+60" />
        <source>Actions</source>
        <translation>Действия</translation>
    </message>
</context>
<context>
    <name>ForceQuitAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+95" />
        <source>Force Quit Application</source>
        <translation>Принудительно завершить приложение</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Failed to force quit %1</source>
        <translation>Не удалось принудительно завершить %1</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Force quit %1</source>
        <translation>Принудительно завершить %1</translation>
    </message>
</context>
<context>
    <name>ForgetTelemetryCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+171" />
        <source>Forget Past Vicinae Telemetry</source>
        <translation>Забыть прошлую телеметрию Vicinae</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Asks the vicinae server to anonymize telemetry data that was sent with your vicinae instance ID attached. The ID is only linked to your vicinae install, which has no direct relationship with your system.</source>
        <translation>Запрашивает у сервера vicinae анонимизацию телеметрии, отправленной с привязанным идентификатором вашего экземпляра vicinae. Этот идентификатор связан только с вашей установкой vicinae и не имеет прямой связи с вашей системой.</translation>
    </message>
    <message>
        <location line="+19" />
        <source>Processing...</source>
        <translation>Обработка...</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Past telemetry was successfully detached from your vicinae user ID.</source>
        <translation>Прошлая телеметрия успешно отвязана от вашего идентификатора vicinae.</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to forget past telemetry data</source>
        <translation>Не удалось удалить данные прошлой телеметрии</translation>
    </message>
</context>
<context>
    <name>FormAppSelector</name>
    <message>
        <location filename="../src/qml/qml/FormAppSelector.qml" line="+46" />
        <source>All applications</source>
        <translation>Все приложения</translation>
    </message>
    <message>
        <location line="+70" />
        <source>+ Restrict to app…</source>
        <translation>+ Ограничить приложением…</translation>
    </message>
</context>
<context>
    <name>FormFilePicker</name>
    <message>
        <location filename="../src/qml/qml/FormFilePicker.qml" line="+85" />
        <source>Select files</source>
        <translation>Выбрать файлы</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Select a file</source>
        <translation>Выбрать файл</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Select a directory</source>
        <translation>Выбрать каталог</translation>
    </message>
    <message>
        <location line="+46" />
        <source>No directory selected</source>
        <translation>Каталог не выбран</translation>
    </message>
    <message>
        <location line="+0" />
        <source>No file selected</source>
        <translation>Файл не выбран</translation>
    </message>
    <message>
        <location line="+164" />
        <source>+ Add folder…</source>
        <translation>+ Добавить папку…</translation>
    </message>
    <message>
        <location line="+0" />
        <source>+ Add file…</source>
        <translation>+ Добавить файл…</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsModel</name>
    <message>
        <location filename="../src/qml/general-settings-model.cpp" line="+189" />
        <location line="+11" />
        <source>None</source>
        <translation>Нет</translation>
    </message>
    <message>
        <location line="-10" />
        <location line="+10" />
        <source>Blurred</source>
        <translation>Размытый</translation>
    </message>
    <message>
        <location line="-8" />
        <location line="+8" />
        <source>Liquid Glass</source>
        <translation>Жидкое стекло</translation>
    </message>
    <message>
        <location line="-7" />
        <source>Window material</source>
        <translation>Материал окна</translation>
    </message>
    <message>
        <location line="+23" />
        <source>Themes</source>
        <translation>Темы</translation>
    </message>
    <message>
        <location line="+17" />
        <source>Fonts</source>
        <translation>Шрифты</translation>
    </message>
    <message>
        <location line="+17" />
        <source>Icon Themes</source>
        <translation>Темы значков</translation>
    </message>
    <message>
        <location line="+15" />
        <source>Favicon Services</source>
        <translation>Сервисы favicon</translation>
    </message>
    <message>
        <location line="+13" />
        <location line="+10" />
        <source>Default</source>
        <translation>По умолчанию</translation>
    </message>
    <message>
        <location line="-7" />
        <source>Keybinding Schemes</source>
        <translation>Схемы горячих клавиш</translation>
    </message>
    <message>
        <location line="+48" />
        <location line="+10" />
        <source>System default</source>
        <translation>Системный по умолчанию</translation>
    </message>
    <message>
        <location line="-6" />
        <source>Languages</source>
        <translation>Языки</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/GeneralSettingsPage.qml" line="+33" />
        <source>Behavior</source>
        <translation>Поведение</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Launcher hotkey</source>
        <translation>Горячая клавиша лаунчера</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Global shortcut to toggle the Vicinae launcher.</source>
        <translation>Глобальное сочетание клавиш для показа/скрытия лаунчера Vicinae.</translation>
    </message>
    <message>
        <location line="+13" />
        <source>Close on focus loss</source>
        <translation>Закрывать при потере фокуса</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Close on Escape</source>
        <translation>Закрывать по Esc</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Pressing Escape closes the launcher instead of navigating one view back.</source>
        <translation>Нажатие Esc закрывает лаунчер, а не возвращает на один уровень назад.</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Pop to root on close</source>
        <translation>Возврат к корневому уровню при закрытии</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Reset the navigation state when the launcher window is closed.</source>
        <translation>Сбрасывать состояние навигации при закрытии окна лаунчера.</translation>
    </message>
    <message>
        <location line="+10" />
        <location line="+7" />
        <source>Language</source>
        <translation>Язык</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Requires restarting Vicinae to take effect.</source>
        <translation>Для применения требуется перезапуск Vicinae.</translation>
    </message>
    <message>
        <location line="+12" />
        <source>Privacy</source>
        <translation>Конфиденциальность</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Basic usage statistics</source>
        <translation>Базовая статистика использования</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Send basic system and vicinae installation information on startup to help improve Vicinae.</source>
        <translation>Отправлять при запуске базовую информацию о системе и установке vicinae, чтобы помочь улучшить Vicinae.</translation>
    </message>
</context>
<context>
    <name>GenericGridView</name>
    <message>
        <location filename="../src/qml/qml/GenericGridView.qml" line="+39" />
        <source>No results</source>
        <translation>Нет результатов</translation>
    </message>
</context>
<context>
    <name>GenericListView</name>
    <message>
        <location filename="../src/qml/qml/GenericListView.qml" line="+31" />
        <source>No results</source>
        <translation>Нет результатов</translation>
    </message>
</context>
<context>
    <name>GlobalShortcutService</name>
    <message>
        <location filename="../src/services/global-shortcuts/global-shortcut-service.cpp" line="+57" />
        <source>Toggle Vicinae</source>
        <translation>Показать/скрыть Vicinae</translation>
    </message>
    <message>
        <location line="+111" />
        <source>the launcher hotkey</source>
        <translation>горячая клавиша лаунчера</translation>
    </message>
    <message>
        <location line="+11" />
        <source>another command</source>
        <translation>другая команда</translation>
    </message>
</context>
<context>
    <name>Gnome::Workspace</name>
    <message>
        <location filename="../src/services/window-manager/gnome/gnome-workspace.cpp" line="+18" />
        <source>Workspace %1</source>
        <translation>Рабочее пространство %1</translation>
    </message>
</context>
<context>
    <name>HibernateCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+127" />
        <source>Hibernate System</source>
        <translation>Перевести систему в гибернацию</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Suspend the system to disk. This turns off the system completely and saves its state on disk, to be restored on next boot.</source>
        <translation>Приостановить работу системы с записью на диск. Система полностью выключится, а её состояние сохранится на диске и будет восстановлено при следующей загрузке.</translation>
    </message>
    <message>
        <location line="+14" />
        <source>System can't hibernate</source>
        <translation>Система не может перейти в гибернацию</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Failed to hibernate</source>
        <translation>Не удалось перевести в гибернацию</translation>
    </message>
</context>
<context>
    <name>IconBrowserCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+26" />
        <source>Search Builtin Icons</source>
        <translation>Поиск встроенных значков</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Search Vicinae builtin set of icons</source>
        <translation>Поиск по встроенному набору значков Vicinae</translation>
    </message>
</context>
<context>
    <name>ImageViewer</name>
    <message>
        <location filename="../src/qml/qml/ImageViewer.qml" line="+159" />
        <source>%1 / %2</source>
        <translation>%1 / %2</translation>
    </message>
</context>
<context>
    <name>InspectLocalStorage</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+13" />
        <source>Inspect Local Storage</source>
        <translation>Просмотр локального хранилища</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Browse data stored in Vicinae's local storage. This includes data stored for builtin extensions as well as third-party extensions making use of the LocalStorage API.</source>
        <translation>Просмотр данных в локальном хранилище Vicinae. Это включает данные встроенных расширений, а также сторонних расширений, использующих API LocalStorage.</translation>
    </message>
</context>
<context>
    <name>InstallUpdateAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+188" />
        <source>Install Update</source>
        <translation>Установить обновление</translation>
    </message>
    <message>
        <location line="+7" />
        <source>An update is already in progress</source>
        <translation>Обновление уже выполняется</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsSection</name>
    <message>
        <location filename="../src/qml/installed-extensions-model.hpp" line="+18" />
        <source>Installed Extensions ({count})</source>
        <translation>Установленные расширения ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/installed-extensions-model.cpp" line="+38" />
        <source>Local</source>
        <translation>Локальные</translation>
    </message>
    <message>
        <location line="+16" />
        <source>Copy</source>
        <translation>Копировать</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Copy Name</source>
        <translation>Копировать название</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Copy ID</source>
        <translation>Копировать ID</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Copy Path</source>
        <translation>Копировать путь</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Copy Author</source>
        <translation>Копировать автора</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsViewHost</name>
    <message>
        <location filename="../src/qml/installed-extensions-view-host.cpp" line="+12" />
        <source>Search extensions...</source>
        <translation>Поиск расширений...</translation>
    </message>
</context>
<context>
    <name>InternalExtension</name>
    <message>
        <location filename="../src/extensions/internal/internal-extension.hpp" line="+12" />
        <location line="+1" />
        <source>Internal Commands</source>
        <translation>Внутренние команды</translation>
    </message>
</context>
<context>
    <name>KeyboardBridge</name>
    <message>
        <location filename="../src/qml/keyboard-bridge.hpp" line="+51" />
        <source>Modifier required</source>
        <translation>Требуется модификатор</translation>
    </message>
</context>
<context>
    <name>LauncherWindow</name>
    <message>
        <location filename="../src/qml/qml/LauncherWindow.qml" line="+32" />
        <source>Vicinae Launcher</source>
        <translation>Лаунчер Vicinae</translation>
    </message>
    <message>
        <location filename="../src/qml/launcher-window.cpp" line="+640" />
        <source>Open Settings</source>
        <translation>Открыть настройки</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Keyboard Shortcuts</source>
        <translation>Горячие клавиши</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Extension Store</source>
        <translation>Магазин расширений</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Documentation</source>
        <translation>Документация</translation>
    </message>
    <message>
        <location line="+3" />
        <location line="+5" />
        <source>Opened in browser</source>
        <translation>Открыто в браузере</translation>
    </message>
    <message>
        <location line="-2" />
        <source>Report a Bug</source>
        <translation>Сообщить об ошибке</translation>
    </message>
    <message>
        <location line="+4" />
        <source>About Vicinae</source>
        <translation>О Vicinae</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemSection</name>
    <message>
        <location filename="../src/qml/local-storage-model.hpp" line="+19" />
        <source>Items ({count})</source>
        <translation>Элементы ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/local-storage-model.cpp" line="+31" />
        <source>Show value</source>
        <translation>Показать значение</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemViewHost</name>
    <message>
        <location filename="../src/qml/local-storage-view-host.cpp" line="+23" />
        <source>Search items...</source>
        <translation>Поиск элементов...</translation>
    </message>
</context>
<context>
    <name>LocalStorageNamespaceSection</name>
    <message>
        <location filename="../src/qml/local-storage-model.hpp" line="-11" />
        <source>Namespaces ({count})</source>
        <translation>Пространства имён ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/local-storage-model.cpp" line="-16" />
        <source>Browse namespace</source>
        <translation>Просмотреть пространство имён</translation>
    </message>
</context>
<context>
    <name>LocalStorageViewHost</name>
    <message>
        <location filename="../src/qml/local-storage-view-host.cpp" line="-15" />
        <source>Search namespaces...</source>
        <translation>Поиск пространств имён...</translation>
    </message>
</context>
<context>
    <name>LockCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-48" />
        <source>Lock Session</source>
        <translation>Заблокировать сеанс</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Lock the current user session</source>
        <translation>Заблокировать текущий сеанс пользователя</translation>
    </message>
    <message>
        <location line="+13" />
        <source>System can't lock</source>
        <translation>Система не может выполнить блокировку</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Failed to lock</source>
        <translation>Не удалось заблокировать</translation>
    </message>
</context>
<context>
    <name>LogOutCommand</name>
    <message>
        <location line="+176" />
        <source>Log Out</source>
        <translation>Выйти из системы</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Terminate the current user session. If you simply want to lock your session you should use 'Lock Session' instead.</source>
        <translation>Завершить текущий сеанс пользователя. Если нужно просто заблокировать сеанс, используйте вместо этого «Заблокировать сеанс».</translation>
    </message>
    <message>
        <location line="+15" />
        <source>System can't logout</source>
        <translation>Система не может выполнить выход</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Failed to log out</source>
        <translation>Не удалось выполнить выход</translation>
    </message>
</context>
<context>
    <name>MacAppDatabase</name>
    <message>
        <location filename="../src/services/app-service/macos/mac-app-database.mm" line="+159" />
        <source>Application directories</source>
        <translation>Каталоги приложений</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Directories applications are sourced from. System directories are always scanned and cannot be removed.</source>
        <translation>Каталоги, из которых берутся приложения. Системные каталоги сканируются всегда и не могут быть удалены.</translation>
    </message>
</context>
<context>
    <name>MacOSGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/macos-global-shortcut-backend.cpp" line="+238" />
        <location line="+40" />
        <source>unsupported or invalid trigger</source>
        <translation>неподдерживаемый или неверный триггер</translation>
    </message>
    <message>
        <location line="-32" />
        <source>failed to register hot key (%1)</source>
        <translation>не удалось зарегистрировать горячую клавишу (%1)</translation>
    </message>
</context>
<context>
    <name>MacSettingsRootItem</name>
    <message>
        <location filename="../src/root-search/macos-settings/macos-settings-root-provider.mm" line="+132" />
        <location line="+9" />
        <source>System Settings</source>
        <translation>Системные настройки</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Name</source>
        <translation>Название</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Bundle ID</source>
        <translation>ID пакета</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Legacy ID</source>
        <translation>Устаревший ID</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Where</source>
        <translation>Расположение</translation>
    </message>
    <message>
        <location line="+12" />
        <source>Open %1 Settings</source>
        <translation>Открыть настройки %1</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Copy URL</source>
        <translation>Копировать URL</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Copy Bundle ID</source>
        <translation>Копировать ID пакета</translation>
    </message>
</context>
<context>
    <name>MacSettingsRootProvider</name>
    <message>
        <location line="+13" />
        <source>System Settings</source>
        <translation>Системные настройки</translation>
    </message>
</context>
<context>
    <name>MacosUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/macos-update-installer.mm" line="+207" />
        <source>This installation cannot update itself</source>
        <translation>Эта установка не может обновить себя сама</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Mounting update image…</source>
        <translation>Монтирование образа обновления…</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Failed to mount the update image</source>
        <translation>Не удалось смонтировать образ обновления</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Could not find the update image mount point</source>
        <translation>Не удалось найти точку монтирования образа обновления</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Verifying update…</source>
        <translation>Проверка обновления…</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Installing update…</source>
        <translation>Установка обновления…</translation>
    </message>
    <message>
        <location line="+12" />
        <source>Failed to stage update: %1</source>
        <translation>Не удалось подготовить обновление: %1</translation>
    </message>
    <message>
        <location line="+23" />
        <source>Failed to move the current app aside: %1</source>
        <translation>Не удалось переместить текущее приложение в сторону: %1</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Failed to install the new app: %1</source>
        <translation>Не удалось установить новое приложение: %1</translation>
    </message>
</context>
<context>
    <name>ManageFallbackActions</name>
    <message>
        <location filename="../src/actions/fallback-actions.hpp" line="+15" />
        <source>Manage Fallback Actions</source>
        <translation>Управление резервными действиями</translation>
    </message>
</context>
<context>
    <name>ManageFallbackCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/configure-fallback-command.hpp" line="+11" />
        <source>Configure Fallback Commands</source>
        <translation>Настройка резервных команд</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Configure what commands are to be presented as fallback options when nothing matches the search in the root search.</source>
        <translation>Настройка команд, которые должны показываться как резервные, когда поиск в корневом уровне не даёт совпадений.</translation>
    </message>
</context>
<context>
    <name>ManageFallbackViewHost</name>
    <message>
        <location filename="../src/qml/manage-fallback-view-host.cpp" line="+12" />
        <source>Search commands...</source>
        <translation>Поиск команд...</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsCommand</name>
    <message>
        <location filename="../src/extensions/shortcut/shortcut-extension.hpp" line="+11" />
        <source>Manage Shortcuts</source>
        <translation>Управление ярлыками</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsSection</name>
    <message>
        <location filename="../src/qml/manage-shortcuts-model.hpp" line="+17" />
        <source>Shortcuts ({count})</source>
        <translation>Ярлыки ({count})</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsViewHost</name>
    <message>
        <location filename="../src/qml/manage-shortcuts-view-host.cpp" line="+28" />
        <source>Search shortcuts...</source>
        <translation>Поиск ярлыков...</translation>
    </message>
    <message>
        <location line="+28" />
        <source>Name</source>
        <translation>Название</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Application</source>
        <translation>Приложение</translation>
    </message>
    <message>
        <location line="+2" />
        <source>%1 (Default)</source>
        <translation>%1 (По умолчанию)</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Opened</source>
        <translation>Открыто</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Last Opened</source>
        <translation>Последнее открытие</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Never</source>
        <translation>Никогда</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Created at</source>
        <translation>Создано</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsCommand</name>
    <message>
        <location filename="../src/extensions/snippet/manage-snippets-command.hpp" line="+10" />
        <source>Manage Snippets</source>
        <translation>Управление сниппетами</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsSection</name>
    <message>
        <location filename="../src/qml/manage-snippets-model.hpp" line="+18" />
        <source>Snippets ({count})</source>
        <translation>Сниппеты ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-snippets-model.cpp" line="+33" />
        <source>Copy to clipboard</source>
        <translation>Копировать в буфер обмена</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Copied to clipboard</source>
        <translation>Скопировано в буфер обмена</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to copy to clipboard</source>
        <translation>Не удалось скопировать в буфер обмена</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Edit snippet</source>
        <translation>Изменить сниппет</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Duplicate snippet</source>
        <translation>Дублировать сниппет</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Remove snippet</source>
        <translation>Удалить сниппет</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to remove snippet</source>
        <translation>Не удалось удалить сниппет</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsViewHost</name>
    <message>
        <location filename="../src/qml/manage-snippets-view-host.hpp" line="+54" />
        <source>No snippets</source>
        <translation>Нет сниппетов</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Create a snippet to get started</source>
        <translation>Создайте сниппет, чтобы начать</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-snippets-view-host.cpp" line="+28" />
        <source>Search for snippets...</source>
        <translation>Поиск сниппетов...</translation>
    </message>
    <message>
        <location line="+25" />
        <source>Text</source>
        <translation>Текст</translation>
    </message>
    <message>
        <location line="+1" />
        <source>File</source>
        <translation>Файл</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Type</source>
        <translation>Тип</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Created at</source>
        <translation>Создано</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Updated at</source>
        <translation>Обновлено</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Keyword</source>
        <translation>Ключевое слово</translation>
    </message>
    <message>
        <location line="+16" />
        <source>Apps</source>
        <translation>Приложения</translation>
    </message>
    <message>
        <location line="+68" />
        <source>Create snippet</source>
        <translation>Создать сниппет</translation>
    </message>
</context>
<context>
    <name>MarkItemAsFavorite</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="-60" />
        <source>Mark as favorite</source>
        <translation>Отметить как избранное</translation>
    </message>
</context>
<context>
    <name>MarkdownShowcase</name>
    <message>
        <location filename="../src/extensions/internal/markdown-showcase-command.hpp" line="+172" />
        <source>Markdown Showcase</source>
        <translation>Витрина Markdown</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Preview all supported markdown features</source>
        <translation>Предпросмотр всех поддерживаемых функций Markdown</translation>
    </message>
</context>
<context>
    <name>MarkdownView</name>
    <message>
        <location filename="../src/qml/qml/markdown/MarkdownView.qml" line="+270" />
        <source>Copy</source>
        <translation>Копировать</translation>
    </message>
    <message>
        <location line="+16" />
        <source>Select All</source>
        <translation>Выбрать всё</translation>
    </message>
</context>
<context>
    <name>MdCallout</name>
    <message>
        <location filename="../src/qml/qml/markdown/MdCallout.qml" line="+35" />
        <source>Caution</source>
        <translation>Осторожно</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Warning</source>
        <translation>Внимание</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Important</source>
        <translation>Важно</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Tip</source>
        <translation>Совет</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Note</source>
        <translation>Примечание</translation>
    </message>
</context>
<context>
    <name>MdCodeBlock</name>
    <message>
        <location filename="../src/qml/qml/markdown/MdCodeBlock.qml" line="+58" />
        <source>Copied!</source>
        <translation>Скопировано!</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Copy</source>
        <translation>Копировать</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceView</name>
    <message>
        <location filename="../src/qml/qml/MissingPreferenceView.qml" line="+28" />
        <source>Welcome to %1</source>
        <translation>Добро пожаловать в %1</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Before you can use this command, you need to fill in the required preference fields below.</source>
        <translation>Прежде чем использовать эту команду, заполните обязательные поля настроек ниже.</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceViewHost</name>
    <message>
        <location filename="../src/qml/missing-preference-view-host.cpp" line="+212" />
        <source>Save preferences</source>
        <translation>Сохранить настройки</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Please fill in all required fields</source>
        <translation>Заполните все обязательные поля</translation>
    </message>
</context>
<context>
    <name>NavigationController</name>
    <message>
        <location filename="../src/navigation-controller.cpp" line="+653" />
        <source>Extension manager is not running</source>
        <translation>Менеджер расширений не запущен</translation>
    </message>
</context>
<context>
    <name>NewsService</name>
    <message>
        <location filename="../src/services/news/news-service.cpp" line="+68" />
        <source>Telemetry</source>
        <translation>Телеметрия</translation>
    </message>
    <message>
        <location line="+1" />
        <source>We now collect basic usage statistics on startup</source>
        <translation>Теперь мы собираем базовую статистику использования при запуске</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Learn more</source>
        <translation>Подробнее</translation>
    </message>
</context>
<context>
    <name>NullUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/null-update-installer.hpp" line="+14" />
        <source>Self update is not supported on this platform</source>
        <translation>Самообновление не поддерживается на этой платформе</translation>
    </message>
</context>
<context>
    <name>OAuthOverlayView</name>
    <message>
        <location filename="../src/qml/qml/OAuthOverlayView.qml" line="+91" />
        <source>Continue with %1</source>
        <translation>Продолжить с %1</translation>
    </message>
    <message>
        <location line="+33" />
        <source>You're in!</source>
        <translation>Готово!</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Successfully connected to %1.
Back to command in an instant...</source>
        <translation>Успешно подключено к %1.
Скоро вернёмся к команде...</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-33" />
        <source>Manage OAuth Token Sets</source>
        <translation>Управление наборами OAuth-токенов</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Manage OAuth token sets that have been saved by extensions providing OAuth integrations.</source>
        <translation>Управление наборами OAuth-токенов, сохранёнными расширениями с OAuth-интеграциями.</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreSection</name>
    <message>
        <location filename="../src/qml/oauth-token-store-model.hpp" line="+17" />
        <source>OAuth Token Sets ({count})</source>
        <translation>Наборы OAuth-токенов ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/oauth-token-store-model.cpp" line="+20" />
        <source>Expired</source>
        <translation>Истёк</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Remove token set</source>
        <translation>Удалить набор токенов</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Are you sure?</source>
        <translation>Вы уверены?</translation>
    </message>
    <message>
        <location line="+1" />
        <source>You will need to go through the OAuth login flow again the next time you want to use this service</source>
        <translation>В следующий раз при использовании этого сервиса вам придётся снова пройти процедуру входа OAuth</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Failed to remove token set</source>
        <translation>Не удалось удалить набор токенов</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Token set removed</source>
        <translation>Набор токенов удалён</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Copy</source>
        <translation>Копировать</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Copy Access Token</source>
        <translation>Копировать токен доступа</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Copy Refresh Token</source>
        <translation>Копировать токен обновления</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Copy ID Token</source>
        <translation>Копировать ID-токен</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Copy Scopes</source>
        <translation>Копировать области</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Copy Expiration Date</source>
        <translation>Копировать срок действия</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreViewHost</name>
    <message>
        <location filename="../src/qml/oauth-token-store-view-host.cpp" line="+12" />
        <source>Search token sets...</source>
        <translation>Поиск наборов токенов...</translation>
    </message>
</context>
<context>
    <name>OnboardingWindow</name>
    <message>
        <location filename="../src/qml/qml/OnboardingWindow.qml" line="+36" />
        <source>Grant Access</source>
        <translation>Предоставить доступ</translation>
    </message>
    <message>
        <location line="+18" />
        <source>Granted</source>
        <translation>Предоставлено</translation>
    </message>
    <message>
        <location line="+16" />
        <location line="+39" />
        <source>Welcome to Vicinae</source>
        <translation>Добро пожаловать в Vicinae</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Let's set it up. It only takes a minute.</source>
        <translation>Давайте настроим. Это займёт минуту.</translation>
    </message>
    <message>
        <location line="+17" />
        <source>Permissions</source>
        <translation>Разрешения</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Vicinae needs additional permissions in order to make the best of your Mac.</source>
        <translation>Для полноценной работы на вашем Mac Vicinae нужны дополнительные разрешения.</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Accessibility</source>
        <translation>Специальные возможности</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Used to paste, expand snippets, and move windows.</source>
        <translation>Используется для вставки, раскрытия сниппетов и перемещения окон.</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Full Disk Access</source>
        <translation>Полный доступ к диску</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Notifications</source>
        <translation>Уведомления</translation>
    </message>
    <message>
        <location line="-8" />
        <source>Allows file search to cover your entire disk.</source>
        <translation>Позволяет поиску файлов охватывать весь диск.</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Allows extensions to send desktop notifications.</source>
        <translation>Позволяет расширениям отправлять настольные уведомления.</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Full disk access needs to be explicitly enabled if you want file search to cover all your files.</source>
        <translation>Полный доступ к диску нужно включить вручную, если вы хотите, чтобы поиск файлов охватывал все ваши файлы.</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Without accessibility access, paste, snippet expansion, and window management are unavailable.</source>
        <translation>Без доступа к специальным возможностям вставка, раскрытие сниппетов и управление окнами недоступны.</translation>
    </message>
    <message>
        <location line="+18" />
        <source>Make it your own</source>
        <translation>Настройте под себя</translation>
    </message>
    <message>
        <location line="+9" />
        <source>You will be able to change these settings later.</source>
        <translation>Эти настройки можно будет изменить позже.</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Theme</source>
        <translation>Тема</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Shared across the entire app.</source>
        <translation>Единая для всего приложения.</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Global hotkey</source>
        <translation>Глобальная горячая клавиша</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Opens the launcher from anywhere.</source>
        <translation>Открывает лаунчер из любого места.</translation>
    </message>
    <message>
        <location line="+16" />
        <source>Launch at login</source>
        <translation>Запускать при входе</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Starts Vicinae in the background at login.</source>
        <translation>Запускает Vicinae в фоне при входе в систему.</translation>
    </message>
    <message>
        <location line="+19" />
        <source>Setup complete</source>
        <translation>Настройка завершена</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Vicinae is running. Open the launcher with:</source>
        <translation>Vicinae работает. Откройте лаунчер с помощью:</translation>
    </message>
    <message>
        <location line="+17" />
        <source>Vicinae is open source software.</source>
        <translation>Vicinae — это программное обеспечение с открытым исходным кодом.</translation>
    </message>
    <message>
        <location line="+20" />
        <source>Sponsor</source>
        <translation>Поддержать</translation>
    </message>
    <message>
        <location line="+17" />
        <source>Back</source>
        <translation>Назад</translation>
    </message>
    <message>
        <location line="+38" />
        <source>Finish</source>
        <translation>Готово</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Continue</source>
        <translation>Продолжить</translation>
    </message>
</context>
<context>
    <name>OpenAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-56" />
        <source>Failed to start app</source>
        <translation>Не удалось запустить приложение</translation>
    </message>
</context>
<context>
    <name>OpenAppLocationAction</name>
    <message>
        <location line="-36" />
        <source>Open Location</source>
        <translation>Открыть расположение</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Failed to open app location</source>
        <translation>Не удалось открыть расположение приложения</translation>
    </message>
</context>
<context>
    <name>OpenBuiltinCommandAction</name>
    <message>
        <location filename="../src/command-actions.hpp" line="+17" />
        <source>Open command</source>
        <translation>Открыть команду</translation>
    </message>
</context>
<context>
    <name>OpenCalculatorHistoryAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+9" />
        <source>Open Calculator History</source>
        <translation>Открыть историю калькулятора</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-32" />
        <source>Open shortcut</source>
        <translation>Открыть ярлык</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutWithAction</name>
    <message>
        <location line="+85" />
        <source>Open with...</source>
        <translation>Открыть с помощью...</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelItemAction</name>
    <message>
        <location filename="../src/root-search/control-panel/control-panel-root-provider.cpp" line="+45" />
        <source>Failed to open settings</source>
        <translation>Не удалось открыть настройки</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelTaskAction</name>
    <message>
        <location line="+26" />
        <source>Failed to open settings</source>
        <translation>Не удалось открыть настройки</translation>
    </message>
</context>
<context>
    <name>OpenDefaultVicinaeConfig</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-117" />
        <source>Open Default Config File</source>
        <translation>Открыть файл конфигурации по умолчанию</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Open the default vicinae configuration file</source>
        <translation>Открыть файл конфигурации vicinae по умолчанию</translation>
    </message>
    <message>
        <location line="+16" />
        <source>Failed to open temporary file</source>
        <translation>Не удалось открыть временный файл</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Failed to open default config file</source>
        <translation>Не удалось открыть файл конфигурации по умолчанию</translation>
    </message>
</context>
<context>
    <name>OpenDiscordCommand</name>
    <message>
        <location line="-85" />
        <source>Join the Discord Server</source>
        <translation>Присоединиться к серверу в Discord</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Open link to join the official Vicinae discord server.</source>
        <translation>Открыть ссылку для вступления на официальный сервер Vicinae в Discord.</translation>
    </message>
</context>
<context>
    <name>OpenFileAction</name>
    <message>
        <location filename="../src/actions/files/file-actions.hpp" line="+18" />
        <source>Open with %1</source>
        <translation>Открыть с помощью %1</translation>
    </message>
</context>
<context>
    <name>OpenInBrowserAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="+105" />
        <source>Open in browser</source>
        <translation>Открыть в браузере</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+95" />
        <source>Failed to open in browser</source>
        <translation>Не удалось открыть в браузере</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Opened in browser</source>
        <translation>Открыто в браузере</translation>
    </message>
</context>
<context>
    <name>OpenInTerminalAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="-62" />
        <source>Open in %1</source>
        <translation>Открыть в %1</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-85" />
        <source>Failed to start app</source>
        <translation>Не удалось запустить приложение</translation>
    </message>
</context>
<context>
    <name>OpenItemPreferencesAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="-28" />
        <source>Open Preferences</source>
        <translation>Открыть настройки</translation>
    </message>
</context>
<context>
    <name>OpenRawProgramAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="+25" />
        <source>Execute program</source>
        <translation>Выполнить программу</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+35" />
        <source>Failed to start app</source>
        <translation>Не удалось запустить приложение</translation>
    </message>
</context>
<context>
    <name>OpenSettingsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+100" />
        <source>Open Vicinae Settings</source>
        <translation>Открыть настройки Vicinae</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Open the vicinae settings window, which is an independent floating window.</source>
        <translation>Открыть окно настроек vicinae — это отдельное плавающее окно.</translation>
    </message>
</context>
<context>
    <name>OpenSettingsPaneAction</name>
    <message>
        <location filename="../src/root-search/macos-settings/macos-settings-root-provider.mm" line="-134" />
        <source>Failed to open System Settings</source>
        <translation>Не удалось открыть Системные настройки</translation>
    </message>
</context>
<context>
    <name>OpenShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-132" />
        <source>No default app to open %1</source>
        <translation>Нет приложения по умолчанию для открытия %1</translation>
    </message>
    <message>
        <location line="+1" />
        <source>No app with id %1</source>
        <translation>Нет приложения с ID %1</translation>
    </message>
    <message>
        <location line="+12" />
        <location line="+7" />
        <source>Open shortcut</source>
        <translation>Открыть ярлык</translation>
    </message>
</context>
<context>
    <name>OpenShortcutFromSearchText</name>
    <message>
        <location line="+43" />
        <source>Open shortcut</source>
        <translation>Открыть ярлык</translation>
    </message>
</context>
<context>
    <name>OpenVicinaeConfig</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-76" />
        <source>Open Config File</source>
        <translation>Открыть файл конфигурации</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Open the main vicinae configuration file</source>
        <translation>Открыть основной файл конфигурации vicinae</translation>
    </message>
    <message>
        <location line="+17" />
        <source>Show Log File</source>
        <translation>Показать файл журнала</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Open the Vicinae log file in your file browser</source>
        <translation>Открыть файл журнала Vicinae в вашем файловом менеджере</translation>
    </message>
</context>
<context>
    <name>OpenWindowsSettingAction</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="+147" />
        <source>Failed to open settings</source>
        <translation>Не удалось открыть настройки</translation>
    </message>
</context>
<context>
    <name>OpenWithAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+54" />
        <source>Open with...</source>
        <translation>Открыть с помощью...</translation>
    </message>
</context>
<context>
    <name>PasteToFocusedWindowAction</name>
    <message>
        <location filename="../src/clipboard-actions.hpp" line="+15" />
        <source>Paste to active window</source>
        <translation>Вставить в активное окно</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Copy to focused window</source>
        <translation>Копировать в активное окно</translation>
    </message>
</context>
<context>
    <name>PinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+28" />
        <source>Entry pinned</source>
        <translation>Запись закреплена</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Pin entry</source>
        <translation>Закрепить запись</translation>
    </message>
</context>
<context>
    <name>PinClipboardAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="-27" />
        <source>Selection pinned</source>
        <translation>Выделение закреплено</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Selection unpinned</source>
        <translation>Выделение откреплено</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to change pin status</source>
        <translation>Не удалось изменить статус закрепления</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Pin</source>
        <translation>Закрепить</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Unpin</source>
        <translation>Открепить</translation>
    </message>
</context>
<context>
    <name>PinEmojiAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-324" />
        <source>Pin emoji</source>
        <translation>Закрепить эмодзи</translation>
    </message>
</context>
<context>
    <name>PinWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="+37" />
        <source>Unpin from all workspaces</source>
        <translation>Открепить от всех рабочих пространств</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Pin to all workspaces</source>
        <translation>Закрепить во всех рабочих пространствах</translation>
    </message>
</context>
<context>
    <name>PowerManagementCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-283" />
        <source>Ask for confirmation</source>
        <translation>Спрашивать подтверждение</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Custom program</source>
        <translation>Своя программа</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Custom POSIX shell command to run instead of the default implementation</source>
        <translation>Своя POSIX-команда оболочки вместо реализации по умолчанию</translation>
    </message>
    <message>
        <location line="+27" />
        <source>Failed to execute custom program %1</source>
        <translation>Не удалось выполнить свою программу %1</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Are you sure</source>
        <translation>Вы уверены</translation>
    </message>
    <message>
        <location line="+0" />
        <source>High-impact operation, please confirm</source>
        <translation>Операция с высоким влиянием, подтвердите действие</translation>
    </message>
</context>
<context>
    <name>PowerManagementExtension</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.hpp" line="+8" />
        <source>Power Management</source>
        <translation>Управление питанием</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Power off, suspend, sleep, hibernate your computer.</source>
        <translation>Выключение, приостановка, сон и гибернация вашего компьютера.</translation>
    </message>
</context>
<context>
    <name>PowerOffCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+137" />
        <source>Power Off System</source>
        <translation>Выключить систему</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Power off the system</source>
        <translation>Выключить систему</translation>
    </message>
    <message>
        <location line="+11" />
        <source>System cannot power off</source>
        <translation>Система не может выключиться</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Failed to power off</source>
        <translation>Не удалось выключить</translation>
    </message>
</context>
<context>
    <name>PreviewFontAction</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="-104" />
        <source>Preview font</source>
        <translation>Предпросмотр шрифта</translation>
    </message>
</context>
<context>
    <name>ProgramsSection</name>
    <message>
        <location filename="../src/qml/system-run-model.hpp" line="+21" />
        <source>Programs (%1)</source>
        <translation>Программы (%1)</translation>
    </message>
    <message>
        <location filename="../src/qml/system-run-model.cpp" line="+52" />
        <source>Open in %1 (hold)</source>
        <translation>Открыть в %1 (удерживать)</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Open in %1</source>
        <translation>Открыть в %1</translation>
    </message>
    <message>
        <location line="+20" />
        <source>Copy exec path</source>
        <translation>Копировать путь к исполняемому файлу</translation>
    </message>
</context>
<context>
    <name>ProviderSearchSection</name>
    <message>
        <location filename="../src/qml/provider-search-model.hpp" line="+11" />
        <source>Results ({count})</source>
        <translation>Результаты ({count})</translation>
    </message>
</context>
<context>
    <name>ProviderSearchViewHost</name>
    <message>
        <location filename="../src/qml/provider-search-view-host.cpp" line="+15" />
        <source>Search %1</source>
        <translation>Поиск: %1</translation>
    </message>
</context>
<context>
    <name>PutCalculatorAnswerInSearchBar</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-20" />
        <source>Put answer in search bar</source>
        <translation>Поместить ответ в строку поиска</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/utils/utils.cpp" line="+50" />
        <source>in the future</source>
        <translation>в будущем</translation>
    </message>
    <message numerus="yes">
        <location line="+8" />
        <source>%n year(s) ago</source>
        <translation>
            <numerusform>%n год назад</numerusform>
            <numerusform>%n года назад</numerusform>
            <numerusform>%n лет назад</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+3" />
        <source>%n month(s) ago</source>
        <translation>
            <numerusform>%n месяц назад</numerusform>
            <numerusform>%n месяца назад</numerusform>
            <numerusform>%n месяцев назад</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2" />
        <source>%n day(s) ago</source>
        <translation>
            <numerusform>%n день назад</numerusform>
            <numerusform>%n дня назад</numerusform>
            <numerusform>%n дней назад</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2" />
        <source>%n hour(s) ago</source>
        <translation>
            <numerusform>%n час назад</numerusform>
            <numerusform>%n часа назад</numerusform>
            <numerusform>%n часов назад</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2" />
        <source>%n minute(s) ago</source>
        <translation>
            <numerusform>%n минуту назад</numerusform>
            <numerusform>%n минуты назад</numerusform>
            <numerusform>%n минут назад</numerusform>
        </translation>
    </message>
    <message>
        <location line="+2" />
        <source>just now</source>
        <translation>только что</translation>
    </message>
</context>
<context>
    <name>QuitAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-43" />
        <source>Quit Application</source>
        <translation>Завершить приложение</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Failed to quit %1</source>
        <translation>Не удалось завершить %1</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Quit %1</source>
        <translation>Завершить %1</translation>
    </message>
</context>
<context>
    <name>RaycastCompatExtension</name>
    <message>
        <location filename="../src/extensions/raycast/raycast-compat-extension.hpp" line="+15" />
        <source>Raycast compatibility features</source>
        <translation>Функции совместимости с Raycast</translation>
    </message>
</context>
<context>
    <name>RaycastStoreCommand</name>
    <message>
        <location filename="../src/extensions/raycast/raycast-store-command.hpp" line="+13" />
        <source>Install compatible extensions from the Raycast store</source>
        <translation>Устанавливать совместимые расширения из магазина Raycast</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Always show intro</source>
        <translation>Всегда показывать вступление</translation>
    </message>
    <message>
        <location line="+11" />
        <source>
# Welcome to the Raycast Extension Store

Vicinae provides direct integration with the official [Raycast store](https://www.raycast.com/store), allowing you to search and install Raycast extensions directly from Vicinae.
</source>
        <translation>
# Добро пожаловать в магазин расширений Raycast

Vicinae имеет прямую интеграцию с официальным [магазином Raycast](https://www.raycast.com/store), что позволяет искать и устанавливать расширения Raycast прямо из Vicinae.
</translation>
    </message>
    <message>
        <location line="+6" />
        <source>
Each extension has a colored compatibility indicator showing how well it works on Linux.

Vicinae also has its own [extension store](vicinae://launch/core/store), which does not suffer from these limitations.
</source>
        <translation>
У каждого расширения есть цветной индикатор совместимости, показывающий, насколько хорошо оно работает в Linux.

У Vicinae также есть собственный [магазин расширений](vicinae://launch/core/store), у которого нет таких ограничений.
</translation>
    </message>
    <message>
        <location line="+6" />
        <source>
Vicinae also has its own [extension store](vicinae://launch/core/store).
</source>
        <translation>
У Vicinae также есть собственный [магазин расширений](vicinae://launch/core/store).
</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Continue to store</source>
        <translation>Перейти в магазин</translation>
    </message>
</context>
<context>
    <name>RaycastStoreDetailHost</name>
    <message>
        <location filename="../src/qml/raycast-store-detail-host.cpp" line="+43" />
        <source>Failed to load extension</source>
        <translation>Не удалось загрузить расширение</translation>
    </message>
    <message>
        <location line="+1" />
        <source>The extension "%1" could not be loaded. It may not exist or the store may be unreachable.</source>
        <translation>Расширение «%1» не удалось загрузить. Возможно, оно не существует или магазин недоступен.</translation>
    </message>
    <message>
        <location line="+27" />
        <source>Extension Store - %1</source>
        <translation>Магазин расширений - %1</translation>
    </message>
    <message>
        <location line="+31" />
        <source>This extension should be fully compatible.</source>
        <translation>Это расширение должно быть полностью совместимым.</translation>
    </message>
    <message>
        <location line="+4" />
        <source>This extension works but has a few quirks.</source>
        <translation>Это расширение работает, но имеет некоторые недочёты.</translation>
    </message>
    <message>
        <location line="+4" />
        <source>This extension is not compatible.</source>
        <translation>Это расширение несовместимо.</translation>
    </message>
    <message>
        <location line="+4" />
        <source>No compatibility data is available for this extension.</source>
        <translation>Нет данных о совместимости этого расширения.</translation>
    </message>
    <message>
        <location line="+21" />
        <source>No compatibility data is available — this extension may or may not work.</source>
        <translation>Данных о совместимости нет — это расширение может работать, а может и нет.</translation>
    </message>
    <message>
        <location line="+75" />
        <source>Extension Store</source>
        <translation>Магазин расширений</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Install extension</source>
        <translation>Установить расширение</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Downloading extension...</source>
        <translation>Загрузка расширения...</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Failed to download extension</source>
        <translation>Не удалось загрузить расширение</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Failed to extract extension archive</source>
        <translation>Не удалось распаковать архив расширения</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Extension installed</source>
        <translation>Расширение установлено</translation>
    </message>
    <message>
        <location line="+14" />
        <source>Report issue</source>
        <translation>Сообщить о проблеме</translation>
    </message>
</context>
<context>
    <name>RaycastStoreSection</name>
    <message>
        <location filename="../src/qml/raycast-store-model.cpp" line="+45" />
        <source>Show details</source>
        <translation>Показать подробности</translation>
    </message>
</context>
<context>
    <name>RaycastStoreViewHost</name>
    <message>
        <location filename="../src/qml/raycast-store-view-host.cpp" line="+37" />
        <source>Browse Raycast extensions</source>
        <translation>Просмотр расширений Raycast</translation>
    </message>
    <message>
        <location line="+32" />
        <source>Failed to fetch extensions</source>
        <translation>Не удалось получить расширения</translation>
    </message>
    <message>
        <location line="+18" />
        <source>Extensions</source>
        <translation>Расширения</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Failed to search extensions</source>
        <translation>Не удалось выполнить поиск расширений</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Results</source>
        <translation>Результаты</translation>
    </message>
</context>
<context>
    <name>RebootCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-72" />
        <source>Reboot System</source>
        <translation>Перезагрузить систему</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Reboot the system</source>
        <translation>Перезагрузить систему</translation>
    </message>
    <message>
        <location line="+12" />
        <source>System can't reboot</source>
        <translation>Система не может перезагрузиться</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Failed to reboot</source>
        <translation>Не удалось перезагрузить</translation>
    </message>
</context>
<context>
    <name>RebuildFileIndexCommand</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="-72" />
        <source>Rebuild File Index</source>
        <translation>Пересоздать индекс файлов</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Fully rebuild the file index. Running this manually can be useful if the file search feels particularly out of date.</source>
        <translation>Полностью пересоздать индекс файлов. Ручной запуск может быть полезен, если поиск файлов кажется особенно устаревшим.</translation>
    </message>
    <message>
        <location line="+13" />
        <source>Are you sure?</source>
        <translation>Вы уверены?</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Rebuilding the entire index can be time consuming and CPU intensive, depending on the number of files present in your home directory.</source>
        <translation>Пересоздание всего индекса может занять время и сильно нагрузить процессор — это зависит от числа файлов в вашем домашнем каталоге.</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Reset</source>
        <translation>Сбросить</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Index rebuild started...</source>
        <translation>Пересоздание индекса начато...</translation>
    </message>
</context>
<context>
    <name>RefreshAppsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/refresh-apps-command.hpp" line="+12" />
        <source>Refresh Apps</source>
        <translation>Обновить приложения</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Force a refresh of the application database. The database should normally automatically update itself on changes, but this can help working around some edge cases.</source>
        <translation>Принудительно обновить базу данных приложений. Обычно она обновляется сама при изменениях, но это может помочь в некоторых особых случаях.</translation>
    </message>
    <message>
        <location filename="../src/extensions/vicinae/refresh-apps-command.cpp" line="+15" />
        <source>Apps successfully refreshed</source>
        <translation>Приложения успешно обновлены</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to refresh apps</source>
        <translation>Не удалось обновить приложения</translation>
    </message>
</context>
<context>
    <name>ReloadScriptDirectoriesCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+76" />
        <source>Reload Script Directories</source>
        <translation>Перезагрузить каталоги скриптов</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Reload script directories</source>
        <translation>Перезагрузить каталоги скриптов</translation>
    </message>
    <message>
        <location line="+9" />
        <source>New scan triggered, index will update shortly</source>
        <translation>Запущено новое сканирование, индекс скоро обновится</translation>
    </message>
</context>
<context>
    <name>RemoveAllCalculatorHistoryRecordsAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+97" />
        <source>Delete all entries</source>
        <translation>Удалить все записи</translation>
    </message>
</context>
<context>
    <name>RemoveAllSelectionsAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+28" />
        <source>Are you sure?</source>
        <translation>Вы уверены?</translation>
    </message>
    <message>
        <location line="+1" />
        <source>All your clipboard history will be lost forever</source>
        <translation>Вся ваша история буфера обмена будет утеряна навсегда</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Delete all</source>
        <translation>Удалить всё</translation>
    </message>
    <message>
        <location line="+5" />
        <source>All selections were removed</source>
        <translation>Все выделения удалены</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to remove all selections</source>
        <translation>Не удалось удалить все выделения</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Remove all</source>
        <translation>Удалить все</translation>
    </message>
</context>
<context>
    <name>RemoveCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-43" />
        <source>Entry removed</source>
        <translation>Запись удалена</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to remove entry</source>
        <translation>Не удалось удалить запись</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Delete entry</source>
        <translation>Удалить запись</translation>
    </message>
</context>
<context>
    <name>RemoveSelectionAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="-71" />
        <source>Entry removed</source>
        <translation>Запись удалена</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to remove entry</source>
        <translation>Не удалось удалить запись</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Remove entry</source>
        <translation>Удалить запись</translation>
    </message>
</context>
<context>
    <name>RemoveShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="+33" />
        <source>Removed link</source>
        <translation>Ссылка удалена</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to remove link</source>
        <translation>Не удалось удалить ссылку</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Remove link</source>
        <translation>Удалить ссылку</translation>
    </message>
</context>
<context>
    <name>ReportVicinaeBugCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/report-bug-command.hpp" line="+10" />
        <source>Report a Vicinae Bug</source>
        <translation>Сообщить об ошибке Vicinae</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Navigate to Vicinae issue creation page with all relevant informations pre-filled.</source>
        <translation>Открыть страницу создания задачи Vicinae с предзаполненной необходимой информацией.</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Title</source>
        <translation>Заголовок</translation>
    </message>
</context>
<context>
    <name>ResetEmojiRankingAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+21" />
        <source>Reset ranking</source>
        <translation>Сбросить рейтинг</translation>
    </message>
</context>
<context>
    <name>ResetEmojiSkinToneAction</name>
    <message>
        <location line="+34" />
        <source>Reset to preference</source>
        <translation>Сбросить к настройке</translation>
    </message>
</context>
<context>
    <name>ResetItemRanking</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="-25" />
        <source>Ranking was successfully reset</source>
        <translation>Рейтинг успешно сброшен</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Unable to reset ranking</source>
        <translation>Не удалось сбросить рейтинг</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Are you sure?</source>
        <translation>Вы уверены?</translation>
    </message>
    <message>
        <location line="+2" />
        <source>You will have to rebuild search history for this item in order for it to reappear on top of the root search results.</source>
        <translation>Чтобы этот элемент снова появился вверху результатов корневого поиска, вам придётся перестроить его историю поиска.</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Reset</source>
        <translation>Сбросить</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Reset ranking</source>
        <translation>Сбросить рейтинг</translation>
    </message>
</context>
<context>
    <name>RevealFileInFolderAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+26" />
        <source>Show in file browser</source>
        <translation>Показать в файловом менеджере</translation>
    </message>
    <message>
        <location line="+13" />
        <source>Failed to open folder</source>
        <translation>Не удалось открыть папку</translation>
    </message>
</context>
<context>
    <name>RootCalculatorSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+91" />
        <source>Calculator</source>
        <translation>Калькулятор</translation>
    </message>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="+175" />
        <source>Copy unformatted answer</source>
        <translation>Копировать ответ без форматирования</translation>
    </message>
</context>
<context>
    <name>RootFallbackSection</name>
    <message>
        <location line="+260" />
        <source>Use "%1" with...</source>
        <translation>Использовать «%1» с...</translation>
    </message>
</context>
<context>
    <name>RootFavoritesSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+67" />
        <source>Favorites</source>
        <translation>Избранное</translation>
    </message>
</context>
<context>
    <name>RootFilesSection</name>
    <message>
        <location line="+50" />
        <source>Files</source>
        <translation>Файлы</translation>
    </message>
</context>
<context>
    <name>RootLinkSection</name>
    <message>
        <location line="-139" />
        <source>Link</source>
        <translation>Ссылка</translation>
    </message>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="-313" />
        <source>Open in %1</source>
        <translation>Открыть в %1</translation>
    </message>
</context>
<context>
    <name>RootNewsSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+66" />
        <source>What's New</source>
        <translation>Что нового</translation>
    </message>
</context>
<context>
    <name>RootResultsSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="+207" />
        <source>Suggestions</source>
        <translation>Рекомендации</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Results (%1)</source>
        <translation>Результаты (%1)</translation>
    </message>
</context>
<context>
    <name>RootSearchActionGenerator</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+71" />
        <source>Copy ID</source>
        <translation>Копировать ID</translation>
    </message>
</context>
<context>
    <name>RootShortcutItem</name>
    <message>
        <location filename="../src/root-search/shortcuts/shortcut-root-provider.cpp" line="+69" />
        <location line="+11" />
        <source>Shortcut</source>
        <translation>Ярлык</translation>
    </message>
</context>
<context>
    <name>RootUpdateSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="-21" />
        <location filename="../src/qml/root-search-sources.cpp" line="-125" />
        <source>Update</source>
        <translation>Обновление</translation>
    </message>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="-14" />
        <source>Vicinae %1 is available</source>
        <translation>Доступна версия Vicinae %1</translation>
    </message>
    <message>
        <location line="+5" />
        <source>You are running %1</source>
        <translation>У вас запущена версия %1</translation>
    </message>
    <message>
        <location line="+27" />
        <source>View Release Notes</source>
        <translation>Смотреть заметки о выпуске</translation>
    </message>
</context>
<context>
    <name>RootViewHost</name>
    <message>
        <location filename="../src/qml/root-view-host.hpp" line="+15" />
        <source>Search for anything...</source>
        <translation>Найти что угодно…</translation>
    </message>
</context>
<context>
    <name>ScriptExecutorViewHost</name>
    <message>
        <location filename="../src/qml/script-executor-view-host.cpp" line="+76" />
        <source>Script execution failed: %1</source>
        <translation>Ошибка выполнения скрипта: %1</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Running... (%1s ago)</source>
        <translation>Выполнение... (%1 с назад)</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Done in %1s (exit=%2)</source>
        <translation>Готово за %1 с (код выхода=%2)</translation>
    </message>
    <message>
        <location line="+12" />
        <location line="+9" />
        <source>Script process killed</source>
        <translation>Процесс скрипта завершён</translation>
    </message>
    <message>
        <location line="+20" />
        <source>Running...</source>
        <translation>Выполнение...</translation>
    </message>
    <message>
        <location line="+15" />
        <source>Kill process</source>
        <translation>Завершить процесс</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Run script again</source>
        <translation>Запустить скрипт снова</translation>
    </message>
</context>
<context>
    <name>ScriptRootItem</name>
    <message>
        <location filename="../src/root-search/scripts/script-root-provider.hpp" line="+27" />
        <location line="+86" />
        <source>Script</source>
        <translation>Скрипт</translation>
    </message>
    <message>
        <location line="-43" />
        <source>Mode</source>
        <translation>Режим</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Path</source>
        <translation>Путь</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Author</source>
        <translation>Автор</translation>
    </message>
    <message>
        <location line="+23" />
        <source>Open script directory</source>
        <translation>Открыть каталог скриптов</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Copy path to script</source>
        <translation>Копировать путь к скрипту</translation>
    </message>
</context>
<context>
    <name>ScriptRootProvider</name>
    <message>
        <location line="+47" />
        <source>Script Commands</source>
        <translation>Скриптовые команды</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Custom directories</source>
        <translation>Свои каталоги</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Additional list of directories to source scripts from. These directories always take precedence over the default system ones</source>
        <translation>Дополнительный список каталогов для загрузки скриптов. Эти каталоги всегда приоритетнее системных по умолчанию.</translation>
    </message>
</context>
<context>
    <name>SearchBrowserTabsCommand</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="+15" />
        <source>Search Browser Tabs</source>
        <translation>Поиск вкладок браузера</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Search tabs from all connected browsers</source>
        <translation>Поиск вкладок во всех подключённых браузерах</translation>
    </message>
</context>
<context>
    <name>SearchEmojiCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/search-emoji-command.hpp" line="+15" />
        <source>Search Emojis &amp; Symbols</source>
        <translation>Поиск эмодзи и символов</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Search for any emoji or symbol</source>
        <translation>Поиск любого эмодзи или символа</translation>
    </message>
    <message>
        <location line="+13" />
        <source>Paste</source>
        <translation>Вставить</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Copy</source>
        <translation>Копировать</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Default Action</source>
        <translation>Действие по умолчанию</translation>
    </message>
    <message>
        <location line="+1" />
        <source>The default action to perform on pressing return. Paste is only available if your environment supports it.</source>
        <translation>Действие, выполняемое по нажатию Enter. Вставка доступна только при поддержке её вашим окружением.</translation>
    </message>
    <message>
        <location line="+21" />
        <source>Skin tone</source>
        <translation>Тон кожи</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Skin tone to use for relevant emojis.</source>
        <translation>Тон кожи для соответствующих эмодзи.</translation>
    </message>
</context>
<context>
    <name>SearchEmojiGridSource</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.hpp" line="-22" />
        <source>Results (%1)</source>
        <translation>Результаты (%1)</translation>
    </message>
</context>
<context>
    <name>SearchFilesCommand</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="-38" />
        <source>Search Files</source>
        <translation>Поиск файлов</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Search files on your system</source>
        <translation>Поиск файлов на вашей системе</translation>
    </message>
</context>
<context>
    <name>SearchFilesView</name>
    <message>
        <location filename="../src/qml/qml/SearchFilesView.qml" line="+37" />
        <source>Name</source>
        <translation>Название</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Path</source>
        <translation>Путь</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Type</source>
        <translation>Тип</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Last modified</source>
        <translation>Изменён</translation>
    </message>
</context>
<context>
    <name>SearchFilesViewHost</name>
    <message>
        <location filename="../src/qml/search-files-view-host.cpp" line="+59" />
        <source>Search for files...</source>
        <translation>Поиск файлов...</translation>
    </message>
    <message>
        <location line="+31" />
        <location line="+4" />
        <source>Direct file path</source>
        <translation>Прямой путь к файлу</translation>
    </message>
    <message>
        <location line="+18" />
        <source>Recently Accessed</source>
        <translation>Недавно открытые</translation>
    </message>
    <message>
        <location line="+30" />
        <source>Results</source>
        <translation>Результаты</translation>
    </message>
    <message>
        <location line="+40" />
        <source>All</source>
        <translation>Все</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Other</source>
        <translation>Другое</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Directories</source>
        <translation>Каталоги</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Images</source>
        <translation>Изображения</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Videos</source>
        <translation>Видео</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Audio</source>
        <translation>Аудио</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Documents</source>
        <translation>Документы</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Archives</source>
        <translation>Архивы</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Applications</source>
        <translation>Приложения</translation>
    </message>
</context>
<context>
    <name>SetAppFont</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="-15" />
        <source>Set as vicinae font</source>
        <translation>Использовать как шрифт vicinae</translation>
    </message>
</context>
<context>
    <name>SetDefaultTerminal</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="+265" />
        <source>Set Default Terminal</source>
        <translation>Задать терминал по умолчанию</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Change the default system terminal</source>
        <translation>Изменить системный терминал по умолчанию</translation>
    </message>
</context>
<context>
    <name>SetRootItemAliasAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="-34" />
        <source>Set alias</source>
        <translation>Задать алиас</translation>
    </message>
</context>
<context>
    <name>SetRootItemShortcutAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="+83" />
        <source>Set Global Shortcut</source>
        <translation>Задать глобальное сочетание клавиш</translation>
    </message>
</context>
<context>
    <name>SetThemeAction</name>
    <message>
        <location filename="../src/actions/theme/theme-actions.cpp" line="+11" />
        <source>Theme successfully updated</source>
        <translation>Тема успешно обновлена</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Set theme</source>
        <translation>Применить тему</translation>
    </message>
</context>
<context>
    <name>SetThemeCommand</name>
    <message>
        <location filename="../src/extensions/theme/set-theme-command.hpp" line="+9" />
        <source>Set Theme</source>
        <translation>Задать тему</translation>
    </message>
</context>
<context>
    <name>SetVolumeCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-49" />
        <source>Set Volume to %1%</source>
        <translation>Установить громкость на %1%</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Set system volume to %1%</source>
        <translation>Установить системную громкость на %1%</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Failed to set volume</source>
        <translation>Не удалось установить громкость</translation>
    </message>
</context>
<context>
    <name>SetWallpaperAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+17" />
        <source>Set as wallpaper</source>
        <translation>Установить как обои</translation>
    </message>
    <message>
        <location line="+12" />
        <source>Wallpaper set</source>
        <translation>Обои установлены</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to set wallpaper</source>
        <translation>Не удалось установить обои</translation>
    </message>
</context>
<context>
    <name>SettingsSidebar</name>
    <message>
        <location filename="../src/qml/qml/SettingsSidebar.qml" line="+124" />
        <source>Search...</source>
        <translation>Поиск...</translation>
    </message>
</context>
<context>
    <name>SettingsSidebarModel</name>
    <message>
        <location filename="../src/qml/settings-sidebar-model.cpp" line="+90" />
        <source>General</source>
        <translation>Общие</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Appearance</source>
        <translation>Внешний вид</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Keybindings</source>
        <translation>Горячие клавиши</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Advanced</source>
        <translation>Дополнительно</translation>
    </message>
    <message>
        <location line="+1" />
        <source>About</source>
        <translation>О приложении</translation>
    </message>
</context>
<context>
    <name>SettingsWindow</name>
    <message>
        <location filename="../src/qml/qml/SettingsWindow.qml" line="+11" />
        <source>General</source>
        <translation>Общие</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Appearance</source>
        <translation>Внешний вид</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Keybindings</source>
        <translation>Горячие клавиши</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Advanced</source>
        <translation>Дополнительно</translation>
    </message>
    <message>
        <location line="+4" />
        <source>About</source>
        <translation>О приложении</translation>
    </message>
    <message>
        <location line="+27" />
        <source>Vicinae Settings</source>
        <translation>Настройки Vicinae</translation>
    </message>
    <message>
        <location line="+194" />
        <source>Imported from Raycast</source>
        <translation>Импортировано из Raycast</translation>
    </message>
    <message>
        <location line="+2" />
        <source>From the Vicinae store</source>
        <translation>Из магазина Vicinae</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Locally installed extension</source>
        <translation>Расширение, установленное локально</translation>
    </message>
</context>
<context>
    <name>ShortcutExtension</name>
    <message>
        <location filename="../src/extensions/shortcut/shortcut-extension.hpp" line="+11" />
        <source>Manage Shortcuts</source>
        <translation>Управление ярлыками</translation>
    </message>
</context>
<context>
    <name>ShortcutField</name>
    <message>
        <location filename="../src/qml/qml/ShortcutField.qml" line="+14" />
        <source>Record shortcut</source>
        <translation>Записать сочетание клавиш</translation>
    </message>
</context>
<context>
    <name>ShortcutFormView</name>
    <message>
        <location filename="../src/qml/qml/ShortcutFormView.qml" line="+14" />
        <source>Name</source>
        <translation>Название</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Shortcut Name</source>
        <translation>Название ярлыка</translation>
    </message>
    <message>
        <location line="+9" />
        <source>The URL that will be opened by the specified app. You can make it dynamic by using placeholders such as {argument}.</source>
        <translation>URL, который будет открыт указанным приложением. Вы можете сделать его динамическим с помощью плейсхолдеров, например {argument}.</translation>
    </message>
    <message>
        <location line="+22" />
        <source>Open with</source>
        <translation>Открыть с помощью</translation>
    </message>
    <message>
        <location line="+13" />
        <source>Icon</source>
        <translation>Значок</translation>
    </message>
</context>
<context>
    <name>ShortcutFormViewHost</name>
    <message>
        <location filename="../src/qml/shortcut-form-view-host.cpp" line="+48" />
        <source>Submit</source>
        <translation>Отправить</translation>
    </message>
    <message>
        <location line="+13" />
        <source>Copy of %1</source>
        <translation>Копия %1</translation>
    </message>
    <message>
        <location line="+45" />
        <source>Edit "%1"</source>
        <translation>Изменить «%1»</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Duplicate "%1"</source>
        <translation>Дублировать «%1»</translation>
    </message>
    <message>
        <location line="+9" />
        <location line="+109" />
        <location line="+43" />
        <source>Default</source>
        <translation>По умолчанию</translation>
    </message>
    <message>
        <location line="-136" />
        <source>Selected Text</source>
        <translation>Выделенный текст</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Clipboard Text</source>
        <translation>Текст из буфера обмена</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Argument</source>
        <translation>Аргумент</translation>
    </message>
    <message>
        <location line="+25" />
        <location line="+5" />
        <location line="+5" />
        <source>Required</source>
        <translation>Обязательно</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Validation failed</source>
        <translation>Проверка не пройдена</translation>
    </message>
    <message>
        <location line="+12" />
        <source>Failed to update shortcut</source>
        <translation>Не удалось обновить ярлык</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Shortcut updated</source>
        <translation>Ярлык обновлён</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Failed to create shortcut</source>
        <translation>Не удалось создать ярлык</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Shortcut created</source>
        <translation>Ярлык создан</translation>
    </message>
</context>
<context>
    <name>ShortcutRecorderCapture</name>
    <message>
        <location filename="../src/qml/qml/ShortcutRecorderCapture.qml" line="+26" />
        <location line="+10" />
        <location line="+32" />
        <source>Recording...</source>
        <translation>Запись...</translation>
    </message>
    <message>
        <location line="+14" />
        <source>Keybind updated</source>
        <translation>Привязка клавиш обновлена</translation>
    </message>
</context>
<context>
    <name>ShortcutRecorderPanel</name>
    <message>
        <location filename="../src/qml/qml/ShortcutRecorderPanel.qml" line="+106" />
        <source>Press Backspace to remove the current shortcut</source>
        <translation>Нажмите Backspace, чтобы удалить текущее сочетание клавиш</translation>
    </message>
</context>
<context>
    <name>ShortcutRootProvider</name>
    <message>
        <location filename="../src/root-search/shortcuts/shortcut-root-provider.cpp" line="+42" />
        <source>Shortcuts</source>
        <translation>Ярлыки</translation>
    </message>
</context>
<context>
    <name>ShortcutsSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/ShortcutsSettingsPage.qml" line="+59" />
        <source>Keybindings</source>
        <translation>Горячие клавиши</translation>
    </message>
    <message>
        <location line="+78" />
        <source>Record Shortcut</source>
        <translation>Записать сочетание клавиш</translation>
    </message>
</context>
<context>
    <name>SkipUpdateVersionAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+8" />
        <source>Skip This Version</source>
        <translation>Пропустить эту версию</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Skipped %1</source>
        <translation>Пропущено %1</translation>
    </message>
</context>
<context>
    <name>SleepCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+94" />
        <source>Put System to Sleep</source>
        <translation>Перевести систему в спящий режим</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Put system to sleep</source>
        <translation>Перевести систему в спящий режим</translation>
    </message>
    <message>
        <location line="+10" />
        <source>System can't sleep</source>
        <translation>Система не может уснуть</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Failed to sleep</source>
        <translation>Не удалось перевести в спящий режим</translation>
    </message>
</context>
<context>
    <name>SnippetDatabase</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.cpp" line="+42" />
        <location line="+56" />
        <source>keyword already assigned to "%1"</source>
        <translation>ключевое слово уже назначено «%1»</translation>
    </message>
    <message>
        <location line="-43" />
        <source>No snippet with that ID</source>
        <translation>Нет сниппета с таким ID</translation>
    </message>
    <message>
        <location line="+15" />
        <source>No such snippet</source>
        <translation>Нет такого сниппета</translation>
    </message>
    <message>
        <location line="+22" />
        <source>Snippet limit reached (%1)</source>
        <translation>Достигнут лимит сниппетов (%1)</translation>
    </message>
    <message>
        <location line="+26" />
        <source>Failed to save snippets on disk: %1</source>
        <translation>Не удалось сохранить сниппеты на диск: %1</translation>
    </message>
</context>
<context>
    <name>SnippetExtension</name>
    <message>
        <location filename="../src/extensions/snippet/snippet-extension.cpp" line="+38" />
        <source>Expansion</source>
        <translation>Раскрытие</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Enable automatic snippet expansion when triggers are typed</source>
        <translation>Автоматически раскрывать сниппеты при вводе триггеров</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Undo</source>
        <translation>Отменить</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Press backspace immediately after expansion to undo and restore the trigger text</source>
        <translation>Нажмите Backspace сразу после раскрытия, чтобы отменить его и восстановить текст триггера</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Keyboard layout</source>
        <translation>Раскладка клавиатуры</translation>
    </message>
    <message>
        <location line="+2" />
        <source>XKB layout used for trigger detection (e.g. "us", "fr"). Leave empty for system default.</source>
        <translation>Раскладка XKB для определения триггеров (например, «us», «fr»). Оставьте пустым для системной по умолчанию.</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Pre-paste delay (ms)</source>
        <translation>Задержка перед вставкой (мс)</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Delay between setting clipboard and injecting paste shortcut. Increase if expansions paste empty on slow compositors.</source>
        <translation>Задержка между установкой буфера обмена и нажатием сочетания вставки. Увеличьте, если на медленных композиторах вставка после раскрытия пустая.</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Key injection delay (ms)</source>
        <translation>Задержка ввода клавиш (мс)</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Delay between injected key events. Increase if expansions produce missing or garbled characters on slow compositors.</source>
        <translation>Задержка между вводимыми нажатиями клавиш. Увеличьте, если на медленных композиторах в расширении теряются или искажаются символы.</translation>
    </message>
    <message>
        <location filename="../src/extensions/snippet/snippet-extension.hpp" line="+11" />
        <source>Snippets</source>
        <translation>Сниппеты</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Text expansion and snippet management</source>
        <translation>Текстовое раскрытие и управление сниппетами</translation>
    </message>
</context>
<context>
    <name>SnippetFormView</name>
    <message>
        <location filename="../src/qml/qml/SnippetFormView.qml" line="+15" />
        <source>Title</source>
        <translation>Название</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Euro symbol</source>
        <translation>Символ евро</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Content</source>
        <translation>Содержимое</translation>
    </message>
    <message>
        <location line="+2" />
        <source>You can use {dynamic placeholders} to make the content dynamic: &lt;a href="https://docs.vicinae.com/snippets"&gt;learn more&lt;/a&gt;.</source>
        <translation>Вы можете использовать {dynamic placeholders}, чтобы сделать содержимое динамическим: &lt;a href="https://docs.vicinae.com/snippets"&gt;узнать больше&lt;/a&gt;.</translation>
    </message>
    <message>
        <location line="+16" />
        <source>Keyword</source>
        <translation>Ключевое слово</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Typing this keyword anywhere will result in it being replaced by the content of the snippet.</source>
        <translation>Ввод этого ключевого слова в любом месте приведёт к его замене на содержимое сниппета.</translation>
    </message>
    <message>
        <location line="+0" />
        <source>The snippet server is not running. Keyword expansion is unavailable. &lt;a href="https://docs.vicinae.com/snippets"&gt;Learn more&lt;/a&gt;.</source>
        <translation>Сервер сниппетов не запущен. Раскрытие по ключевым словам недоступно. &lt;a href="https://docs.vicinae.com/snippets"&gt;Узнать больше&lt;/a&gt;.</translation>
    </message>
    <message>
        <location line="+12" />
        <source>Applications</source>
        <translation>Приложения</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Restrict expansion to specific applications. By default, it works everywhere.</source>
        <translation>Ограничить раскрытие определёнными приложениями. По умолчанию оно работает везде.</translation>
    </message>
    <message>
        <location line="+12" />
        <source>Expand as word</source>
        <translation>Раскрывать как слово</translation>
    </message>
    <message>
        <location line="+1" />
        <source>If a keyword is typed, it will only be expanded after space or punctuation.</source>
        <translation>Если введено ключевое слово, оно будет раскрыто только после пробела или знака препинания.</translation>
    </message>
</context>
<context>
    <name>SnippetFormViewHost</name>
    <message>
        <location filename="../src/qml/snippet-form-view-host.cpp" line="+42" />
        <source>Submit</source>
        <translation>Отправить</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Copy of %1</source>
        <translation>Копия %1</translation>
    </message>
    <message>
        <location line="+24" />
        <source>Edit "%1"</source>
        <translation>Изменить «%1»</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Duplicate "%1"</source>
        <translation>Дублировать «%1»</translation>
    </message>
    <message>
        <location line="+15" />
        <source>2 chars min.</source>
        <translation>Минимум 2 символа.</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Content should not be empty</source>
        <translation>Содержимое не должно быть пустым</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Only one {cursor} placeholder is allowed</source>
        <translation>Допускается только один плейсхолдер {cursor}</translation>
    </message>
    <message>
        <location line="+14" />
        <source>Validation failed</source>
        <translation>Проверка не пройдена</translation>
    </message>
    <message>
        <location line="+26" />
        <source>Snippet updated</source>
        <translation>Сниппет обновлён</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Snippet successfully created</source>
        <translation>Сниппет успешно создан</translation>
    </message>
    <message>
        <location line="+12" />
        <source>Cursor Position</source>
        <translation>Позиция курсора</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Clipboard Text</source>
        <translation>Текст из буфера обмена</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Date</source>
        <translation>Дата</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Argument</source>
        <translation>Аргумент</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Shell Command</source>
        <translation>Команда оболочки</translation>
    </message>
</context>
<context>
    <name>SoftRebootCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-99" />
        <source>Soft Reboot System</source>
        <translation>Мягкая перезагрузка системы</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Soft reboot the system, which usually means only userspace is rebooted.</source>
        <translation>Мягкая перезагрузка системы, обычно означающая перезапуск только пользовательского пространства.</translation>
    </message>
    <message>
        <location line="+13" />
        <source>System can't soft reboot</source>
        <translation>Система не может выполнить мягкую перезагрузку</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Failed to soft reboot</source>
        <translation>Не удалось выполнить мягкую перезагрузку</translation>
    </message>
</context>
<context>
    <name>SponsorVicinaeCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-118" />
        <source>Donate to Vicinae</source>
        <translation>Пожертвовать Vicinae</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Open link to Vicinae's GitHub sponsor page</source>
        <translation>Открыть ссылку на страницу спонсорства Vicinae на GitHub</translation>
    </message>
</context>
<context>
    <name>StoreDetailView</name>
    <message>
        <location filename="../src/qml/qml/StoreDetailView.qml" line="+201" />
        <source>Installed</source>
        <translation>Установлено</translation>
    </message>
    <message>
        <location line="+166" />
        <source>Description</source>
        <translation>Описание</translation>
    </message>
    <message>
        <location line="+26" />
        <source>Commands</source>
        <translation>Команды</translation>
    </message>
    <message>
        <location line="+74" />
        <source>Open README</source>
        <translation>Открыть README</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Last update</source>
        <translation>Последнее обновление</translation>
    </message>
    <message>
        <location line="+14" />
        <source>Contributors</source>
        <translation>Авторы</translation>
    </message>
    <message>
        <location line="+29" />
        <source>Categories</source>
        <translation>Категории</translation>
    </message>
    <message>
        <location line="+19" />
        <source>Source Code</source>
        <translation>Исходный код</translation>
    </message>
    <message>
        <location line="+3" />
        <source>View Code</source>
        <translation>Просмотреть код</translation>
    </message>
</context>
<context>
    <name>SuspendCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+36" />
        <source>Suspend System</source>
        <translation>Приостановить систему</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Suspend the system to RAM. Unlike hibernation, this does not turn the computer off and will break on power loss.</source>
        <translation>Приостановить систему с сохранением в ОЗУ. В отличие от гибернации, компьютер не выключается, и при отключении питания данные будут потеряны.</translation>
    </message>
    <message>
        <location line="+13" />
        <source>System cannot suspend</source>
        <translation>Система не может приостановиться</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Failed to suspend</source>
        <translation>Не удалось приостановить</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsCommand</name>
    <message>
        <location filename="../src/extensions/wm/wm-extension.cpp" line="+89" />
        <source>Switch Windows</source>
        <translation>Переключение окон</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsSection</name>
    <message>
        <location filename="../src/qml/switch-windows-model.hpp" line="+28" />
        <source>Open Windows</source>
        <translation>Открытые окна</translation>
    </message>
    <message>
        <location filename="../src/qml/switch-windows-model.cpp" line="+19" />
        <source>WS %1</source>
        <translation>РП %1</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Window Actions</source>
        <translation>Действия с окном</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsViewHost</name>
    <message>
        <location filename="../src/qml/switch-windows-view-host.cpp" line="+12" />
        <source>Search open window...</source>
        <translation>Поиск открытого окна...</translation>
    </message>
</context>
<context>
    <name>SwitchWorkspacesCommand</name>
    <message>
        <location filename="../src/extensions/wm/wm-extension.cpp" line="+10" />
        <source>Switch Workspaces</source>
        <translation>Переключение рабочих пространств</translation>
    </message>
</context>
<context>
    <name>SwitchWorkspacesSection</name>
    <message>
        <location filename="../src/qml/switch-workspaces-model.hpp" line="+47" />
        <source>%1</source>
        <translation>%1</translation>
    </message>
    <message numerus="yes">
        <location line="+3" />
        <source>%n window(s)</source>
        <translation>
            <numerusform>%n окно</numerusform><numerusform>%n окна</numerusform><numerusform>%n окон</numerusform></translation>
    </message>
    <message>
        <location line="+0" />
        <source>empty</source>
        <translation>пусто</translation>
    </message>
    <message>
        <location line="+21" />
        <source>Switch to workspace</source>
        <translation>Переключиться на рабочее пространство</translation>
    </message>
</context>
<context>
    <name>SwitchWorkspacesViewHost</name>
    <message>
        <location filename="../src/qml/switch-workspaces-view-host.hpp" line="+16" />
        <source>Open Workspaces</source>
        <translation>Открытые рабочие пространства</translation>
    </message>
    <message>
        <location filename="../src/qml/switch-workspaces-view-host.cpp" line="+12" />
        <source>Search workspaces...</source>
        <translation>Поиск рабочих пространств...</translation>
    </message>
</context>
<context>
    <name>SystemBrowseApps</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-123" />
        <source>Browse Apps</source>
        <translation>Обзор приложений</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Browse all applications that are installed on the system</source>
        <translation>Обзор всех приложений, установленных в системе</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Show hidden apps</source>
        <translation>Показывать скрытые приложения</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Sort alphabetically</source>
        <translation>Сортировать по алфавиту</translation>
    </message>
</context>
<context>
    <name>SystemExtension</name>
    <message>
        <location line="+163" />
        <source>System</source>
        <translation>Система</translation>
    </message>
    <message>
        <location line="+1" />
        <source>System-related commands</source>
        <translation>Команды, связанные с системой</translation>
    </message>
</context>
<context>
    <name>SystemRunCommand</name>
    <message>
        <location line="-253" />
        <source>Run Terminal Program</source>
        <translation>Запустить программу в терминале</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Run a program in a terminal window</source>
        <translation>Запустить программу в окне терминала</translation>
    </message>
    <message>
        <location line="+7" />
        <source>command</source>
        <translation>команда</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Run in terminal</source>
        <translation>Запустить в терминале</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Run in terminal (hold)</source>
        <translation>Запустить в терминале (удерживать)</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Run directly</source>
        <translation>Запустить напрямую</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Default Action</source>
        <translation>Действие по умолчанию</translation>
    </message>
    <message>
        <location line="+1" />
        <source>The default action to run on pressing return</source>
        <translation>Действие, выполняемое по нажатию Enter</translation>
    </message>
    <message>
        <location line="+29" />
        <source>Not a valid executable</source>
        <translation>Не является допустимым исполняемым файлом</translation>
    </message>
</context>
<context>
    <name>SystemRunViewHost</name>
    <message>
        <location filename="../src/qml/system-run-view-host.cpp" line="+20" />
        <source>Search for a program to execute...</source>
        <translation>Поиск программы для запуска...</translation>
    </message>
</context>
<context>
    <name>ThemeExtension</name>
    <message>
        <location filename="../src/extensions/theme/theme-extension.hpp" line="+9" />
        <source>Theme</source>
        <translation>Тема</translation>
    </message>
</context>
<context>
    <name>ThemeSection</name>
    <message>
        <location filename="../src/qml/theme-list-model.cpp" line="+22" />
        <source>Default theme description</source>
        <translation>Описание темы по умолчанию</translation>
    </message>
    <message>
        <location line="+64" />
        <source>Open theme file</source>
        <translation>Открыть файл темы</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Copy ID</source>
        <translation>Копировать ID</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Copy path</source>
        <translation>Копировать путь</translation>
    </message>
</context>
<context>
    <name>ThemeViewHost</name>
    <message>
        <location filename="../src/qml/theme-view-host.cpp" line="+22" />
        <source>Search for a theme...</source>
        <translation>Поиск темы...</translation>
    </message>
    <message>
        <location line="+53" />
        <source>Current Theme</source>
        <translation>Текущая тема</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Available Themes</source>
        <translation>Доступные темы</translation>
    </message>
</context>
<context>
    <name>ToggleFloatingWindowCommand</name>
    <message>
        <location filename="../src/extensions/wm/wm-extension.cpp" line="-51" />
        <source>Toggle Floating</source>
        <translation>Плавающий режим (вкл/выкл)</translation>
    </message>
    <message>
        <location line="+13" />
        <source>Active window is not on the current workspace</source>
        <translation>Активное окно не находится в текущем рабочем пространстве</translation>
    </message>
    <message>
        <location line="+5" />
        <source>No window to toggle</source>
        <translation>Нет окна для переключения</translation>
    </message>
</context>
<context>
    <name>ToggleFullscreenWindowCommand</name>
    <message>
        <location line="-44" />
        <source>Toggle Fullscreen</source>
        <translation>Полноэкранный режим (вкл/выкл)</translation>
    </message>
    <message>
        <location line="+13" />
        <source>Active window is not on the current workspace</source>
        <translation>Активное окно не находится в текущем рабочем пространстве</translation>
    </message>
    <message>
        <location line="+5" />
        <source>No window to fullscreen</source>
        <translation>Нет окна для перевода в полноэкранный режим</translation>
    </message>
</context>
<context>
    <name>ToggleItemAsFavorite</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="-66" />
        <source>Remove from favorites</source>
        <translation>Удалить из избранного</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Add to favorites</source>
        <translation>Добавить в избранное</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Successfuly added to favorites</source>
        <translation>Успешно добавлено в избранное</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Successfuly removed from favorites</source>
        <translation>Успешно удалено из избранного</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Failed to add to favorites</source>
        <translation>Не удалось добавить в избранное</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to remove from favorites</source>
        <translation>Не удалось удалить из избранного</translation>
    </message>
</context>
<context>
    <name>ToggleMuteCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="+163" />
        <source>Toggle Mute</source>
        <translation>Включить/выключить звук</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Mute or unmute system audio</source>
        <translation>Отключить или включить системный звук</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Failed to toggle mute</source>
        <translation>Не удалось переключить беззвучие</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Muted</source>
        <translation>Звук выключен</translation>
    </message>
</context>
<context>
    <name>ToggleOverviewCommand</name>
    <message>
        <location filename="../src/extensions/wm/wm-extension.cpp" line="+34" />
        <source>Toggle Overview</source>
        <translation>Переключить обзор</translation>
    </message>
</context>
<context>
    <name>UninstallExtensionAction</name>
    <message>
        <location filename="../src/actions/extension/extension-actions.cpp" line="+11" />
        <source>Are you sure?</source>
        <translation>Вы уверены?</translation>
    </message>
    <message>
        <location line="+2" />
        <source>All this extension data will be permanently lost. If you just want the extension to not appear in the root search anymore, consider disabling it instead.</source>
        <translation>Все данные этого расширения будут безвозвратно потеряны. Если вы просто хотите, чтобы расширение больше не появлялось в корневом поиске, вместо этого отключите его.</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Uninstall</source>
        <translation>Удалить</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Extension uninstalled</source>
        <translation>Расширение удалено</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Failed to uninstall extension</source>
        <translation>Не удалось удалить расширение</translation>
    </message>
    <message>
        <location filename="../src/actions/extension/extension-actions.hpp" line="+14" />
        <source>Uninstall Extension</source>
        <translation>Удалить расширение</translation>
    </message>
</context>
<context>
    <name>UnpinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-23" />
        <source>Entry unpinned</source>
        <translation>Запись откреплена</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Unpin entry</source>
        <translation>Открепить запись</translation>
    </message>
</context>
<context>
    <name>UnpinEmojiAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-45" />
        <source>Unpin emoji</source>
        <translation>Открепить эмодзи</translation>
    </message>
</context>
<context>
    <name>UpdateService</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="-167" />
        <source>Update installed</source>
        <translation>Обновление установлено</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Restarting…</source>
        <translation>Перезапуск…</translation>
    </message>
    <message>
        <location line="+88" />
        <source>Downloading Vicinae %1…</source>
        <translation>Загрузка Vicinae %1…</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Downloading Vicinae %1… %2%</source>
        <translation>Загрузка Vicinae %1… %2%</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Installing update…</source>
        <translation>Установка обновления…</translation>
    </message>
    <message>
        <location line="+24" />
        <source>Update failed</source>
        <translation>Не удалось обновить</translation>
    </message>
</context>
<context>
    <name>VicinaeExtension</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.hpp" line="+11" />
        <source>General vicinae-related commands.</source>
        <translation>Общие команды, связанные с vicinae.</translation>
    </message>
</context>
<context>
    <name>VicinaeHotkeyGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/vicinae-hotkey-global-shortcut-backend.cpp" line="+75" />
        <source>Compositor does not support global hotkeys</source>
        <translation>Композитор не поддерживает глобальные горячие клавиши</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Unsupported trigger key</source>
        <translation>Неподдерживаемая клавиша триггера</translation>
    </message>
    <message>
        <location line="+17" />
        <source>Hotkey binding was lost</source>
        <translation>Привязка горячей клавиши утеряна</translation>
    </message>
</context>
<context>
    <name>VicinaeListInstalledExtensionsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/list-installed-extensions-command.hpp" line="+11" />
        <source>Show Installed Extensions</source>
        <translation>Показать установленные расширения</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Show all third-party extensions that have been installed. This includes local extensions as well as extensions downloaded from the stores (vicinae and raycast).</source>
        <translation>Показать все установленные сторонние расширения. Это включает локальные расширения, а также скачанные из магазинов (vicinae и raycast).</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-store-command.hpp" line="+13" />
        <source>Install extensions from the Vicinae store</source>
        <translation>Устанавливать расширения из магазина Vicinae</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Always show intro</source>
        <translation>Всегда показывать вступление</translation>
    </message>
    <message>
        <location line="+10" />
        <source>
# Welcome to the vicinae extension store

The vicinae extension store features community-built extensions that have been approved by our core contributors.

Every extension listed here has its source code available in the [vicinaehq/extensions](https://github.com/vicinaehq/extensions) repository.

If you're looking to build your own extension, take a look at the [documentation](https://docs.vicinae.com/extensions/introduction). If you think your extension would be a good fit for the store, feel free to submit it!
</source>
        <translation>
# Добро пожаловать в магазин расширений vicinae

В магазине расширений vicinae представлены расширения от сообщества, одобренные нашими ключевыми участниками.

Исходный код каждого расширения доступен в репозитории [vicinaehq/extensions](https://github.com/vicinaehq/extensions).

Если вы хотите создать собственное расширение, ознакомьтесь с [документацией](https://docs.vicinae.com/extensions/introduction). Если считаете, что ваше расширение подойдёт для магазина, смело отправляйте его!
</translation>
    </message>
    <message>
        <location line="+13" />
        <source>Continue to store</source>
        <translation>Перейти в магазин</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreDetailHost</name>
    <message>
        <location filename="../src/qml/vicinae-store-detail-host.cpp" line="+38" />
        <source>Failed to load extension</source>
        <translation>Не удалось загрузить расширение</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Could not fetch extension data from the store.</source>
        <translation>Не удалось получить данные расширения из магазина.</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Extension not found</source>
        <translation>Расширение не найдено</translation>
    </message>
    <message>
        <location line="+0" />
        <source>The extension "%1" could not be found in the store.</source>
        <translation>Расширение «%1» не найдено в магазине.</translation>
    </message>
    <message>
        <location line="+20" />
        <source>Extension Store - %1</source>
        <translation>Магазин расширений - %1</translation>
    </message>
    <message>
        <location line="+73" />
        <source>Extension Store</source>
        <translation>Магазин расширений</translation>
    </message>
    <message>
        <location line="+8" />
        <source>Install extension</source>
        <translation>Установить расширение</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Downloading extension...</source>
        <translation>Загрузка расширения...</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Failed to download extension</source>
        <translation>Не удалось загрузить расширение</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Failed to extract extension archive</source>
        <translation>Не удалось распаковать архив расширения</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Extension installed</source>
        <translation>Расширение установлено</translation>
    </message>
    <message>
        <location line="+14" />
        <source>Report issue</source>
        <translation>Сообщить о проблеме</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreSection</name>
    <message>
        <location filename="../src/qml/vicinae-store-model.cpp" line="+41" />
        <source>Show details</source>
        <translation>Показать подробности</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreViewHost</name>
    <message>
        <location filename="../src/qml/vicinae-store-view-host.cpp" line="+27" />
        <source>Browse Vicinae extensions</source>
        <translation>Просмотр расширений Vicinae</translation>
    </message>
    <message>
        <location line="+23" />
        <source>Failed to fetch extensions</source>
        <translation>Не удалось получить расширения</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Extensions</source>
        <translation>Расширения</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Extension Store</source>
        <translation>Магазин расширений</translation>
    </message>
</context>
<context>
    <name>VolumeDownCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-74" />
        <source>Turn Volume Down</source>
        <translation>Уменьшить громкость</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Decrease system volume</source>
        <translation>Уменьшить системную громкость</translation>
    </message>
    <message>
        <location line="+18" />
        <source>Invalid step value</source>
        <translation>Неверное значение шага</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Failed to adjust volume</source>
        <translation>Не удалось изменить громкость</translation>
    </message>
</context>
<context>
    <name>VolumeUpCommand</name>
    <message>
        <location line="-61" />
        <source>Turn Volume Up</source>
        <translation>Увеличить громкость</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Increase system volume</source>
        <translation>Увеличить системную громкость</translation>
    </message>
    <message>
        <location line="+18" />
        <source>Invalid step value</source>
        <translation>Неверное значение шага</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Failed to adjust volume</source>
        <translation>Не удалось изменить громкость</translation>
    </message>
</context>
<context>
    <name>WallpaperManager</name>
    <message>
        <location filename="../src/services/wallpaper/wallpaper-manager.cpp" line="+68" />
        <source>Setting the wallpaper is not supported in the current environment</source>
        <translation>Установка обоев не поддерживается в текущем окружении</translation>
    </message>
    <message>
        <location line="+4" />
        <source>No such file: %1</source>
        <translation>Нет такого файла: %1</translation>
    </message>
</context>
<context>
    <name>WinControlPanelRootItem</name>
    <message>
        <location filename="../src/root-search/control-panel/control-panel-root-provider.cpp" line="+104" />
        <location line="+10" />
        <source>Control Panel</source>
        <translation>Панель управления</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Name</source>
        <translation>Название</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Where</source>
        <translation>Расположение</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Open Applet</source>
        <translation>Открыть элемент</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Copy Path</source>
        <translation>Копировать путь</translation>
    </message>
</context>
<context>
    <name>WinControlPanelRootProvider</name>
    <message>
        <location line="+49" />
        <source>Control Panel</source>
        <translation>Панель управления</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Control Panel applets and system tasks.</source>
        <translation>Элементы панели управления и системные задачи.</translation>
    </message>
</context>
<context>
    <name>WinControlPanelTaskRootItem</name>
    <message>
        <location line="-40" />
        <location line="+11" />
        <source>Control Panel</source>
        <translation>Панель управления</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Name</source>
        <translation>Название</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Task ID</source>
        <translation>ID задачи</translation>
    </message>
    <message>
        <location line="+9" />
        <source>Open</source>
        <translation>Открыть</translation>
    </message>
</context>
<context>
    <name>WinSettingsPage</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="-114" />
        <source>Display</source>
        <translation>Дисплей</translation>
    </message>
    <message>
        <location line="+0" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <source>System</source>
        <translation>Система</translation>
    </message>
    <message>
        <location line="-17" />
        <source>Night Light</source>
        <translation>Ночной свет</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Sound</source>
        <translation>Звук</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Volume Mixer</source>
        <translation>Микшер громкости</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Notifications</source>
        <translation>Уведомления</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Focus</source>
        <translation>Фокус</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Power &amp; Battery</source>
        <translation>Питание и батарея</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Storage</source>
        <translation>Память</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Nearby Sharing</source>
        <translation>Общий доступ поблизости</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Multitasking</source>
        <translation>Многозадачность</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Activation</source>
        <translation>Активация</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Troubleshoot</source>
        <translation>Устранение неполадок</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Recovery</source>
        <translation>Восстановление</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Projecting to This PC</source>
        <translation>Проецирование на этот компьютер</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Remote Desktop</source>
        <translation>Удалённый рабочий стол</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Clipboard</source>
        <translation>Буфер обмена</translation>
    </message>
    <message>
        <location line="+1" />
        <source>About</source>
        <translation>О системе</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Optional Features</source>
        <translation>Дополнительные компоненты</translation>
    </message>
    <message>
        <location line="+1" />
        <source>For Developers</source>
        <translation>Для разработчиков</translation>
    </message>
    <message>
        <location line="+2" />
        <location line="+0" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <source>Bluetooth &amp; Devices</source>
        <translation>Bluetooth и устройства</translation>
    </message>
    <message>
        <location line="-8" />
        <source>Devices</source>
        <translation>Устройства</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Printers &amp; Scanners</source>
        <translation>Принтеры и сканеры</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Mobile Devices</source>
        <translation>Мобильные устройства</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Cameras</source>
        <translation>Камеры</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Mouse</source>
        <translation>Мышь</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Touchpad</source>
        <translation>Сенсорная панель</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Pen &amp; Windows Ink</source>
        <translation>Перо и Windows Ink</translation>
    </message>
    <message>
        <location line="+1" />
        <source>AutoPlay</source>
        <translation>Автозапуск</translation>
    </message>
    <message>
        <location line="+3" />
        <location line="+0" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <source>Network &amp; Internet</source>
        <translation>Сеть и Интернет</translation>
    </message>
    <message>
        <location line="-7" />
        <source>Wi-Fi</source>
        <translation>Wi-Fi</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Ethernet</source>
        <translation>Ethernet</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Mobile Hotspot</source>
        <translation>Мобильная точка доступа</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Airplane Mode</source>
        <translation>Режим «В самолёте»</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Proxy</source>
        <translation>Прокси</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Dial-up</source>
        <translation>Коммутируемое подключение</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Advanced Network Settings</source>
        <translation>Дополнительные параметры сети</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Background</source>
        <translation>Фон</translation>
    </message>
    <message>
        <location line="+0" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <source>Personalization</source>
        <translation>Персонализация</translation>
    </message>
    <message>
        <location line="-7" />
        <source>Colors</source>
        <translation>Цвета</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Themes</source>
        <translation>Темы</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Lock Screen</source>
        <translation>Экран блокировки</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Touch Keyboard</source>
        <translation>Сенсорная клавиатура</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Start</source>
        <translation>Пуск</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Taskbar</source>
        <translation>Панель задач</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Fonts</source>
        <translation>Шрифты</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Dynamic Lighting</source>
        <translation>Динамическая подсветка</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Installed Apps</source>
        <translation>Установленные приложения</translation>
    </message>
    <message>
        <location line="+0" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <source>Apps</source>
        <translation>Приложения</translation>
    </message>
    <message>
        <location line="-4" />
        <source>Default Apps</source>
        <translation>Приложения по умолчанию</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Offline Maps</source>
        <translation>Автономные карты</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Apps for Websites</source>
        <translation>Приложения для веб-сайтов</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Video Playback</source>
        <translation>Воспроизведение видео</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Startup Apps</source>
        <translation>Приложения автозагрузки</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Your Info</source>
        <translation>Ваши данные</translation>
    </message>
    <message>
        <location line="+0" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <source>Accounts</source>
        <translation>Учётные записи</translation>
    </message>
    <message>
        <location line="-4" />
        <source>Email &amp; Accounts</source>
        <translation>Эл. почта и учётные записи</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Sign-in Options</source>
        <translation>Способы входа</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Access Work or School</source>
        <translation>Доступ к работе или учёбе</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Family &amp; Other Users</source>
        <translation>Семья и другие пользователи</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Windows Backup</source>
        <translation>Резервное копирование Windows</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Date &amp; Time</source>
        <translation>Дата и время</translation>
    </message>
    <message>
        <location line="+0" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <source>Time &amp; Language</source>
        <translation>Время и язык</translation>
    </message>
    <message>
        <location line="-2" />
        <source>Language &amp; Region</source>
        <translation>Язык и регион</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Typing</source>
        <translation>Ввод</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Speech</source>
        <translation>Речь</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Game Bar</source>
        <translation>Игровая панель</translation>
    </message>
    <message>
        <location line="+0" />
        <location line="+1" />
        <location line="+1" />
        <source>Gaming</source>
        <translation>Игры</translation>
    </message>
    <message>
        <location line="-1" />
        <source>Captures</source>
        <translation>Записи экрана</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Game Mode</source>
        <translation>Игровой режим</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Text Size</source>
        <translation>Размер текста</translation>
    </message>
    <message>
        <location line="+0" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <source>Accessibility</source>
        <translation>Специальные возможности</translation>
    </message>
    <message>
        <location line="-9" />
        <source>Visual Effects</source>
        <translation>Визуальные эффекты</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Magnifier</source>
        <translation>Экранная лупа</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Color Filters</source>
        <translation>Цветовые фильтры</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Contrast Themes</source>
        <translation>Контрастные темы</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Narrator</source>
        <translation>Экранный диктор</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Accessibility Audio</source>
        <translation>Специальные возможности: звук</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Captions</source>
        <translation>Субтитры</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Accessibility Keyboard</source>
        <translation>Специальные возможности: клавиатура</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Accessibility Mouse</source>
        <translation>Специальные возможности: мышь</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Eye Control</source>
        <translation>Управление взглядом</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Windows Security</source>
        <translation>Безопасность Windows</translation>
    </message>
    <message>
        <location line="+0" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <source>Privacy &amp; Security</source>
        <translation>Конфиденциальность и безопасность</translation>
    </message>
    <message>
        <location line="-7" />
        <source>Find My Device</source>
        <translation>Поиск устройства</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Privacy</source>
        <translation>Конфиденциальность</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Location</source>
        <translation>Геолокация</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Camera Access</source>
        <translation>Доступ к камере</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Microphone Access</source>
        <translation>Доступ к микрофону</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Activity History</source>
        <translation>Журнал действий</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Diagnostics &amp; Feedback</source>
        <translation>Диагностика и отзывы</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Search Permissions</source>
        <translation>Разрешения поиска</translation>
    </message>
    <message>
        <location line="+2" />
        <location line="+0" />
        <location line="+1" />
        <location line="+1" />
        <location line="+1" />
        <source>Windows Update</source>
        <translation>Центр обновления Windows</translation>
    </message>
    <message>
        <location line="-2" />
        <source>Update History</source>
        <translation>Журнал обновлений</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Advanced Update Options</source>
        <translation>Дополнительные параметры обновления</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Windows Insider Program</source>
        <translation>Программа предварительной оценки Windows</translation>
    </message>
</context>
<context>
    <name>WinSettingsPageRootItem</name>
    <message>
        <location line="+37" />
        <source>System Settings</source>
        <translation>Параметры системы</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Settings</source>
        <translation>Параметры</translation>
    </message>
    <message>
        <location line="+12" />
        <source>Name</source>
        <translation>Название</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Category</source>
        <translation>Категория</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Open %1 Settings</source>
        <translation>Открыть параметры %1</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Copy URL</source>
        <translation>Копировать URL</translation>
    </message>
</context>
<context>
    <name>WinSettingsRootProvider</name>
    <message>
        <location line="+12" />
        <source>Windows Settings</source>
        <translation>Параметры Windows</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Pages of the Windows Settings app.</source>
        <translation>Страницы приложения «Параметры Windows».</translation>
    </message>
</context>
<context>
    <name>WindowManagementExtension</name>
    <message>
        <location filename="../src/extensions/wm/wm-extension.cpp" line="+49" />
        <source>Window Management</source>
        <translation>Управление окнами</translation>
    </message>
</context>
<context>
    <name>WindowsAppDatabase</name>
    <message>
        <location filename="../src/services/app-service/windows/win-app-database.cpp" line="+993" />
        <source>Focus window</source>
        <translation>Сфокусировать окно</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Launch app</source>
        <translation>Запустить приложение</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Default action</source>
        <translation>Действие по умолчанию</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Action to perform when the return key is pressed. Always default to 'launch' if the app has no open window.</source>
        <translation>Действие, выполняемое по нажатию Enter. Если у приложения нет открытого окна, всегда выполняется запуск.</translation>
    </message>
    <message>
        <location line="+15" />
        <source>File Explorer</source>
        <translation>Проводник</translation>
    </message>
    <message>
        <location line="+19" />
        <source>Terminal</source>
        <translation>Терминал</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Command Prompt</source>
        <translation>Командная строка</translation>
    </message>
</context>
<context>
    <name>WindowsApplication</name>
    <message>
        <location filename="../src/services/app-service/windows/win-app.hpp" line="+70" />
        <source>%1: Run as Administrator</source>
        <translation>%1: запуск от имени администратора</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Run as Administrator</source>
        <translation>Запуск от имени администратора</translation>
    </message>
</context>
<context>
    <name>WindowsGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/windows-global-shortcut-backend.cpp" line="+215" />
        <source>unsupported or invalid trigger</source>
        <translation>неподдерживаемый или неверный триггер</translation>
    </message>
    <message>
        <location line="+9" />
        <source>already registered by another application</source>
        <translation>уже зарегистрировано другим приложением</translation>
    </message>
</context>
<context>
    <name>X11GlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/x11-global-shortcut-backend.cpp" line="+122" />
        <source>This shortcut is already in use by another application</source>
        <translation>Это сочетание клавиш уже используется другим приложением</translation>
    </message>
    <message>
        <location line="+17" />
        <source>Unsupported trigger key</source>
        <translation>Неподдерживаемая клавиша триггера</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Trigger key is not present on this keyboard</source>
        <translation>Клавиша триггера отсутствует на этой клавиатуре</translation>
    </message>
</context>
<context>
    <name>X11Workspace</name>
    <message>
        <location filename="../src/services/window-manager/x11/x11-window-manager.cpp" line="+426" />
        <source>Desktop %1</source>
        <translation>Рабочий стол %1</translation>
    </message>
</context>
<context>
    <name>XdgAppDatabase</name>
    <message>
        <location filename="../src/services/app-service/xdg/xdg-app-database.cpp" line="+585" />
        <source>Focus window</source>
        <translation>Сфокусировать окно</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Launch app</source>
        <translation>Запустить приложение</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Default action</source>
        <translation>Действие по умолчанию</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Action to perform when the return key is pressed. Always default to 'launch' if the app has no open window.</source>
        <translation>Действие, выполняемое по нажатию Enter. Если у приложения нет открытого окна, всегда выполняется запуск.</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Launch Prefix</source>
        <translation>Префикс запуска</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Custom app launcher to use. Affects applications as well as their sub-actions.</source>
        <translation>Свой лаунчер приложений для использования. Влияет на приложения и их поддействия.</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Application directories</source>
        <translation>Каталоги приложений</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Directories applications are sourced from. The list cannot be modified directly. In order to do so, you need to append additonal paths to the &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt; environment variables.</source>
        <translation>Каталоги, из которых берутся приложения. Список нельзя изменить напрямую. Для этого нужно добавить дополнительные пути к переменной среды &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt;.</translation>
    </message>
</context>
<context>
    <name>XdpFileChooser</name>
    <message>
        <location filename="../src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.cpp" line="+39" />
        <source>Open Directory</source>
        <translation>Открыть каталог</translation>
    </message>
    <message>
        <location line="+0" />
        <source>Open File</source>
        <translation>Открыть файл</translation>
    </message>
</context>
<context>
    <name>browser-extension</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="-58" />
        <source>No browser connected</source>
        <translation>Браузер не подключён</translation>
    </message>
    <message>
        <location line="+1" />
        <source>You need to connect at least one browser to vicinae using the browser extension in order to use this command.</source>
        <translation>Чтобы использовать эту команду, подключите к vicinae хотя бы один браузер через браузерное расширение.</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Open documentation</source>
        <translation>Открыть документацию</translation>
    </message>
</context>
<context>
    <name>clipboard-history-view-host</name>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.cpp" line="-211" />
        <source>Text</source>
        <translation>Текст</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Link</source>
        <translation>Ссылка</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Image</source>
        <translation>Изображение</translation>
    </message>
    <message>
        <location line="+2" />
        <source>File</source>
        <translation>Файл</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Unknown</source>
        <translation>Неизвестно</translation>
    </message>
</context>
<context>
    <name>emoji-categories</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-52" />
        <source>Smileys &amp; Emotion</source>
        <translation>Смайлы и эмоции</translation>
    </message>
    <message>
        <location line="+1" />
        <source>People &amp; Body</source>
        <translation>Люди и тело</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Animals &amp; Nature</source>
        <translation>Животные и природа</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Food &amp; Drink</source>
        <translation>Еда и напитки</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Travel &amp; Places</source>
        <translation>Путешествия и места</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Activities</source>
        <translation>Занятия</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Objects</source>
        <translation>Объекты</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Symbols</source>
        <translation>Символы</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Flags</source>
        <translation>Флаги</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Math</source>
        <translation>Математика</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Arrows</source>
        <translation>Стрелки</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Currency</source>
        <translation>Валюта</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Punctuation</source>
        <translation>Пунктуация</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Shapes</source>
        <translation>Фигуры</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Misc Symbols</source>
        <translation>Прочие символы</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Greek</source>
        <translation>Греческий</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Number Forms</source>
        <translation>Числовые формы</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Fancy Letters</source>
        <translation>Декоративные буквы</translation>
    </message>
</context>
<context>
    <name>emoji-grid-model</name>
    <message>
        <location line="+131" />
        <source>Copy</source>
        <translation>Копировать</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Copy name</source>
        <translation>Копировать название</translation>
    </message>
    <message>
        <location line="+4" />
        <source>Copy unicode codepoint</source>
        <translation>Копировать unicode-код</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Copy category</source>
        <translation>Копировать категорию</translation>
    </message>
    <message>
        <location line="+40" />
        <source>Skin tones</source>
        <translation>Тоны кожи</translation>
    </message>
</context>
<context>
    <name>file-list-item</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+35" />
        <source>Copy file</source>
        <translation>Копировать файл</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Copy file path</source>
        <translation>Копировать путь к файлу</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Copy file name</source>
        <translation>Копировать имя файла</translation>
    </message>
    <message>
        <location line="+20" />
        <source>Copy mime type</source>
        <translation>Копировать тип MIME</translation>
    </message>
</context>
<context>
    <name>font-categories</name>
    <message>
        <location filename="../src/font-service.cpp" line="+127" />
        <source>Latin</source>
        <translation>Латиница</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Cyrillic</source>
        <translation>Кириллица</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Greek</source>
        <translation>Греческий</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Monospace</source>
        <translation>Моноширинный</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Emoji</source>
        <translation>Эмодзи</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Japanese</source>
        <translation>Японский</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Korean</source>
        <translation>Корейский</translation>
    </message>
    <message>
        <location line="+3" />
        <source>Simplified Chinese</source>
        <translation>Упрощённый китайский</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Traditional Chinese</source>
        <translation>Традиционный китайский</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Arabic</source>
        <translation>Арабский</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Hebrew</source>
        <translation>Иврит</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Thai</source>
        <translation>Тайский</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Lao</source>
        <translation>Лаосский</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Devanagari</source>
        <translation>Деванагари</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Bengali</source>
        <translation>Бенгальский</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Gurmukhi</source>
        <translation>Гурмукхи</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Gujarati</source>
        <translation>Гуджарати</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Tamil</source>
        <translation>Тамильский</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Telugu</source>
        <translation>Телугу</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Kannada</source>
        <translation>Каннада</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Malayalam</source>
        <translation>Малаялам</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Sinhala</source>
        <translation>Сингальский</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Armenian</source>
        <translation>Армянский</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Georgian</source>
        <translation>Грузинский</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Thaana</source>
        <translation>Тана</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Tibetan</source>
        <translation>Тибетский</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Myanmar</source>
        <translation>Бирманский</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Khmer</source>
        <translation>Кхмерский</translation>
    </message>
    <message>
        <location line="+2" />
        <source>Syriac</source>
        <translation>Сирийский</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Ogham</source>
        <translation>Огам</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Runic</source>
        <translation>Рунический</translation>
    </message>
    <message>
        <location line="+1" />
        <source>N'Ko</source>
        <translation>Нко</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Symbols</source>
        <translation>Символы</translation>
    </message>
</context>
<context>
    <name>font-grid-model</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="+26" />
        <source>Copy font family</source>
        <translation>Копировать семейство шрифтов</translation>
    </message>
</context>
<context>
    <name>keybind-manager</name>
    <message>
        <location filename="../src/internal/keyboard/keybind-manager.cpp" line="+9" />
        <source>Toggle action panel</source>
        <translation>Переключить панель действий</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Toggle the action panel to access and filter through the list of available actions for the currently selected item</source>
        <translation>Переключить панель действий, чтобы открыть и фильтровать список доступных действий для выбранного элемента</translation>
    </message>
    <message>
        <location line="+10" />
        <source>Open Search Filter</source>
        <translation>Открыть фильтр поиска</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Open the search filter selector if present</source>
        <translation>Открыть выбор фильтра поиска, если он есть</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Open settings window</source>
        <translation>Открыть окно настроек</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Open this settings window from the launcher window</source>
        <translation>Открыть это окно настроек из окна лаунчера</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Generic Open Action</source>
        <translation>Общее действие открытия</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Can be used by actions that can open the selected item</source>
        <translation>Может использоваться действиями, которые могут открыть выбранный элемент</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Generic Copy Action</source>
        <translation>Общее действие копирования</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Can be used by actions that can copy the selected item</source>
        <translation>Может использоваться действиями, которые могут скопировать выбранный элемент</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Copy Name Action</source>
        <translation>Действие копирования названия</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Can be used by actions that can copy the name of the selected item</source>
        <translation>Может использоваться действиями, которые могут скопировать название выбранного элемента</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Copy Path Action</source>
        <translation>Действие копирования пути</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Can be used by actions that can copy the path of the selected item</source>
        <translation>Может использоваться действиями, которые могут скопировать путь выбранного элемента</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Save Action</source>
        <translation>Действие сохранения</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Can be used by actions that can save the selected item</source>
        <translation>Может использоваться действиями, которые могут сохранить выбранный элемент</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Duplicate Action</source>
        <translation>Действие дублирования</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Can be used by actions that can duplicate the selected item</source>
        <translation>Может использоваться действиями, которые могут продублировать выбранный элемент</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Generic New Action</source>
        <translation>Общее действие создания</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Can be used by actions that create something</source>
        <translation>Может использоваться действиями, которые что-то создают</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Generic Move Up Action</source>
        <translation>Общее действие перемещения вверх</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Can be used by actions that can move up the selected item. This does not affect list navigation controls.</source>
        <translation>Может использоваться действиями, которые могут переместить выбранный элемент вверх. Это не влияет на элементы управления навигацией по списку.</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Generic Move Down Action</source>
        <translation>Общее действие перемещения вниз</translation>
    </message>
    <message>
        <location line="+21" />
        <source>Remove Action</source>
        <translation>Действие удаления</translation>
    </message>
    <message>
        <location line="-20" />
        <source>Can be used by actions that can move down the selected item. This does not affect list navigation controls.</source>
        <translation>Может использоваться действиями, которые могут переместить выбранный элемент вниз. Это не влияет на элементы управления навигацией по списку.</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Generic Refresh Action</source>
        <translation>Общее действие обновления</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Can be used by actions that can refresh the selected item</source>
        <translation>Может использоваться действиями, которые могут обновить выбранный элемент</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Generic Pin Action</source>
        <translation>Общее действие закрепления</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Can be used by actions that can pin the selected item</source>
        <translation>Может использоваться действиями, которые могут закрепить выбранный элемент</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Can be used by actions that can remove the selected item. This is normally used for small, not too impactful removals.</source>
        <translation>Может использоваться действиями, которые могут удалить выбранный элемент. Обычно используется для небольших и не слишком значимых удалений.</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Dangerous Remove Action</source>
        <translation>Опасное действие удаления</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Can be used by actions that perform an impactful removal, generally accompanied by a confirmation dialog.</source>
        <translation>Может использоваться действиями, которые выполняют значимое удаление, обычно сопровождаемое диалогом подтверждения.</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Edit Action</source>
        <translation>Действие редактирования</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Can be used by actions that can edit the currently selected item</source>
        <translation>Может использоваться действиями, которые могут изменить выбранный элемент</translation>
    </message>
    <message>
        <location line="+6" />
        <source>Edit Secondary Action</source>
        <translation>Действие вторичного редактирования</translation>
    </message>
    <message>
        <location line="+1" />
        <source>Can be used by actions that can edit a secondary characteristic of the currently selected item</source>
        <translation>Может использоваться действиями, которые могут изменить дополнительную характеристику выбранного элемента</translation>
    </message>
</context>
<context>
    <name>macos-update-installer</name>
    <message>
        <location filename="../src/services/update/macos-update-installer.mm" line="-183" />
        <source>Update image contains more than one app</source>
        <translation>Образ обновления содержит более одного приложения</translation>
    </message>
    <message>
        <location line="+7" />
        <source>Failed to list update image: %1</source>
        <translation>Не удалось получить список образа обновления: %1</translation>
    </message>
    <message>
        <location line="+5" />
        <source>No app found in update image</source>
        <translation>В образе обновления не найдено приложений</translation>
    </message>
    <message>
        <location line="+12" />
        <source>Failed to read the update's code signature</source>
        <translation>Не удалось прочитать подпись кода обновления</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Failed to build the signature requirement</source>
        <translation>Не удалось сформировать требование к подписи</translation>
    </message>
    <message>
        <location line="+11" />
        <source>Update signature verification failed (%1)</source>
        <translation>Не удалось проверить подпись обновления (%1)</translation>
    </message>
    <message>
        <location line="+15" />
        <source>Update has no CFBundleShortVersionString</source>
        <translation>У обновления нет CFBundleShortVersionString</translation>
    </message>
    <message>
        <location line="+5" />
        <source>Update version mismatch: expected %1, found %2</source>
        <translation>Несовпадение версии обновления: ожидалось %1, найдено %2</translation>
    </message>
</context>
<context>
    <name>shortcut-conflict</name>
    <message>
        <location filename="../src/qml/shortcut-conflict.cpp" line="+10" />
        <source>Modifier required</source>
        <translation>Требуется модификатор</translation>
    </message>
    <message>
        <location line="+4" />
        <location line="+5" />
        <source>Already bound to "%1"</source>
        <translation>Уже привязано к «%1»</translation>
    </message>
</context>
<context>
    <name>system-extension</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-34" />
        <source>Volume %1%</source>
        <translation>Громкость %1%</translation>
    </message>
</context>
<context>
    <name>utils</name>
    <message>
        <location filename="../src/utils/utils.cpp" line="+88" />
        <source>0 bytes</source>
        <translation>0 байт</translation>
    </message>
    <message>
        <location line="+2" />
        <source>bytes</source>
        <translation>байт</translation>
    </message>
</context>
<context>
    <name>virtual-desktops</name>
    <message>
        <location filename="../src/services/window-manager/windows/virtual-desktops.cpp" line="+67" />
        <source>Desktop %1</source>
        <translation>Рабочий стол %1</translation>
    </message>
</context>
</TS>