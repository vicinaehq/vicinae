<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="de_DE">
<context>
    <name>AboutSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AboutSettingsPage.qml" line="+57"/>
        <source>Version %1 - Commit %2
(%3)</source>
        <translation>Version %1 – Commit %2
(%3)</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Documentation</source>
        <translation>Dokumentation</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Report a Bug</source>
        <translation>Fehler melden</translation>
    </message>
</context>
<context>
    <name>ActionListPanel</name>
    <message>
        <location filename="../src/qml/qml/ActionListPanel.qml" line="+113"/>
        <source>No matching actions</source>
        <translation>Keine passenden Aktionen</translation>
    </message>
    <message>
        <location line="+135"/>
        <source>Filter actions...</source>
        <translation>Aktionen filtern...</translation>
    </message>
</context>
<context>
    <name>AdvancedSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AdvancedSettingsPage.qml" line="+29"/>
        <source>Input &amp; Navigation</source>
        <translation>Eingabe &amp; Navigation</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Pop on backspace</source>
        <translation>Mit Rücktaste zurückgehen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pop back in navigation on backspace when no input is present.</source>
        <translation>Mit der Rücktaste zur vorherigen Ansicht zurückkehren, wenn keine Eingabe vorhanden ist.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Activate on single click</source>
        <translation>Mit einem Klick aktivieren</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activate items with a single click instead of requiring a double click.</source>
        <translation>Elemente mit einem einfachen Klick statt mit einem Doppelklick aktivieren.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Wrap navigation</source>
        <translation>Zyklische Navigation</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Wrap around to the opposite end when moving past the first or last item.</source>
        <translation>Beim Überschreiten des ersten oder letzten Elements am jeweils anderen Ende fortfahren.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>IME handling</source>
        <translation>IME-Verarbeitung</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Include IME Preedit strings as part of search queries.</source>
        <translation>IME-Vorbearbeitungstext in Suchanfragen einbeziehen.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Keybinding Scheme</source>
        <translation>Tastaturbelegung</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Default uses the standard macOS keys (arrows, Ctrl+N/P); Vim uses Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Opt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>Standardmäßig werden die Standardtasten macOS verwendet (Pfeile, Ctrl+N/P); Vim verwendet Ctrl+J/K und Ctrl+H/L; Emacs verwendet Ctrl+N/P und Ctrl+Opt+B/F für die Navigation sowie Emacs-Bearbeitung in der Suchleiste.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Default and Vim use Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Alt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>Standard und Vim verwenden Ctrl+J/K und Ctrl+H/L; Emacs verwendet Ctrl+N/P und Ctrl+Alt+B/F für die Navigation sowie Emacs-Bearbeitung in der Suchleiste.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Search</source>
        <translation>Suchen</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Root file search</source>
        <translation>Dateisuche in der Hauptansicht</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Files are searched asynchronously, so if enabled you should expect a slight delay for file search results to show up.</source>
        <translation>Dateien werden asynchron durchsucht. Wenn diese Option aktiviert ist, sollten Sie daher mit einer leichten Verzögerung rechnen, bis die Ergebnisse der Dateisuche angezeigt werden.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Favicon Fetching</source>
        <translation>Favicon-Abruf</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The favicon provider used to load favicons where needed. Select &apos;None&apos; to turn off favicon loading.</source>
        <translation>Der Anbieter, über den Favicons bei Bedarf geladen werden. „Keine“ wählen, um das Laden von Favicons zu deaktivieren.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>System</source>
        <translation>System</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Input server</source>
        <translation>Eingabeserver</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Whether to spawn the input server at startup. This needs to be enabled in order to support snippets, paste to active window, and other features that require input monitoring or injection.</source>
        <translation>Ob der Eingabeserver beim Start erzeugt werden soll. Dies muss aktiviert werden, um Snippets, Einfügen in das aktive Fenster und andere Funktionen zu unterstützen, die eine Eingabeüberwachung oder -injektion erfordern.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Security</source>
        <translation>Sicherheit</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Encrypt sensitive data</source>
        <translation>Sensible Daten verschlüsseln</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Encrypt sensitive data at rest, such as clipboard history and internal databases (OAuth tokens, extension local storage, API keys). Note that some components, such as on-disk clipboard history, may not be retroactively affected when toggling this option. Turning on this option may ask you to unlock your keychain. Requires a restart in order to apply.</source>
        <translation>Verschlüsseln Sie vertrauliche Daten im Ruhezustand, z. B. den Verlauf der Zwischenablage und interne Datenbanken (OAuth-Tokens, lokaler Erweiterungsspeicher, API-Schlüssel). Beachten Sie, dass einige Komponenten, wie z. B. der Verlauf der Zwischenablage auf der Festplatte, möglicherweise nicht rückwirkend beeinflusst werden, wenn diese Option aktiviert wird. Wenn Sie diese Option aktivieren, werden Sie möglicherweise aufgefordert, Ihren Schlüsselbund zu entsperren. Zur Anwendung ist ein Neustart erforderlich.</translation>
    </message>
</context>
<context>
    <name>AlertWidget</name>
    <message>
        <location filename="../src/ui/alert/alert.hpp" line="+15"/>
        <source>Are you sure?</source>
        <translation>Wirklich fortfahren?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>This action cannot be undone</source>
        <translation>Diese Aktion kann nicht rückgängig gemacht werden</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Confirm</source>
        <translation>Bestätigen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Cancel</source>
        <translation>Abbrechen</translation>
    </message>
</context>
<context>
    <name>AliasFormView</name>
    <message>
        <location filename="../src/qml/qml/AliasFormView.qml" line="+15"/>
        <source>Alias</source>
        <translation>Alias</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Additional words to index this item against</source>
        <translation>Zusätzliche Wörter, anhand derer dieses Element indiziert werden soll</translation>
    </message>
</context>
<context>
    <name>AliasFormViewHost</name>
    <message>
        <location filename="../src/qml/alias-form-view-host.cpp" line="+28"/>
        <source>Set alias - %1</source>
        <translation>Alias festlegen: %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Submit</source>
        <translation>Senden</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Alias modified</source>
        <translation>Alias geändert</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to modify alias</source>
        <translation>Alias konnte nicht geändert werden</translation>
    </message>
</context>
<context>
    <name>AppRootItem</name>
    <message>
        <location filename="../src/root-search/apps/app-root-provider.cpp" line="+22"/>
        <location line="+29"/>
        <source>Application</source>
        <translation>Anwendung</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Where</source>
        <translation>Wo</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Opens in terminal</source>
        <translation>Öffnet im Terminal</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Yes</source>
        <translation>Ja</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No</source>
        <translation>Nein</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Open Application</source>
        <translation>Anwendung öffnen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy App ID</source>
        <translation>App-ID kopieren</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy App Location</source>
        <translation>App-Speicherort kopieren</translation>
    </message>
</context>
<context>
    <name>AppRootProvider</name>
    <message>
        <location line="+75"/>
        <source>Applications</source>
        <translation>Anwendungen</translation>
    </message>
</context>
<context>
    <name>AppSelectorModel</name>
    <message>
        <location filename="../src/qml/app-selector-model.cpp" line="+20"/>
        <location line="+65"/>
        <source>%1 (Default)</source>
        <translation>%1 (Standard)</translation>
    </message>
</context>
<context>
    <name>AppearanceSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AppearanceSettingsPage.qml" line="+29"/>
        <location line="+7"/>
        <source>Theme</source>
        <translation>Design</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Font</source>
        <translation>Schriftart</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Font size</source>
        <translation>Schriftgröße</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The base point size used to compute font sizes. Fractional values are accepted. Recommended range is [10.0;12.0].</source>
        <translation>Die Basispunktgröße, die zur Berechnung der Schriftgrößen verwendet wird. Bruchwerte werden akzeptiert. Der empfohlene Bereich ist [10,0;12,0].</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>e.g. 11</source>
        <translation>z.B. 11</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Icon Theme</source>
        <translation>Symboldesign</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The icon theme used for system icons (applications, mime types, folder icons...). Does not affect builtin Vicinae icons.</source>
        <translation>Das Symbolthema, das für Systemsymbole (Anwendungen, MIME-Typen, Ordnersymbole usw.) verwendet wird. Hat keine Auswirkungen auf die integrierten Vicinae-Symbole.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Window</source>
        <translation>Fenster</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Window material</source>
        <translation>Fensterhintergrund</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Background material applied to the launcher window. Lower the window opacity to see it.</source>
        <translation>Auf das Launcher-Fenster angewendeter Hintergrund. Die Fensterdeckkraft verringern, damit er sichtbar wird.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Window opacity</source>
        <translation>Fensterdeckkraft</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>e.g. 1.0</source>
        <translation>z.B. 1,0</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Compact mode</source>
        <translation>Kompaktmodus</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Show only the search bar at root; expand when a query is entered.</source>
        <translation>Nur die Suchleiste im Stammverzeichnis anzeigen; wird erweitert, wenn eine Abfrage eingegeben wird.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Use layer shell</source>
        <translation>Layer-Shell verwenden</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Anchor the launcher as a Wayland layer surface (wlr-layer-shell) instead of a regular window. May require reopening Vicinae to fully apply.</source>
        <translation>Verankern Sie den Launcher als Wayland-Layer-Oberfläche (wlr-layer-shell) anstelle eines regulären Fensters. Möglicherweise muss Vicinae erneut geöffnet werden, damit es vollständig angewendet werden kann.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Client-side decorations</source>
        <translation>Clientseitige Fensterdekoration</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Let Vicinae draw its own rounded borders and shadow instead of relying on the windowing system.</source>
        <translation>Vicinae zeichnet abgerundete Ränder und Schatten selbst, statt sie vom Fenstersystem darstellen zu lassen.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Corner rounding</source>
        <translation>Eckenrundung</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Radius of the launcher window corners, in pixels.</source>
        <translation>Radius der Ecken des Launcher-Fensters in Pixel.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>e.g. 10</source>
        <translation>z.B. 10</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Border width</source>
        <translation>Randbreite</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Thickness of the launcher window border, in pixels.</source>
        <translation>Dicke des Launcher-Fensterrands in Pixel.</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>e.g. 3</source>
        <translation>z.B. 3</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Shadow size</source>
        <translation>Schattengröße</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Size of the drop shadow cast by the launcher window, in pixels.</source>
        <translation>Größe des vom Launcher-Fenster geworfenen Schlagschattens in Pixel.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>e.g. 12</source>
        <translation>z.B. 12</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Native font rendering</source>
        <translation>Native Textdarstellung</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Use the platform&apos;s native text rendering for system-consistent text. Disable for Qt distance-field rendering (usually faster). May require reopening Vicinae to fully apply.</source>
        <translation>Nutzen Sie die native Textwiedergabe der Plattform für systemkonsistenten Text. Für Qt-Distanzfeld-Rendering deaktivieren (normalerweise schneller). Möglicherweise muss Vicinae erneut geöffnet werden, damit es vollständig angewendet werden kann.</translation>
    </message>
</context>
<context>
    <name>AvailableFallbackSection</name>
    <message>
        <location filename="../src/qml/manage-fallback-model.hpp" line="+49"/>
        <source>Available</source>
        <translation>Verfügbar</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-fallback-model.cpp" line="+50"/>
        <source>Enable fallback</source>
        <translation>Fallback aktivieren</translation>
    </message>
</context>
<context>
    <name>BringToWorkspaceAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="+72"/>
        <source>Bring to current workspace</source>
        <translation>In den aktuellen Arbeitsbereich bringen</translation>
    </message>
</context>
<context>
    <name>BrowseAppsSection</name>
    <message>
        <location filename="../src/qml/browse-apps-model.hpp" line="+32"/>
        <source>Applications ({count})</source>
        <translation>Anwendungen ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/browse-apps-model.cpp" line="+21"/>
        <source>Hidden</source>
        <translation>Versteckt</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Open Application</source>
        <translation>Anwendung öffnen</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Copy App ID</source>
        <translation>App-ID kopieren</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy App Location</source>
        <translation>App-Speicherort kopieren</translation>
    </message>
</context>
<context>
    <name>BrowseAppsViewHost</name>
    <message>
        <location filename="../src/qml/browse-apps-view-host.cpp" line="+12"/>
        <source>Search apps...</source>
        <translation>Apps suchen...</translation>
    </message>
</context>
<context>
    <name>BrowseFontsCommand</name>
    <message>
        <location filename="../src/extensions/font/browse-fonts-command.hpp" line="+8"/>
        <source>Search Fonts</source>
        <translation>Schriftarten suchen</translation>
    </message>
</context>
<context>
    <name>BrowserExtension</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.hpp" line="+12"/>
        <source>Browser Extension</source>
        <translation>Browser-Erweiterung</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Browser extension related commands.</source>
        <translation>Befehle für Browsererweiterungen.</translation>
    </message>
</context>
<context>
    <name>BrowserTabActionGenerator</name>
    <message>
        <location filename="../src/actions/browser-tab-actions.hpp" line="+24"/>
        <source>Switch to tab</source>
        <translation>Zum Tab wechseln</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Convert to shortcut</source>
        <translation>In Verknüpfung umwandeln</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Convert tab to shortcut</source>
        <translation>Tab in Verknüpfung umwandeln</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Close tab</source>
        <translation>Tab schließen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to close tab: %1</source>
        <translation>Tab konnte nicht geschlossen werden: %1</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy URL</source>
        <translation>URL kopieren</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy Title</source>
        <translation>Titel kopieren</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy ID</source>
        <translation>ID kopieren</translation>
    </message>
</context>
<context>
    <name>BrowserTabProvider</name>
    <message>
        <location filename="../src/root-search/browser-tabs/browser-tabs-provider.hpp" line="+70"/>
        <source>Browser Tabs</source>
        <translation>Browser-Tabs</translation>
    </message>
</context>
<context>
    <name>BrowserTabRootItem</name>
    <message>
        <location line="-51"/>
        <source>Browser Tab</source>
        <translation>Browser Tab</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Tab</source>
        <translation>Tab</translation>
    </message>
</context>
<context>
    <name>BrowserTabsSection</name>
    <message>
        <location filename="../src/qml/browser-tabs-model.hpp" line="+17"/>
        <source>Tabs ({count})</source>
        <translation>Registerkarten ({count})</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Playing Media ({count})</source>
        <translation>Medienwiedergabe ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/browser-tabs-model.cpp" line="+15"/>
        <source>Muted</source>
        <translation>Stummgeschaltet</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Playing</source>
        <translation>Wiedergabe läuft</translation>
    </message>
</context>
<context>
    <name>BrowserTabsViewHost</name>
    <message>
        <location filename="../src/qml/browser-tabs-view-host.cpp" line="+11"/>
        <source>Search, focus and close tabs</source>
        <translation>Tabs suchen, fokussieren und schließen</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsSection</name>
    <message>
        <location filename="../src/qml/builtin-icons-model.hpp" line="+20"/>
        <source>Icons ({count})</source>
        <translation>Symbole ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/builtin-icons-model.cpp" line="+15"/>
        <source>Copy Icon Name</source>
        <translation>Symbolnamen kopieren</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsViewHost</name>
    <message>
        <location filename="../src/qml/builtin-icons-view-host.cpp" line="+10"/>
        <source>Search icons...</source>
        <translation>Symbole suchen...</translation>
    </message>
</context>
<context>
    <name>CalcHistoryListView</name>
    <message>
        <location filename="../src/qml/qml/CalcHistoryListView.qml" line="+12"/>
        <source>No results</source>
        <translation>Keine Ergebnisse</translation>
    </message>
</context>
<context>
    <name>CalcHistorySection</name>
    <message>
        <location filename="../src/qml/calc-history-model.cpp" line="+41"/>
        <source>Copy answer</source>
        <translation>Antwort kopieren</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy question</source>
        <translation>Frage kopieren</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy question and answer</source>
        <translation>Frage und Antwort kopieren</translation>
    </message>
</context>
<context>
    <name>CalcHistoryViewHost</name>
    <message>
        <location filename="../src/qml/calc-history-view-host.cpp" line="+83"/>
        <source>Search past calculations...</source>
        <translation>Vergangene Berechnungen durchsuchen...</translation>
    </message>
</context>
<context>
    <name>CalcLiveSection</name>
    <message>
        <location filename="../src/qml/calc-history-view-host.hpp" line="+27"/>
        <source>Calculator</source>
        <translation>Rechner</translation>
    </message>
</context>
<context>
    <name>CalculatorExtension</name>
    <message>
        <location filename="../src/extensions/calculator/calculator-extension.hpp" line="+73"/>
        <source>Calculator</source>
        <translation>Rechner</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Do maths, convert units or search past calculations...</source>
        <translation>Berechnungen durchführen, Einheiten umrechnen oder frühere Berechnungen durchsuchen...</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Calculator Backend</source>
        <translation>Rechner-Backend</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Which backend to use to perform calculations</source>
        <translation>Welches Backend zur Durchführung von Berechnungen verwendet werden soll</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Refresh rates on startup</source>
        <translation>Wechselkurse beim Start aktualisieren</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Whether exchange rates should be refreshed every time the vicinae server is started. If the current backend does not support it, this is ignored.</source>
        <translation>Ob die Wechselkurse bei jedem Start des Vicinae-Servers aktualisiert werden sollen. Wenn das aktuelle Backend dies nicht unterstützt, wird dies ignoriert.</translation>
    </message>
</context>
<context>
    <name>CalculatorHistoryCommand</name>
    <message>
        <location line="-86"/>
        <source>Calculator history</source>
        <translation>Rechenverlauf</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse past calculations. You need to copy the result of a calculation for it to be saved in history.</source>
        <translation>Durchsuchen Sie vergangene Berechnungen. Sie müssen das Ergebnis einer Berechnung kopieren, damit es im Verlauf gespeichert wird.</translation>
    </message>
</context>
<context>
    <name>CalculatorRefreshRatesCommand</name>
    <message>
        <location line="+11"/>
        <source>Refresh Exchange Rates</source>
        <translation>Wechselkurse aktualisieren</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Refresh exchange rates used by the calculator to provide currency conversion features. Not all backends may support currency conversions or manually refreshing the rates.</source>
        <translation>Aktualisieren Sie die vom Rechner verwendeten Wechselkurse, um Währungsumrechnungsfunktionen bereitzustellen. Möglicherweise unterstützen nicht alle Backends Währungsumrechnungen oder die manuelle Aktualisierung der Kurse.</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>%1 can&apos;t refresh rates</source>
        <translation>%1 kann Wechselkurse nicht aktualisieren</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Refreshing rates...</source>
        <translation>Wechselkurse werden aktualisiert...</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Rates successfully refreshed</source>
        <translation>Wechselkurse erfolgreich aktualisiert</translation>
    </message>
</context>
<context>
    <name>CalculatorResultDelegate</name>
    <message>
        <location filename="../src/qml/qml/CalculatorResultDelegate.qml" line="+40"/>
        <source>Question</source>
        <translation>Frage</translation>
    </message>
    <message>
        <location line="+57"/>
        <source>Answer</source>
        <translation>Antwort</translation>
    </message>
</context>
<context>
    <name>CalculatorService</name>
    <message>
        <location filename="../src/services/calculator-service/calculator-service.cpp" line="+119"/>
        <source>Pinned</source>
        <translation>Angeheftet</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Today</source>
        <translation>Heute</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>This week</source>
        <translation>Diese Woche</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>This month</source>
        <translation>Diesen Monat</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>This year</source>
        <translation>Dieses Jahr</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>A few years ago</source>
        <translation>Vor ein paar Jahren</translation>
    </message>
</context>
<context>
    <name>CategoryFilterAccessory</name>
    <message>
        <location filename="../src/qml/qml/CategoryFilterAccessory.qml" line="+8"/>
        <source>All</source>
        <translation>Alle</translation>
    </message>
</context>
<context>
    <name>ChangeEmojiSkinToneAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+99"/>
        <source>%1 skin tone</source>
        <translation>%1 Hautton</translation>
    </message>
</context>
<context>
    <name>ClearClipboardHistoryCommand</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.cpp" line="+35"/>
        <source>Clear Clipboard History</source>
        <translation>Zwischenablageverlauf löschen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clear the clipboard history</source>
        <translation>Zwischenablageverlauf löschen</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Are you sure?</source>
        <translation>Wirklich fortfahren?</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Your clipboard history will be gone forever :(</source>
        <translation>Der Zwischenablageverlauf wird unwiderruflich gelöscht :(</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to clear clipboard history</source>
        <translation>Der Verlauf der Zwischenablage konnte nicht gelöscht werden</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard history cleared</source>
        <translation>Der Verlauf der Zwischenablage wurde gelöscht</translation>
    </message>
</context>
<context>
    <name>ClipboardClearCommand</name>
    <message>
        <location line="-39"/>
        <source>Clear Current Clipboard Data</source>
        <translation>Aktuelle Zwischenablagedaten löschen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clear the current content of the clipboard</source>
        <translation>Aktuellen Inhalt der Zwischenablage löschen</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Failed to clear clipboard</source>
        <translation>Die Zwischenablage konnte nicht gelöscht werden</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard cleared</source>
        <translation>Zwischenablage geleert</translation>
    </message>
</context>
<context>
    <name>ClipboardExtension</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.hpp" line="+13"/>
        <source>Clipboard</source>
        <translation>Zwischenablage</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>System clipboard integration</source>
        <translation>Integration der Systemzwischenablage</translation>
    </message>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.cpp" line="+55"/>
        <source>Erase on startup</source>
        <translation>Beim Start löschen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Erase clipboard history every time the vicinae server is started</source>
        <translation>Zwischenablageverlauf bei jedem Start des Vicinae-Servers löschen</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Clipboard monitoring</source>
        <translation>Überwachung der Zwischenablage</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Whether clipboard activity is recorded in the history. Every clipboard action performed while this is turned off will not be recorded.</source>
        <translation>Ob die Aktivität in der Zwischenablage im Verlauf aufgezeichnet wird. Jede Aktion in der Zwischenablage, die ausgeführt wird, während diese Option deaktiviert ist, wird nicht aufgezeichnet.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Ignore Passwords</source>
        <translation>Passwörter ignorieren</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Ignore selections that can be identified as a password. This relies on the application providing an explicit hint that the selection is a password. While most password managers and private browser windows do, some might not implement this properly.</source>
        <translation>Als Passwort erkennbare Inhalte ignorieren. Dazu muss die Anwendung ausdrücklich kennzeichnen, dass es sich bei der Auswahl um ein Passwort handelt. Die meisten Passwortmanager und privaten Browserfenster tun dies, einige Anwendungen möglicherweise jedoch nicht.</translation>
    </message>
</context>
<context>
    <name>ClipboardFilterAccessory</name>
    <message>
        <location filename="../src/qml/qml/ClipboardFilterAccessory.qml" line="+8"/>
        <source>All</source>
        <translation>Alle</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Text</source>
        <translation>Text</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Images</source>
        <translation>Bilder</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Links</source>
        <translation>Links</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Files</source>
        <translation>Dateien</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryCommand</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-history-command.cpp" line="+16"/>
        <source>Paste</source>
        <translation>Einfügen</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy</source>
        <translation>Kopieren</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Default Action</source>
        <translation>Standardaktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to perform on pressing return. Paste is only available if your environment supports it.</source>
        <translation>Die Standardaktion, die beim Drücken der Eingabetaste ausgeführt wird. Einfügen ist nur verfügbar, wenn Ihre Umgebung dies unterstützt.</translation>
    </message>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-history-command.hpp" line="+11"/>
        <source>Clipboard History</source>
        <translation>Verlauf der Zwischenablage</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse your clipboard&apos;s history, pin, edit and remove entries.</source>
        <translation>Zwischenablageverlauf durchsuchen sowie Einträge anheften, bearbeiten und entfernen.</translation>
    </message>
</context>
<context>
    <name>ClipboardHistorySection</name>
    <message>
        <location filename="../src/qml/clipboard-history-model.cpp" line="+58"/>
        <source>Open Settings</source>
        <translation>Einstellungen öffnen</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryView</name>
    <message>
        <location filename="../src/qml/qml/ClipboardHistoryView.qml" line="+187"/>
        <source>Type</source>
        <translation>Typ</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Size</source>
        <translation>Größe</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copied at</source>
        <translation>Kopiert am</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>Preview not available for this content type</source>
        <translation>Für diesen Inhaltstyp ist keine Vorschau verfügbar</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryViewHost</name>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.hpp" line="+84"/>
        <source>Loading...</source>
        <translation>Laden...</translation>
    </message>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.cpp" line="+101"/>
        <source>Browse clipboard history...</source>
        <translation>Verlauf der Zwischenablage durchsuchen...</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard monitoring unavailable</source>
        <translation>Überwachung der Zwischenablage nicht verfügbar</translation>
    </message>
    <message>
        <location line="+67"/>
        <source>Pause clipboard</source>
        <translation>Zwischenablageüberwachung anhalten</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Resume clipboard</source>
        <translation>Zwischenablageüberwachung fortsetzen</translation>
    </message>
    <message numerus="yes">
        <location line="+8"/>
        <source>%n Items</source>
        <translation>
            <numerusform>%n Element</numerusform>
            <numerusform>%n Elemente</numerusform>
        </translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Decryption failed</source>
        <translation>Die Entschlüsselung ist fehlgeschlagen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Vicinae could not decrypt the data for this selection. It was most likely encrypted with a different key and cannot be recovered. You can remove this entry from the history.</source>
        <translation>Vicinae konnte die Daten für diese Auswahl nicht entschlüsseln. Es wurde höchstwahrscheinlich mit einem anderen Schlüssel verschlüsselt und kann nicht wiederhergestellt werden. Sie können diesen Eintrag aus dem Verlauf entfernen.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Data unavailable</source>
        <translation>Daten nicht verfügbar</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The data for this selection could not be found on disk.</source>
        <translation>Die Daten für diese Auswahl konnten nicht auf der Festplatte gefunden werden.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Data is encrypted</source>
        <translation>Daten sind verschlüsselt</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Data for this selection was previously encrypted but the clipboard is not currently configured to use encryption. You should be able to fix this by enabling it in the settings.</source>
        <translation>Die Daten für diese Auswahl wurden zuvor verschlüsselt, aber die Zwischenablage ist derzeit nicht für die Verwendung der Verschlüsselung konfiguriert. Sie sollten dies beheben können, indem Sie es in den Einstellungen aktivieren.</translation>
    </message>
</context>
<context>
    <name>ClipboardService</name>
    <message>
        <location filename="../src/services/clipboard/clipboard-service.cpp" line="+316"/>
        <source>Image (%1x%2)</source>
        <translation>Bild (%1x%2)</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Image</source>
        <translation>Bild</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Unknown</source>
        <translation>Unbekannt</translation>
    </message>
</context>
<context>
    <name>CloseWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="-37"/>
        <source>Close window</source>
        <translation>Fenster schließen</translation>
    </message>
</context>
<context>
    <name>CommandLineSection</name>
    <message>
        <location filename="../src/qml/system-run-model.hpp" line="+19"/>
        <source>Execute query</source>
        <translation>Abfrage ausführen</translation>
    </message>
    <message>
        <location filename="../src/qml/system-run-model.cpp" line="+44"/>
        <source>Open in %1 (hold)</source>
        <translation>In %1 öffnen (gedrückt halten)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open in %1</source>
        <translation>In %1 öffnen</translation>
    </message>
</context>
<context>
    <name>CommandListView</name>
    <message>
        <location filename="../src/qml/qml/CommandListView.qml" line="+12"/>
        <source>No results</source>
        <translation>Keine Ergebnisse</translation>
    </message>
</context>
<context>
    <name>CommandRootItem</name>
    <message>
        <location filename="../src/root-search/extensions/extension-root-provider.cpp" line="+28"/>
        <location line="+48"/>
        <source>Command</source>
        <translation>Befehl</translation>
    </message>
    <message>
        <location line="-43"/>
        <location line="+27"/>
        <source>Open command</source>
        <translation>Befehl öffnen</translation>
    </message>
    <message>
        <location line="-13"/>
        <source>Copy extension path</source>
        <translation>Erweiterungspfad kopieren</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Internal Command</source>
        <translation>Interner Befehl</translation>
    </message>
</context>
<context>
    <name>CompletionPopup</name>
    <message>
        <location filename="../src/qml/qml/CompletionPopup.qml" line="+11"/>
        <source>Filter...</source>
        <translation>Filtern...</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorAnswerAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+29"/>
        <source>Answer copied to clipboard</source>
        <translation>Antwort in die Zwischenablage kopiert</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy answer</source>
        <translation>Antwort konnte nicht kopiert werden</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy Result</source>
        <translation>Ergebnis kopieren</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorQuestionAndAnswerAction</name>
    <message>
        <location line="+18"/>
        <source>Answer copied to clipboard</source>
        <translation>Antwort in die Zwischenablage kopiert</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy answer</source>
        <translation>Antwort konnte nicht kopiert werden</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy Question And Answer</source>
        <translation>Frage und Antwort kopieren</translation>
    </message>
</context>
<context>
    <name>CopyClipboardSelection</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+29"/>
        <source>Selection copied to clipboard</source>
        <translation>Auswahl in die Zwischenablage kopiert</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to copy to clipboard</source>
        <translation>Kopieren in die Zwischenablage fehlgeschlagen</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy to clipboard</source>
        <translation>In die Zwischenablage kopieren</translation>
    </message>
</context>
<context>
    <name>CopyItemDeeplink</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+75"/>
        <source>Deeplink copied in clipboard</source>
        <translation>Deeplink in Zwischenablage kopiert</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Deeplink</source>
        <translation>Deeplink kopieren</translation>
    </message>
</context>
<context>
    <name>CopyShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="+230"/>
        <source>Copied to clipboard</source>
        <translation>In die Zwischenablage kopiert</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy shortcut</source>
        <translation>Verknüpfung kopieren</translation>
    </message>
</context>
<context>
    <name>CopyToClipboardAction</name>
    <message>
        <location filename="../src/clipboard-actions.hpp" line="+23"/>
        <source>Copied to clipboard</source>
        <translation>In die Zwischenablage kopiert</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy to clipboard</source>
        <translation>In die Zwischenablage kopieren</translation>
    </message>
</context>
<context>
    <name>CreateExtensionCommand</name>
    <message>
        <location filename="../src/extensions/developer/developer-extension.hpp" line="+10"/>
        <source>Create Extension</source>
        <translation>Erweiterung erstellen</translation>
    </message>
</context>
<context>
    <name>CreateExtensionFormView</name>
    <message>
        <location filename="../src/qml/qml/CreateExtensionFormView.qml" line="+15"/>
        <source>Author</source>
        <translation>Autor</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>If you plan on submitting your extension to the &lt;a href=&quot;vicinae://launch/core/store&quot;&gt;Vicinae store&lt;/a&gt;, this must exactly match your GitHub handle. Otherwise, you can set it to anything.</source>
        <translation>Wenn Sie vorhaben, Ihre Erweiterung an den &lt;a href=&quot;vicinae://launch/core/store&quot;&gt;Vicinae-Store&lt;/a&gt; zu übermitteln, muss diese genau mit Ihrem GitHub-Handle übereinstimmen. Ansonsten können Sie alles einstellen.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Username</source>
        <translation>Benutzername</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Extension Title</source>
        <translation>Erweiterungstitel</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>My Extension</source>
        <translation>Meine Erweiterung</translation>
    </message>
    <message>
        <location line="+8"/>
        <location line="+42"/>
        <source>Description</source>
        <translation>Beschreibung</translation>
    </message>
    <message>
        <location line="-36"/>
        <source>An extension that does super cool things</source>
        <translation>Eine Erweiterung, die super coole Dinge macht</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Location</source>
        <translation>Standort</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Command Title</source>
        <translation>Befehlstitel</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>My Wonderful Command</source>
        <translation>Mein wunderbarer Befehl</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>My command does this, and that...</source>
        <translation>Mein Befehl macht dies und das ...</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Template</source>
        <translation>Vorlage</translation>
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
# Erweiterung erfolgreich erstellt

Ihre neue Erweiterung %1 wurde erfolgreich unter `%2` erstellt.

Damit Befehle dieser Erweiterung von Vicinae übernommen werden können, müssen Sie Ihre Erweiterung mindestens einmal im Entwicklungsmodus ausführen:

```bash
cd %2
npm install
npm run dev
```

Weitere Informationen zur Erweiterungsentwicklung finden Sie in der [Vicinae-Dokumentation](https://docs.vicinae.com/).
</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Open in %1</source>
        <translation>In %1 öffnen</translation>
    </message>
</context>
<context>
    <name>CreateExtensionViewHost</name>
    <message>
        <location filename="../src/qml/create-extension-view-host.cpp" line="+44"/>
        <source>Create extension</source>
        <translation>Erweiterung erstellen</translation>
    </message>
    <message>
        <location line="+18"/>
        <location line="+4"/>
        <location line="+19"/>
        <location line="+5"/>
        <source>Min. 3 chars</source>
        <translation>Min. 3 Zeichen</translation>
    </message>
    <message>
        <location line="-20"/>
        <source>Min. 16 chars</source>
        <translation>Min. 16 Zeichen</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Must exist</source>
        <translation>Muss vorhanden sein</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Form has errors</source>
        <translation>Das Formular weist Fehler auf</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Failed to create extension</source>
        <translation>Erweiterung konnte nicht erstellt werden</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Extension created!</source>
        <translation>Erweiterung erstellt!</translation>
    </message>
</context>
<context>
    <name>CreateShortcutCommand</name>
    <message>
        <location filename="../src/extensions/shortcut/create-shortcut-command.hpp" line="+10"/>
        <source>Create Shortcut</source>
        <translation>Verknüpfung erstellen</translation>
    </message>
</context>
<context>
    <name>CreateShortcutFromActiveBrowserTabCommand</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="+44"/>
        <source>Create Shortcut from Active Tab</source>
        <translation>Verknüpfung aus aktivem Tab erstellen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Create a vicinae shortcut from the currently active browser tab. May yield unexpected results if many browsers are connected at once.</source>
        <translation>Erstellen Sie eine Vicinae-Verknüpfung über die aktuell aktive Browser-Registerkarte. Kann zu unerwarteten Ergebnissen führen, wenn viele Browser gleichzeitig verbunden sind.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>No active tab!</source>
        <translation>Kein aktiver Tab!</translation>
    </message>
</context>
<context>
    <name>CreateSnippetCommand</name>
    <message>
        <location filename="../src/extensions/snippet/create-snippet-command.hpp" line="+10"/>
        <source>Create Snippet</source>
        <translation>Snippet erstellen</translation>
    </message>
</context>
<context>
    <name>DMenuSection</name>
    <message>
        <location filename="../src/qml/dmenu-model.cpp" line="+93"/>
        <source>Select entry</source>
        <translation>Eintrag auswählen</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pass search text</source>
        <translation>Suchtext übergeben</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Select and copy entry</source>
        <translation>Eintrag auswählen und kopieren</translation>
    </message>
</context>
<context>
    <name>DMenuView</name>
    <message>
        <location filename="../src/qml/qml/DMenuView.qml" line="+79"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Path</source>
        <translation>Pfad</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>Typ</translation>
    </message>
</context>
<context>
    <name>DMenuViewHost</name>
    <message>
        <location filename="../src/qml/dmenu-view-host.cpp" line="+34"/>
        <source>Search entries...</source>
        <translation>Einträge durchsuchen...</translation>
    </message>
    <message>
        <location line="+73"/>
        <source>Pass search text</source>
        <translation>Suchtext übergeben</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Pass and copy search text</source>
        <translation>Suchtext übergeben und kopieren</translation>
    </message>
</context>
<context>
    <name>DetailListView</name>
    <message>
        <location filename="../src/qml/qml/DetailListView.qml" line="+32"/>
        <source>No results</source>
        <translation>Keine Ergebnisse</translation>
    </message>
</context>
<context>
    <name>DeveloperExtension</name>
    <message>
        <location filename="../src/extensions/developer/developer-extension.hpp" line="+10"/>
        <source>Developer</source>
        <translation>Entwickler</translation>
    </message>
</context>
<context>
    <name>DisableApplication</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+8"/>
        <source>Disable item</source>
        <translation>Element deaktivieren</translation>
    </message>
</context>
<context>
    <name>DisableItemAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="+88"/>
        <source>Are you sure?</source>
        <translation>Wirklich fortfahren?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You will need to go in the settings to manually re-enable it.</source>
        <translation>Sie müssen die Einstellungen aufrufen, um es manuell wieder zu aktivieren.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Disable</source>
        <translation>Deaktivieren</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Item disabled</source>
        <translation>Element deaktiviert</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to disable</source>
        <translation>Deaktivierung fehlgeschlagen</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Disable item</source>
        <translation>Element deaktivieren</translation>
    </message>
</context>
<context>
    <name>DismissNewsAction</name>
    <message>
        <location filename="../src/services/news/news-service.cpp" line="+34"/>
        <source>Dismiss</source>
        <translation>Ausblenden</translation>
    </message>
</context>
<context>
    <name>DuplicateShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-50"/>
        <source>Duplicate link</source>
        <translation>Link duplizieren</translation>
    </message>
</context>
<context>
    <name>EditClipboardKeywordsAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+49"/>
        <source>Additional keywords that will be used to index this selection.</source>
        <translation>Zusätzliche Schlüsselwörter, die zum Indexieren dieser Auswahl verwendet werden.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit keywords</source>
        <translation>Schlüsselwörter bearbeiten</translation>
    </message>
</context>
<context>
    <name>EditEmojiKeywordsAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+35"/>
        <source>Additional keywords that will be used to index this glyph</source>
        <translation>Zusätzliche Schlüsselwörter, die zum Indexieren dieses Glyphen verwendet werden</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Edit keyword</source>
        <translation>Schlüsselwort bearbeiten</translation>
    </message>
</context>
<context>
    <name>EditKeywordsFormView</name>
    <message>
        <location filename="../src/qml/qml/EditKeywordsFormView.qml" line="+19"/>
        <source>Keywords</source>
        <translation>Schlüsselwörter</translation>
    </message>
</context>
<context>
    <name>EditKeywordsViewHost</name>
    <message>
        <location filename="../src/qml/edit-keywords-view-host.cpp" line="+26"/>
        <source>Submit</source>
        <translation>Senden</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Keywords edited</source>
        <translation>Schlüsselwörter bearbeitet</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to edit keywords</source>
        <translation>Schlüsselwörter konnten nicht bearbeitet werden</translation>
    </message>
</context>
<context>
    <name>EditShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-41"/>
        <source>Edit shortcut</source>
        <translation>Verknüpfung bearbeiten</translation>
    </message>
</context>
<context>
    <name>EmojiCategoryFilterAccessory</name>
    <message>
        <location filename="../src/qml/qml/EmojiCategoryFilterAccessory.qml" line="+8"/>
        <source>All</source>
        <translation>Alle</translation>
    </message>
</context>
<context>
    <name>EmojiGridModel</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.hpp" line="+58"/>
        <source>Search for emojis and symbols...</source>
        <translation>Nach Emojis und Symbolen suchen...</translation>
    </message>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+241"/>
        <source>Pinned</source>
        <translation>Angeheftet</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Recently used</source>
        <translation>Kürzlich verwendet</translation>
    </message>
</context>
<context>
    <name>EmojiGridViewHost</name>
    <message>
        <location filename="../src/qml/emoji-grid-view-host.hpp" line="+40"/>
        <source>All</source>
        <translation>Alle</translation>
    </message>
</context>
<context>
    <name>EmptyView</name>
    <message>
        <location filename="../src/qml/qml/EmptyView.qml" line="+7"/>
        <source>No results</source>
        <translation>Keine Ergebnisse</translation>
    </message>
</context>
<context>
    <name>EnabledFallbackSection</name>
    <message>
        <location filename="../src/qml/manage-fallback-model.hpp" line="-19"/>
        <source>Enabled</source>
        <translation>Aktiviert</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-fallback-model.cpp" line="-22"/>
        <source>Disable fallback</source>
        <translation>Fallback deaktivieren</translation>
    </message>
</context>
<context>
    <name>Expansion</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.hpp" line="+31"/>
        <source>Keyword cannot be empty</source>
        <translation>Das Schlüsselwort darf nicht leer sein</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Keyword exceeds maximum length of %1</source>
        <translation>Das Schlüsselwort überschreitet die maximale Länge von %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keyword must only contain printable ASCII characters (no spaces)</source>
        <translation>Das Schlüsselwort darf nur druckbare ASCII-Zeichen enthalten (keine Leerzeichen).</translation>
    </message>
</context>
<context>
    <name>ExtensionBoilerplateGenerator</name>
    <message>
        <location filename="../src/services/extension-boilerplate-generator/extension-boilerplate-generator.cpp" line="+24"/>
        <source>Simple List</source>
        <translation>Einfache Liste</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>List with Detail</source>
        <translation>Liste mit Details</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Controlled List</source>
        <translation>Kontrollierte Liste</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Simple Detail</source>
        <translation>Einfaches Detail</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>No View</source>
        <translation>Keine Ansicht</translation>
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
        <translation># Erweiterung abgestürzt 💥!

Diese Erweiterung löste eine nicht abgefangene Ausnahme aus und stürzte daraufhin ab.

Den vollständigen Stacktrace finden Sie unten. Sie können es auch direkt aus dem Aktionsmenü kopieren.

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
        <translation>Ein oder mehrere Felder enthalten Fehler</translation>
    </message>
</context>
<context>
    <name>ExtensionGridModel</name>
    <message>
        <location filename="../src/qml/extension-grid-model.cpp" line="+186"/>
        <source>Search...</source>
        <translation>Suchen...</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>No results</source>
        <translation>Keine Ergebnisse</translation>
    </message>
</context>
<context>
    <name>ExtensionGridView</name>
    <message>
        <location filename="../src/qml/qml/ExtensionGridView.qml" line="+9"/>
        <source>No results</source>
        <translation>Keine Ergebnisse</translation>
    </message>
</context>
<context>
    <name>ExtensionListModel</name>
    <message>
        <location filename="../src/qml/extension-list-model.cpp" line="+189"/>
        <source>Search...</source>
        <translation>Suchen...</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No results</source>
        <translation>Keine Ergebnisse</translation>
    </message>
</context>
<context>
    <name>ExtensionSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/ExtensionSettingsPage.qml" line="+94"/>
        <source>Description</source>
        <translation>Beschreibung</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Preferences</source>
        <translation>Einstellungen</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Commands</source>
        <translation>Befehle</translation>
    </message>
    <message>
        <location line="+105"/>
        <source>Shortcut</source>
        <translation>Verknüpfung</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Add Alias</source>
        <translation>Alias hinzufügen</translation>
    </message>
</context>
<context>
    <name>ExtensionView</name>
    <message>
        <location filename="../src/qml/qml/ExtensionView.qml" line="+99"/>
        <source>No results</source>
        <translation>Keine Ergebnisse</translation>
    </message>
</context>
<context>
    <name>FileExtension</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="+65"/>
        <source>System files</source>
        <translation>Systemdateien</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Integrate with system files</source>
        <translation>Mit Systemdateien integrieren</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Enabled</source>
        <translation>Aktiviert</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Whether to run the file indexer in the background. When turned off, the indexer process is stopped entirely and file search becomes unavailable until it is turned back on.</source>
        <translation>Ob der Dateiindexer im Hintergrund ausgeführt werden soll. Bei Deaktivierung wird der Indexerprozess vollständig gestoppt und die Dateisuche ist erst wieder verfügbar, wenn sie wieder aktiviert wird.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Search paths</source>
        <translation>Suchpfade</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Directories that Vicinae will search</source>
        <translation>Verzeichnisse, die Vicinae durchsucht</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Excluded search paths</source>
        <translation>Ausgeschlossene Suchpfade</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Directories to exclude from file indexing</source>
        <translation>Verzeichnisse, die von der Dateiindizierung ausgeschlossen werden sollen</translation>
    </message>
</context>
<context>
    <name>FilePreview</name>
    <message>
        <location filename="../src/qml/qml/FilePreview.qml" line="+42"/>
        <source>Preview not available for this file type</source>
        <translation>Für diesen Dateityp ist keine Vorschau verfügbar</translation>
    </message>
</context>
<context>
    <name>FocusWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="-17"/>
        <source>Focus window</source>
        <translation>Fenster fokussieren</translation>
    </message>
</context>
<context>
    <name>FontBrowserViewHost</name>
    <message>
        <location filename="../src/qml/font-browser-view-host.hpp" line="+38"/>
        <source>All</source>
        <translation>Alle</translation>
    </message>
</context>
<context>
    <name>FontExtension</name>
    <message>
        <location filename="../src/extensions/font/font-extension.hpp" line="+9"/>
        <source>Font</source>
        <translation>Schriftart</translation>
    </message>
</context>
<context>
    <name>FontGridModel</name>
    <message>
        <location filename="../src/qml/font-grid-model.hpp" line="+47"/>
        <source>Search fonts...</source>
        <translation>Schriftarten suchen...</translation>
    </message>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="+133"/>
        <source>All Fonts (%1)</source>
        <translation>Alle Schriftarten (%1)</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Results (%1)</source>
        <translation>Ergebnisse (%1)</translation>
    </message>
</context>
<context>
    <name>Footer</name>
    <message>
        <location filename="../src/qml/qml/Footer.qml" line="+60"/>
        <source>Actions</source>
        <translation>Aktionen</translation>
    </message>
</context>
<context>
    <name>ForceQuitAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+95"/>
        <source>Force Quit Application</source>
        <translation>Anwendung beenden erzwingen</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to force quit %1</source>
        <translation>Das Beenden von %1 konnte nicht erzwungen werden</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Force quit %1</source>
        <translation>Beenden von %1 erzwingen</translation>
    </message>
</context>
<context>
    <name>ForgetTelemetryCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+210"/>
        <source>Forget Past Vicinae Telemetry</source>
        <translation>Frühere Vicinae-Telemetrie anonymisieren</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Asks the vicinae server to anonymize telemetry data that was sent with your vicinae instance ID attached. The ID is only linked to your vicinae install, which has no direct relationship with your system.</source>
        <translation>Fordert den Vicinae-Server auf, Telemetriedaten zu anonymisieren, die mit Ihrer Vicinae-Instanz-ID gesendet wurden. Die ID ist nur mit Ihrer Vicinae-Installation verknüpft, die keine direkte Beziehung zu Ihrem System hat.</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Processing...</source>
        <translation>Verarbeitung...</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Past telemetry was successfully detached from your vicinae user ID.</source>
        <translation>Frühere Telemetriedaten wurden erfolgreich von Ihrer Vicinae-Benutzer-ID getrennt.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to forget past telemetry data</source>
        <translation>Frühere Telemetriedaten konnten nicht anonymisiert werden</translation>
    </message>
</context>
<context>
    <name>FormAppSelector</name>
    <message>
        <location filename="../src/qml/qml/FormAppSelector.qml" line="+50"/>
        <source>All applications</source>
        <translation>Alle Anwendungen</translation>
    </message>
    <message>
        <location line="+70"/>
        <source>+ Restrict to app…</source>
        <translation>+ Auf App beschränken…</translation>
    </message>
</context>
<context>
    <name>FormFilePicker</name>
    <message>
        <location filename="../src/qml/qml/FormFilePicker.qml" line="+84"/>
        <source>Select files</source>
        <translation>Dateien auswählen</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Select a file</source>
        <translation>Datei auswählen</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Select a directory</source>
        <translation>Verzeichnis auswählen</translation>
    </message>
    <message>
        <location line="+46"/>
        <source>No directory selected</source>
        <translation>Kein Verzeichnis ausgewählt</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No file selected</source>
        <translation>Keine Datei ausgewählt</translation>
    </message>
    <message>
        <location line="+115"/>
        <source>+ Add folder…</source>
        <translation>+ Ordner hinzufügen…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>+ Add file…</source>
        <translation>+ Datei hinzufügen…</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsModel</name>
    <message>
        <location filename="../src/qml/general-settings-model.cpp" line="+176"/>
        <location line="+11"/>
        <source>None</source>
        <translation>Keine</translation>
    </message>
    <message>
        <location line="-10"/>
        <location line="+10"/>
        <source>Blurred</source>
        <translation>Verschwommen</translation>
    </message>
    <message>
        <location line="-8"/>
        <location line="+8"/>
        <source>Liquid Glass</source>
        <translation>Flüssiges Glas</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Window material</source>
        <translation>Fensterhintergrund</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Themes</source>
        <translation>Themen</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Fonts</source>
        <translation>Schriftarten</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Icon Themes</source>
        <translation>Symbolthemen</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Favicon Services</source>
        <translation>Favicon-Dienste</translation>
    </message>
    <message>
        <location line="+13"/>
        <location line="+10"/>
        <source>Default</source>
        <translation>Standard</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Keybinding Schemes</source>
        <translation>Tastaturbelegungen</translation>
    </message>
    <message>
        <location line="+48"/>
        <location line="+10"/>
        <source>System default</source>
        <translation>Systemvorgabe</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>Languages</source>
        <translation>Sprachen</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/GeneralSettingsPage.qml" line="+29"/>
        <source>Behavior</source>
        <translation>Verhalten</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Launcher hotkey</source>
        <translation>Launcher-Tastenkürzel</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Global shortcut to toggle the Vicinae launcher.</source>
        <translation>Globales Tastenkürzel zum Ein- und Ausblenden des Vicinae-Launchers.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Close on focus loss</source>
        <translation>Bei Fokusverlust schließen</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Close on Escape</source>
        <translation>Mit Escape schließen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pressing Escape closes the launcher instead of navigating one view back.</source>
        <translation>Durch Drücken von Escape wird der Launcher geschlossen, anstatt eine Ansicht zurück zu navigieren.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Pop to root on close</source>
        <translation>Beim Schließen zur Hauptansicht zurückkehren</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reset the navigation state when the launcher window is closed.</source>
        <translation>Navigationsstatus beim Schließen des Launcher-Fensters zurücksetzen.</translation>
    </message>
    <message>
        <location line="+10"/>
        <location line="+7"/>
        <source>Language</source>
        <translation>Sprache</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Requires restarting Vicinae to take effect.</source>
        <translation>Erfordert einen Neustart von Vicinae, damit es wirksam wird.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Privacy</source>
        <translation>Privatsphäre</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Basic usage statistics</source>
        <translation>Grundlegende Nutzungsstatistiken</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Send basic system and vicinae installation information on startup to help improve Vicinae.</source>
        <translation>Senden Sie beim Start grundlegende System- und Vicinae-Installationsinformationen, um zur Verbesserung von Vicinae beizutragen.</translation>
    </message>
</context>
<context>
    <name>GenericGridView</name>
    <message>
        <location filename="../src/qml/qml/GenericGridView.qml" line="+33"/>
        <source>No results</source>
        <translation>Keine Ergebnisse</translation>
    </message>
</context>
<context>
    <name>GenericListView</name>
    <message>
        <location filename="../src/qml/qml/GenericListView.qml" line="+25"/>
        <source>No results</source>
        <translation>Keine Ergebnisse</translation>
    </message>
</context>
<context>
    <name>GlobalShortcutService</name>
    <message>
        <location filename="../src/services/global-shortcuts/global-shortcut-service.cpp" line="+43"/>
        <source>Toggle Vicinae</source>
        <translation>Vicinae ein-/ausblenden</translation>
    </message>
    <message>
        <location line="+84"/>
        <source>the launcher hotkey</source>
        <translation>der Launcher-Hotkey</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>another command</source>
        <translation>ein weiterer Befehl</translation>
    </message>
</context>
<context>
    <name>Gnome::Workspace</name>
    <message>
        <location filename="../src/services/window-manager/gnome/gnome-workspace.cpp" line="+18"/>
        <source>Workspace %1</source>
        <translation>Arbeitsbereich %1</translation>
    </message>
</context>
<context>
    <name>HibernateCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+127"/>
        <source>Hibernate System</source>
        <translation>System in den Ruhezustand versetzen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Suspend the system to disk. This turns off the system completely and saves its state on disk, to be restored on next boot.</source>
        <translation>Systemzustand auf der Festplatte speichern und das System vollständig ausschalten. Der Zustand wird beim nächsten Start wiederhergestellt.</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>System can&apos;t hibernate</source>
        <translation>Das System kann nicht in den Ruhezustand wechseln</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to hibernate</source>
        <translation>Der Ruhezustand konnte nicht durchgeführt werden</translation>
    </message>
</context>
<context>
    <name>IconBrowserCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+25"/>
        <source>Search Builtin Icons</source>
        <translation>Integrierte Symbole durchsuchen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search Vicinae builtin set of icons</source>
        <translation>Integrierten Symbolsatz von Vicinae durchsuchen</translation>
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
        <translation>Lokalen Speicher untersuchen</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Browse data stored in Vicinae&apos;s local storage. This includes data stored for builtin extensions as well as third-party extensions making use of the LocalStorage API.</source>
        <translation>Durchsuchen Sie die im lokalen Speicher von Vicinae gespeicherten Daten. Dazu gehören Daten, die für integrierte Erweiterungen gespeichert sind, sowie für Erweiterungen von Drittanbietern, die LocalStorage API nutzen.</translation>
    </message>
</context>
<context>
    <name>InstallUpdateAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+188"/>
        <source>Install Update</source>
        <translation>Update installieren</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>An update is already in progress</source>
        <translation>Ein Update ist bereits in Bearbeitung</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsSection</name>
    <message>
        <location filename="../src/qml/installed-extensions-model.hpp" line="+18"/>
        <source>Installed Extensions ({count})</source>
        <translation>Installierte Erweiterungen ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/installed-extensions-model.cpp" line="+36"/>
        <source>Local</source>
        <translation>Lokal</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Copy</source>
        <translation>Kopieren</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy Name</source>
        <translation>Namen kopieren</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy ID</source>
        <translation>ID kopieren</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy Path</source>
        <translation>Pfad kopieren</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy Author</source>
        <translation>Autor kopieren</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsViewHost</name>
    <message>
        <location filename="../src/qml/installed-extensions-view-host.cpp" line="+12"/>
        <source>Search extensions...</source>
        <translation>Erweiterungen suchen...</translation>
    </message>
</context>
<context>
    <name>InternalExtension</name>
    <message>
        <location filename="../src/extensions/internal/internal-extension.hpp" line="+32"/>
        <location line="+1"/>
        <source>Internal Commands</source>
        <translation>Interne Befehle</translation>
    </message>
</context>
<context>
    <name>KeyboardBridge</name>
    <message>
        <location filename="../src/qml/keyboard-bridge.hpp" line="+51"/>
        <source>Modifier required</source>
        <translation>Modifikator erforderlich</translation>
    </message>
</context>
<context>
    <name>LauncherWindow</name>
    <message>
        <location filename="../src/qml/qml/LauncherWindow.qml" line="+29"/>
        <source>Vicinae Launcher</source>
        <translation>Vicinae Launcher</translation>
    </message>
    <message>
        <location filename="../src/qml/launcher-window.cpp" line="+571"/>
        <source>Open Settings</source>
        <translation>Einstellungen öffnen</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keyboard Shortcuts</source>
        <translation>Tastaturkürzel</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Documentation</source>
        <translation>Dokumentation</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+5"/>
        <source>Opened in browser</source>
        <translation>Im Browser geöffnet</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Report a Bug</source>
        <translation>Fehler melden</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>About Vicinae</source>
        <translation>Über Vicinae</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemSection</name>
    <message>
        <location filename="../src/qml/local-storage-model.hpp" line="+19"/>
        <source>Items ({count})</source>
        <translation>Einträge ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/local-storage-model.cpp" line="+31"/>
        <source>Show value</source>
        <translation>Wert anzeigen</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemViewHost</name>
    <message>
        <location filename="../src/qml/local-storage-view-host.cpp" line="+23"/>
        <source>Search items...</source>
        <translation>Einträge suchen...</translation>
    </message>
</context>
<context>
    <name>LocalStorageNamespaceSection</name>
    <message>
        <location filename="../src/qml/local-storage-model.hpp" line="-11"/>
        <source>Namespaces ({count})</source>
        <translation>Namensräume ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/local-storage-model.cpp" line="-16"/>
        <source>Browse namespace</source>
        <translation>Namespace durchsuchen</translation>
    </message>
</context>
<context>
    <name>LocalStorageViewHost</name>
    <message>
        <location filename="../src/qml/local-storage-view-host.cpp" line="-15"/>
        <source>Search namespaces...</source>
        <translation>Namensräume durchsuchen...</translation>
    </message>
</context>
<context>
    <name>LockCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-48"/>
        <source>Lock Session</source>
        <translation>Sitzung sperren</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lock the current user session</source>
        <translation>Aktuelle Benutzersitzung sperren</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System can&apos;t lock</source>
        <translation>System kann nicht gesperrt werden</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to lock</source>
        <translation>Sperren fehlgeschlagen</translation>
    </message>
</context>
<context>
    <name>LogOutCommand</name>
    <message>
        <location line="+176"/>
        <source>Log Out</source>
        <translation>Abmelden</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Terminate the current user session. If you simply want to lock your session you should use &apos;Lock Session&apos; instead.</source>
        <translation>Beenden Sie die aktuelle Benutzersitzung. Wenn Sie Ihre Sitzung lediglich sperren möchten, sollten Sie stattdessen „Sitzung sperren“ verwenden.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>System can&apos;t logout</source>
        <translation>Das System kann sich nicht abmelden</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to log out</source>
        <translation>Abmelden fehlgeschlagen</translation>
    </message>
</context>
<context>
    <name>MacOSGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/macos-global-shortcut-backend.cpp" line="+238"/>
        <source>unsupported or invalid trigger</source>
        <translation>nicht unterstützter oder ungültiger Trigger</translation>
    </message>
</context>
<context>
    <name>MacSettingsRootItem</name>
    <message>
        <location filename="../src/root-search/macos-settings/macos-settings-root-provider.mm" line="+132"/>
        <location line="+9"/>
        <source>System Settings</source>
        <translation>Systemeinstellungen</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Bundle ID</source>
        <translation>Bundle-ID</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Legacy ID</source>
        <translation>Legacy-ID</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Where</source>
        <translation>Wo</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Open %1 Settings</source>
        <translation>%1-Einstellungen öffnen</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy URL</source>
        <translation>URL kopieren</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy Bundle ID</source>
        <translation>Bundle-ID kopieren</translation>
    </message>
</context>
<context>
    <name>MacSettingsRootProvider</name>
    <message>
        <location line="+13"/>
        <source>System Settings</source>
        <translation>Systemeinstellungen</translation>
    </message>
</context>
<context>
    <name>MacosUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/macos-update-installer.mm" line="+208"/>
        <source>This installation cannot update itself</source>
        <translation>Diese Installation kann sich nicht selbst aktualisieren</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Mounting update image…</source>
        <translation>Update-Image wird geladen…</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Failed to mount the update image</source>
        <translation>Das Update-Image konnte nicht gemountet werden</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Could not find the update image mount point</source>
        <translation>Der Mountpunkt für das Update-Image konnte nicht gefunden werden</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Verifying update…</source>
        <translation>Aktualisierung wird überprüft…</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Installing update…</source>
        <translation>Update wird installiert…</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to stage update: %1</source>
        <translation>Aktualisierung konnte nicht bereitgestellt werden: %1</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Failed to move the current app aside: %1</source>
        <translation>Die aktuelle App konnte nicht zur Seite verschoben werden: %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to install the new app: %1</source>
        <translation>Die neue App konnte nicht installiert werden: %1</translation>
    </message>
</context>
<context>
    <name>ManageFallbackActions</name>
    <message>
        <location filename="../src/actions/fallback-actions.hpp" line="+15"/>
        <source>Manage Fallback Actions</source>
        <translation>Fallback-Aktionen verwalten</translation>
    </message>
</context>
<context>
    <name>ManageFallbackCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/configure-fallback-command.hpp" line="+11"/>
        <source>Configure Fallback Commands</source>
        <translation>Fallback-Befehle konfigurieren</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Configure what commands are to be presented as fallback options when nothing matches the search in the root search.</source>
        <translation>Konfigurieren Sie, welche Befehle als Fallback-Optionen angezeigt werden sollen, wenn nichts mit der Suche in der Stammsuche übereinstimmt.</translation>
    </message>
</context>
<context>
    <name>ManageFallbackViewHost</name>
    <message>
        <location filename="../src/qml/manage-fallback-view-host.cpp" line="+12"/>
        <source>Search commands...</source>
        <translation>Befehle suchen...</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsCommand</name>
    <message>
        <location filename="../src/extensions/shortcut/manage-shortcuts-command.hpp" line="+9"/>
        <source>Manage Shortcuts</source>
        <translation>Verknüpfungen verwalten</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsSection</name>
    <message>
        <location filename="../src/qml/manage-shortcuts-model.hpp" line="+17"/>
        <source>Shortcuts ({count})</source>
        <translation>Verknüpfungen ({count})</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsViewHost</name>
    <message>
        <location filename="../src/qml/manage-shortcuts-view-host.cpp" line="+28"/>
        <source>Search shortcuts...</source>
        <translation>Verknüpfungen suchen...</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Application</source>
        <translation>Anwendung</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Opened</source>
        <translation>Geöffnet</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Last Opened</source>
        <translation>Zuletzt geöffnet</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Never</source>
        <translation>Niemals</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Created at</source>
        <translation>Erstellt am</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsCommand</name>
    <message>
        <location filename="../src/extensions/snippet/manage-snippets-command.hpp" line="+10"/>
        <source>Manage Snippets</source>
        <translation>Snippets verwalten</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsSection</name>
    <message>
        <location filename="../src/qml/manage-snippets-model.hpp" line="+18"/>
        <source>Snippets ({count})</source>
        <translation>Snippets ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-snippets-model.cpp" line="+32"/>
        <source>Copy to clipboard</source>
        <translation>In die Zwischenablage kopieren</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copied to clipboard</source>
        <translation>In die Zwischenablage kopiert</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy to clipboard</source>
        <translation>Kopieren in die Zwischenablage fehlgeschlagen</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Edit snippet</source>
        <translation>Snippet bearbeiten</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Duplicate snippet</source>
        <translation>Snippet duplizieren</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Remove snippet</source>
        <translation>Snippet entfernen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove snippet</source>
        <translation>Snippet konnte nicht entfernt werden</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsViewHost</name>
    <message>
        <location filename="../src/qml/manage-snippets-view-host.hpp" line="+54"/>
        <source>No snippets</source>
        <translation>Keine Snippets</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Create a snippet to get started</source>
        <translation>Zum Einstieg ein Snippet erstellen</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-snippets-view-host.cpp" line="+28"/>
        <source>Search for snippets...</source>
        <translation>Snippets suchen...</translation>
    </message>
    <message>
        <location line="+25"/>
        <source>Text</source>
        <translation>Text</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>File</source>
        <translation>Datei</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>Typ</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Created at</source>
        <translation>Erstellt am</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Updated at</source>
        <translation>Aktualisiert am</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Keyword</source>
        <translation>Stichwort</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Apps</source>
        <translation>Apps</translation>
    </message>
    <message>
        <location line="+68"/>
        <source>Create snippet</source>
        <translation>Snippet erstellen</translation>
    </message>
</context>
<context>
    <name>MarkItemAsFavorite</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="-60"/>
        <source>Mark as favorite</source>
        <translation>Als Favorit markieren</translation>
    </message>
</context>
<context>
    <name>MarkdownShowcase</name>
    <message>
        <location filename="../src/extensions/internal/markdown-showcase-command.hpp" line="+172"/>
        <source>Markdown Showcase</source>
        <translation>Markdown-Showcase</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Preview all supported markdown features</source>
        <translation>Vorschau aller unterstützten Markdown-Funktionen</translation>
    </message>
</context>
<context>
    <name>MarkdownView</name>
    <message>
        <location filename="../src/qml/qml/markdown/MarkdownView.qml" line="+265"/>
        <source>Copy</source>
        <translation>Kopieren</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Select All</source>
        <translation>Alle auswählen</translation>
    </message>
</context>
<context>
    <name>MdCallout</name>
    <message>
        <location filename="../src/qml/qml/markdown/MdCallout.qml" line="+35"/>
        <source>Caution</source>
        <translation>Achtung</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Warning</source>
        <translation>Warnung</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Important</source>
        <translation>Wichtig</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tip</source>
        <translation>Tipp</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Note</source>
        <translation>Hinweis</translation>
    </message>
</context>
<context>
    <name>MdCodeBlock</name>
    <message>
        <location filename="../src/qml/qml/markdown/MdCodeBlock.qml" line="+58"/>
        <source>Copied!</source>
        <translation>Kopiert!</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Copy</source>
        <translation>Kopieren</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceView</name>
    <message>
        <location filename="../src/qml/qml/MissingPreferenceView.qml" line="+28"/>
        <source>Welcome to %1</source>
        <translation>Willkommen bei %1</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Before you can use this command, you need to fill in the required preference fields below.</source>
        <translation>Bevor Sie diesen Befehl verwenden können, müssen Sie unten die erforderlichen Einstellungsfelder ausfüllen.</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceViewHost</name>
    <message>
        <location filename="../src/qml/missing-preference-view-host.cpp" line="+202"/>
        <source>Save preferences</source>
        <translation>Einstellungen speichern</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Please fill in all required fields</source>
        <translation>Bitte füllen Sie alle erforderlichen Felder aus</translation>
    </message>
</context>
<context>
    <name>NavigationController</name>
    <message>
        <location filename="../src/navigation-controller.cpp" line="+654"/>
        <source>Extension manager is not running</source>
        <translation>Der Erweiterungsmanager wird nicht ausgeführt</translation>
    </message>
</context>
<context>
    <name>NewsService</name>
    <message>
        <location filename="../src/services/news/news-service.cpp" line="+68"/>
        <source>Telemetry</source>
        <translation>Telemetrie</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>We now collect basic usage statistics on startup</source>
        <translation>Wir sammeln jetzt grundlegende Nutzungsstatistiken beim Start</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Learn more</source>
        <translation>Mehr erfahren</translation>
    </message>
</context>
<context>
    <name>NullUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/null-update-installer.hpp" line="+14"/>
        <source>Self update is not supported on this platform</source>
        <translation>Selbstaktualisierung wird auf dieser Plattform nicht unterstützt</translation>
    </message>
</context>
<context>
    <name>OAuthOverlayView</name>
    <message>
        <location filename="../src/qml/qml/OAuthOverlayView.qml" line="+91"/>
        <source>Continue with %1</source>
        <translation>Mit %1 fortfahren</translation>
    </message>
    <message>
        <location line="+33"/>
        <source>You&apos;re in!</source>
        <translation>Verbunden!</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Successfully connected to %1.
Back to command in an instant...</source>
        <translation>Erfolgreich mit %1 verbunden.
Sofort zurück zum Befehl ...</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-31"/>
        <source>Manage OAuth Token Sets</source>
        <translation>OAuth-Token-Sets verwalten</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Manage OAuth token sets that have been saved by extensions providing OAuth integrations.</source>
        <translation>Verwalten Sie OAuth-Token-Sets, die von Erweiterungen gespeichert wurden, die OAuth-Integrationen bereitstellen.</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreSection</name>
    <message>
        <location filename="../src/qml/oauth-token-store-model.hpp" line="+17"/>
        <source>OAuth Token Sets ({count})</source>
        <translation>OAuth Token-Sets ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/oauth-token-store-model.cpp" line="+20"/>
        <source>Expired</source>
        <translation>Abgelaufen</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Remove token set</source>
        <translation>Tokensatz entfernen</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Are you sure?</source>
        <translation>Wirklich fortfahren?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You will need to go through the OAuth login flow again the next time you want to use this service</source>
        <translation>Wenn Sie diesen Dienst das nächste Mal nutzen möchten, müssen Sie den OAuth-Anmeldevorgang erneut durchlaufen</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to remove token set</source>
        <translation>Tokensatz konnte nicht entfernt werden</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Token set removed</source>
        <translation>Tokensatz entfernt</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy</source>
        <translation>Kopieren</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy Access Token</source>
        <translation>Zugriffstoken kopieren</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Refresh Token</source>
        <translation>Aktualisierungstoken kopieren</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy ID Token</source>
        <translation>ID-Token kopieren</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy Scopes</source>
        <translation>Berechtigungsbereiche kopieren</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Expiration Date</source>
        <translation>Ablaufdatum kopieren</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreViewHost</name>
    <message>
        <location filename="../src/qml/oauth-token-store-view-host.cpp" line="+12"/>
        <source>Search token sets...</source>
        <translation>Token-Sets suchen...</translation>
    </message>
</context>
<context>
    <name>OnboardingWindow</name>
    <message>
        <location filename="../src/qml/qml/OnboardingWindow.qml" line="+39"/>
        <source>Grant Access</source>
        <translation>Zugriff gewähren</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Granted</source>
        <translation>Gewährt</translation>
    </message>
    <message>
        <location line="+16"/>
        <location line="+39"/>
        <source>Welcome to Vicinae</source>
        <translation>Willkommen bei Vicinae</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Let&apos;s set it up. It only takes a minute.</source>
        <translation>Die Einrichtung dauert nur eine Minute.</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Permissions</source>
        <translation>Berechtigungen</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Vicinae needs additional permissions in order to make the best of your Mac.</source>
        <translation>Vicinae benötigt zusätzliche Berechtigungen, um das Beste aus Ihrem Mac herauszuholen.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Accessibility</source>
        <translation>Barrierefreiheit</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Used to paste, expand snippets, and move windows.</source>
        <translation>Wird zum Einfügen, Erweitern von Snippets und Verschieben von Fenstern verwendet.</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Full Disk Access</source>
        <translation>Vollständiger Festplattenzugriff</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Notifications</source>
        <translation>Benachrichtigungen</translation>
    </message>
    <message>
        <location line="-8"/>
        <source>Allows file search to cover your entire disk.</source>
        <translation>Ermöglicht die Dateisuche auf Ihrer gesamten Festplatte.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Allows extensions to send desktop notifications.</source>
        <translation>Ermöglicht Erweiterungen das Senden von Desktop-Benachrichtigungen.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Accessibility is required: global shortcuts, paste, and snippet expansion cannot work without it.</source>
        <translation>Bedienungshilfen sind erforderlich: Globale Tastenkürzel, Einfügen und Snippet-Erweiterung funktionieren ohne sie nicht.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Full disk access needs to be explicitly enabled if you want file search to cover all your files.</source>
        <translation>Wenn Sie möchten, dass die Dateisuche alle Ihre Dateien abdeckt, muss der vollständige Festplattenzugriff explizit aktiviert werden.</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Make it your own</source>
        <translation>Nach eigenen Wünschen anpassen</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>You will be able to change these settings later.</source>
        <translation>Sie können diese Einstellungen später ändern.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Theme</source>
        <translation>Design</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Shared across the entire app.</source>
        <translation>Wird in der gesamten App geteilt.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Global hotkey</source>
        <translation>Globales Tastenkürzel</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Opens the launcher from anywhere.</source>
        <translation>Öffnet den Launcher von überall.</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Launch at login</source>
        <translation>Bei der Anmeldung starten</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Starts Vicinae in the background at login.</source>
        <translation>Startet Vicinae im Hintergrund bei der Anmeldung.</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Setup complete</source>
        <translation>Einrichtung abgeschlossen</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Vicinae is running. Open the launcher with:</source>
        <translation>Vicinae läuft. Öffnen Sie den Launcher mit:</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Vicinae is open source software.</source>
        <translation>Vicinae ist Open-Source-Software.</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Sponsor</source>
        <translation>Sponsor</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Back</source>
        <translation>Zurück</translation>
    </message>
    <message>
        <location line="+44"/>
        <source>Finish</source>
        <translation>Fertig</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Continue</source>
        <translation>Weiter</translation>
    </message>
</context>
<context>
    <name>OpenAboutCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/open-about-command.hpp" line="+13"/>
        <source>About</source>
        <translation>Über</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the &quot;About&quot; tab of the vicinae settings.</source>
        <translation>Öffnen Sie die Registerkarte „Info“ der Vicinae-Einstellungen.</translation>
    </message>
</context>
<context>
    <name>OpenAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-56"/>
        <source>Failed to start app</source>
        <translation>App konnte nicht gestartet werden</translation>
    </message>
</context>
<context>
    <name>OpenAppLocationAction</name>
    <message>
        <location line="-36"/>
        <source>Open Location</source>
        <translation>Speicherort öffnen</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to open app location</source>
        <translation>App-Standort konnte nicht geöffnet werden</translation>
    </message>
</context>
<context>
    <name>OpenBuiltinCommandAction</name>
    <message>
        <location filename="../src/command-actions.hpp" line="+17"/>
        <source>Open command</source>
        <translation>Befehl öffnen</translation>
    </message>
</context>
<context>
    <name>OpenCalculatorHistoryAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+9"/>
        <source>Open Calculator History</source>
        <translation>Rechenverlauf öffnen</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-32"/>
        <source>Open shortcut</source>
        <translation>Verknüpfung öffnen</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutWithAction</name>
    <message>
        <location line="+85"/>
        <source>Open with...</source>
        <translation>Öffnen mit...</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelItemAction</name>
    <message>
        <location filename="../src/root-search/control-panel/control-panel-root-provider.cpp" line="+45"/>
        <source>Failed to open settings</source>
        <translation>Die Einstellungen konnten nicht geöffnet werden</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelTaskAction</name>
    <message>
        <location line="+26"/>
        <source>Failed to open settings</source>
        <translation>Die Einstellungen konnten nicht geöffnet werden</translation>
    </message>
</context>
<context>
    <name>OpenDefaultVicinaeConfig</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-157"/>
        <source>Open Default Config File</source>
        <translation>Standardkonfiguration öffnen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the default vicinae configuration file</source>
        <translation>Standardkonfigurationsdatei von Vicinae öffnen</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Failed to open temporary file</source>
        <translation>Die temporäre Datei konnte nicht geöffnet werden</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Failed to open default config file</source>
        <translation>Standardkonfigurationsdatei konnte nicht geöffnet werden</translation>
    </message>
</context>
<context>
    <name>OpenDiscordCommand</name>
    <message>
        <location line="-66"/>
        <source>Join the Discord Server</source>
        <translation>Discord-Server beitreten</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open link to join the official Vicinae discord server.</source>
        <translation>Öffnen Sie den Link, um dem offiziellen Vicinae-Discord-Server beizutreten.</translation>
    </message>
</context>
<context>
    <name>OpenDocumentationCommand</name>
    <message>
        <location line="-16"/>
        <source>Open Online Documentation</source>
        <translation>Online-Dokumentation öffnen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Navigate to the official vicinae documentation website.</source>
        <translation>Navigieren Sie zur offiziellen Vicinae-Dokumentationswebsite.</translation>
    </message>
</context>
<context>
    <name>OpenFileAction</name>
    <message>
        <location filename="../src/actions/files/file-actions.hpp" line="+18"/>
        <source>Open with %1</source>
        <translation>Mit %1 öffnen</translation>
    </message>
</context>
<context>
    <name>OpenInBrowserAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="+105"/>
        <source>Open in browser</source>
        <translation>Im Browser öffnen</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+95"/>
        <source>Failed to open in browser</source>
        <translation>Öffnen im Browser fehlgeschlagen</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Opened in browser</source>
        <translation>Im Browser geöffnet</translation>
    </message>
</context>
<context>
    <name>OpenInTerminalAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="-62"/>
        <source>Open in %1</source>
        <translation>In %1 öffnen</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-85"/>
        <source>Failed to start app</source>
        <translation>App konnte nicht gestartet werden</translation>
    </message>
</context>
<context>
    <name>OpenItemPreferencesAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="-28"/>
        <source>Open Preferences</source>
        <translation>Einstellungen öffnen</translation>
    </message>
</context>
<context>
    <name>OpenKeybindSettingsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+156"/>
        <source>Open Vicinae Keybind Settings</source>
        <translation>Vicinae-Tastaturbelegung öffnen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the vicinae keybind settings window</source>
        <translation>Einstellungen für die Vicinae-Tastaturbelegung öffnen</translation>
    </message>
</context>
<context>
    <name>OpenRawProgramAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="+25"/>
        <source>Execute program</source>
        <translation>Programm ausführen</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+35"/>
        <source>Failed to start app</source>
        <translation>App konnte nicht gestartet werden</translation>
    </message>
</context>
<context>
    <name>OpenSettingsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-40"/>
        <source>Open Vicinae Settings</source>
        <translation>Vicinae-Einstellungen öffnen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open the vicinae settings window, which is an independent floating window.</source>
        <translation>Öffnen Sie das Vicinae-Einstellungsfenster, bei dem es sich um ein unabhängiges schwebendes Fenster handelt.</translation>
    </message>
</context>
<context>
    <name>OpenSettingsPaneAction</name>
    <message>
        <location filename="../src/root-search/macos-settings/macos-settings-root-provider.mm" line="-135"/>
        <source>Failed to open System Settings</source>
        <translation>Die Systemeinstellungen konnten nicht geöffnet werden</translation>
    </message>
</context>
<context>
    <name>OpenShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-128"/>
        <source>No app with id %1</source>
        <translation>Keine App mit der ID %1</translation>
    </message>
    <message>
        <location line="+9"/>
        <location line="+7"/>
        <source>Open shortcut</source>
        <translation>Verknüpfung öffnen</translation>
    </message>
</context>
<context>
    <name>OpenShortcutFromSearchText</name>
    <message>
        <location line="+43"/>
        <source>Open shortcut</source>
        <translation>Verknüpfung öffnen</translation>
    </message>
</context>
<context>
    <name>OpenVicinaeConfig</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-81"/>
        <source>Open Config File</source>
        <translation>Konfigurationsdatei öffnen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the main vicinae configuration file</source>
        <translation>Hauptkonfigurationsdatei von Vicinae öffnen</translation>
    </message>
</context>
<context>
    <name>OpenWindowsSettingAction</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="+147"/>
        <source>Failed to open settings</source>
        <translation>Die Einstellungen konnten nicht geöffnet werden</translation>
    </message>
</context>
<context>
    <name>OpenWithAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+54"/>
        <source>Open with...</source>
        <translation>Öffnen mit...</translation>
    </message>
</context>
<context>
    <name>PasteToFocusedWindowAction</name>
    <message>
        <location filename="../src/clipboard-actions.hpp" line="+11"/>
        <source>Paste to active window</source>
        <translation>In aktives Fenster einfügen</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy to focused window</source>
        <translation>In fokussiertes Fenster kopieren</translation>
    </message>
</context>
<context>
    <name>PinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+28"/>
        <source>Entry pinned</source>
        <translation>Eintrag angepinnt</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pin entry</source>
        <translation>Eintrag anheften</translation>
    </message>
</context>
<context>
    <name>PinClipboardAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="-26"/>
        <source>Selection pinned</source>
        <translation>Auswahl angeheftet</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Selection unpinned</source>
        <translation>Auswahl gelöst</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to change pin status</source>
        <translation>Der Pin-Status konnte nicht geändert werden</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Pin</source>
        <translation>Anheften</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unpin</source>
        <translation>Lösen</translation>
    </message>
</context>
<context>
    <name>PinEmojiAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-322"/>
        <source>Pin emoji</source>
        <translation>Emoji anheften</translation>
    </message>
</context>
<context>
    <name>PinWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="+37"/>
        <source>Unpin from all workspaces</source>
        <translation>Von allen Arbeitsbereichen lösen</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Pin to all workspaces</source>
        <translation>An alle Arbeitsbereiche anheften</translation>
    </message>
</context>
<context>
    <name>PowerManagementCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-283"/>
        <source>Ask for confirmation</source>
        <translation>Bestätigung anfordern</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Custom program</source>
        <translation>Benutzerdefiniertes Programm</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Custom POSIX shell command to run instead of the default implementation</source>
        <translation>Benutzerdefinierter POSIX-Shell-Befehl, der anstelle der Standardimplementierung ausgeführt wird</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Failed to execute custom program %1</source>
        <translation>Das benutzerdefinierte Programm %1 konnte nicht ausgeführt werden</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Are you sure</source>
        <translation>Wirklich fortfahren</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>High-impact operation, please confirm</source>
        <translation>Vorgang mit weitreichenden Folgen – bitte bestätigen</translation>
    </message>
</context>
<context>
    <name>PowerManagementExtension</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.hpp" line="+8"/>
        <source>Power Management</source>
        <translation>Energieverwaltung</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power off, suspend, sleep, hibernate your computer.</source>
        <translation>Computer ausschalten, anhalten oder in den Energiespar- beziehungsweise Ruhezustand versetzen.</translation>
    </message>
</context>
<context>
    <name>PowerOffCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+137"/>
        <source>Power Off System</source>
        <translation>System ausschalten</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power off the system</source>
        <translation>System ausschalten</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>System cannot power off</source>
        <translation>Das System lässt sich nicht ausschalten</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to power off</source>
        <translation>Ausschalten fehlgeschlagen</translation>
    </message>
</context>
<context>
    <name>PreviewFontAction</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="-105"/>
        <source>Preview font</source>
        <translation>Schriftartvorschau</translation>
    </message>
</context>
<context>
    <name>ProgramsSection</name>
    <message>
        <location filename="../src/qml/system-run-model.hpp" line="+21"/>
        <source>Programs (%1)</source>
        <translation>Programme (%1)</translation>
    </message>
    <message>
        <location filename="../src/qml/system-run-model.cpp" line="+52"/>
        <source>Open in %1 (hold)</source>
        <translation>In %1 öffnen (gedrückt halten)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open in %1</source>
        <translation>In %1 öffnen</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Copy exec path</source>
        <translation>Programmpfad kopieren</translation>
    </message>
</context>
<context>
    <name>ProviderSearchSection</name>
    <message>
        <location filename="../src/qml/provider-search-model.hpp" line="+11"/>
        <source>Results ({count})</source>
        <translation>Ergebnisse ({count})</translation>
    </message>
</context>
<context>
    <name>ProviderSearchViewHost</name>
    <message>
        <location filename="../src/qml/provider-search-view-host.cpp" line="+15"/>
        <source>Search %1</source>
        <translation>%1 durchsuchen</translation>
    </message>
</context>
<context>
    <name>PruneMemoryCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+56"/>
        <source>Prune Vicinae Memory Usage</source>
        <translation>Vicinae-Speichernutzung bereinigen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Try pruning vicinae&apos;s memory usage by clearing pixmap cache and calling malloc_trim(). Mostly provided for internal testing.</source>
        <translation>Versuchen Sie, die Speichernutzung von Vicinae zu reduzieren, indem Sie den Pixmap-Cache leeren und malloc_trim() aufrufen. Wird hauptsächlich für interne Tests bereitgestellt.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Pruned 🥊</source>
        <translation>Speicher bereinigt 🥊</translation>
    </message>
</context>
<context>
    <name>PutCalculatorAnswerInSearchBar</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-20"/>
        <source>Put answer in search bar</source>
        <translation>Ergebnis in die Suchleiste übernehmen</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/utils/utils.cpp" line="+50"/>
        <source>in the future</source>
        <translation>in der Zukunft</translation>
    </message>
    <message numerus="yes">
        <location line="+8"/>
        <source>%n year(s) ago</source>
        <translation>
            <numerusform>vor %n Jahr</numerusform>
            <numerusform>vor %n Jahren</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+3"/>
        <source>%n month(s) ago</source>
        <translation>
            <numerusform>vor %n Monat</numerusform>
            <numerusform>vor %n Monaten</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n day(s) ago</source>
        <translation>
            <numerusform>vor %n Tag</numerusform>
            <numerusform>vor %n Tagen</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n hour(s) ago</source>
        <translation>
            <numerusform>vor %n Stunde</numerusform>
            <numerusform>vor %n Stunden</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n minute(s) ago</source>
        <translation>
            <numerusform>vor %n Minute</numerusform>
            <numerusform>vor %n Minuten</numerusform>
        </translation>
    </message>
    <message>
        <location line="+2"/>
        <source>just now</source>
        <translation>gerade jetzt</translation>
    </message>
</context>
<context>
    <name>QuitAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-43"/>
        <source>Quit Application</source>
        <translation>Anwendung beenden</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to quit %1</source>
        <translation>%1 konnte nicht beendet werden</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Quit %1</source>
        <translation>%1 beenden</translation>
    </message>
</context>
<context>
    <name>RaycastCompatExtension</name>
    <message>
        <location filename="../src/extensions/raycast/raycast-compat-extension.hpp" line="+15"/>
        <source>Raycast compatibility features</source>
        <translation>Raycast-Kompatibilitätsfunktionen</translation>
    </message>
</context>
<context>
    <name>RaycastStoreCommand</name>
    <message>
        <location filename="../src/extensions/raycast/raycast-store-command.hpp" line="+13"/>
        <source>Install compatible extensions from the Raycast store</source>
        <translation>Kompatible Erweiterungen aus dem Raycast Store installieren</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Always show intro</source>
        <translation>Immer Intro anzeigen</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>
# Welcome to the Raycast Extension Store

Vicinae provides direct integration with the official [Raycast store](https://www.raycast.com/store), allowing you to search and install Raycast extensions directly from Vicinae.
</source>
        <translation>
# Willkommen im Raycast Extension Store

Vicinae bietet eine direkte Integration mit dem offiziellen [Raycast-Store](https://www.raycast.com/store), sodass Sie Raycast-Erweiterungen direkt von Vicinae aus suchen und installieren können.
</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>
Each extension has a colored compatibility indicator showing how well it works on Linux.

Vicinae also has its own [extension store](vicinae://launch/core/store), which does not suffer from these limitations.
</source>
        <translation>
Jede Erweiterung verfügt über einen farbigen Kompatibilitätsindikator, der anzeigt, wie gut sie unter Linux funktioniert.

Vicinae verfügt außerdem über einen eigenen [Erweiterungsspeicher](vicinae://launch/core/store), der nicht unter diesen Einschränkungen leidet.
</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>
Vicinae also has its own [extension store](vicinae://launch/core/store).
</source>
        <translation>
Vicinae verfügt außerdem über einen eigenen [Erweiterungsspeicher](vicinae://launch/core/store).
</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Continue to store</source>
        <translation>Weiter zum Store</translation>
    </message>
</context>
<context>
    <name>RaycastStoreDetailHost</name>
    <message>
        <location filename="../src/qml/raycast-store-detail-host.cpp" line="+43"/>
        <source>Failed to load extension</source>
        <translation>Die Erweiterung konnte nicht geladen werden</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The extension &quot;%1&quot; could not be loaded. It may not exist or the store may be unreachable.</source>
        <translation>Die Erweiterung „%1“ konnte nicht geladen werden. Es existiert möglicherweise nicht oder der Store ist möglicherweise nicht erreichbar.</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Extension Store - %1</source>
        <translation>Extension Store – %1</translation>
    </message>
    <message>
        <location line="+31"/>
        <source>This extension should be fully compatible.</source>
        <translation>Diese Erweiterung sollte vollständig kompatibel sein.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>This extension works but has a few quirks.</source>
        <translation>Diese Erweiterung funktioniert, weist jedoch einige Macken auf.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>This extension is not compatible.</source>
        <translation>Diese Erweiterung ist nicht kompatibel.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No compatibility data is available for this extension.</source>
        <translation>Für diese Erweiterung sind keine Kompatibilitätsdaten verfügbar.</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>No compatibility data is available — this extension may or may not work.</source>
        <translation>Es sind keine Kompatibilitätsdaten verfügbar. Diese Erweiterung funktioniert möglicherweise nicht.</translation>
    </message>
    <message>
        <location line="+83"/>
        <source>Install extension</source>
        <translation>Erweiterung installieren</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading extension...</source>
        <translation>Erweiterung wird heruntergeladen...</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to download extension</source>
        <translation>Die Erweiterung konnte nicht heruntergeladen werden</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to extract extension archive</source>
        <translation>Das Extrahieren des Erweiterungsarchivs ist fehlgeschlagen</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Extension installed</source>
        <translation>Erweiterung installiert</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Report issue</source>
        <translation>Problem melden</translation>
    </message>
</context>
<context>
    <name>RaycastStoreSection</name>
    <message>
        <location filename="../src/qml/raycast-store-model.cpp" line="+45"/>
        <source>Show details</source>
        <translation>Details anzeigen</translation>
    </message>
</context>
<context>
    <name>RaycastStoreViewHost</name>
    <message>
        <location filename="../src/qml/raycast-store-view-host.cpp" line="+37"/>
        <source>Browse Raycast extensions</source>
        <translation>Raycast-Erweiterungen durchsuchen</translation>
    </message>
    <message>
        <location line="+32"/>
        <source>Failed to fetch extensions</source>
        <translation>Erweiterungen konnten nicht abgerufen werden</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Extensions</source>
        <translation>Erweiterungen</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to search extensions</source>
        <translation>Die Suche nach Erweiterungen ist fehlgeschlagen</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Results</source>
        <translation>Ergebnisse</translation>
    </message>
</context>
<context>
    <name>RebootCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-72"/>
        <source>Reboot System</source>
        <translation>System neu starten</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reboot the system</source>
        <translation>System neu starten</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>System can&apos;t reboot</source>
        <translation>Das System kann nicht neu gestartet werden</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to reboot</source>
        <translation>Neustart fehlgeschlagen</translation>
    </message>
</context>
<context>
    <name>RebuildFileIndexCommand</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="-72"/>
        <source>Rebuild File Index</source>
        <translation>Dateiindex neu erstellen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Fully rebuild the file index. Running this manually can be useful if the file search feels particularly out of date.</source>
        <translation>Erstellen Sie den Dateiindex vollständig neu. Dies manuell auszuführen kann hilfreich sein, wenn die Dateisuche besonders veraltet erscheint.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Are you sure?</source>
        <translation>Wirklich fortfahren?</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Rebuilding the entire index can be time consuming and CPU intensive, depending on the number of files present in your home directory.</source>
        <translation>Die Neuerstellung des gesamten Index kann je nach Anzahl der in Ihrem Home-Verzeichnis vorhandenen Dateien zeitaufwändig und rechenintensiv sein.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Reset</source>
        <translation>Zurücksetzen</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Index rebuild started...</source>
        <translation>Der Indexneuaufbau wurde gestartet...</translation>
    </message>
</context>
<context>
    <name>RefreshAppsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/refresh-apps-command.hpp" line="+12"/>
        <source>Refresh Apps</source>
        <translation>Apps aktualisieren</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Force a refresh of the application database. The database should normally automatically update itself on changes, but this can help working around some edge cases.</source>
        <translation>Erzwingen Sie eine Aktualisierung der Anwendungsdatenbank. Die Datenbank sollte sich normalerweise bei Änderungen automatisch aktualisieren, dies kann jedoch bei der Umgehung einiger Randfälle hilfreich sein.</translation>
    </message>
    <message>
        <location filename="../src/extensions/vicinae/refresh-apps-command.cpp" line="+15"/>
        <source>Apps successfully refreshed</source>
        <translation>Apps erfolgreich aktualisiert</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to refresh apps</source>
        <translation>Apps konnten nicht aktualisiert werden</translation>
    </message>
</context>
<context>
    <name>ReloadScriptDirectoriesCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+29"/>
        <source>Reload Script Directories</source>
        <translation>Skriptverzeichnisse neu laden</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reload script directories</source>
        <translation>Skriptverzeichnisse neu laden</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>New scan triggered, index will update shortly</source>
        <translation>Neuer Scan ausgelöst, Index wird in Kürze aktualisiert</translation>
    </message>
</context>
<context>
    <name>RemoveAllCalculatorHistoryRecordsAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+97"/>
        <source>Delete all entries</source>
        <translation>Alle Einträge löschen</translation>
    </message>
</context>
<context>
    <name>RemoveAllSelectionsAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+27"/>
        <source>Are you sure?</source>
        <translation>Wirklich fortfahren?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>All your clipboard history will be lost forever</source>
        <translation>Der gesamte Verlauf Ihrer Zwischenablage geht für immer verloren</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Delete all</source>
        <translation>Alles löschen</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>All selections were removed</source>
        <translation>Alle Auswahlen wurden entfernt</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove all selections</source>
        <translation>Es konnten nicht alle Auswahlen entfernt werden</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Remove all</source>
        <translation>Alles entfernen</translation>
    </message>
</context>
<context>
    <name>RemoveCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-43"/>
        <source>Entry removed</source>
        <translation>Eintrag entfernt</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove entry</source>
        <translation>Eintrag konnte nicht entfernt werden</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Delete entry</source>
        <translation>Eintrag löschen</translation>
    </message>
</context>
<context>
    <name>RemoveSelectionAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="-70"/>
        <source>Entry removed</source>
        <translation>Eintrag entfernt</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove entry</source>
        <translation>Eintrag konnte nicht entfernt werden</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Remove entry</source>
        <translation>Eintrag entfernen</translation>
    </message>
</context>
<context>
    <name>RemoveShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="+33"/>
        <source>Removed link</source>
        <translation>Link entfernt</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove link</source>
        <translation>Link konnte nicht entfernt werden</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Remove link</source>
        <translation>Link entfernen</translation>
    </message>
</context>
<context>
    <name>ReportVicinaeBugCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/report-bug-command.hpp" line="+10"/>
        <source>Report a Vicinae Bug</source>
        <translation>Vicinae-Fehler melden</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Navigate to Vicinae issue creation page with all relevant informations pre-filled.</source>
        <translation>Vicinae-Seite zum Erstellen eines Issues mit vorausgefüllten relevanten Informationen öffnen.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Title</source>
        <translation>Titel</translation>
    </message>
</context>
<context>
    <name>ResetEmojiRankingAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+21"/>
        <source>Reset ranking</source>
        <translation>Ranking zurücksetzen</translation>
    </message>
</context>
<context>
    <name>ResetEmojiSkinToneAction</name>
    <message>
        <location line="+32"/>
        <source>Reset to preference</source>
        <translation>Zurücksetzen auf Präferenz</translation>
    </message>
</context>
<context>
    <name>ResetItemRanking</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="-25"/>
        <source>Ranking was successfully reset</source>
        <translation>Das Ranking wurde erfolgreich zurückgesetzt</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Unable to reset ranking</source>
        <translation>Das Ranking konnte nicht zurückgesetzt werden</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Are you sure?</source>
        <translation>Wirklich fortfahren?</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You will have to rebuild search history for this item in order for it to reappear on top of the root search results.</source>
        <translation>Sie müssen den Suchverlauf für dieses Element neu erstellen, damit es wieder oben in den Stammsuchergebnissen angezeigt wird.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Reset</source>
        <translation>Zurücksetzen</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Reset ranking</source>
        <translation>Ranking zurücksetzen</translation>
    </message>
</context>
<context>
    <name>RevealFileInFolderAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+26"/>
        <source>Show in file browser</source>
        <translation>Im Dateibrowser anzeigen</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to open folder</source>
        <translation>Ordner konnte nicht geöffnet werden</translation>
    </message>
</context>
<context>
    <name>RootCalculatorSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+79"/>
        <source>Calculator</source>
        <translation>Rechner</translation>
    </message>
</context>
<context>
    <name>RootFallbackSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="+407"/>
        <source>Use &quot;%1&quot; with...</source>
        <translation>„%1“ verwenden mit...</translation>
    </message>
</context>
<context>
    <name>RootFavoritesSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+67"/>
        <source>Favorites</source>
        <translation>Favoriten</translation>
    </message>
</context>
<context>
    <name>RootFilesSection</name>
    <message>
        <location line="+50"/>
        <source>Files</source>
        <translation>Dateien</translation>
    </message>
</context>
<context>
    <name>RootLinkSection</name>
    <message>
        <location line="-137"/>
        <source>Link</source>
        <translation>Link</translation>
    </message>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="-295"/>
        <source>Open in %1</source>
        <translation>In %1 öffnen</translation>
    </message>
</context>
<context>
    <name>RootNewsSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+64"/>
        <source>What&apos;s New</source>
        <translation>Was ist neu</translation>
    </message>
</context>
<context>
    <name>RootResultsSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="+199"/>
        <source>Suggestions</source>
        <translation>Vorschläge</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Results (%1)</source>
        <translation>Ergebnisse (%1)</translation>
    </message>
</context>
<context>
    <name>RootSearchActionGenerator</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+55"/>
        <source>Copy ID</source>
        <translation>ID kopieren</translation>
    </message>
</context>
<context>
    <name>RootShortcutItem</name>
    <message>
        <location filename="../src/root-search/shortcuts/shortcut-root-provider.cpp" line="+67"/>
        <location line="+11"/>
        <source>Shortcut</source>
        <translation>Verknüpfung</translation>
    </message>
</context>
<context>
    <name>RootUpdateSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="-21"/>
        <location filename="../src/qml/root-search-sources.cpp" line="-125"/>
        <source>Update</source>
        <translation>Aktualisieren</translation>
    </message>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="-14"/>
        <source>Vicinae %1 is available</source>
        <translation>Vicinae %1 ist verfügbar</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>You are running %1</source>
        <translation>Sie führen %1 aus</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>View Release Notes</source>
        <translation>Versionshinweise anzeigen</translation>
    </message>
</context>
<context>
    <name>RootViewHost</name>
    <message>
        <location filename="../src/qml/root-view-host.hpp" line="+15"/>
        <source>Search for anything...</source>
        <translation>Nach irgendetwas suchen...</translation>
    </message>
</context>
<context>
    <name>ScriptExecutorViewHost</name>
    <message>
        <location filename="../src/qml/script-executor-view-host.cpp" line="+76"/>
        <source>Script execution failed: %1</source>
        <translation>Skriptausführung fehlgeschlagen: %1</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Running... (%1s ago)</source>
        <translation>Läuft... (vor %1s)</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Done in %1s (exit=%2)</source>
        <translation>Fertig in %1s (exit=%2)</translation>
    </message>
    <message>
        <location line="+12"/>
        <location line="+9"/>
        <source>Script process killed</source>
        <translation>Skriptprozess abgebrochen</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Running...</source>
        <translation>Wird ausgeführt...</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Kill process</source>
        <translation>Prozess beenden</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Run script again</source>
        <translation>Skript erneut ausführen</translation>
    </message>
</context>
<context>
    <name>ScriptRootItem</name>
    <message>
        <location filename="../src/root-search/scripts/script-root-provider.hpp" line="+27"/>
        <location line="+86"/>
        <source>Script</source>
        <translation>Skript</translation>
    </message>
    <message>
        <location line="-43"/>
        <source>Mode</source>
        <translation>Modus</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Path</source>
        <translation>Pfad</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Author</source>
        <translation>Autor</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Open script directory</source>
        <translation>Skriptverzeichnis öffnen</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy path to script</source>
        <translation>Skriptpfad kopieren</translation>
    </message>
</context>
<context>
    <name>ScriptRootProvider</name>
    <message>
        <location line="+47"/>
        <source>Script Commands</source>
        <translation>Skriptbefehle</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Custom directories</source>
        <translation>Benutzerdefinierte Verzeichnisse</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Additional list of directories to source scripts from. These directories always take precedence over the default system ones</source>
        <translation>Zusätzliche Liste von Verzeichnissen, aus denen Skripte stammen. Diese Verzeichnisse haben immer Vorrang vor den Standardverzeichnissen des Systems</translation>
    </message>
</context>
<context>
    <name>SearchBrowserTabsCommand</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="+15"/>
        <source>Search Browser Tabs</source>
        <translation>Browser-Registerkarten durchsuchen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search tabs from all connected browsers</source>
        <translation>Tabs aller verbundenen Browser durchsuchen</translation>
    </message>
</context>
<context>
    <name>SearchEmojiCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/search-emoji-command.hpp" line="+15"/>
        <source>Search Emojis &amp; Symbols</source>
        <translation>Emojis und Symbole durchsuchen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search for any emoji or symbol</source>
        <translation>Nach Emojis und Symbolen suchen</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Paste</source>
        <translation>Einfügen</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy</source>
        <translation>Kopieren</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Default Action</source>
        <translation>Standardaktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to perform on pressing return. Paste is only available if your environment supports it.</source>
        <translation>Die Standardaktion, die beim Drücken der Eingabetaste ausgeführt wird. Einfügen ist nur verfügbar, wenn Ihre Umgebung dies unterstützt.</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Skin tone</source>
        <translation>Hautton</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Skin tone to use for relevant emojis.</source>
        <translation>Hautton, der für relevante Emojis verwendet werden soll.</translation>
    </message>
</context>
<context>
    <name>SearchEmojiGridSource</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.hpp" line="-22"/>
        <source>Results (%1)</source>
        <translation>Ergebnisse (%1)</translation>
    </message>
</context>
<context>
    <name>SearchFilesCommand</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="-38"/>
        <source>Search Files</source>
        <translation>Dateien durchsuchen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search files on your system</source>
        <translation>Dateien auf dem System durchsuchen</translation>
    </message>
</context>
<context>
    <name>SearchFilesView</name>
    <message>
        <location filename="../src/qml/qml/SearchFilesView.qml" line="+37"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Path</source>
        <translation>Pfad</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>Typ</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Last modified</source>
        <translation>Zuletzt geändert</translation>
    </message>
</context>
<context>
    <name>SearchFilesViewHost</name>
    <message>
        <location filename="../src/qml/search-files-view-host.cpp" line="+59"/>
        <source>Search for files...</source>
        <translation>Nach Dateien suchen...</translation>
    </message>
    <message>
        <location line="+30"/>
        <location line="+4"/>
        <source>Direct file path</source>
        <translation>Direkter Dateipfad</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Recently Accessed</source>
        <translation>Kürzlich aufgerufen</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Results</source>
        <translation>Ergebnisse</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>All</source>
        <translation>Alle</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Other</source>
        <translation>Andere</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Directories</source>
        <translation>Verzeichnisse</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Images</source>
        <translation>Bilder</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Videos</source>
        <translation>Videos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Audio</source>
        <translation>Audio</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Documents</source>
        <translation>Dokumente</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Archives</source>
        <translation>Archive</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Applications</source>
        <translation>Anwendungen</translation>
    </message>
</context>
<context>
    <name>SetAppFont</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="-15"/>
        <source>Set as vicinae font</source>
        <translation>Als Vicinae-Schriftart festlegen</translation>
    </message>
</context>
<context>
    <name>SetRootItemAliasAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="-18"/>
        <source>Set alias</source>
        <translation>Alias festlegen</translation>
    </message>
</context>
<context>
    <name>SetThemeAction</name>
    <message>
        <location filename="../src/actions/theme/theme-actions.cpp" line="+11"/>
        <source>Theme successfully updated</source>
        <translation>Design erfolgreich aktualisiert</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Set theme</source>
        <translation>Design festlegen</translation>
    </message>
</context>
<context>
    <name>SetThemeCommand</name>
    <message>
        <location filename="../src/extensions/theme/set-theme-command.hpp" line="+9"/>
        <source>Set Theme</source>
        <translation>Design festlegen</translation>
    </message>
</context>
<context>
    <name>SetVolumeCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="+213"/>
        <source>Set Volume to %1%</source>
        <translation>Lautstärke auf %1 % setzen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Set system volume to %1%</source>
        <translation>Systemlautstärke auf %1% einstellen</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to set volume</source>
        <translation>Lautstärke konnte nicht eingestellt werden</translation>
    </message>
</context>
<context>
    <name>SetWallpaperAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+17"/>
        <source>Set as wallpaper</source>
        <translation>Als Hintergrundbild festlegen</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Wallpaper set</source>
        <translation>Hintergrundbild festgelegt</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to set wallpaper</source>
        <translation>Hintergrundbild konnte nicht festgelegt werden</translation>
    </message>
</context>
<context>
    <name>SettingsSidebar</name>
    <message>
        <location filename="../src/qml/qml/SettingsSidebar.qml" line="+99"/>
        <source>Search...</source>
        <translation>Suchen...</translation>
    </message>
</context>
<context>
    <name>SettingsSidebarModel</name>
    <message>
        <location filename="../src/qml/settings-sidebar-model.cpp" line="+90"/>
        <source>General</source>
        <translation>Allgemein</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Appearance</source>
        <translation>Aussehen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Keybindings</source>
        <translation>Tastenkombinationen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced</source>
        <translation>Erweitert</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>About</source>
        <translation>Über</translation>
    </message>
</context>
<context>
    <name>SettingsWindow</name>
    <message>
        <location filename="../src/qml/qml/SettingsWindow.qml" line="+10"/>
        <source>General</source>
        <translation>Allgemein</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Appearance</source>
        <translation>Aussehen</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keybindings</source>
        <translation>Tastenkombinationen</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Advanced</source>
        <translation>Erweitert</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>About</source>
        <translation>Über</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Vicinae Settings</source>
        <translation>Vicinae-Einstellungen</translation>
    </message>
    <message>
        <location line="+88"/>
        <source>Imported from Raycast</source>
        <translation>Importiert von Raycast</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>From the Vicinae store</source>
        <translation>Aus dem Vicinae Store</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Locally installed extension</source>
        <translation>Lokal installierte Erweiterung</translation>
    </message>
</context>
<context>
    <name>ShortcutExtension</name>
    <message>
        <location filename="../src/extensions/shortcut/shortcut-extension.hpp" line="+11"/>
        <source>Manage Shortcuts</source>
        <translation>Verknüpfungen verwalten</translation>
    </message>
</context>
<context>
    <name>ShortcutField</name>
    <message>
        <location filename="../src/qml/qml/ShortcutField.qml" line="+14"/>
        <source>Record shortcut</source>
        <translation>Tastenkürzel aufzeichnen</translation>
    </message>
</context>
<context>
    <name>ShortcutFormView</name>
    <message>
        <location filename="../src/qml/qml/ShortcutFormView.qml" line="+14"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Shortcut Name</source>
        <translation>Verknüpfungsname</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>The URL that will be opened by the specified app. You can make it dynamic by using placeholders such as {argument}.</source>
        <translation>Die URL, die von der angegebenen App geöffnet wird. Mit Platzhaltern wie {argument} kann sie dynamisch gestaltet werden.</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Open with</source>
        <translation>Öffnen mit</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Icon</source>
        <translation>Symbol</translation>
    </message>
</context>
<context>
    <name>ShortcutFormViewHost</name>
    <message>
        <location filename="../src/qml/shortcut-form-view-host.cpp" line="+47"/>
        <source>Submit</source>
        <translation>Senden</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Copy of %1</source>
        <translation>Kopie von %1</translation>
    </message>
    <message>
        <location line="+67"/>
        <source>Edit &quot;%1&quot;</source>
        <translation>„%1“ bearbeiten</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Duplicate &quot;%1&quot;</source>
        <translation>„%1“ duplizieren</translation>
    </message>
    <message>
        <location line="+11"/>
        <location line="+131"/>
        <location line="+50"/>
        <source>Default</source>
        <translation>Standard</translation>
    </message>
    <message>
        <location line="-157"/>
        <source>Selected Text</source>
        <translation>Ausgewählter Text</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clipboard Text</source>
        <translation>Zwischenablagetext</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Argument</source>
        <translation>Argument</translation>
    </message>
    <message>
        <location line="+35"/>
        <location line="+5"/>
        <location line="+5"/>
        <source>Required</source>
        <translation>Erforderlich</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Validation failed</source>
        <translation>Die Validierung ist fehlgeschlagen</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Failed to update shortcut</source>
        <translation>Verknüpfung konnte nicht aktualisiert werden</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Shortcut updated</source>
        <translation>Verknüpfung aktualisiert</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to create shortcut</source>
        <translation>Verknüpfung konnte nicht erstellt werden</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Shortcut created</source>
        <translation>Verknüpfung erstellt</translation>
    </message>
</context>
<context>
    <name>ShortcutRecorderField</name>
    <message>
        <location filename="../src/qml/qml/ShortcutRecorderField.qml" line="+24"/>
        <location line="+22"/>
        <location line="+62"/>
        <source>Recording...</source>
        <translation>Aufzeichnung...</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Keybind updated</source>
        <translation>Tastenkombination aktualisiert</translation>
    </message>
</context>
<context>
    <name>ShortcutRootProvider</name>
    <message>
        <location filename="../src/root-search/shortcuts/shortcut-root-provider.cpp" line="+42"/>
        <source>Shortcuts</source>
        <translation>Verknüpfungen</translation>
    </message>
</context>
<context>
    <name>ShortcutsSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/ShortcutsSettingsPage.qml" line="+55"/>
        <source>Keybindings</source>
        <translation>Tastenkombinationen</translation>
    </message>
    <message>
        <location line="+78"/>
        <source>Record Shortcut</source>
        <translation>Tastenkürzel aufzeichnen</translation>
    </message>
</context>
<context>
    <name>SkipUpdateVersionAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+8"/>
        <source>Skip This Version</source>
        <translation>Diese Version überspringen</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Skipped %1</source>
        <translation>%1 übersprungen</translation>
    </message>
</context>
<context>
    <name>SleepCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+94"/>
        <source>Put System to Sleep</source>
        <translation>System in den Ruhezustand versetzen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Put system to sleep</source>
        <translation>System in den Ruhezustand versetzen</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>System can&apos;t sleep</source>
        <translation>System kann nicht in den Ruhezustand wechseln</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to sleep</source>
        <translation>Energiesparmodus fehlgeschlagen</translation>
    </message>
</context>
<context>
    <name>SnippetDatabase</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.cpp" line="+42"/>
        <location line="+56"/>
        <source>keyword already assigned to &quot;%1&quot;</source>
        <translation>Schlüsselwort bereits „%1“ zugewiesen</translation>
    </message>
    <message>
        <location line="-43"/>
        <source>No snippet with that ID</source>
        <translation>Kein Snippet mit dieser ID</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>No such snippet</source>
        <translation>Snippet nicht vorhanden</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Snippet limit reached (%1)</source>
        <translation>Snippet-Limit erreicht (%1)</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Failed to save snippets on disk: %1</source>
        <translation>Snippets konnten nicht auf der Festplatte gespeichert werden: %1</translation>
    </message>
</context>
<context>
    <name>SnippetExtension</name>
    <message>
        <location filename="../src/extensions/snippet/snippet-extension.cpp" line="+38"/>
        <source>Expansion</source>
        <translation>Textersetzung</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Enable automatic snippet expansion when triggers are typed</source>
        <translation>Automatische Snippet-Erweiterung bei Eingabe von Auslösern aktivieren</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Undo</source>
        <translation>Rückgängig machen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Press backspace immediately after expansion to undo and restore the trigger text</source>
        <translation>Direkt nach der Erweiterung die Rücktaste drücken, um sie rückgängig zu machen und den Auslösertext wiederherzustellen</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Keyboard layout</source>
        <translation>Tastaturlayout</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>XKB layout used for trigger detection (e.g. &quot;us&quot;, &quot;fr&quot;). Leave empty for system default.</source>
        <translation>XKB-Layout, das zur Triggererkennung verwendet wird (z. B. „us“, „fr“). Für den Systemstandard leer lassen.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Pre-paste delay (ms)</source>
        <translation>Verzögerung vor dem Einfügen (ms)</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Delay between setting clipboard and injecting paste shortcut. Increase if expansions paste empty on slow compositors.</source>
        <translation>Verzögerung zwischen dem Setzen der Zwischenablage und dem simulierten Einfügen. Erhöhen, falls Snippets bei langsamen Compositors leer eingefügt werden.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Key injection delay (ms)</source>
        <translation>Verzögerung zwischen Tasteneingaben (ms)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Delay between injected key events. Increase if expansions produce missing or garbled characters on slow compositors.</source>
        <translation>Verzögerung zwischen simulierten Tastenereignissen. Erhöhen, falls Snippets bei langsamen Compositors fehlende oder fehlerhafte Zeichen erzeugen.</translation>
    </message>
    <message>
        <location filename="../src/extensions/snippet/snippet-extension.hpp" line="+11"/>
        <source>Snippets</source>
        <translation>Snippets</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Text expansion and snippet management</source>
        <translation>Textersetzung und Snippet-Verwaltung</translation>
    </message>
</context>
<context>
    <name>SnippetFormView</name>
    <message>
        <location filename="../src/qml/qml/SnippetFormView.qml" line="+15"/>
        <source>Title</source>
        <translation>Titel</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Euro symbol</source>
        <translation>Euro-Symbol</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Content</source>
        <translation>Inhalt</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You can use {dynamic placeholders} to make the content dynamic: &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;learn more&lt;/a&gt;.</source>
        <translation>Mit {dynamic placeholders} können Sie den Inhalt dynamisch gestalten: &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;mehr erfahren&lt;/a&gt;.</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Keyword</source>
        <translation>Stichwort</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Typing this keyword anywhere will result in it being replaced by the content of the snippet.</source>
        <translation>Wenn Sie dieses Schlüsselwort an einer beliebigen Stelle eingeben, wird es durch den Inhalt des Snippets ersetzt.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The snippet server is not running. Keyword expansion is unavailable. &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;Learn more&lt;/a&gt;.</source>
        <translation>Der Snippet-Server läuft nicht. Die Keyword-Erweiterung ist nicht verfügbar. &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;Weitere Informationen&lt;/a&gt;.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Applications</source>
        <translation>Anwendungen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Restrict expansion to specific applications. By default, it works everywhere.</source>
        <translation>Erweiterung auf bestimmte Anwendungen beschränken. Standardmäßig funktioniert sie überall.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Expand as word</source>
        <translation>Als Wort erweitern</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>If a keyword is typed, it will only be expanded after space or punctuation.</source>
        <translation>Wenn ein Schlüsselwort eingegeben wird, wird es erst nach Leerzeichen oder Satzzeichen erweitert.</translation>
    </message>
</context>
<context>
    <name>SnippetFormViewHost</name>
    <message>
        <location filename="../src/qml/snippet-form-view-host.cpp" line="+49"/>
        <source>Submit</source>
        <translation>Senden</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Copy of %1</source>
        <translation>Kopie von %1</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Edit &quot;%1&quot;</source>
        <translation>„%1“ bearbeiten</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Duplicate &quot;%1&quot;</source>
        <translation>„%1“ duplizieren</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>2 chars min.</source>
        <translation>Mindestens 2 Zeichen.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Content should not be empty</source>
        <translation>Der Inhalt sollte nicht leer sein</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Only one {cursor} placeholder is allowed</source>
        <translation>Es ist nur ein {cursor}-Platzhalter zulässig</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Validation failed</source>
        <translation>Die Validierung ist fehlgeschlagen</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Snippet updated</source>
        <translation>Snippet aktualisiert</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Snippet successfully created</source>
        <translation>Snippet erfolgreich erstellt</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Cursor Position</source>
        <translation>Cursorposition</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clipboard Text</source>
        <translation>Zwischenablagetext</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Date</source>
        <translation>Datum</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Argument</source>
        <translation>Argument</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Shell Command</source>
        <translation>Shell-Befehl</translation>
    </message>
</context>
<context>
    <name>SoftRebootCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-99"/>
        <source>Soft Reboot System</source>
        <translation>System sanft neu starten</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Soft reboot the system, which usually means only userspace is rebooted.</source>
        <translation>System sanft neu starten; dabei wird üblicherweise nur der Userspace neu gestartet.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System can&apos;t soft reboot</source>
        <translation>Das System kann keinen Soft-Neustart durchführen</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to soft reboot</source>
        <translation>Soft-Neustart fehlgeschlagen</translation>
    </message>
</context>
<context>
    <name>SponsorVicinaeCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-121"/>
        <source>Donate to Vicinae</source>
        <translation>An Vicinae spenden</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open link to Vicinae&apos;s GitHub sponsor page</source>
        <translation>Öffnen Sie den Link zur GitHub-Sponsorenseite von Vicinae</translation>
    </message>
</context>
<context>
    <name>StoreDetailView</name>
    <message>
        <location filename="../src/qml/qml/StoreDetailView.qml" line="+196"/>
        <source>Installed</source>
        <translation>Installiert</translation>
    </message>
    <message>
        <location line="+166"/>
        <source>Description</source>
        <translation>Beschreibung</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Commands</source>
        <translation>Befehle</translation>
    </message>
    <message>
        <location line="+74"/>
        <source>Open README</source>
        <translation>README öffnen</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Last update</source>
        <translation>Letztes Update</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Contributors</source>
        <translation>Mitwirkende</translation>
    </message>
    <message>
        <location line="+29"/>
        <source>Categories</source>
        <translation>Kategorien</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Source Code</source>
        <translation>Quellcode</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>View Code</source>
        <translation>Code anzeigen</translation>
    </message>
</context>
<context>
    <name>SuspendCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+36"/>
        <source>Suspend System</source>
        <translation>System in den Energiesparmodus versetzen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Suspend the system to RAM. Unlike hibernation, this does not turn the computer off and will break on power loss.</source>
        <translation>Systemzustand im Arbeitsspeicher halten. Anders als beim Ruhezustand bleibt der Computer eingeschaltet; bei einem Stromausfall geht der Zustand verloren.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System cannot suspend</source>
        <translation>Das System kann nicht in den Energiesparmodus versetzt werden</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to suspend</source>
        <translation>Energiesparmodus fehlgeschlagen</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsCommand</name>
    <message>
        <location filename="../src/extensions/wm/switch-windows-command.hpp" line="+10"/>
        <source>Switch Windows</source>
        <translation>Zwischen Fenstern wechseln</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsSection</name>
    <message>
        <location filename="../src/qml/switch-windows-model.hpp" line="+28"/>
        <source>Open Windows</source>
        <translation>Fenster öffnen</translation>
    </message>
    <message>
        <location filename="../src/qml/switch-windows-model.cpp" line="+19"/>
        <source>WS %1</source>
        <translation>WS %1</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Window Actions</source>
        <translation>Fensteraktionen</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsViewHost</name>
    <message>
        <location filename="../src/qml/switch-windows-view-host.cpp" line="+12"/>
        <source>Search open window...</source>
        <translation>Offene Fenster suchen...</translation>
    </message>
</context>
<context>
    <name>SystemBrowseApps</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-123"/>
        <source>Browse Apps</source>
        <translation>Apps durchsuchen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse all applications that are installed on the system</source>
        <translation>Alle auf dem System installierten Anwendungen durchsuchen</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Show hidden apps</source>
        <translation>Versteckte Apps anzeigen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sort alphabetically</source>
        <translation>Alphabetisch sortieren</translation>
    </message>
</context>
<context>
    <name>SystemExtension</name>
    <message>
        <location line="+149"/>
        <source>System</source>
        <translation>System</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>System-related commands</source>
        <translation>Systembezogene Befehle</translation>
    </message>
</context>
<context>
    <name>SystemRunCommand</name>
    <message>
        <location line="-239"/>
        <source>Run Terminal Program</source>
        <translation>Terminalprogramm ausführen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run a program in a terminal window</source>
        <translation>Programm in einem Terminalfenster ausführen</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>command</source>
        <translation>Befehl</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Run in terminal</source>
        <translation>Im Terminal ausführen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run in terminal (hold)</source>
        <translation>Im Terminal ausführen (gedrückt halten)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run directly</source>
        <translation>Direkt ausführen</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Default Action</source>
        <translation>Standardaktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to run on pressing return</source>
        <translation>Die Standardaktion, die beim Drücken der Eingabetaste ausgeführt wird</translation>
    </message>
    <message>
        <location line="+29"/>
        <source>Not a valid executable</source>
        <translation>Keine gültige ausführbare Datei</translation>
    </message>
</context>
<context>
    <name>SystemRunViewHost</name>
    <message>
        <location filename="../src/qml/system-run-view-host.cpp" line="+20"/>
        <source>Search for a program to execute...</source>
        <translation>Nach einem auszuführenden Programm suchen...</translation>
    </message>
</context>
<context>
    <name>ThemeExtension</name>
    <message>
        <location filename="../src/extensions/theme/theme-extension.hpp" line="+9"/>
        <source>Theme</source>
        <translation>Design</translation>
    </message>
</context>
<context>
    <name>ThemeSection</name>
    <message>
        <location filename="../src/qml/theme-list-model.cpp" line="+22"/>
        <source>Default theme description</source>
        <translation>Beschreibung des Standarddesigns</translation>
    </message>
    <message>
        <location line="+64"/>
        <source>Open theme file</source>
        <translation>Designdatei öffnen</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy ID</source>
        <translation>ID kopieren</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy path</source>
        <translation>Pfad kopieren</translation>
    </message>
</context>
<context>
    <name>ThemeViewHost</name>
    <message>
        <location filename="../src/qml/theme-view-host.cpp" line="+22"/>
        <source>Search for a theme...</source>
        <translation>Nach einem Design suchen...</translation>
    </message>
    <message>
        <location line="+51"/>
        <source>Current Theme</source>
        <translation>Aktuelles Design</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Available Themes</source>
        <translation>Verfügbare Designs</translation>
    </message>
</context>
<context>
    <name>ToggleItemAsFavorite</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="+17"/>
        <source>Remove from favorites</source>
        <translation>Aus Favoriten entfernen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Add to favorites</source>
        <translation>Zu Favoriten hinzufügen</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Successfuly added to favorites</source>
        <translation>Erfolgreich zu den Favoriten hinzugefügt</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Successfuly removed from favorites</source>
        <translation>Erfolgreich aus den Favoriten entfernt</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to add to favorites</source>
        <translation>Das Hinzufügen zu den Favoriten ist fehlgeschlagen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove from favorites</source>
        <translation>Das Entfernen aus den Favoriten ist fehlgeschlagen</translation>
    </message>
</context>
<context>
    <name>ToggleMuteCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="+163"/>
        <source>Toggle Mute</source>
        <translation>Stummschaltung umschalten</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mute or unmute system audio</source>
        <translation>Systemaudio stummschalten oder Stummschaltung aufheben</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Failed to toggle mute</source>
        <translation>Die Stummschaltung konnte nicht umgeschaltet werden</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Muted</source>
        <translation>Stummgeschaltet</translation>
    </message>
</context>
<context>
    <name>UIPlayground</name>
    <message>
        <location filename="../src/extensions/internal/internal-extension.hpp" line="-15"/>
        <source>UI Showcase</source>
        <translation>UI-Showcase</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Showcase of vicinae UI elements</source>
        <translation>Präsentation der UI-Elemente von Vicinae</translation>
    </message>
</context>
<context>
    <name>UIShowcase</name>
    <message>
        <location filename="../src/qml/qml/UIShowcase.qml" line="+26"/>
        <source>Buttons</source>
        <translation>Schaltflächen</translation>
    </message>
</context>
<context>
    <name>UninstallExtensionAction</name>
    <message>
        <location filename="../src/actions/extension/extension-actions.cpp" line="+11"/>
        <source>Are you sure?</source>
        <translation>Wirklich fortfahren?</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>All this extension data will be permanently lost. If you just want the extension to not appear in the root search anymore, consider disabling it instead.</source>
        <translation>Alle diese Erweiterungsdaten gehen dauerhaft verloren. Wenn Sie lediglich möchten, dass die Erweiterung nicht mehr in der Stammsuche angezeigt wird, sollten Sie sie stattdessen deaktivieren.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Uninstall</source>
        <translation>Deinstallieren</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Extension uninstalled</source>
        <translation>Erweiterung deinstalliert</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to uninstall extension</source>
        <translation>Die Deinstallation der Erweiterung ist fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../src/actions/extension/extension-actions.hpp" line="+14"/>
        <source>Uninstall Extension</source>
        <translation>Erweiterung deinstallieren</translation>
    </message>
</context>
<context>
    <name>UnpinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-23"/>
        <source>Entry unpinned</source>
        <translation>Eintrag gelöst</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Unpin entry</source>
        <translation>Eintrag lösen</translation>
    </message>
</context>
<context>
    <name>UnpinEmojiAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-43"/>
        <source>Unpin emoji</source>
        <translation>Emoji lösen</translation>
    </message>
</context>
<context>
    <name>UpdateService</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="-167"/>
        <source>Update installed</source>
        <translation>Update installiert</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Restarting…</source>
        <translation>Neustart…</translation>
    </message>
    <message>
        <location line="+88"/>
        <source>Downloading Vicinae %1…</source>
        <translation>Vicinae %1 wird heruntergeladen…</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading Vicinae %1… %2%</source>
        <translation>Vicinae %1 wird heruntergeladen… %2 %</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Installing update…</source>
        <translation>Update wird installiert…</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Update failed</source>
        <translation>Update fehlgeschlagen</translation>
    </message>
</context>
<context>
    <name>VicinaeExtension</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.hpp" line="+11"/>
        <source>General vicinae-related commands.</source>
        <translation>Allgemeine Vicinae-Befehle.</translation>
    </message>
</context>
<context>
    <name>VicinaeHotkeyGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/vicinae-hotkey-global-shortcut-backend.cpp" line="+48"/>
        <source>Unsupported trigger key</source>
        <translation>Nicht unterstützter Triggerschlüssel</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Hotkey binding was lost</source>
        <translation>Tastenkürzelbelegung wurde aufgehoben</translation>
    </message>
</context>
<context>
    <name>VicinaeListInstalledExtensionsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/list-installed-extensions-command.hpp" line="+11"/>
        <source>Show Installed Extensions</source>
        <translation>Installierte Erweiterungen anzeigen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Show all third-party extensions that have been installed. This includes local extensions as well as extensions downloaded from the stores (vicinae and raycast).</source>
        <translation>Alle installierten Erweiterungen von Drittanbietern anzeigen. Dazu gehören sowohl lokale Erweiterungen als auch aus den Stores heruntergeladene Erweiterungen (Vicinae und Raycast).</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-store-command.hpp" line="+13"/>
        <source>Install extensions from the Vicinae store</source>
        <translation>Erweiterungen aus dem Vicinae Store installieren</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Always show intro</source>
        <translation>Immer Intro anzeigen</translation>
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
# Willkommen im Vicinae Extension Store

Der Vicinae Extension Store bietet von der Community erstellte Erweiterungen, die von unseren wichtigsten Mitwirkenden genehmigt wurden.

Für jede hier aufgeführte Erweiterung ist der Quellcode im Repository [vicinaehq/extensions](https://github.com/vicinaehq/extensions) verfügbar.

Wenn Sie Ihre eigene Erweiterung erstellen möchten, werfen Sie einen Blick auf die [Dokumentation](https://docs.vicinae.com/extensions/introduction). Wenn Sie der Meinung sind, dass Ihre Erweiterung gut zum Shop passen würde, können Sie sie gerne einreichen!
</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Continue to store</source>
        <translation>Weiter zum Store</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreDetailHost</name>
    <message>
        <location filename="../src/qml/vicinae-store-detail-host.cpp" line="+38"/>
        <source>Failed to load extension</source>
        <translation>Die Erweiterung konnte nicht geladen werden</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Could not fetch extension data from the store.</source>
        <translation>Erweiterungsdaten konnten nicht aus dem Store abgerufen werden.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Extension not found</source>
        <translation>Erweiterung nicht gefunden</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The extension &quot;%1&quot; could not be found in the store.</source>
        <translation>Die Erweiterung „%1“ konnte im Store nicht gefunden werden.</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Extension Store - %1</source>
        <translation>Extension Store – %1</translation>
    </message>
    <message>
        <location line="+81"/>
        <source>Install extension</source>
        <translation>Erweiterung installieren</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading extension...</source>
        <translation>Erweiterung wird heruntergeladen...</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to download extension</source>
        <translation>Die Erweiterung konnte nicht heruntergeladen werden</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to extract extension archive</source>
        <translation>Das Extrahieren des Erweiterungsarchivs ist fehlgeschlagen</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Extension installed</source>
        <translation>Erweiterung installiert</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Report issue</source>
        <translation>Problem melden</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreSection</name>
    <message>
        <location filename="../src/qml/vicinae-store-model.cpp" line="+41"/>
        <source>Show details</source>
        <translation>Details anzeigen</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreViewHost</name>
    <message>
        <location filename="../src/qml/vicinae-store-view-host.cpp" line="+27"/>
        <source>Browse Vicinae extensions</source>
        <translation>Vicinae-Erweiterungen durchsuchen</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Failed to fetch extensions</source>
        <translation>Erweiterungen konnten nicht abgerufen werden</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Extensions</source>
        <translation>Erweiterungen</translation>
    </message>
</context>
<context>
    <name>VolumeDownCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-74"/>
        <source>Turn Volume Down</source>
        <translation>Lautstärke verringern</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Decrease system volume</source>
        <translation>Systemlautstärke verringern</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid step value</source>
        <translation>Ungültiger Schrittwert</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to adjust volume</source>
        <translation>Lautstärke konnte nicht angepasst werden</translation>
    </message>
</context>
<context>
    <name>VolumeUpCommand</name>
    <message>
        <location line="-61"/>
        <source>Turn Volume Up</source>
        <translation>Lautstärke erhöhen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Increase system volume</source>
        <translation>Systemlautstärke erhöhen</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid step value</source>
        <translation>Ungültiger Schrittwert</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to adjust volume</source>
        <translation>Lautstärke konnte nicht angepasst werden</translation>
    </message>
</context>
<context>
    <name>WallpaperManager</name>
    <message>
        <location filename="../src/services/wallpaper/wallpaper-manager.cpp" line="+68"/>
        <source>Setting the wallpaper is not supported in the current environment</source>
        <translation>Das Festlegen des Hintergrundbilds wird in der aktuellen Umgebung nicht unterstützt</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No such file: %1</source>
        <translation>Keine solche Datei: %1</translation>
    </message>
</context>
<context>
    <name>WinControlPanelRootItem</name>
    <message>
        <location filename="../src/root-search/control-panel/control-panel-root-provider.cpp" line="+104"/>
        <location line="+10"/>
        <source>Control Panel</source>
        <translation>Systemsteuerung</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Where</source>
        <translation>Wo</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Open Applet</source>
        <translation>Applet öffnen</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Path</source>
        <translation>Pfad kopieren</translation>
    </message>
</context>
<context>
    <name>WinControlPanelRootProvider</name>
    <message>
        <location line="+49"/>
        <source>Control Panel</source>
        <translation>Systemsteuerung</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Control Panel applets and system tasks.</source>
        <translation>Systemsteuerungs-Applets und Systemaufgaben.</translation>
    </message>
</context>
<context>
    <name>WinControlPanelTaskRootItem</name>
    <message>
        <location line="-40"/>
        <location line="+11"/>
        <source>Control Panel</source>
        <translation>Systemsteuerung</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Task ID</source>
        <translation>Aufgaben-ID</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Open</source>
        <translation>Öffnen</translation>
    </message>
</context>
<context>
    <name>WinSettingsPage</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="-114"/>
        <source>Display</source>
        <translation>Anzeige</translation>
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
        <translation>System</translation>
    </message>
    <message>
        <location line="-17"/>
        <source>Night Light</source>
        <translation>Nachtlicht</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sound</source>
        <translation>Ton</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Volume Mixer</source>
        <translation>Lautstärkemischer</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Notifications</source>
        <translation>Benachrichtigungen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Focus</source>
        <translation>Fokussieren</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power &amp; Battery</source>
        <translation>Energie &amp; Akku</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Storage</source>
        <translation>Speicher</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Nearby Sharing</source>
        <translation>Umgebungsfreigabe</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Multitasking</source>
        <translation>Multitasking</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activation</source>
        <translation>Aktivierung</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Troubleshoot</source>
        <translation>Fehlerbehebung</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Recovery</source>
        <translation>Wiederherstellung</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Projecting to This PC</source>
        <translation>Auf diesen PC projizieren</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Remote Desktop</source>
        <translation>Remotedesktop</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clipboard</source>
        <translation>Zwischenablage</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>About</source>
        <translation>Über</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Optional Features</source>
        <translation>Optionale Funktionen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>For Developers</source>
        <translation>Für Entwickler</translation>
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
        <translation>Bluetooth und Geräte</translation>
    </message>
    <message>
        <location line="-8"/>
        <source>Devices</source>
        <translation>Geräte</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Printers &amp; Scanners</source>
        <translation>Drucker und Scanner</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mobile Devices</source>
        <translation>Mobile Geräte</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Cameras</source>
        <translation>Kameras</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mouse</source>
        <translation>Maus</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Touchpad</source>
        <translation>Touchpad</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pen &amp; Windows Ink</source>
        <translation>Stift &amp; Windows Ink</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>AutoPlay</source>
        <translation>AutoPlay</translation>
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
        <translation>Netzwerk &amp; Internet</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Wi-Fi</source>
        <translation>WLAN</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ethernet</source>
        <translation>Ethernet</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Mobile Hotspot</source>
        <translation>Mobiler Hotspot</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Airplane Mode</source>
        <translation>Flugzeugmodus</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Proxy</source>
        <translation>Proxy</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dial-up</source>
        <translation>Einwahl</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced Network Settings</source>
        <translation>Erweiterte Netzwerkeinstellungen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Background</source>
        <translation>Hintergrund</translation>
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
        <translation>Personalisierung</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Colors</source>
        <translation>Farben</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Themes</source>
        <translation>Themen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lock Screen</source>
        <translation>Sperrbildschirm</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Touch Keyboard</source>
        <translation>Bildschirmtastatur</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Start</source>
        <translation>Start</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Taskbar</source>
        <translation>Taskleiste</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Fonts</source>
        <translation>Schriftarten</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dynamic Lighting</source>
        <translation>Dynamische Beleuchtung</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Installed Apps</source>
        <translation>Installierte Apps</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Apps</source>
        <translation>Apps</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>Default Apps</source>
        <translation>Standard-Apps</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Offline Maps</source>
        <translation>Offline-Karten</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Apps for Websites</source>
        <translation>Apps für Websites</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Video Playback</source>
        <translation>Videowiedergabe</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Startup Apps</source>
        <translation>Autostart-Apps</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Your Info</source>
        <translation>Ihre Informationen</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Accounts</source>
        <translation>Konten</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>Email &amp; Accounts</source>
        <translation>E-Mail und Konten</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sign-in Options</source>
        <translation>Anmeldeoptionen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Access Work or School</source>
        <translation>Auf Arbeits- oder Schulkonto zugreifen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Family &amp; Other Users</source>
        <translation>Familie und andere Benutzer</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Windows Backup</source>
        <translation>Windows-Sicherung</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Date &amp; Time</source>
        <translation>Datum und Uhrzeit</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Time &amp; Language</source>
        <translation>Zeit &amp; Sprache</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Language &amp; Region</source>
        <translation>Sprache und Region</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Typing</source>
        <translation>Tippen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Speech</source>
        <translation>Spracherkennung</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Game Bar</source>
        <translation>Spielleiste</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Gaming</source>
        <translation>Gaming</translation>
    </message>
    <message>
        <location line="-1"/>
        <source>Captures</source>
        <translation>Aufnahmen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Game Mode</source>
        <translation>Spielmodus</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Text Size</source>
        <translation>Textgröße</translation>
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
        <translation>Barrierefreiheit</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Visual Effects</source>
        <translation>Visuelle Effekte</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Magnifier</source>
        <translation>Lupe</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Color Filters</source>
        <translation>Farbfilter</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Contrast Themes</source>
        <translation>Kontrastthemen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Narrator</source>
        <translation>Sprachausgabe</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Audio</source>
        <translation>Barrierefreies Audio</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Captions</source>
        <translation>Untertitel</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Keyboard</source>
        <translation>Barrierefreie Tastatur</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Mouse</source>
        <translation>Barrierefreie Maus</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Eye Control</source>
        <translation>Augensteuerung</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Windows Security</source>
        <translation>Windows-Sicherheit</translation>
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
        <translation>Datenschutz und Sicherheit</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Find My Device</source>
        <translation>Mein Gerät suchen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Privacy</source>
        <translation>Privatsphäre</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Location</source>
        <translation>Standort</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Camera Access</source>
        <translation>Kamerazugriff</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Microphone Access</source>
        <translation>Mikrofonzugriff</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activity History</source>
        <translation>Aktivitätsverlauf</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Diagnostics &amp; Feedback</source>
        <translation>Diagnose und Feedback</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search Permissions</source>
        <translation>Suchberechtigungen</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Windows Update</source>
        <translation>Windows-Update</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Update History</source>
        <translation>Update-Verlauf</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced Update Options</source>
        <translation>Erweiterte Update-Optionen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Windows Insider Program</source>
        <translation>Windows Insider-Programm</translation>
    </message>
</context>
<context>
    <name>WinSettingsPageRootItem</name>
    <message>
        <location line="+37"/>
        <source>System Settings</source>
        <translation>Systemeinstellungen</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Settings</source>
        <translation>Einstellungen</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Category</source>
        <translation>Kategorie</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Open %1 Settings</source>
        <translation>%1-Einstellungen öffnen</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy URL</source>
        <translation>URL kopieren</translation>
    </message>
</context>
<context>
    <name>WinSettingsRootProvider</name>
    <message>
        <location line="+12"/>
        <source>Windows Settings</source>
        <translation>Windows-Einstellungen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Pages of the Windows Settings app.</source>
        <translation>Seiten der Windows-Einstellungen-App.</translation>
    </message>
</context>
<context>
    <name>WindowManagementExtension</name>
    <message>
        <location filename="../src/extensions/wm/wm-extension.hpp" line="+12"/>
        <source>Window Management</source>
        <translation>Fensterverwaltung</translation>
    </message>
</context>
<context>
    <name>WindowsAppDatabase</name>
    <message>
        <location filename="../src/services/app-service/windows/win-app-database.cpp" line="+993"/>
        <source>Focus window</source>
        <translation>Fenster fokussieren</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Launch app</source>
        <translation>App starten</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Default action</source>
        <translation>Standardaktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Action to perform when the return key is pressed. Always default to &apos;launch&apos; if the app has no open window.</source>
        <translation>Aktion, die ausgeführt werden soll, wenn die Eingabetaste gedrückt wird. Wenn die App kein geöffnetes Fenster hat, ist die Standardeinstellung immer „Starten“.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>File Explorer</source>
        <translation>Datei-Explorer</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Terminal</source>
        <translation>Terminal</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Command Prompt</source>
        <translation>Eingabeaufforderung</translation>
    </message>
</context>
<context>
    <name>WindowsApplication</name>
    <message>
        <location filename="../src/services/app-service/windows/win-app.hpp" line="+70"/>
        <source>%1: Run as Administrator</source>
        <translation>%1: Als Administrator ausführen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run as Administrator</source>
        <translation>Als Administrator ausführen</translation>
    </message>
</context>
<context>
    <name>WindowsGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/windows-global-shortcut-backend.cpp" line="+215"/>
        <source>unsupported or invalid trigger</source>
        <translation>nicht unterstützter oder ungültiger Trigger</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>already registered by another application</source>
        <translation>bereits durch eine andere Anwendung registriert</translation>
    </message>
</context>
<context>
    <name>X11GlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/x11-global-shortcut-backend.cpp" line="+122"/>
        <source>This shortcut is already in use by another application</source>
        <translation>Dieses Tastenkürzel wird bereits von einer anderen Anwendung verwendet</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Unsupported trigger key</source>
        <translation>Nicht unterstützter Triggerschlüssel</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Trigger key is not present on this keyboard</source>
        <translation>Auf dieser Tastatur ist keine Auslösetaste vorhanden</translation>
    </message>
</context>
<context>
    <name>X11Workspace</name>
    <message>
        <location filename="../src/services/window-manager/x11/x11-window-manager.cpp" line="+426"/>
        <source>Desktop %1</source>
        <translation>Desktop %1</translation>
    </message>
</context>
<context>
    <name>XdgAppDatabase</name>
    <message>
        <location filename="../src/services/app-service/xdg/xdg-app-database.cpp" line="+564"/>
        <source>Focus window</source>
        <translation>Fenster fokussieren</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Launch app</source>
        <translation>App starten</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Default action</source>
        <translation>Standardaktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Action to perform when the return key is pressed. Always default to &apos;launch&apos; if the app has no open window.</source>
        <translation>Aktion, die ausgeführt werden soll, wenn die Eingabetaste gedrückt wird. Wenn die App kein geöffnetes Fenster hat, ist die Standardeinstellung immer „Starten“.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Launch Prefix</source>
        <translation>Startpräfix</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Custom app launcher to use. Affects applications as well as their sub-actions.</source>
        <translation>Zu verwendender benutzerdefinierter App-Launcher. Wirkt sich auf Anwendungen und deren Unteraktionen aus.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Application directories</source>
        <translation>Anwendungsverzeichnisse</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Directories applications are sourced from. The list cannot be modified directly. In order to do so, you need to append additonal paths to the &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt; environment variables.</source>
        <translation>Verzeichnisse, aus denen Anwendungen stammen. Die Liste kann nicht direkt geändert werden. Dazu müssen Sie zusätzliche Pfade an die Umgebungsvariablen &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt; anhängen.</translation>
    </message>
</context>
<context>
    <name>XdpFileChooser</name>
    <message>
        <location filename="../src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.cpp" line="+39"/>
        <source>Open Directory</source>
        <translation>Verzeichnis öffnen</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Open File</source>
        <translation>Datei öffnen</translation>
    </message>
</context>
<context>
    <name>browser-extension</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="-58"/>
        <source>No browser connected</source>
        <translation>Kein Browser verbunden</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You need to connect at least one browser to vicinae using the browser extension in order to use this command.</source>
        <translation>Um diesen Befehl verwenden zu können, müssen Sie über die Browsererweiterung mindestens einen Browser mit vicinae verbinden.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Open documentation</source>
        <translation>Dokumentation öffnen</translation>
    </message>
</context>
<context>
    <name>clipboard-history-view-host</name>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.cpp" line="-207"/>
        <source>Text</source>
        <translation>Text</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Link</source>
        <translation>Link</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Image</source>
        <translation>Bild</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>File</source>
        <translation>Datei</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Unknown</source>
        <translation>Unbekannt</translation>
    </message>
</context>
<context>
    <name>emoji-categories</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-52"/>
        <source>Smileys &amp; Emotion</source>
        <translation>Smileys &amp; Emotionen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>People &amp; Body</source>
        <translation>Menschen &amp; Körper</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Animals &amp; Nature</source>
        <translation>Tiere &amp; Natur</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Food &amp; Drink</source>
        <translation>Essen &amp; Trinken</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Travel &amp; Places</source>
        <translation>Reisen &amp; Orte</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activities</source>
        <translation>Aktivitäten</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Objects</source>
        <translation>Objekte</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Symbols</source>
        <translation>Symbole</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Flags</source>
        <translation>Flaggen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Math</source>
        <translation>Mathe</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Arrows</source>
        <translation>Pfeile</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Currency</source>
        <translation>Währung</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Punctuation</source>
        <translation>Interpunktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Shapes</source>
        <translation>Formen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Misc Symbols</source>
        <translation>Verschiedene Symbole</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Greek</source>
        <translation>Griechisch</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Number Forms</source>
        <translation>Zahlenformen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Fancy Letters</source>
        <translation>Ausgefallene Buchstaben</translation>
    </message>
</context>
<context>
    <name>emoji-grid-model</name>
    <message>
        <location line="+129"/>
        <source>Copy</source>
        <translation>Kopieren</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy name</source>
        <translation>Namen kopieren</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy unicode codepoint</source>
        <translation>Unicode-Codepunkt kopieren</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy category</source>
        <translation>Kategorie kopieren</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>Skin tones</source>
        <translation>Hauttöne</translation>
    </message>
</context>
<context>
    <name>file-list-item</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+35"/>
        <source>Copy file</source>
        <translation>Datei kopieren</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy file path</source>
        <translation>Dateipfad kopieren</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy file name</source>
        <translation>Dateinamen kopieren</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Copy mime type</source>
        <translation>MIME-Typ kopieren</translation>
    </message>
</context>
<context>
    <name>font-categories</name>
    <message>
        <location filename="../src/font-service.cpp" line="+127"/>
        <source>Latin</source>
        <translation>Latein</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Cyrillic</source>
        <translation>Kyrillisch</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Greek</source>
        <translation>Griechisch</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Monospace</source>
        <translation>Monospace</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Emoji</source>
        <translation>Emoji</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Japanese</source>
        <translation>Japanisch</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Korean</source>
        <translation>Koreanisch</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Simplified Chinese</source>
        <translation>Vereinfachtes Chinesisch</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Traditional Chinese</source>
        <translation>Traditionelles Chinesisch</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Arabic</source>
        <translation>Arabisch</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Hebrew</source>
        <translation>Hebräisch</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Thai</source>
        <translation>Thailändisch</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lao</source>
        <translation>Laotisch</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Devanagari</source>
        <translation>Devanagari</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Bengali</source>
        <translation>Bengali</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Gurmukhi</source>
        <translation>Gurmukhi</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Gujarati</source>
        <translation>Gujarati</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tamil</source>
        <translation>Tamilisch</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Telugu</source>
        <translation>Telugu</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Kannada</source>
        <translation>Kannada</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Malayalam</source>
        <translation>Malayalam</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Sinhala</source>
        <translation>Singhalesisch</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Armenian</source>
        <translation>Armenisch</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Georgian</source>
        <translation>Georgisch</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Thaana</source>
        <translation>Thaana</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tibetan</source>
        <translation>Tibetisch</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Myanmar</source>
        <translation>Myanmar</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Khmer</source>
        <translation>Khmer</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Syriac</source>
        <translation>Syrisch</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ogham</source>
        <translation>Ogham</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Runic</source>
        <translation>Runen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>N&apos;Ko</source>
        <translation>N&apos;Ko</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Symbols</source>
        <translation>Symbole</translation>
    </message>
</context>
<context>
    <name>font-grid-model</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="+26"/>
        <source>Copy font family</source>
        <translation>Schriftfamilie kopieren</translation>
    </message>
</context>
<context>
    <name>keybind-manager</name>
    <message>
        <location filename="../src/internal/keyboard/keybind-manager.cpp" line="+9"/>
        <source>Toggle action panel</source>
        <translation>Aktionsleiste ein-/ausblenden</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Toggle the action panel to access and filter through the list of available actions for the currently selected item</source>
        <translation>Aktionsleiste öffnen oder schließen, um die verfügbaren Aktionen für das ausgewählte Element aufzurufen und zu filtern</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Open Search Filter</source>
        <translation>Suchfilter öffnen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the search filter selector if present</source>
        <translation>Öffnen Sie die Suchfilterauswahl, falls vorhanden</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Open settings window</source>
        <translation>Einstellungsfenster öffnen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open this settings window from the launcher window</source>
        <translation>Öffnen Sie dieses Einstellungsfenster über das Launcher-Fenster</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Open Action</source>
        <translation>Allgemeine Öffnen-Aktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can open the selected item</source>
        <translation>Kann von Aktionen verwendet werden, die das ausgewählte Element öffnen können</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Copy Action</source>
        <translation>Allgemeine Kopieren-Aktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the selected item</source>
        <translation>Kann von Aktionen verwendet werden, die das ausgewählte Element kopieren können</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Name Action</source>
        <translation>Namen kopieren</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the name of the selected item</source>
        <translation>Kann von Aktionen verwendet werden, die den Namen des ausgewählten Elements kopieren können</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Path Action</source>
        <translation>Pfad-kopieren-Aktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the path of the selected item</source>
        <translation>Kann von Aktionen verwendet werden, die den Pfad des ausgewählten Elements kopieren können</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Save Action</source>
        <translation>Speichern</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can save the selected item</source>
        <translation>Kann von Aktionen verwendet werden, die das ausgewählte Element speichern können</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Duplicate Action</source>
        <translation>Duplizieren-Aktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can duplicate the selected item</source>
        <translation>Kann von Aktionen verwendet werden, die das ausgewählte Element duplizieren können</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic New Action</source>
        <translation>Allgemeine Neu-Aktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that create something</source>
        <translation>Kann von Aktionen verwendet werden, die etwas erschaffen</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Move Up Action</source>
        <translation>Allgemeine Nach-oben-Aktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can move up the selected item. This does not affect list navigation controls.</source>
        <translation>Kann von Aktionen verwendet werden, die das ausgewählte Element nach oben verschieben können. Dies hat keine Auswirkungen auf die Steuerelemente für die Listennavigation.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Move Down Action</source>
        <translation>Allgemeine Nach-unten-Aktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can move down the selected item. This does not affect list navigation controls.</source>
        <translation>Kann von Aktionen verwendet werden, die das ausgewählte Element nach unten verschieben können. Dies hat keine Auswirkungen auf die Steuerelemente für die Listennavigation.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Refresh Action</source>
        <translation>Allgemeine Aktualisieren-Aktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can refresh the selected item</source>
        <translation>Kann von Aktionen verwendet werden, die das ausgewählte Element aktualisieren können</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Pin Action</source>
        <translation>Allgemeine Anheften-Aktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can pin the selected item</source>
        <translation>Kann von Aktionen verwendet werden, die das ausgewählte Element anheften können</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Remove Action</source>
        <translation>Entfernen-Aktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can remove the selected item. This is normally used for small, not too impactful removals.</source>
        <translation>Kann von Aktionen verwendet werden, die das ausgewählte Element entfernen können. Dies wird normalerweise für kleine, nicht allzu wirkungsvolle Entfernungen verwendet.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Dangerous Remove Action</source>
        <translation>Gefährliche Entfernen-Aktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that perform an impactful removal, generally accompanied by a confirmation dialog.</source>
        <translation>Kann von Aktionen verwendet werden, die eine wirkungsvolle Entfernung durchführen, im Allgemeinen begleitet von einem Bestätigungsdialog.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit Action</source>
        <translation>Bearbeiten</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can edit the currently selected item</source>
        <translation>Kann von Aktionen verwendet werden, die das aktuell ausgewählte Element bearbeiten können</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit Secondary Action</source>
        <translation>Sekundäre Bearbeiten-Aktion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can edit a secondary characteristic of the currently selected item</source>
        <translation>Kann von Aktionen verwendet werden, die ein sekundäres Merkmal des aktuell ausgewählten Elements bearbeiten können</translation>
    </message>
</context>
<context>
    <name>macos-update-installer</name>
    <message>
        <location filename="../src/services/update/macos-update-installer.mm" line="-184"/>
        <source>Update image contains more than one app</source>
        <translation>Das Update-Image enthält mehr als eine App</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to list update image: %1</source>
        <translation>Update-Image konnte nicht aufgelistet werden: %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No app found in update image</source>
        <translation>Im Update-Image wurde keine App gefunden</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to read the update&apos;s code signature</source>
        <translation>Die Codesignatur des Updates konnte nicht gelesen werden</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to build the signature requirement</source>
        <translation>Die Signaturanforderung konnte nicht erstellt werden</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Update signature verification failed (%1)</source>
        <translation>Überprüfung der Update-Signatur fehlgeschlagen (%1)</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Update has no CFBundleShortVersionString</source>
        <translation>Das Update hat keinen CFBundleShortVersionString</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Update version mismatch: expected %1, found %2</source>
        <translation>Falsche Update-Version: %1 erwartet, %2 gefunden</translation>
    </message>
</context>
<context>
    <name>shortcut-conflict</name>
    <message>
        <location filename="../src/qml/shortcut-conflict.cpp" line="+10"/>
        <source>Modifier required</source>
        <translation>Modifikator erforderlich</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+5"/>
        <source>Already bound to &quot;%1&quot;</source>
        <translation>Bereits an „%1“ gebunden</translation>
    </message>
</context>
<context>
    <name>system-extension</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-34"/>
        <source>Volume %1%</source>
        <translation>Lautstärke %1 %</translation>
    </message>
</context>
<context>
    <name>utils</name>
    <message>
        <location filename="../src/utils/utils.cpp" line="+88"/>
        <source>0 bytes</source>
        <translation>0 Bytes</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>bytes</source>
        <translation>Bytes</translation>
    </message>
</context>
<context>
    <name>virtual-desktops</name>
    <message>
        <location filename="../src/services/window-manager/windows/virtual-desktops.cpp" line="+67"/>
        <source>Desktop %1</source>
        <translation>Desktop %1</translation>
    </message>
</context>
<context>
    <name>TrayService</name>
    <message>
        <source>Toggle Vicinae</source>
        <translation>Vicinae ein-/ausblenden</translation>
    </message>
    <message>
        <source>About Vicinae</source>
        <translation>Über Vicinae</translation>
    </message>
    <message>
        <source>Check for Updates…</source>
        <translation>Nach Updates suchen…</translation>
    </message>
    <message>
        <source>Update Available: %1</source>
        <translation>Update verfügbar: %1</translation>
    </message>
    <message>
        <source>Settings…</source>
        <translation>Einstellungen…</translation>
    </message>
    <message>
        <source>Preferences…</source>
        <translation>Einstellungen…</translation>
    </message>
    <message>
        <source>Sponsor Vicinae</source>
        <translation>Vicinae unterstützen</translation>
    </message>
    <message>
        <source>Join the Discord</source>
        <translation>Discord-Server beitreten</translation>
    </message>
    <message>
        <source>Follow on X</source>
        <translation>Auf X folgen</translation>
    </message>
    <message>
        <source>Quit Vicinae</source>
        <translation>Vicinae beenden</translation>
    </message>
</context>
</TS>
