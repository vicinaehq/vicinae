<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="es_ES">
<context>
    <name>AboutSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AboutSettingsPage.qml" line="+57"/>
        <source>Version %1 - Commit %2
(%3)</source>
        <translation>Versión %1 – Commit %2
(%3)</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Documentation</source>
        <translation>Documentación</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Report a Bug</source>
        <translation>Informar de un error</translation>
    </message>
</context>
<context>
    <name>ActionListPanel</name>
    <message>
        <location filename="../src/qml/qml/ActionListPanel.qml" line="+113"/>
        <source>No matching actions</source>
        <translation>No hay coincidencias</translation>
    </message>
    <message>
        <location line="+135"/>
        <source>Filter actions...</source>
        <translation>Filtrar acciones...</translation>
    </message>
</context>
<context>
    <name>AdvancedSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AdvancedSettingsPage.qml" line="+29"/>
        <source>Input &amp; Navigation</source>
        <translation>Entrada y navegación</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Pop on backspace</source>
        <translation>Ir hacia atrás con la tecla de retroceso</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pop back in navigation on backspace when no input is present.</source>
        <translation>Ir hacia atrás en la navegación al presionar la tecla de retroceso si la entrada está vacía.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Activate on single click</source>
        <translation>Ejecutar acción asociada con un solo clic</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activate items with a single click instead of requiring a double click.</source>
        <translation>Ejecutar la acción asociada al elemento seleccionado con un solo clic en lugar de requerir un doble clic.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Wrap navigation</source>
        <translation>Navegación cíclica</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Wrap around to the opposite end when moving past the first or last item.</source>
        <translation>Ir al extremo opuesto cuando se navega hacia arriba desde el primer elemento o hacia abajo desde el último elemento.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>IME handling</source>
        <translation>Gestión de IME</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Include IME Preedit strings as part of search queries.</source>
        <translation>Incluir las cadenas de preedición de IME como parte de las consultas de búsqueda.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Keybinding Scheme</source>
        <translation>Esquema de navegación</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Default uses the standard macOS keys (arrows, Ctrl+N/P); Vim uses Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Opt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>El esquema por defecto utiliza las teclas estándar de macOS (flechas, Ctrl+N/P); Vim utiliza Ctrl+J/K y Ctrl+H/L; Emacs utiliza Ctrl+N/P y Ctrl+Opt+B/F para la navegación, además de la edición en la barra de búsqueda.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Default and Vim use Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Alt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>El esquema por defecto y Vim utilizan Ctrl+J/K y Ctrl+H/L; Emacs utiliza Ctrl+N/P y Ctrl+Alt+B/F para la navegación, además de la edición en la barra de búsqueda.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Search</source>
        <translation>Búsqueda</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Root file search</source>
        <translation>Buscar archivos desde el directorio raíz</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Files are searched asynchronously, so if enabled you should expect a slight delay for file search results to show up.</source>
        <translation>Los archivos se buscarán de forma asíncrona. Es posible que haya un ligero retraso hasta que aparezcan los resultados de la búsqueda de archivos.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Favicon Fetching</source>
        <translation>Obtención de favicons</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The favicon provider used to load favicons where needed. Select &apos;None&apos; to turn off favicon loading.</source>
        <translation>Proveedor de favicons utilizado para cargar los favicons cuando sea necesario. Seleccione &apos;Ninguno&apos; para desactivar la carga de favicons.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>System</source>
        <translation>Sistema</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Input server</source>
        <translation>Servidor de entrada</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Whether to spawn the input server at startup. This needs to be enabled in order to support snippets, paste to active window, and other features that require input monitoring or injection.</source>
        <translation>Activar si se desea iniciar el servidor de entrada al arrancar. Es necesario que esté activada esta opción para soportar snippets, pegar en la ventana activa y otras características que requieran monitoreo o inyección de entrada.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Security</source>
        <translation>Seguridad</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Encrypt sensitive data</source>
        <translation>Cifrar datos sensibles</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Encrypt sensitive data at rest, such as clipboard history and internal databases (OAuth tokens, extension local storage, API keys). Note that some components, such as on-disk clipboard history, may not be retroactively affected when toggling this option. Turning on this option may ask you to unlock your keychain. Requires a restart in order to apply.</source>
        <translation>Cifrar los datos sensibles como el historial del portapapeles o las bases de datos internas (tokens de OAuth, almacenamiento local de extensiones, claves de API) cuando se detecte que el equipo está inactivo. Es posible que algunos componentes como el historial del portapapeles en disco, puedan no verse afectados retroactivamente al cambiar esta opción. Activar esta opción puede pedirle que desbloquee su keychain. Requiere un reinicio para aplicar los cambios.</translation>
    </message>
</context>
<context>
    <name>AlertWidget</name>
    <message>
        <location filename="../src/ui/alert/alert.hpp" line="+15"/>
        <source>Are you sure?</source>
        <translation>¿Estás seguro?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>This action cannot be undone</source>
        <translation>Esta acción no se puede deshacer</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Confirm</source>
        <translation>Confirmar</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Cancel</source>
        <translation>Cancelar</translation>
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
        <translation>Palabras adicionales para indexar este elemento</translation>
    </message>
</context>
<context>
    <name>AliasFormViewHost</name>
    <message>
        <location filename="../src/qml/alias-form-view-host.cpp" line="+28"/>
        <source>Set alias - %1</source>
        <translation>Asignar alias - %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Submit</source>
        <translation>Aceptar</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Alias modified</source>
        <translation>Alias modificado</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to modify alias</source>
        <translation>No se pudo modificar el alias</translation>
    </message>
</context>
<context>
    <name>AppRootItem</name>
    <message>
        <location filename="../src/root-search/apps/app-root-provider.cpp" line="+22"/>
        <location line="+29"/>
        <source>Application</source>
        <translation>Aplicación</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Where</source>
        <translation>Dónde</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Opens in terminal</source>
        <translation>Abrir en la terminal</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Yes</source>
        <translation>Sí</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No</source>
        <translation>No</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Open Application</source>
        <translation>Abrir aplicación</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy App ID</source>
        <translation>Copiar ID de la aplicación</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy App Location</source>
        <translation>Copiar ubicación de la aplicación</translation>
    </message>
</context>
<context>
    <name>AppRootProvider</name>
    <message>
        <location line="+75"/>
        <source>Applications</source>
        <translation>Aplicaciones</translation>
    </message>
</context>
<context>
    <name>AppSelectorModel</name>
    <message>
        <location filename="../src/qml/app-selector-model.cpp" line="+20"/>
        <location line="+65"/>
        <source>%1 (Default)</source>
        <translation>%1 (Por defecto)</translation>
    </message>
</context>
<context>
    <name>AppearanceSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AppearanceSettingsPage.qml" line="+29"/>
        <location line="+7"/>
        <source>Theme</source>
        <translation>Diseño</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Font</source>
        <translation>Fuente</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Font size</source>
        <translation>Tamaño de la fuente</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The base point size used to compute font sizes. Fractional values are accepted. Recommended range is [10.0;12.0].</source>
        <translation>El tamaño base utilizado para calcular los tamaños de fuente. Se aceptan valores fraccionarios. El rango recomendado es [10.0, 12.0].</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>e.g. 11</source>
        <translation>ej. 11</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Icon Theme</source>
        <translation>Paquete de iconos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The icon theme used for system icons (applications, mime types, folder icons...). Does not affect builtin Vicinae icons.</source>
        <translation>El conjunto de iconos utilizado para los iconos del sistema (aplicaciones, tipos MIME, iconos de carpetas, etc.). No afecta a los iconos integrados en Vicinae.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Window</source>
        <translation>Ventana</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Window material</source>
        <translation>Textura de la ventana</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Background material applied to the launcher window. Lower the window opacity to see it.</source>
        <translation>Material de fondo aplicado a la ventana del lanzador. Reduce la opacidad de la ventana para verlo.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Window opacity</source>
        <translation>Opacidad de la ventana</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>e.g. 1.0</source>
        <translation>ej. 1,0</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Compact mode</source>
        <translation>Modo compacto</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Show only the search bar at root; expand when a query is entered.</source>
        <translation>Mostrar únicamente la barra de búsqueda al inicio y expandir a medida que se introduce una consulta.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Use layer shell</source>
        <translation>Usar layer shell</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Anchor the launcher as a Wayland layer surface (wlr-layer-shell) instead of a regular window. May require reopening Vicinae to fully apply.</source>
        <translation>Anclar el lanzador como un layer surface de Wayland (wlr-layer-shell) en lugar de una ventana normal. Puede requerir volver a abrir Vicinae para aplicar completamente.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Client-side decorations</source>
        <translation>Retoques del lado del cliente</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Let Vicinae draw its own rounded borders and shadow instead of relying on the windowing system.</source>
        <translation>Permitir que Vicinae dibuje sus propios bordes redondeados y sombras en lugar de depender del sistema de ventanas.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Corner rounding</source>
        <translation>Redondeo de las esquinas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Radius of the launcher window corners, in pixels.</source>
        <translation>Radio de las esquinas de la ventana del lanzador en píxeles.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>e.g. 10</source>
        <translation>ej. 10</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Border width</source>
        <translation>Ancho del borde</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Thickness of the launcher window border, in pixels.</source>
        <translation>Ancho del borde de la ventana del lanzador en píxeles.</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>e.g. 3</source>
        <translation>ej. 3</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Shadow size</source>
        <translation>Tamaño de la sombra</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Size of the drop shadow cast by the launcher window, in pixels.</source>
        <translation>Tamaño de la sombra proyectada por la ventana del lanzador en píxeles.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>e.g. 12</source>
        <translation>ej. 12</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Native font rendering</source>
        <translation>Renderizado de fuentes nativas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Use the platform&apos;s native text rendering for system-consistent text. Disable for Qt distance-field rendering (usually faster). May require reopening Vicinae to fully apply.</source>
        <translation>Usar el renderizado de texto nativo de la plataforma para obtener un aspecto coherente con el sistema. Desactivar para usar el renderizado de campo de distancia de Qt (normalmente más rápido). Puede requerir volver a abrir Vicinae para aplicar los cambios completamente.</translation>
    </message>
</context>
<context>
    <name>AvailableFallbackSection</name>
    <message>
        <location filename="../src/qml/manage-fallback-model.hpp" line="+49"/>
        <source>Available</source>
        <translation>Disponible</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-fallback-model.cpp" line="+50"/>
        <source>Enable fallback</source>
        <translation>Activar respaldo</translation>
    </message>
</context>
<context>
    <name>BringToWorkspaceAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="+72"/>
        <source>Bring to current workspace</source>
        <translation>Mover a la zona de trabajo actual</translation>
    </message>
</context>
<context>
    <name>BrowseAppsSection</name>
    <message>
        <location filename="../src/qml/browse-apps-model.hpp" line="+32"/>
        <source>Applications ({count})</source>
        <translation>({count}) aplicaciones</translation>
    </message>
    <message>
        <location filename="../src/qml/browse-apps-model.cpp" line="+21"/>
        <source>Hidden</source>
        <translation>Oculto</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Open Application</source>
        <translation>Abrir aplicación</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Copy App ID</source>
        <translation>Copiar ID de la aplicación</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy App Location</source>
        <translation>Copiar ubicación de la aplicación</translation>
    </message>
</context>
<context>
    <name>BrowseAppsViewHost</name>
    <message>
        <location filename="../src/qml/browse-apps-view-host.cpp" line="+12"/>
        <source>Search apps...</source>
        <translation>Buscar aplicaciones...</translation>
    </message>
</context>
<context>
    <name>BrowseFontsCommand</name>
    <message>
        <location filename="../src/extensions/font/browse-fonts-command.hpp" line="+8"/>
        <source>Search Fonts</source>
        <translation>Buscar fuentes</translation>
    </message>
</context>
<context>
    <name>BrowserExtension</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.hpp" line="+12"/>
        <source>Browser Extension</source>
        <translation>Extensión de navegador</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Browser extension related commands.</source>
        <translation>Comandos relacionados con extensiones de navegador.</translation>
    </message>
</context>
<context>
    <name>BrowserTabActionGenerator</name>
    <message>
        <location filename="../src/actions/browser-tab-actions.hpp" line="+24"/>
        <source>Switch to tab</source>
        <translation>Cambiar a la pestaña</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Convert to shortcut</source>
        <translation>Convertir en acceso directo</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Convert tab to shortcut</source>
        <translation>Convertir pestaña en acceso directo</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Close tab</source>
        <translation>Cerrar pestaña</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to close tab: %1</source>
        <translation>No se pudo cerrar la pestaña: %1</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy URL</source>
        <translation>Copiar URL</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy Title</source>
        <translation>Copiar título</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy ID</source>
        <translation>Copiar ID</translation>
    </message>
</context>
<context>
    <name>BrowserTabProvider</name>
    <message>
        <location filename="../src/root-search/browser-tabs/browser-tabs-provider.hpp" line="+70"/>
        <source>Browser Tabs</source>
        <translation>Pestañas del navegador</translation>
    </message>
</context>
<context>
    <name>BrowserTabRootItem</name>
    <message>
        <location line="-51"/>
        <source>Browser Tab</source>
        <translation>Pestaña del navegador</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Tab</source>
        <translation>Pestaña</translation>
    </message>
</context>
<context>
    <name>BrowserTabsSection</name>
    <message>
        <location filename="../src/qml/browser-tabs-model.hpp" line="+17"/>
        <source>Tabs ({count})</source>
        <translation>({count}) pestañas</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Playing Media ({count})</source>
        <translation>({count}) reproduciendo contenido multimedia</translation>
    </message>
    <message>
        <location filename="../src/qml/browser-tabs-model.cpp" line="+15"/>
        <source>Muted</source>
        <translation>Silenciada</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Playing</source>
        <translation>Reproducción</translation>
    </message>
</context>
<context>
    <name>BrowserTabsViewHost</name>
    <message>
        <location filename="../src/qml/browser-tabs-view-host.cpp" line="+11"/>
        <source>Search, focus and close tabs</source>
        <translation>Buscar, enfocar y cerrar pestañas</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsSection</name>
    <message>
        <location filename="../src/qml/builtin-icons-model.hpp" line="+20"/>
        <source>Icons ({count})</source>
        <translation>({count}) iconos</translation>
    </message>
    <message>
        <location filename="../src/qml/builtin-icons-model.cpp" line="+15"/>
        <source>Copy Icon Name</source>
        <translation>Copiar nombre del icono</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsViewHost</name>
    <message>
        <location filename="../src/qml/builtin-icons-view-host.cpp" line="+10"/>
        <source>Search icons...</source>
        <translation>Buscar iconos...</translation>
    </message>
</context>
<context>
    <name>CalcHistoryListView</name>
    <message>
        <location filename="../src/qml/qml/CalcHistoryListView.qml" line="+12"/>
        <source>No results</source>
        <translation>Sin resultados</translation>
    </message>
</context>
<context>
    <name>CalcHistorySection</name>
    <message>
        <location filename="../src/qml/calc-history-model.cpp" line="+41"/>
        <source>Copy answer</source>
        <translation>Copiar respuesta</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy question</source>
        <translation>Copiar pregunta</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy question and answer</source>
        <translation>Copiar pregunta y respuesta</translation>
    </message>
</context>
<context>
    <name>CalcHistoryViewHost</name>
    <message>
        <location filename="../src/qml/calc-history-view-host.cpp" line="+83"/>
        <source>Search past calculations...</source>
        <translation>Buscar cálculos anteriores...</translation>
    </message>
</context>
<context>
    <name>CalcLiveSection</name>
    <message>
        <location filename="../src/qml/calc-history-view-host.hpp" line="+27"/>
        <source>Calculator</source>
        <translation>Calculadora</translation>
    </message>
</context>
<context>
    <name>CalculatorExtension</name>
    <message>
        <location filename="../src/extensions/calculator/calculator-extension.hpp" line="+73"/>
        <source>Calculator</source>
        <translation>Calculadora</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Do maths, convert units or search past calculations...</source>
        <translation>Hacer cálculos, convertir unidades o buscar cálculos anteriores...</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Calculator Backend</source>
        <translation>Backend de la calculadora</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Which backend to use to perform calculations</source>
        <translation>Backend elegido para realizar cálculos</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Refresh rates on startup</source>
        <translation>Actualizar tasas de cambio al iniciar</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Whether exchange rates should be refreshed every time the vicinae server is started. If the current backend does not support it, this is ignored.</source>
        <translation>Indica si las tasas de cambio deben actualizarse cada vez que se inicia el servidor vicinae. Si el backend actual no lo admite, se ignora esta opción.</translation>
    </message>
</context>
<context>
    <name>CalculatorHistoryCommand</name>
    <message>
        <location line="-86"/>
        <source>Calculator history</source>
        <translation>Historial de la calculadora</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse past calculations. You need to copy the result of a calculation for it to be saved in history.</source>
        <translation>Buscar cálculos anteriores. Necesita copiar el resultado de un cálculo para que se guarde en el historial.</translation>
    </message>
</context>
<context>
    <name>CalculatorRefreshRatesCommand</name>
    <message>
        <location line="+11"/>
        <source>Refresh Exchange Rates</source>
        <translation>Actualizar tasas de cambio</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Refresh exchange rates used by the calculator to provide currency conversion features. Not all backends may support currency conversions or manually refreshing the rates.</source>
        <translation>Actualizar las tasas de cambio utilizadas por la calculadora para proporcionar funciones de conversión de monedas. No todos los backends pueden soportar conversiones de monedas o la actualización manual de las tasas.</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>%1 can&apos;t refresh rates</source>
        <translation>%1 no puede actualizar las tasas</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Refreshing rates...</source>
        <translation>Actualizando tasas...</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Rates successfully refreshed</source>
        <translation>Tasas actualizadas correctamente</translation>
    </message>
</context>
<context>
    <name>CalculatorResultDelegate</name>
    <message>
        <location filename="../src/qml/qml/CalculatorResultDelegate.qml" line="+40"/>
        <source>Question</source>
        <translation>Pregunta</translation>
    </message>
    <message>
        <location line="+57"/>
        <source>Answer</source>
        <translation>Respuesta</translation>
    </message>
</context>
<context>
    <name>CalculatorService</name>
    <message>
        <location filename="../src/services/calculator-service/calculator-service.cpp" line="+119"/>
        <source>Pinned</source>
        <translation>Fijado</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Today</source>
        <translation>Hoy</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>This week</source>
        <translation>Esta semana</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>This month</source>
        <translation>Este mes</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>This year</source>
        <translation>Este año</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>A few years ago</source>
        <translation>Hace unos años</translation>
    </message>
</context>
<context>
    <name>CategoryFilterAccessory</name>
    <message>
        <location filename="../src/qml/qml/CategoryFilterAccessory.qml" line="+8"/>
        <source>All</source>
        <translation>Todos</translation>
    </message>
</context>
<context>
    <name>ChangeEmojiSkinToneAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+99"/>
        <source>%1 skin tone</source>
        <translation>%1 tono de piel</translation>
    </message>
</context>
<context>
    <name>ClearClipboardHistoryCommand</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.cpp" line="+35"/>
        <source>Clear Clipboard History</source>
        <translation>Vaciar historial del portapapeles</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clear the clipboard history</source>
        <translation>Vaciar el historial del portapapeles</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Are you sure?</source>
        <translation>¿Estás seguro?</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Your clipboard history will be gone forever :(</source>
        <translation>El historial del portapapeles se perderá para siempre 🤐</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to clear clipboard history</source>
        <translation>No se pudo vaciar el historial del portapapeles</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard history cleared</source>
        <translation>El historial del portapapeles se ha vaciado</translation>
    </message>
</context>
<context>
    <name>ClipboardClearCommand</name>
    <message>
        <location line="-39"/>
        <source>Clear Current Clipboard Data</source>
        <translation>Limpiar los datos actuales del portapapeles</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clear the current content of the clipboard</source>
        <translation>Limpiar el contenido actual del portapapeles</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Failed to clear clipboard</source>
        <translation>No se pudo limpiar el portapapeles</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard cleared</source>
        <translation>Portapapeles vaciado</translation>
    </message>
</context>
<context>
    <name>ClipboardExtension</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.hpp" line="+13"/>
        <source>Clipboard</source>
        <translation>Portapapeles</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>System clipboard integration</source>
        <translation>Integration con el portapapeles del sistema</translation>
    </message>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.cpp" line="+55"/>
        <source>Erase on startup</source>
        <translation>Borrar al iniciar</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Erase clipboard history every time the vicinae server is started</source>
        <translation>Vaciar el historial del portapapeles cada vez que se inicie el servidor Vicinae</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Clipboard monitoring</source>
        <translation>Monitoreo del portapapeles</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Whether clipboard activity is recorded in the history. Every clipboard action performed while this is turned off will not be recorded.</source>
        <translation>Decidir si la actividad del portapapeles se registra en el historial. Cada acción del portapapeles realizada mientras esta opción está desactivada no será registrada.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Ignore Passwords</source>
        <translation>Ignorar Contraseñas</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Ignore selections that can be identified as a password. This relies on the application providing an explicit hint that the selection is a password. While most password managers and private browser windows do, some might not implement this properly.</source>
        <translation>Ignorar las selecciones que puedan ser identificadas como contraseñas. Esto depende de que la aplicación proporcione algún indicio de que la selección es una contraseña. Aunque la mayoría de los gestores de contraseñas y ventanas de navegación privada lo hacen, algunas aplicaciones pueden no hacerlo correctamente.</translation>
    </message>
</context>
<context>
    <name>ClipboardFilterAccessory</name>
    <message>
        <location filename="../src/qml/qml/ClipboardFilterAccessory.qml" line="+8"/>
        <source>All</source>
        <translation>Todos</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Text</source>
        <translation>Texto</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Images</source>
        <translation>Imágenes</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Links</source>
        <translation>Enlaces</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Files</source>
        <translation>Archivos</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryCommand</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-history-command.cpp" line="+16"/>
        <source>Paste</source>
        <translation>Pegar</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy</source>
        <translation>Copiar</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Default Action</source>
        <translation>Acción por defecto</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to perform on pressing return. Paste is only available if your environment supports it.</source>
        <translation>La acción por defecto a realizar al presionar la tecla Enter. Pegar solo está disponible si su sistema lo soporta.</translation>
    </message>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-history-command.hpp" line="+11"/>
        <source>Clipboard History</source>
        <translation>Historial del portapapeles</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse your clipboard&apos;s history, pin, edit and remove entries.</source>
        <translation>Navegar por el historial de su portapapeles. Fijar, editar y eliminar entradas.</translation>
    </message>
</context>
<context>
    <name>ClipboardHistorySection</name>
    <message>
        <location filename="../src/qml/clipboard-history-model.cpp" line="+58"/>
        <source>Open Settings</source>
        <translation>Abrir configuración</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryView</name>
    <message>
        <location filename="../src/qml/qml/ClipboardHistoryView.qml" line="+187"/>
        <source>Type</source>
        <translation>Tipo</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Size</source>
        <translation>Tamaño</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copied at</source>
        <translation>Copiado a las</translation>
    </message>
    <message>
        <location line="+52"/>
            <source>Preview not available for this content type</source>
            <translation>Para este tipo de contenido no está disponible ninguna vista previa</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryViewHost</name>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.hpp" line="+84"/>
        <source>Loading...</source>
        <translation>Cargando...</translation>
    </message>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.cpp" line="+101"/>
        <source>Browse clipboard history...</source>
        <translation>Navegar por el historial del portapapeles...</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard monitoring unavailable</source>
        <translation>Monitorización del portapapeles no disponible</translation>
    </message>
    <message>
        <location line="+67"/>
        <source>Pause clipboard</source>
        <translation>Pausar el portapapeles</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Resume clipboard</source>
        <translation>Reanudar el portapapeles</translation>
    </message>
    <message numerus="yes">
        <location line="+8"/>
        <source>%n Items</source>
        <translation>
            <numerusform>%n elemento</numerusform>
            <numerusform>%n elementos</numerusform>
        </translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Decryption failed</source>
        <translation>El descifrado ha fallado</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Vicinae could not decrypt the data for this selection. It was most likely encrypted with a different key and cannot be recovered. You can remove this entry from the history.</source>
        <translation>Vicinae no pudo descifrar los datos para esta selección. Es muy probable que haya sido cifrado con una clave diferente y no se pueda recuperar. Puede eliminar esta entrada del historial.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Data unavailable</source>
        <translation>Datos no disponibles</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The data for this selection could not be found on disk.</source>
        <translation>Los datos para esta selección no se pudieron encontrar en la memoria.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Data is encrypted</source>
        <translation>Los datos están cifrados</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Data for this selection was previously encrypted but the clipboard is not currently configured to use encryption. You should be able to fix this by enabling it in the settings.</source>
        <translation>Los datos para esta selección fueron previamente cifrados, pero el portapapeles no está actualmente configurado para usar cifrado. Se debería poder solucionar esto activando la opción en la configuración.</translation>
    </message>
</context>
<context>
    <name>ClipboardService</name>
    <message>
        <location filename="../src/services/clipboard/clipboard-service.cpp" line="+316"/>
        <source>Image (%1x%2)</source>
        <translation>Imagen (%1x%2)</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Image</source>
        <translation>Imagen</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Unknown</source>
        <translation>Desconocido</translation>
    </message>
</context>
<context>
    <name>CloseWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="-37"/>
        <source>Close window</source>
        <translation>Cerrar ventana</translation>
    </message>
</context>
<context>
    <name>CommandLineSection</name>
    <message>
        <location filename="../src/qml/system-run-model.hpp" line="+19"/>
        <source>Execute query</source>
        <translation>Ejecutar consulta</translation>
    </message>
    <message>
        <location filename="../src/qml/system-run-model.cpp" line="+44"/>
        <source>Open in %1 (hold)</source>
        <translation>Abrir en %1 (mantener)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open in %1</source>
        <translation>Abrir en %1</translation>
    </message>
</context>
<context>
    <name>CommandListView</name>
    <message>
        <location filename="../src/qml/qml/CommandListView.qml" line="+12"/>
        <source>No results</source>
        <translation>Sin resultados</translation>
    </message>
</context>
<context>
    <name>CommandRootItem</name>
    <message>
        <location filename="../src/root-search/extensions/extension-root-provider.cpp" line="+28"/>
        <location line="+48"/>
        <source>Command</source>
        <translation>Comando</translation>
    </message>
    <message>
        <location line="-43"/>
        <location line="+27"/>
        <source>Open command</source>
        <translation>Abrir comando</translation>
    </message>
    <message>
        <location line="-13"/>
        <source>Copy extension path</source>
        <translation>Copiar ruta de la extensión</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Internal Command</source>
        <translation>Comando interno</translation>
    </message>
</context>
<context>
    <name>CompletionPopup</name>
    <message>
        <location filename="../src/qml/qml/CompletionPopup.qml" line="+11"/>
        <source>Filter...</source>
        <translation>Filtrando...</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorAnswerAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+29"/>
        <source>Answer copied to clipboard</source>
        <translation>Respuesta copiada al portapapeles</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy answer</source>
        <translation>No se ha podido copiar la respuesta</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy Result</source>
        <translation>Copiar resultado</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorQuestionAndAnswerAction</name>
    <message>
        <location line="+18"/>
        <source>Answer copied to clipboard</source>
        <translation>Respuesta copiada al portapapeles</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy answer</source>
        <translation>No se ha podido copiar la respuesta</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy Question And Answer</source>
        <translation>Copiar pregunta y respuesta</translation>
    </message>
</context>
<context>
    <name>CopyClipboardSelection</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+29"/>
        <source>Selection copied to clipboard</source>
        <translation>Selección copiada al portapapeles</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to copy to clipboard</source>
        <translation>No se ha podido copiar al portapapeles</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy to clipboard</source>
        <translation>Copiar al portapapeles</translation>
    </message>
</context>
<context>
    <name>CopyItemDeeplink</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+75"/>
        <source>Deeplink copied in clipboard</source>
        <translation>Deeplink copiado al portapapeles</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Deeplink</source>
        <translation>Copiar Deeplink</translation>
    </message>
</context>
<context>
    <name>CopyShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="+230"/>
        <source>Copied to clipboard</source>
        <translation>Copiado al portapapeles</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy shortcut</source>
        <translation>Copiar acceso directo</translation>
    </message>
</context>
<context>
    <name>CopyToClipboardAction</name>
    <message>
        <location filename="../src/clipboard-actions.hpp" line="+23"/>
        <source>Copied to clipboard</source>
        <translation>Copiado al portapapeles</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy to clipboard</source>
        <translation>Copiar al portapapeles1</translation>
    </message>
</context>
<context>
    <name>CreateExtensionCommand</name>
    <message>
        <location filename="../src/extensions/developer/developer-extension.hpp" line="+10"/>
        <source>Create Extension</source>
        <translation>Crear extensión</translation>
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
        <translation>Al enviar su extensión al &lt;a href=&quot;vicinae://launch/core/store&quot;&gt;tienda de Vicinae&lt;/a&gt;, debe coincidir exactamente con su nombre de usuario en GitHub. De lo contrario, puede introducir cualquier cosa.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Username</source>
        <translation>Nombre de usuario</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Extension Title</source>
        <translation>Nombre de la extensión</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>My Extension</source>
        <translation>Mi Extensión</translation>
    </message>
    <message>
        <location line="+8"/>
        <location line="+42"/>
        <source>Description</source>
        <translation>Descripción</translation>
    </message>
    <message>
        <location line="-36"/>
        <source>An extension that does super cool things</source>
        <translation>Una extensión que hace cosas</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Location</source>
        <translation>Ubicación</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Command Title</source>
        <translation>Nombre del comando</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>My Wonderful Command</source>
        <translation>Mi comando</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>My command does this, and that...</source>
        <translation>Mi comando hace esto y lo otro..</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Template</source>
        <translation>Plantilla</translation>
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
# Extensión creada con éxito

La nueva extensión %1 ha sido creada con éxito en `%2`.

Para que los comandos de esta extensión sean reconocidos por Vicinae, debe ejecutar su extensión en modo de desarrollo al menos una vez:

```bash
cd %2
npm install
npm run dev
```

Puede obtener más información sobre el desarrollo de extensiones en la [documentación de Vicinae](https://docs.vicinae.com/).
</translation>
    </message>
    <message>
```bash
cd %2
npm install
npm run dev
```

Puede obtener más información sobre el desarrollo de extensiones en la [documentación de Vicinae](https://docs.vicinae.com/).
</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Open in %1</source>
        <translation>Abrir en %1</translation>
    </message>
</context>
<context>
    <name>CreateExtensionViewHost</name>
    <message>
        <location filename="../src/qml/create-extension-view-host.cpp" line="+44"/>
        <source>Create extension</source>
        <translation>Crear extensión</translation>
    </message>
    <message>
        <location line="+18"/>
        <location line="+4"/>
        <location line="+19"/>
        <location line="+5"/>
        <source>Min. 3 chars</source>
        <translation>Min. 3 caracteres</translation>
    </message>
    <message>
        <location line="-20"/>
        <source>Min. 16 chars</source>
        <translation>Min. 16 caracteres</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Must exist</source>
        <translation>Debe existir</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Form has errors</source>
        <translation>El formulario tiene errores</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Failed to create extension</source>
        <translation>La extensión no pudo ser creada</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Extension created!</source>
        <translation>¡Extensión creada!</translation>
    </message>
</context>
<context>
    <name>CreateShortcutCommand</name>
    <message>
        <location filename="../src/extensions/shortcut/create-shortcut-command.hpp" line="+10"/>
        <source>Create Shortcut</source>
        <translation>Crear acceso directo</translation>
    </message>
</context>
<context>
    <name>CreateShortcutFromActiveBrowserTabCommand</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="+44"/>
        <source>Create Shortcut from Active Tab</source>
        <translation>Crear acceso directo a partir de la pestaña activa</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Create a vicinae shortcut from the currently active browser tab. May yield unexpected results if many browsers are connected at once.</source>
        <translation>Crear un acceso directo para Vicinae a partir de la pestaña del navegador activa. Puede producir resultados inesperados si muchos navegadores están abiertos al mismo tiempo.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>No active tab!</source>
        <translation>¡No se ha detectado ninguna pestaña activa!</translation>
    </message>
</context>
<context>
    <name>CreateSnippetCommand</name>
    <message>
        <location filename="../src/extensions/snippet/create-snippet-command.hpp" line="+10"/>
        <source>Create Snippet</source>
        <translation>Crear snippet</translation>
    </message>
</context>
<context>
    <name>DMenuSection</name>
    <message>
        <location filename="../src/qml/dmenu-model.cpp" line="+93"/>
        <source>Select entry</source>
        <translation>Seleccionar entrada</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pass search text</source>
        <translation>Enviar texto de búsqueda</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Select and copy entry</source>
        <translation>Seleccionar y copiar entrada</translation>
    </message>
</context>
<context>
    <name>DMenuView</name>
    <message>
        <location filename="../src/qml/qml/DMenuView.qml" line="+79"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Path</source>
        <translation>Ruta</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>Tipo</translation>
    </message>
</context>
<context>
    <name>DMenuViewHost</name>
    <message>
        <location filename="../src/qml/dmenu-view-host.cpp" line="+34"/>
        <source>Search entries...</source>
        <translation>Buscando entradas...</translation>
    </message>
    <message>
        <location line="+73"/>
        <source>Pass search text</source>
        <translation>Enviar texto de búsqueda</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Pass and copy search text</source>
        <translation>Enviar y copiar texto de búsqueda</translation>
    </message>
</context>
<context>
    <name>DetailListView</name>
    <message>
        <location filename="../src/qml/qml/DetailListView.qml" line="+32"/>
        <source>No results</source>
        <translation>No hay resultados</translation>
    </message>
</context>
<context>
    <name>DeveloperExtension</name>
    <message>
        <location filename="../src/extensions/developer/developer-extension.hpp" line="+10"/>
        <source>Developer</source>
        <translation>Desarrollador</translation>
    </message>
</context>
<context>
    <name>DisableApplication</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+8"/>
        <source>Disable item</source>
        <translation>Desactivar elemento</translation>
    </message>
</context>
<context>
    <name>DisableItemAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="+88"/>
        <source>Are you sure?</source>
        <translation>¿Estás seguro?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You will need to go in the settings to manually re-enable it.</source>
        <translation>Es necesarioreactivarlo manualmente en la configuración.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Disable</source>
        <translation>Desactivar</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Item disabled</source>
        <translation>Elemento desactivado</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to disable</source>
        <translation>No se pudo desactivar</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Disable item</source>
        <translation>Desactivar elemento</translation>
    </message>
</context>
<context>
    <name>DismissNewsAction</name>
    <message>
        <location filename="../src/services/news/news-service.cpp" line="+34"/>
        <source>Dismiss</source>
        <translation>Descartar</translation>
    </message>
</context>
<context>
    <name>DuplicateShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-50"/>
        <source>Duplicate link</source>
        <translation>Enlace duplicado</translation>
    </message>
</context>
<context>
    <name>EditClipboardKeywordsAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+49"/>
        <source>Additional keywords that will be used to index this selection.</source>
        <translation>Palabras clave adicionales que se utilizarán para indexar esta selección.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit keywords</source>
        <translation>Editar palabras clave</translation>
    </message>
</context>
<context>
    <name>EditEmojiKeywordsAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+35"/>
        <source>Additional keywords that will be used to index this glyph</source>
        <translation>Palabras clave adicionales que se utilizarán para indexar este glifo</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Edit keyword</source>
        <translation>Editar palabra clave</translation>
    </message>
</context>
<context>
    <name>EditKeywordsFormView</name>
    <message>
        <location filename="../src/qml/qml/EditKeywordsFormView.qml" line="+19"/>
        <source>Keywords</source>
        <translation>Palabras clave</translation>
    </message>
</context>
<context>
    <name>EditKeywordsViewHost</name>
    <message>
        <location filename="../src/qml/edit-keywords-view-host.cpp" line="+26"/>
        <source>Submit</source>
        <translation>Aceptar</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Keywords edited</source>
        <translation>Palabras clave editadas</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to edit keywords</source>
        <translation>No se pudieron editar las palabras clave</translation>
    </message>
</context>
<context>
    <name>EditShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-41"/>
        <source>Edit shortcut</source>
        <translation>Editar acceso directo</translation>
    </message>
</context>
<context>
    <name>EmojiCategoryFilterAccessory</name>
    <message>
        <location filename="../src/qml/qml/EmojiCategoryFilterAccessory.qml" line="+8"/>
        <source>All</source>
        <translation>Todos</translation>
    </message>
</context>
<context>
    <name>EmojiGridModel</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.hpp" line="+58"/>
        <source>Search for emojis and symbols...</source>
        <translation>Búsqueda de emojis y símbolos</translation>
    </message>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+241"/>
        <source>Pinned</source>
        <translation>Fijados</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Recently used</source>
        <translation>Usados recientemente</translation>
    </message>
</context>
<context>
    <name>EmojiGridViewHost</name>
    <message>
        <location filename="../src/qml/emoji-grid-view-host.hpp" line="+40"/>
        <source>All</source>
        <translation>Todos</translation>
    </message>
</context>
<context>
    <name>EmptyView</name>
    <message>
        <location filename="../src/qml/qml/EmptyView.qml" line="+7"/>
        <source>No results</source>
        <translation>No hay resultados</translation>
    </message>
</context>
<context>
    <name>EnabledFallbackSection</name>
    <message>
        <location filename="../src/qml/manage-fallback-model.hpp" line="-19"/>
        <source>Enabled</source>
        <translation>Activado</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-fallback-model.cpp" line="-22"/>
        <source>Disable fallback</source>
        <translation>Desactivar respaldo</translation>
    </message>
</context>
<context>
    <name>Expansion</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.hpp" line="+31"/>
        <source>Keyword cannot be empty</source>
        <translation>Las palabras clave no pueden estar vacía</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Keyword exceeds maximum length of %1</source>
        <translation>Las palabras clave exceden la longitud máxima de %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keyword must only contain printable ASCII characters (no spaces)</source>
        <translation>Las palabras clave deben contener solo caracteres ASCII imprimibles (sin espacios).</translation>
    </message>
</context>
<context>
    <name>ExtensionBoilerplateGenerator</name>
    <message>
        <location filename="../src/services/extension-boilerplate-generator/extension-boilerplate-generator.cpp" line="+24"/>
        <source>Simple List</source>
        <translation>Lista simple</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>List with Detail</source>
        <translation>Lista con detalles</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Controlled List</source>
        <translation>Lista controlada</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Simple Detail</source>
        <translation>Detalle simple</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>No View</source>
        <translation>No hay vista</translation>
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
        <translation># La extensión no responde 💥!

Esta extensión lanzó una excepción no controlada y no responde.

Se puede encontrar el stacktrace completo a continuación. También es posible copiarlo directamente desde el menú de acciones.

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
        <translation>Uno o más campos tienen errores</translation>
    </message>
</context>
<context>
    <name>ExtensionGridModel</name>
    <message>
        <location filename="../src/qml/extension-grid-model.cpp" line="+186"/>
        <source>Search...</source>
        <translation>Buscar...</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>No results</source>
        <translation>No hay resultados</translation>
    </message>
</context>
<context>
    <name>ExtensionGridView</name>
    <message>
        <location filename="../src/qml/qml/ExtensionGridView.qml" line="+9"/>
        <source>No results</source>
        <translation>No hay resultados</translation>
    </message>
</context>
<context>
    <name>ExtensionListModel</name>
    <message>
        <location filename="../src/qml/extension-list-model.cpp" line="+189"/>
        <source>Search...</source>
        <translation>Buscar...</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No results</source>
        <translation>No hay resultados</translation>
    </message>
</context>
<context>
    <name>ExtensionSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/ExtensionSettingsPage.qml" line="+94"/>
        <source>Description</source>
        <translation>Descripción</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Preferences</source>
        <translation>Preferencias</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Commands</source>
        <translation>Comandos</translation>
    </message>
    <message>
        <location line="+105"/>
        <source>Shortcut</source>
        <translation>Acceso directo</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Add Alias</source>
        <translation>Añadir Alias</translation>
    </message>
</context>
<context>
    <name>ExtensionView</name>
    <message>
        <location filename="../src/qml/qml/ExtensionView.qml" line="+99"/>
        <source>No results</source>
        <translation>No hay resultados</translation>
    </message>
</context>
<context>
    <name>FileExtension</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="+65"/>
        <source>System files</source>
        <translation>Archivos del sistema</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Integrate with system files</source>
        <translation>Integrar con los archivos del sistema</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Enabled</source>
        <translation>Activado</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Whether to run the file indexer in the background. When turned off, the indexer process is stopped entirely and file search becomes unavailable until it is turned back on.</source>
        <translation>Posibilitar la ejecución del indexador de archivos en segundo plano. Cuando está desactivado, el proceso del indexador se detiene por completo y la búsqueda de archivos no está disponible hasta que se active nuevamente.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Search paths</source>
        <translation>Buscar rutas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Directories that Vicinae will search</source>
        <translation>Directorios o carpetas que Vicinae buscará</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Excluded search paths</source>
        <translation>Rutas excluidas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Directories to exclude from file indexing</source>
        <translation>Directorios o carpetas que se excluirán de la indexación de archivos</translation>
    </message>
</context>
<context>
    <name>FilePreview</name>
    <message>
        <location filename="../src/qml/qml/FilePreview.qml" line="+42"/>
        <source>Preview not available for this file type</source>
        <translation>La vista previa no está disponible para este tipo de archivo</translation>
    </message>
</context>
<context>
    <name>FocusWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="-17"/>
        <source>Focus window</source>
        <translation>Enfocar ventana</translation>
    </message>
</context>
<context>
    <name>FontBrowserViewHost</name>
    <message>
        <location filename="../src/qml/font-browser-view-host.hpp" line="+38"/>
        <source>All</source>
        <translation>Todos</translation>
    </message>
</context>
<context>
    <name>FontExtension</name>
    <message>
        <location filename="../src/extensions/font/font-extension.hpp" line="+9"/>
        <source>Font</source>
        <translation>Fuente</translation>
    </message>
</context>
<context>
    <name>FontGridModel</name>
    <message>
        <location filename="../src/qml/font-grid-model.hpp" line="+47"/>
        <source>Search fonts...</source>
        <translation>Buscar fuentes...</translation>
    </message>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="+133"/>
        <source>All Fonts (%1)</source>
        <translation>Todas las fuentes (%1)</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Results (%1)</source>
        <translation>Resultados (%1)</translation>
    </message>
</context>
<context>
    <name>Footer</name>
    <message>
        <location filename="../src/qml/qml/Footer.qml" line="+60"/>
        <source>Actions</source>
        <translation>Acciones</translation>
    </message>
</context>
<context>
    <name>ForceQuitAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+95"/>
        <source>Force Quit Application</source>
        <translation>Forzar cierre de aplicación</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to force quit %1</source>
        <translation>El cierre de %1 no pudo ser forzado</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Force quit %1</source>
        <translation>Forzar cierre de %1</translation>
    </message>
</context>
<context>
    <name>ForgetTelemetryCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+210"/>
        <source>Forget Past Vicinae Telemetry</source>
        <translation>Olvidar la telemetría anterior de Vicinae</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Asks the vicinae server to anonymize telemetry data that was sent with your vicinae instance ID attached. The ID is only linked to your vicinae install, which has no direct relationship with your system.</source>
        <translation>Solicitar al servidor de Vicinae que anonimice los datos de telemetría que fueron enviados con el ID asociado a la instalación de Vicinae. El ID solo está vinculado a la instalación de Vicinae y no tiene relación directa con el sistema.</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Processing...</source>
        <translation>Procesando...</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Past telemetry was successfully detached from your vicinae user ID.</source>
        <translation>La telemetría anterior fue correctamente separada del ID de usuario de Vicinae.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to forget past telemetry data</source>
        <translation>No se pudieron eliminar los datos de telemetría anteriores</translation>
    </message>
</context>
<context>
    <name>FormAppSelector</name>
    <message>
        <location filename="../src/qml/qml/FormAppSelector.qml" line="+50"/>
        <source>All applications</source>
        <translation>Todas las aplicaciones</translation>
    </message>
    <message>
        <location line="+70"/>
        <source>+ Restrict to app…</source>
        <translation>+ Restringir a la aplicación…</translation>
    </message>
</context>
<context>
    <name>FormFilePicker</name>
    <message>
        <location filename="../src/qml/qml/FormFilePicker.qml" line="+84"/>
        <source>Select files</source>
        <translation>Seleccionar archivos</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Select a file</source>
        <translation>Seleccionar un archivo</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Select a directory</source>
        <translation>Seleccionar una carpeta</translation>
    </message>
    <message>
        <location line="+46"/>
        <source>No directory selected</source>
        <translation>No se ha seleccionado ninguna carpeta</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No file selected</source>
        <translation>No se ha seleccionado ningún archivo</translation>
    </message>
    <message>
        <location line="+115"/>
        <source>+ Add folder…</source>
        <translation>+ Agregar carpeta…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>+ Add file…</source>
        <translation>+ Agregar archivo…</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsModel</name>
    <message>
        <location filename="../src/qml/general-settings-model.cpp" line="+176"/>
        <location line="+11"/>
        <source>None</source>
        <translation>Ninguno</translation>
    </message>
    <message>
        <location line="-10"/>
        <location line="+10"/>
        <source>Blurred</source>
        <translation>Desenfocado</translation>
    </message>
    <message>
        <location line="-8"/>
        <location line="+8"/>
        <source>Liquid Glass</source>
        <translation>Liquid glass</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Window material</source>
        <translation>Textura de la ventana</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Themes</source>
        <translation>Temas</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Fonts</source>
        <translation>Fuentes</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Icon Themes</source>
        <translation>Paquetes de iconos</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Favicon Services</source>
        <translation>Servicios de Favicon</translation>
    </message>
    <message>
        <location line="+13"/>
        <location line="+10"/>
        <source>Default</source>
        <translation>Predeterminado</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Keybinding Schemes</source>
        <translation>Esquemas de combinación de teclas</translation>
    </message>
    <message>
        <location line="+48"/>
        <location line="+10"/>
        <source>System default</source>
        <translation>Predeterminado del sistema</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>Languages</source>
        <translation>Idiomas</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/GeneralSettingsPage.qml" line="+29"/>
        <source>Behavior</source>
        <translation>Comportamiento</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Launcher hotkey</source>
        <translation>Tecla de acceso rápido del lanzador</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Global shortcut to toggle the Vicinae launcher.</source>
        <translation>Atajo global para acceder al lanzador de Vicinae.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Close on focus loss</source>
        <translation>Cerrar al cambiar el foco</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Close on Escape</source>
        <translation>Cerrar al pulsar la tecla Esc.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pressing Escape closes the launcher instead of navigating one view back.</source>
        <translation>Pulsar la tecla Esc. cierra el lanzador en lugar de retroceder al menú anterior.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Pop to root on close</source>
        <translation>Regresar a la vista principal al cerrar</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reset the navigation state when the launcher window is closed.</source>
        <translation>Reiniciar el estado de navegación cuando se cierra la ventana del lanzador.</translation>
    </message>
    <message>
        <location line="+10"/>
        <location line="+7"/>
        <source>Language</source>
        <translation>Idioma</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Requires restarting Vicinae to take effect.</source>
        <translation>Es necesario reiniciar Vicinae para que los cambios surtan efecto.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Privacy</source>
        <translation>Privacidad</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Basic usage statistics</source>
        <translation>Estadísticas de uso básico</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Send basic system and vicinae installation information on startup to help improve Vicinae.</source>
        <translation>Enviar información básica del sistema y de la instalación de Vicinae al iniciar para ayudar a mejorar Vicinae.</translation>
    </message>
</context>
<context>
    <name>GenericGridView</name>
    <message>
        <location filename="../src/qml/qml/GenericGridView.qml" line="+33"/>
        <source>No results</source>
        <translation>No hay resultados</translation>
    </message>
</context>
<context>
    <name>GenericListView</name>
    <message>
        <location filename="../src/qml/qml/GenericListView.qml" line="+25"/>
        <source>No results</source>
        <translation>No hay resultados</translation>
    </message>
</context>
<context>
    <name>GlobalShortcutService</name>
    <message>
        <location filename="../src/services/global-shortcuts/global-shortcut-service.cpp" line="+43"/>
        <source>Toggle Vicinae</source>
        <translation>Mostrar/Ocultar Vicinae</translation>
    </message>
    <message>
        <location line="+84"/>
        <source>the launcher hotkey</source>
        <translation>la tecla de acceso rápido del lanzador</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>another command</source>
        <translation>otro comando</translation>
    </message>
</context>
<context>
    <name>Gnome::Workspace</name>
    <message>
        <location filename="../src/services/window-manager/gnome/gnome-workspace.cpp" line="+18"/>
        <source>Workspace %1</source>
        <translation>Área de trabajo %1</translation>
    </message>
</context>
<context>
    <name>HibernateCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+127"/>
        <source>Hibernate System</source>
        <translation>Hibernar el sistema</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Suspend the system to disk. This turns off the system completely and saves its state on disk, to be restored on next boot.</source>
        <translation>Suspender el sistema en disco. Esto apaga el sistema completamente y guarda su estado en disco, para ser restaurado en el próximo arranque.</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>System can&apos;t hibernate</source>
        <translation>El sistema puede hibernar</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to hibernate</source>
        <translation>No se ha podido hibernar</translation>
    </message>
</context>
<context>
    <name>IconBrowserCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+25"/>
        <source>Search Builtin Icons</source>
        <translation>Buscar iconos integrados</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search Vicinae builtin set of icons</source>
        <translation>Buscar en el conjunto de iconos integrados de Vicinae</translation>
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
        <translation>Inspeccionar el almacenamiento</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Browse data stored in Vicinae&apos;s local storage. This includes data stored for builtin extensions as well as third-party extensions making use of the LocalStorage API.</source>
        <translation>Explorar los datos almacenados en el almacenamiento local de Vicinae. Esto incluye los datos almacenados por las extensiones integradas así como las extensiones de terceros que utilicen la API de LocalStorage.</translation>
    </message>
</context>
<context>
    <name>InstallUpdateAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+188"/>
        <source>Install Update</source>
        <translation>Instalar actualización</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>An update is already in progress</source>
        <translation>Ya hay una actualización en curso</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsSection</name>
    <message>
        <location filename="../src/qml/installed-extensions-model.hpp" line="+18"/>
        <source>Installed Extensions ({count})</source>
        <translation>({count}) extensiones instaladas</translation>
    </message>
    <message>
        <location filename="../src/qml/installed-extensions-model.cpp" line="+36"/>
        <source>Local</source>
        <translation>Local</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Copy</source>
        <translation>Copiar</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy Name</source>
        <translation>Copiar nombre</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy ID</source>
        <translation>Copiar ID</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy Path</source>
        <translation>Copiar ruta</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy Author</source>
        <translation>Copiar autor</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsViewHost</name>
    <message>
        <location filename="../src/qml/installed-extensions-view-host.cpp" line="+12"/>
        <source>Search extensions...</source>
        <translation>Buscar extensiones...</translation>
    </message>
</context>
<context>
    <name>InternalExtension</name>
    <message>
        <location filename="../src/extensions/internal/internal-extension.hpp" line="+32"/>
        <location line="+1"/>
        <source>Internal Commands</source>
        <translation>Comandos internos</translation>
    </message>
</context>
<context>
    <name>KeyboardBridge</name>
    <message>
        <location filename="../src/qml/keyboard-bridge.hpp" line="+51"/>
        <source>Modifier required</source>
        <translation>Es necesario un modificador</translation>
    </message>
</context>
<context>
    <name>LauncherWindow</name>
    <message>
        <location filename="../src/qml/qml/LauncherWindow.qml" line="+29"/>
        <source>Vicinae Launcher</source>
        <translation>Lanzador de Vicinae</translation>
    </message>
    <message>
        <location filename="../src/qml/launcher-window.cpp" line="+571"/>
        <source>Open Settings</source>
        <translation>Abrir configuración</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keyboard Shortcuts</source>
        <translation>Atajos de teclado</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Documentation</source>
        <translation>Documentación</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+5"/>
        <source>Opened in browser</source>
        <translation>Abierto en el navegador</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Report a Bug</source>
        <translation>Reportar un error</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>About Vicinae</source>
        <translation>Sobre Vicinae</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemSection</name>
    <message>
        <location filename="../src/qml/local-storage-model.hpp" line="+19"/>
        <source>Items ({count})</source>
        <translation>({count}) elementos</translation>
    </message>
    <message>
        <location filename="../src/qml/local-storage-model.cpp" line="+31"/>
        <source>Show value</source>
        <translation>Mostrar valor</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemViewHost</name>
    <message>
        <location filename="../src/qml/local-storage-view-host.cpp" line="+23"/>
        <source>Search items...</source>
        <translation>Buscar elementos...</translation>
    </message>
</context>
<context>
    <name>LocalStorageNamespaceSection</name>
    <message>
        <location filename="../src/qml/local-storage-model.hpp" line="-11"/>
        <source>Namespaces ({count})</source>
        <translation> ({count}) namespaces</translation>
    </message>
    <message>
        <location filename="../src/qml/local-storage-model.cpp" line="-16"/>
        <source>Browse namespace</source>
        <translation>Buscar namespace</translation>
    </message>
</context>
<context>
    <name>LocalStorageViewHost</name>
    <message>
        <location filename="../src/qml/local-storage-view-host.cpp" line="-15"/>
        <source>Search namespaces...</source>
        <translation>Buscar namespaces...</translation>
    </message>
</context>
<context>
    <name>LockCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-48"/>
        <source>Lock Session</source>
        <translation>Bloquear sesión</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lock the current user session</source>
        <translation>Bloquear la sesión del usuario</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System can&apos;t lock</source>
        <translation>No se puede bloquear el sistema</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to lock</source>
        <translation>No se ha podido bloquear</translation>
    </message>
</context>
<context>
    <name>LogOutCommand</name>
    <message>
        <location line="+176"/>
        <source>Log Out</source>
        <translation>Cerrar sesión</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Terminate the current user session. If you simply want to lock your session you should use &apos;Lock Session&apos; instead.</source>
        <translation>Cerrar la sesión actual. Si solo quieres bloquear la sesión, se debe usar &quot;Bloquear sesión&quot; en su lugar.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>System can&apos;t logout</source>
        <translation>No se puede cerrar la sesión</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to log out</source>
        <translation>No se ha podido cerrar la sesión</translation>
    </message>
</context>
<context>
    <name>MacOSGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/macos-global-shortcut-backend.cpp" line="+238"/>
        <source>unsupported or invalid trigger</source>
        <translation>Condición no soportada o inválida</translation>
    </message>
</context>
<context>
    <name>MacSettingsRootItem</name>
    <message>
        <location filename="../src/root-search/macos-settings/macos-settings-root-provider.mm" line="+132"/>
        <location line="+9"/>
        <source>System Settings</source>
        <translation>Ajustes del sistema</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Bundle ID</source>
        <translation>ID del paquete</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Legacy ID</source>
        <translation>ID heredado</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Where</source>
        <translation>Dónde</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Open %1 Settings</source>
        <translation>Abrir ajustes de %1</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy URL</source>
        <translation>Copiar URL</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy Bundle ID</source>
        <translation>Copiar ID del paquete</translation>
    </message>
</context>
<context>
    <name>MacSettingsRootProvider</name>
    <message>
        <location line="+13"/>
        <source>System Settings</source>
        <translation>Ajustes del sistema</translation>
    </message>
</context>
<context>
    <name>MacosUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/macos-update-installer.mm" line="+208"/>
        <source>This installation cannot update itself</source>
        <translation>La instalación no puede autoactualizarse</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Mounting update image…</source>
        <translation>Montando imagen de actualización…</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Failed to mount the update image</source>
        <translation>No se ha podido montar la imagen de actualización</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Could not find the update image mount point</source>
        <translation>No se ha podido encontrar el punto de montaje de la imagen de actualización</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Verifying update…</source>
        <translation>Comprobando actualización…</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Installing update…</source>
        <translation>Instalando actualización…</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to stage update: %1</source>
        <translation>No se ha podido preparar la actualización: %1</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Failed to move the current app aside: %1</source>
        <translation>No se ha podido mover la aplicación actual: %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to install the new app: %1</source>
        <translation>No se ha podido instalar la nueva aplicación: %1</translation>
    </message>
</context>
<context>
    <name>ManageFallbackActions</name>
    <message>
        <location filename="../src/actions/fallback-actions.hpp" line="+15"/>
        <source>Manage Fallback Actions</source>
        <translation>Gestionar acciones de respaldo</translation>
    </message>
</context>
<context>
    <name>ManageFallbackCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/configure-fallback-command.hpp" line="+11"/>
        <source>Configure Fallback Commands</source>
        <translation>Configurar comandos de respaldo</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Configure what commands are to be presented as fallback options when nothing matches the search in the root search.</source>
        <translation>Configurar qué comandos se deben presentar como opciones de respaldo cuando nada coincide con la búsqueda en la búsqueda raíz.</translation>
    </message>
</context>
<context>
    <name>ManageFallbackViewHost</name>
    <message>
        <location filename="../src/qml/manage-fallback-view-host.cpp" line="+12"/>
        <source>Search commands...</source>
        <translation>Buscar comandos...</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsCommand</name>
    <message>
        <location filename="../src/extensions/shortcut/manage-shortcuts-command.hpp" line="+9"/>
        <source>Manage Shortcuts</source>
        <translation>Gestionar accesos directos</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsSection</name>
    <message>
        <location filename="../src/qml/manage-shortcuts-model.hpp" line="+17"/>
        <source>Shortcuts ({count})</source>
        <translation>({count}) accesos directos</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsViewHost</name>
    <message>
        <location filename="../src/qml/manage-shortcuts-view-host.cpp" line="+28"/>
        <source>Search shortcuts...</source>
        <translation>Buscar accesos directos...</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Application</source>
        <translation>Aplicación</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Opened</source>
        <translation>Abierto</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Last Opened</source>
        <translation>Abierto por última vez</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Never</source>
        <translation>Nunca</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Created at</source>
        <translation>Creado a las</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsCommand</name>
    <message>
        <location filename="../src/extensions/snippet/manage-snippets-command.hpp" line="+10"/>
        <source>Manage Snippets</source>
        <translation>Gestionar snippets</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsSection</name>
    <message>
        <location filename="../src/qml/manage-snippets-model.hpp" line="+18"/>
        <source>Snippets ({count})</source>
        <translation>({count}) snippets</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-snippets-model.cpp" line="+32"/>
        <source>Copy to clipboard</source>
        <translation>Copiar al portapapeles</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copied to clipboard</source>
        <translation>Copiado al portapapeles</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy to clipboard</source>
        <translation>No se ha podido copiar al portapapeles</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Edit snippet</source>
        <translation>Editar snippet</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Duplicate snippet</source>
        <translation>Duplicar snippet</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Remove snippet</source>
        <translation>Eliminar snippet</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove snippet</source>
        <translation>No se ha podido eliminar el snippet</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsViewHost</name>
    <message>
        <location filename="../src/qml/manage-snippets-view-host.hpp" line="+54"/>
        <source>No snippets</source>
        <translation>No hay snippets</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Create a snippet to get started</source>
        <translation>Crear un snippet para empezar</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-snippets-view-host.cpp" line="+28"/>
        <source>Search for snippets...</source>
        <translation>Buscar snippets...</translation>
    </message>
    <message>
        <location line="+25"/>
        <source>Text</source>
        <translation>Texto</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>File</source>
        <translation>Archivo</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation> Tipo</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Created at</source>
        <translation>Creado el</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Updated at</source>
        <translation>Actualizado el</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Keyword</source>
        <translation>Palabra clave</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Apps</source>
        <translation>Aplicaciones</translation>
    </message>
    <message>
        <location line="+68"/>
        <source>Create snippet</source>
        <translation>Crear snippet</translation>
    </message>
</context>
<context>
    <name>MarkItemAsFavorite</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="-60"/>
        <source>Mark as favorite</source>
        <translation>Marcar como favorito</translation>
    </message>
</context>
<context>
    <name>MarkdownShowcase</name>
    <message>
        <location filename="../src/extensions/internal/markdown-showcase-command.hpp" line="+172"/>
        <source>Markdown Showcase</source>
        <translation>Ejemplo de Markdown</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Preview all supported markdown features</source>
        <translation>Previsualizar todas las funciones de markdown soportadas</translation>
    </message>
</context>
<context>
    <name>MarkdownView</name>
    <message>
        <location filename="../src/qml/qml/markdown/MarkdownView.qml" line="+265"/>
        <source>Copy</source>
        <translation>Copiar</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Select All</source>
        <translation>Seleccionar todo</translation>
    </message>
</context>
<context>
    <name>MdCallout</name>
    <message>
        <location filename="../src/qml/qml/markdown/MdCallout.qml" line="+35"/>
        <source>Caution</source>
        <translation>Atención</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Warning</source>
        <translation>Advertencia</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Important</source>
        <translation>Importante</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tip</source>
        <translation>Consejo</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Note</source>
        <translation>Nota</translation>
    </message>
</context>
<context>
    <name>MdCodeBlock</name>
    <message>
        <location filename="../src/qml/qml/markdown/MdCodeBlock.qml" line="+58"/>
        <source>Copied!</source>
        <translation>¡Copiado!</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Copy</source>
        <translation>Copiar</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceView</name>
    <message>
        <location filename="../src/qml/qml/MissingPreferenceView.qml" line="+28"/>
        <source>Welcome to %1</source>
        <translation>Bienvenido a %1</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Before you can use this command, you need to fill in the required preference fields below.</source>
        <translation>Antes de usar este comando, es necesario rellenar los campos de preferencia detallados a continuación.</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceViewHost</name>
    <message>
        <location filename="../src/qml/missing-preference-view-host.cpp" line="+202"/>
        <source>Save preferences</source>
        <translation>Guardar preferencias</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Please fill in all required fields</source>
        <translation>Es necesario rellenar todos los campos obligatorios</translation>
    </message>
</context>
<context>
    <name>NavigationController</name>
    <message>
        <location filename="../src/navigation-controller.cpp" line="+654"/>
        <source>Extension manager is not running</source>
        <translation>El gestor de extensiones no está ejecutándose</translation>
    </message>
</context>
<context>
    <name>NewsService</name>
    <message>
        <location filename="../src/services/news/news-service.cpp" line="+68"/>
        <source>Telemetry</source>
        <translation>Telemetría</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>We now collect basic usage statistics on startup</source>
        <translation>Ahora se recopilan estadísticas básicas de uso al iniciar</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Learn more</source>
        <translation>Más información</translation>
    </message>
</context>
<context>
    <name>NullUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/null-update-installer.hpp" line="+14"/>
        <source>Self update is not supported on this platform</source>
        <translation>La actualización automática no es compatible con esta plataforma</translation>
    </message>
</context>
<context>
    <name>OAuthOverlayView</name>
    <message>
        <location filename="../src/qml/qml/OAuthOverlayView.qml" line="+91"/>
        <source>Continue with %1</source>
        <translation>Continuar con %1</translation>
    </message>
    <message>
        <location line="+33"/>
        <source>You&apos;re in!</source>
        <translation>¡Bienvenido!</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Successfully connected to %1.
Back to command in an instant...</source>
        <translation>Conectado correctamente con %1.
Volviendo al comando en un instante...</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-31"/>
        <source>Manage OAuth Token Sets</source>
        <translation>Administrar conjunto de tokens OAuth</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Manage OAuth token sets that have been saved by extensions providing OAuth integrations.</source>
        <translation>Administre los conjuntos de tokens OAuth almacenados por extensiones que proporcionan integraciones OAuth.</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreSection</name>
    <message>
        <location filename="../src/qml/oauth-token-store-model.hpp" line="+17"/>
        <source>OAuth Token Sets ({count})</source>
        <translation>({count}) conjuntos de tokens OAuth</translation>
    </message>
    <message>
        <location filename="../src/qml/oauth-token-store-model.cpp" line="+20"/>
        <source>Expired</source>
        <translation>Expirado</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Remove token set</source>
        <translation>Eliminar conjunto de tokens</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Are you sure?</source>
        <translation>¿Estás seguro?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You will need to go through the OAuth login flow again the next time you want to use this service</source>
        <translation>La próxima vez que se use este servicio, será necesario pasar por el flujo de inicio de sesión de OAuth nuevamente</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to remove token set</source>
        <translation>No se pudo eliminar el conjunto de tokens</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Token set removed</source>
        <translation>Conjunto de tokens eliminado</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy</source>
        <translation>Copiar</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy Access Token</source>
        <translation>Copiar token de acceso</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Refresh Token</source>
        <translation>Copiar token de actualización</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy ID Token</source>
        <translation>Copiar ID del token</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy Scopes</source>
        <translation>Copiar ámbitos</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Expiration Date</source>
        <translation>Copiar fecha de expiración</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreViewHost</name>
    <message>
        <location filename="../src/qml/oauth-token-store-view-host.cpp" line="+12"/>
        <source>Search token sets...</source>
        <translation>Buscar conjunto de tokens...</translation>
    </message>
</context>
<context>
    <name>OnboardingWindow</name>
    <message>
        <location filename="../src/qml/qml/OnboardingWindow.qml" line="+39"/>
        <source>Grant Access</source>
        <translation>Conceder acceso</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Granted</source>
        <translation>Permitido</translation>
    </message>
    <message>
        <location line="+16"/>
        <location line="+39"/>
        <source>Welcome to Vicinae</source>
        <translation>Bienvenido a Vicinae</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Let&apos;s set it up. It only takes a minute.</source>
        <translation>Comencemos. Solo toma un minuto.</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Permissions</source>
        <translation>Permisos</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Vicinae needs additional permissions in order to make the best of your Mac.</source>
        <translation>Vicinae necesita permisos adicionales para poder sacar el máximo provecho de su equipo.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Accessibility</source>
        <translation>Accesibilidad</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Used to paste, expand snippets, and move windows.</source>
        <translation>Necesario para pegar, expandir fragmentos y mover ventanas.</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Full Disk Access</source>
        <translation>Acceso completo al almacenamiento</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Notifications</source>
        <translation>Notificaciones</translation>
    </message>
    <message>
        <location line="-8"/>
        <source>Allows file search to cover your entire disk.</source>
        <translation>Permite que la búsqueda de archivos acceda a todo el almacenamiento.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Allows extensions to send desktop notifications.</source>
        <translation>Permite que las extensiones envíen notificaciones de escritorio.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Accessibility is required: global shortcuts, paste, and snippet expansion cannot work without it.</source>
        <translation>La accesibilidad es necesaria: los atajos globales, el pegado y la expansión de fragmentos no pueden funcionar sin ella.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Full disk access needs to be explicitly enabled if you want file search to cover all your files.</source>
        <translation>Para que la búsqueda de archivos cubra todos los archivos, es necesario habilitar explícitamente el acceso al almacenamiento.</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Make it your own</source>
        <translation>Personaliza la experiencia</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>You will be able to change these settings later.</source>
        <translation>Es posible cambiar estas configuraciones más adelante.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Theme</source>
        <translation>Diseño</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Shared across the entire app.</source>
        <translation>Compartido por toda la aplicación.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Global hotkey</source>
        <translation>Atajo global</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Opens the launcher from anywhere.</source>
        <translation>Abre el lanzador desde cualquier lugar.</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Launch at login</source>
        <translation>Iniciar al arrancar</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Starts Vicinae in the background at login.</source>
        <translation>Empieza Vicinae en segundo plano al iniciar sesión.</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Setup complete</source>
        <translation>Configuración inicial completada</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Vicinae is running. Open the launcher with:</source>
        <translation>Vicinae se está ejecutando. Se puede abrir el lanzador con:</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Vicinae is open source software.</source>
        <translation>Vicinae es software de código abierto.</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Sponsor</source>
        <translation>Patrocinador</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Back</source>
        <translation>Volver</translation>
    </message>
    <message>
        <location line="+44"/>
        <source>Finish</source>
        <translation>Finalizar</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Continue</source>
        <translation>Continuar</translation>
    </message>
</context>
<context>
    <name>OpenAboutCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/open-about-command.hpp" line="+13"/>
        <source>About</source>
        <translation>Acerca de</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the &quot;About&quot; tab of the vicinae settings.</source>
        <translation>Abre la pestaña &quot;Acerca de&quot; de las configuraciones de Vicinae.</translation>
    </message>
</context>
<context>
    <name>OpenAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-56"/>
        <source>Failed to start app</source>
        <translation>No se ha podido iniciar la aplicación</translation>
    </message>
</context>
<context>
    <name>OpenAppLocationAction</name>
    <message>
        <location line="-36"/>
        <source>Open Location</source>
        <translation>Abrir ubicación</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to open app location</source>
        <translation>No se ha podido abrir la ubicación de la aplicación</translation>
    </message>
</context>
<context>
    <name>OpenBuiltinCommandAction</name>
    <message>
        <location filename="../src/command-actions.hpp" line="+17"/>
        <source>Open command</source>
        <translation>Ejecutar comando</translation>
    </message>
</context>
<context>
    <name>OpenCalculatorHistoryAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+9"/>
        <source>Open Calculator History</source>
        <translation>Abrir historial de la calculadora</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-32"/>
        <source>Open shortcut</source>
        <translation>Ejecutar acceso directo</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutWithAction</name>
    <message>
        <location line="+85"/>
        <source>Open with...</source>
        <translation>Abrir con...</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelItemAction</name>
    <message>
        <location filename="../src/root-search/control-panel/control-panel-root-provider.cpp" line="+45"/>
        <source>Failed to open settings</source>
        <translation>No se han podido abrir las configuraciones</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelTaskAction</name>
    <message>
        <location line="+26"/>
        <source>Failed to open settings</source>
        <translation>No se han podido abrir las configuraciones</translation>
    </message>
</context>
<context>
    <name>OpenDefaultVicinaeConfig</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-157"/>
        <source>Open Default Config File</source>
        <translation>Abrir archivo de configuración predeterminado</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the default vicinae configuration file</source>
        <translation>Abrir el archivo de configuración predeterminado de Vicinae</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Failed to open temporary file</source>
        <translation>No se ha podido abrir el archivo temporal</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Failed to open default config file</source>
        <translation>No se ha podido abrir el archivo de configuración predeterminado</translation>
    </message>
</context>
<context>
    <name>OpenDiscordCommand</name>
    <message>
        <location line="-66"/>
        <source>Join the Discord Server</source>
        <translation>Unirse al servidor de Discord</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open link to join the official Vicinae discord server.</source>
        <translation>Abrir enlace para unirse al servidor oficial de Discord de Vicinae.</translation>
    </message>
</context>
<context>
    <name>OpenDocumentationCommand</name>
    <message>
        <location line="-16"/>
        <source>Open Online Documentation</source>
        <translation>Abrir documentación en línea</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Navigate to the official vicinae documentation website.</source>
        <translation>Navegar a la página web oficial de documentación de Vicinae.</translation>
    </message>
</context>
<context>
    <name>OpenFileAction</name>
    <message>
        <location filename="../src/actions/files/file-actions.hpp" line="+18"/>
        <source>Open with %1</source>
        <translation>Abrir con %1</translation>
    </message>
</context>
<context>
    <name>OpenInBrowserAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="+105"/>
        <source>Open in browser</source>
        <translation>Abrir en el navegador</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+95"/>
        <source>Failed to open in browser</source>
        <translation>No se ha podido abrir en el navegador</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Opened in browser</source>
        <translation>Se ha abierto en el navegador</translation>
    </message>
</context>
<context>
    <name>OpenInTerminalAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="-62"/>
        <source>Open in %1</source>
        <translation>Abrir en %1</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-85"/>
        <source>Failed to start app</source>
        <translation>No se ha podido iniciar la aplicación</translation>
    </message>
</context>
<context>
    <name>OpenItemPreferencesAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="-28"/>
        <source>Open Preferences</source>
        <translation>Abrir Preferencias</translation>
    </message>
</context>
<context>
    <name>OpenKeybindSettingsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+156"/>
        <source>Open Vicinae Keybind Settings</source>
        <translation>Abrir configuración de combinaciones de teclas de Vicinae</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the vicinae keybind settings window</source>
        <translation>Abrir la ventana de configuración de combinaciones de teclas de Vicinae</translation>
    </message>
</context>
<context>
    <name>OpenRawProgramAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="+25"/>
        <source>Execute program</source>
        <translation>Ejecutar programa</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+35"/>
        <source>Failed to start app</source>
        <translation>No se ha podido iniciar la aplicación</translation>
    </message>
</context>
<context>
    <name>OpenSettingsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-40"/>
        <source>Open Vicinae Settings</source>
        <translation>Abrir configuración de Vicinae</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open the vicinae settings window, which is an independent floating window.</source>
        <translation>Abrir la ventana de configuración de Vicinae, que es una ventana flotante independiente.</translation>
    </message>
</context>
<context>
    <name>OpenSettingsPaneAction</name>
    <message>
        <location filename="../src/root-search/macos-settings/macos-settings-root-provider.mm" line="-135"/>
        <source>Failed to open System Settings</source>
        <translation>No se han podido abrir las configuraciones del sistema</translation>
    </message>
</context>
<context>
    <name>OpenShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-128"/>
        <source>No app with id %1</source>
        <translation>No hay ninguna aplicación con el ID %1</translation>
    </message>
    <message>
        <location line="+9"/>
        <location line="+7"/>
        <source>Open shortcut</source>
        <translation>Abrir acceso directo</translation>
    </message>
</context>
<context>
    <name>OpenShortcutFromSearchText</name>
    <message>
        <location line="+43"/>
        <source>Open shortcut</source>
        <translation>Abrir acceso directo</translation>
    </message>
</context>
<context>
    <name>OpenVicinaeConfig</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-81"/>
        <source>Open Config File</source>
        <translation>Abrir archivo de configuración</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the main vicinae configuration file</source>
        <translation>Abrir el archivo de configuración principal de Vicinae</translation>
    </message>
</context>
<context>
    <name>OpenWindowsSettingAction</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="+147"/>
        <source>Failed to open settings</source>
        <translation>No se ha podido abrir la configuración</translation>
    </message>
</context>
<context>
    <name>OpenWithAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+54"/>
        <source>Open with...</source>
        <translation>Abrir con...</translation>
    </message>
</context>
<context>
    <name>PasteToFocusedWindowAction</name>
    <message>
        <location filename="../src/clipboard-actions.hpp" line="+11"/>
        <source>Paste to active window</source>
        <translation>Pegar en la ventana activa</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy to focused window</source>
        <translation>Copiar a la ventana activa</translation>
    </message>
</context>
<context>
    <name>PinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+28"/>
        <source>Entry pinned</source>
        <translation>Entrada fijada</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pin entry</source>
        <translation>Fijar entrada</translation>
    </message>
</context>
<context>
    <name>PinClipboardAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="-26"/>
        <source>Selection pinned</source>
        <translation>Selección fijada</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Selection unpinned</source>
        <translation>Selección desfijada</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to change pin status</source>
        <translation>No se ha podido fijar/desfijar</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Pin</source>
        <translation>Fijar</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unpin</source>
        <translation>Desfijar</translation>
    </message>
</context>
<context>
    <name>PinEmojiAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-322"/>
        <source>Pin emoji</source>
        <translation>Fijar emoticono</translation>
    </message>
</context>
<context>
    <name>PinWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="+37"/>
        <source>Unpin from all workspaces</source>
        <translation>Desfijar de todos los espacios de trabajo</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Pin to all workspaces</source>
        <translation>Fijar en todos los espacios de trabajo</translation>
    </message>
</context>
<context>
    <name>PowerManagementCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-283"/>
        <source>Ask for confirmation</source>
        <translation>Pedir confirmación</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Custom program</source>
        <translation>Programa personalizado</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Custom POSIX shell command to run instead of the default implementation</source>
        <translation>Comando de shell POSIX personalizado para ejecutar en lugar de la implementación por defecto</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Failed to execute custom program %1</source>
        <translation>El programa personalizado %1 no se pudo ejecutar</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Are you sure</source>
        <translation>¿Estás seguro?</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>High-impact operation, please confirm</source>
        <translation>Operación con alto impacto. Es necesario confirmar.</translation>
    </message>
</context>
<context>
    <name>PowerManagementExtension</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.hpp" line="+8"/>
        <source>Power Management</source>
        <translation>Gestión de energía</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power off, suspend, sleep, hibernate your computer.</source>
        <translation>Apagar, suspender, dormir o hibernar el equipo.</translation>
    </message>
</context>
<context>
    <name>PowerOffCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+137"/>
        <source>Power Off System</source>
        <translation>Apagar sistema</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power off the system</source>
        <translation>Apagar el sistema</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>System cannot power off</source>
        <translation>El sistema no se puede apagar</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to power off</source>
        <translation>No se ha podido apagar</translation>
    </message>
</context>
<context>
    <name>PreviewFontAction</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="-105"/>
        <source>Preview font</source>
        <translation>Previsualizar fuente</translation>
    </message>
</context>
<context>
    <name>ProgramsSection</name>
    <message>
        <location filename="../src/qml/system-run-model.hpp" line="+21"/>
        <source>Programs (%1)</source>
        <translation>(%1) programas</translation>
    </message>
    <message>
        <location filename="../src/qml/system-run-model.cpp" line="+52"/>
        <source>Open in %1 (hold)</source>
        <translation>Abrir en %1 (mantener presionado)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open in %1</source>
        <translation>Abrir en %1</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Copy exec path</source>
        <translation>Copiar ruta del ejecutable</translation>
    </message>
</context>
<context>
    <name>ProviderSearchSection</name>
    <message>
        <location filename="../src/qml/provider-search-model.hpp" line="+11"/>
        <source>Results ({count})</source>
        <translation>({count}) resultados</translation>
    </message>
</context>
<context>
    <name>ProviderSearchViewHost</name>
    <message>
        <location filename="../src/qml/provider-search-view-host.cpp" line="+15"/>
        <source>Search %1</source>
        <translation>Buscar %1</translation>
    </message>
</context>
<context>
    <name>PruneMemoryCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+56"/>
        <source>Prune Vicinae Memory Usage</source>
        <translation>Reducir el uso de memoria de Vicinae</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Try pruning vicinae&apos;s memory usage by clearing pixmap cache and calling malloc_trim(). Mostly provided for internal testing.</source>
        <translation>Intentar reducir el uso de memoria de Vicinae limpiando la caché de mapas de bits y llamando a malloc_trim(). Destinado principalmente a pruebas internas.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Pruned 🥊</source>
        <translation>Reducido 🥊</translation>
    </message>
</context>
<context>
    <name>PutCalculatorAnswerInSearchBar</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-20"/>
        <source>Put answer in search bar</source>
        <translation>Introducir respuesta en la barra de búsqueda</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/utils/utils.cpp" line="+50"/>
        <source>in the future</source>
        <translation>en el futuro</translation>
    </message>
    <message numerus="yes">
        <location line="+8"/>
        <source>%n year(s) ago</source>
        <translation>
            <numerusform>hace %n año</numerusform>
            <numerusform>hace %n años</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+3"/>
        <source>%n month(s) ago</source>
        <translation>
            <numerusform>hace %n mes</numerusform>
            <numerusform>hace %n meses</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n day(s) ago</source>
        <translation>
            <numerusform>hace %n día</numerusform>
            <numerusform>hace %n días</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n hour(s) ago</source>
        <translation>
            <numerusform>hace %n hora</numerusform>
            <numerusform>hace %n horas</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n minute(s) ago</source>
        <translation>
            <numerusform>hace %n minuto</numerusform>
            <numerusform>hace %n minutos</numerusform>
        </translation>
    </message>
    <message>
        <location line="+2"/>
        <source>just now</source>
        <translation>ahora mismo</translation>
    </message>
</context>
<context>
    <name>QuitAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-43"/>
        <source>Quit Application</source>
        <translation>Cerrar aplicación</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to quit %1</source>
        <translation>No se ha podido cerrar %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Quit %1</source>
        <translation>Cerrar %1</translation>
    </message>
</context>
<context>
    <name>RaycastCompatExtension</name>
    <message>
        <location filename="../src/extensions/raycast/raycast-compat-extension.hpp" line="+15"/>
        <source>Raycast compatibility features</source>
        <translation>Compatibilidad con Raycast</translation>
    </message>
</context>
<context>
    <name>RaycastStoreCommand</name>
    <message>
        <location filename="../src/extensions/raycast/raycast-store-command.hpp" line="+13"/>
        <source>Install compatible extensions from the Raycast store</source>
        <translation>Instalar extensiones compatibles desde el almacén de Raycast</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Always show intro</source>
        <translation>Mostrar siempre la introducción</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>
# Welcome to the Raycast Extension Store

Vicinae provides direct integration with the official [Raycast store](https://www.raycast.com/store), allowing you to search and install Raycast extensions directly from Vicinae.
</source>
        <translation>
# Bienvenido al catálogo de extensiones de Raycast

Vicinae ofrece una integración directa con el [catálogo oficial de Raycast](https://www.raycast.com/store), lo que te permite buscar y instalar extensiones de Raycast directamente desde Vicinae.
</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>
Each extension has a colored compatibility indicator showing how well it works on Linux.

Vicinae also has its own [extension store](vicinae://launch/core/store), which does not suffer from these limitations.
</source>
        <translation>
Cada extensión tiene un indicador de compatibilidad en forma de color que muestra cómo funciona bien en Linux.

Vicinae también tiene su propio [catálogo de extensiones](vicinae://launch/core/store), que no sufre de estas limitaciones.
</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>
Vicinae also has its own [extension store](vicinae://launch/core/store).
</source>
        <translation>
Vicinae también tiene su propio [catálogo de extensiones](vicinae://launch/core/store).
</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Continue to store</source>
        <translation>Continuar a la tienda</translation>
    </message>
</context>
<context>
    <name>RaycastStoreDetailHost</name>
    <message>
        <location filename="../src/qml/raycast-store-detail-host.cpp" line="+43"/>
        <source>Failed to load extension</source>
        <translation>No se ha podido cargar la extensión</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The extension &quot;%1&quot; could not be loaded. It may not exist or the store may be unreachable.</source>
        <translation>La extensión &quot;%1&quot; no se ha podido cargar. Puede que no exista o que no se haya podido establecer conexión con la tienda.</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Extension Store - %1</source>
        <translation>Catálogo de extensiones – %1</translation>
    </message>
    <message>
        <location line="+31"/>
        <source>This extension should be fully compatible.</source>
        <translation>Esta extensión debería ser completamente compatible.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>This extension works but has a few quirks.</source>
        <translation>Esta extensión funciona pero tiene algunos problemas.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>This extension is not compatible.</source>
        <translation>Esta extensión no es compatible.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No compatibility data is available for this extension.</source>
        <translation>No hay datos de compatibilidad disponibles para esta extensión.</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>No compatibility data is available — this extension may or may not work.</source>
        <translation>No hay datos de compatibilidad disponibles — esta extensión puede o no funcionar.</translation>
    </message>
    <message>
        <location line="+83"/>
        <source>Install extension</source>
        <translation>Instalar extensión</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading extension...</source>
        <translation>Descargando extensión...</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to download extension</source>
        <translation>La extensión no se ha podido descargar</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to extract extension archive</source>
        <translation>No se ha podido extraer el archivo de la extensión</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Extension installed</source>
        <translation>Extensión instalada</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Report issue</source>
        <translation>Informar de un problema</translation>
    </message>
</context>
<context>
    <name>RaycastStoreSection</name>
    <message>
        <location filename="../src/qml/raycast-store-model.cpp" line="+45"/>
        <source>Show details</source>
        <translation>Mostrar detalles</translation>
    </message>
</context>
<context>
    <name>RaycastStoreViewHost</name>
    <message>
        <location filename="../src/qml/raycast-store-view-host.cpp" line="+37"/>
        <source>Browse Raycast extensions</source>
        <translation>Explorar extensiones de Raycast</translation>
    </message>
    <message>
        <location line="+32"/>
        <source>Failed to fetch extensions</source>
        <translation>No se ha podido obtener las extensiones</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Extensions</source>
        <translation>Extensiones</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to search extensions</source>
        <translation>No se han podido buscar las extensiones</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Results</source>
        <translation>Resultados</translation>
    </message>
</context>
<context>
    <name>RebootCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-72"/>
        <source>Reboot System</source>
        <translation>Reiniciar sistema</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reboot the system</source>
        <translation>Reiniciar el sistema</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>System can&apos;t reboot</source>
        <translation>El sistema no puede reiniciarse</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to reboot</source>
        <translation>No se ha podido reiniciar</translation>
    </message>
</context>
<context>
    <name>RebuildFileIndexCommand</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="-72"/>
        <source>Rebuild File Index</source>
        <translation>Reconstruir índice de archivos</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Fully rebuild the file index. Running this manually can be useful if the file search feels particularly out of date.</source>
        <translation>Reconstruye completamente el índice de archivos. Ejecutar esto manualmente puede ser útil si la búsqueda de archivos se siente desactualizada.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Are you sure?</source>
        <translation>¿Estás seguro?</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Rebuilding the entire index can be time consuming and CPU intensive, depending on the number of files present in your home directory.</source>
        <translation>Reconstruir el índice completo puede requerir tiempo y ser exigente en términos de uso de CPU, dependiendo de la cantidad de archivos presentes en su directorio personal.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Reset</source>
        <translation>Restablecer</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Index rebuild started...</source>
        <translation>Reconstrucción del índice iniciada...</translation>
    </message>
</context>
<context>
    <name>RefreshAppsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/refresh-apps-command.hpp" line="+12"/>
        <source>Refresh Apps</source>
        <translation>Actualizar lista de aplicaciones</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Force a refresh of the application database. The database should normally automatically update itself on changes, but this can help working around some edge cases.</source>
        <translation>Forzar una actualización de la base de datos de aplicaciones. La base de datos debería actualizarse automáticamente en caso de cambios, pero esto puede ayudar a resolver algunos casos extremos.</translation>
    </message>
    <message>
        <location filename="../src/extensions/vicinae/refresh-apps-command.cpp" line="+15"/>
        <source>Apps successfully refreshed</source>
        <translation>Lista de aplicaciones actualizada correctamente</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to refresh apps</source>
        <translation>No se ha podido actualizar la lista de aplicaciones</translation>
    </message>
</context>
<context>
    <name>ReloadScriptDirectoriesCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+29"/>
        <source>Reload Script Directories</source>
        <translation>Recargar carpetas de scripts</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reload script directories</source>
        <translation>Recargar carpetas de scripts</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>New scan triggered, index will update shortly</source>
        <translation>Se ha iniciado un nuevo escaneo, el índice se actualizará en breve</translation>
    </message>
</context>
<context>
    <name>RemoveAllCalculatorHistoryRecordsAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+97"/>
        <source>Delete all entries</source>
        <translation>Eliminar todos los registros</translation>
    </message>
</context>
<context>
    <name>RemoveAllSelectionsAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+27"/>
        <source>Are you sure?</source>
        <translation>¿Estás seguro?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>All your clipboard history will be lost forever</source>
        <translation>El historial completo de su portapapeles se perderá para siempre</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Delete all</source>
        <translation>Eliminar todo</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>All selections were removed</source>
        <translation>Toda la selección fue eliminada</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove all selections</source>
        <translation>No se han podido eliminar todas las selecciones</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Remove all</source>
        <translation>Eliminar todo</translation>
    </message>
</context>
<context>
    <name>RemoveCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-43"/>
        <source>Entry removed</source>
        <translation>Entrada eliminada</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove entry</source>
        <translation>No se ha podido eliminar la entrada</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Delete entry</source>
        <translation>Eliminar entrada</translation>
    </message>
</context>
<context>
    <name>RemoveSelectionAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="-70"/>
        <source>Entry removed</source>
        <translation>Entrada eliminada</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove entry</source>
        <translation>No se ha podido eliminar la entrada</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Remove entry</source>
        <translation>Eliminar entrada</translation>
    </message>
</context>
<context>
    <name>RemoveShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="+33"/>
        <source>Removed link</source>
        <translation>Enlace eliminado</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove link</source>
        <translation>No se ha podido eliminar el enlace</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Remove link</source>
        <translation>Eliminar enlace</translation>
    </message>
</context>
<context>
    <name>ReportVicinaeBugCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/report-bug-command.hpp" line="+10"/>
        <source>Report a Vicinae Bug</source>
        <translation>Informar de un error de Vicinae</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Navigate to Vicinae issue creation page with all relevant informations pre-filled.</source>
        <translation>Navegar a la página de creación de incidencias de Vicinae con toda la información relevante prellenada.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Title</source>
        <translation>Título</translation>
    </message>
</context>
<context>
    <name>ResetEmojiRankingAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+21"/>
        <source>Reset ranking</source>
        <translation>Restablecer prioridad</translation>
    </message>
</context>
<context>
    <name>ResetEmojiSkinToneAction</name>
    <message>
        <location line="+32"/>
        <source>Reset to preference</source>
        <translation>Restablecer a la preferencia</translation>
    </message>
</context>
<context>
    <name>ResetItemRanking</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="-25"/>
        <source>Ranking was successfully reset</source>
        <translation>El ranking se ha restablecido correctamente</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Unable to reset ranking</source>
        <translation>No se ha podido restablecer el ranking</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Are you sure?</source>
        <translation>¿Estás seguro?</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You will have to rebuild search history for this item in order for it to reappear on top of the root search results.</source>
        <translation>Será necesario reconstruir el historial de búsqueda de este elemento para que vuelva a aparecer en la parte superior de los resultados de búsqueda</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Reset</source>
        <translation>Restablecer</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Reset ranking</source>
        <translation>Restablecer ranking</translation>
    </message>
</context>
<context>
    <name>RevealFileInFolderAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+26"/>
        <source>Show in file browser</source>
        <translation>Mostrar en el explorador de archivos</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to open folder</source>
        <translation>No se ha podido abrir la carpeta</translation>
    </message>
</context>
<context>
    <name>RootCalculatorSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+79"/>
        <source>Calculator</source>
        <translation>Calculadora</translation>
    </message>
</context>
<context>
    <name>RootFallbackSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="+407"/>
        <source>Use &quot;%1&quot; with...</source>
        <translation>Utilizar &quot;%1&quot; con...</translation>
    </message>
</context>
<context>
    <name>RootFavoritesSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+67"/>
        <source>Favorites</source>
        <translation>Favoritos</translation>
    </message>
</context>
<context>
    <name>RootFilesSection</name>
    <message>
        <location line="+50"/>
        <source>Files</source>
        <translation>Archivos</translation>
    </message>
</context>
<context>
    <name>RootLinkSection</name>
    <message>
        <location line="-137"/>
        <source>Link</source>
        <translation>Enlace</translation>
    </message>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="-295"/>
        <source>Open in %1</source>
        <translation>Abrir %1 en</translation>
    </message>
</context>
<context>
    <name>RootNewsSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+64"/>
        <source>What&apos;s New</source>
        <translation>Novedades</translation>
    </message>
</context>
<context>
    <name>RootResultsSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="+199"/>
        <source>Suggestions</source>
        <translation>Sugerencias</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Results (%1)</source>
        <translation>Resultados (%1)</translation>
    </message>
</context>
<context>
    <name>RootSearchActionGenerator</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+55"/>
        <source>Copy ID</source>
        <translation>Copiar ID</translation>
    </message>
</context>
<context>
    <name>RootShortcutItem</name>
    <message>
        <location filename="../src/root-search/shortcuts/shortcut-root-provider.cpp" line="+67"/>
        <location line="+11"/>
        <source>Shortcut</source>
        <translation>Atajo</translation>
    </message>
</context>
<context>
    <name>RootUpdateSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="-21"/>
        <location filename="../src/qml/root-search-sources.cpp" line="-125"/>
        <source>Update</source>
        <translation>Actualizar</translation>
    </message>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="-14"/>
        <source>Vicinae %1 is available</source>
        <translation>Vicinae %1 está disponible</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>You are running %1</source>
        <translation>Estás ejecutando %1</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>View Release Notes</source>
        <translation>Ver notas de la versión</translation>
    </message>
</context>
<context>
    <name>RootViewHost</name>
    <message>
        <location filename="../src/qml/root-view-host.hpp" line="+15"/>
        <source>Search for anything...</source>
        <translation>Busca cualquier cosa...</translation>
    </message>
</context>
<context>
    <name>ScriptExecutorViewHost</name>
    <message>
        <location filename="../src/qml/script-executor-view-host.cpp" line="+76"/>
        <source>Script execution failed: %1</source>
        <translation>Fallo en la ejecución del script: %1</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Running... (%1s ago)</source>
        <translation>Ejecutándose... (hace %1s)</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Done in %1s (exit=%2)</source>
        <translation>Terminado en %1s (exit=%2)</translation>
    </message>
    <message>
        <location line="+12"/>
        <location line="+9"/>
        <source>Script process killed</source>
        <translation>Proceso de script eliminado</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Running...</source>
        <translation>Ejecutándose...</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Kill process</source>
        <translation>Finalizar proceso</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Run script again</source>
        <translation>Ejecutar script de nuevo</translation>
    </message>
</context>
<context>
    <name>ScriptRootItem</name>
    <message>
        <location filename="../src/root-search/scripts/script-root-provider.hpp" line="+27"/>
        <location line="+86"/>
        <source>Script</source>
        <translation>Script</translation>
    </message>
    <message>
        <location line="-43"/>
        <source>Mode</source>
        <translation>Modo</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Path</source>
        <translation>Ruta</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Author</source>
        <translation>Autor</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Open script directory</source>
        <translation>Abrir carpeta de scripts</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy path to script</source>
        <translation>Copiar ruta del script</translation>
    </message>
</context>
<context>
    <name>ScriptRootProvider</name>
    <message>
        <location line="+47"/>
        <source>Script Commands</source>
        <translation>Comandos de script</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Custom directories</source>
        <translation>Directorios personalizados</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Additional list of directories to source scripts from. These directories always take precedence over the default system ones</source>
        <translation>Lista adicional de carpetas de donde obtener scripts. Estas carpetas siempre tienen prioridad sobre las carpetas predeterminadas del sistema</translation>
    </message>
</context>
<context>
    <name>SearchBrowserTabsCommand</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="+15"/>
        <source>Search Browser Tabs</source>
        <translation>Buscar pestañas del navegador</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search tabs from all connected browsers</source>
        <translation>Buscar pestañas de todos los navegadores conectados</translation>
    </message>
</context>
<context>
    <name>SearchEmojiCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/search-emoji-command.hpp" line="+15"/>
        <source>Search Emojis &amp; Symbols</source>
        <translation>Buscar emojis y símbolos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search for any emoji or symbol</source>
        <translation>Buscar cualquier emoji o símbolo</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Paste</source>
        <translation>Pegar</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy</source>
        <translation>Copiar</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Default Action</source>
        <translation>Acción predeterminada</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to perform on pressing return. Paste is only available if your environment supports it.</source>
        <translation>La acción predeterminada a realizar al presionar la tecla Enter. Pegar solo está disponible si el equipo lo permite.</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Skin tone</source>
        <translation>Tono de piel</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Skin tone to use for relevant emojis.</source>
        <translation>Tono de piel a usar para los emojis.</translation>
    </message>
</context>
<context>
    <name>SearchEmojiGridSource</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.hpp" line="-22"/>
        <source>Results (%1)</source>
        <translation>(%1) resultados</translation>
    </message>
</context>
<context>
    <name>SearchFilesCommand</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="-38"/>
        <source>Search Files</source>
        <translation>Búsqueda de archivos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search files on your system</source>
        <translation>Buscar archivos en el equipo</translation>
    </message>
</context>
<context>
    <name>SearchFilesView</name>
    <message>
        <location filename="../src/qml/qml/SearchFilesView.qml" line="+37"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Path</source>
        <translation>Ruta</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>Tipo</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Last modified</source>
        <translation>Última modificación</translation>
    </message>
</context>
<context>
    <name>SearchFilesViewHost</name>
    <message>
        <location filename="../src/qml/search-files-view-host.cpp" line="+59"/>
        <source>Search for files...</source>
        <translation>Búsqueda de archivos...</translation>
    </message>
    <message>
        <location line="+30"/>
        <location line="+4"/>
        <source>Direct file path</source>
        <translation>Ruta de archivo directa</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Recently Accessed</source>
        <translation>Accedido recientemente</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Results</source>
        <translation>Resultados</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>All</source>
        <translation>Todos</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Other</source>
        <translation>Otros</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Directories</source>
        <translation>Carpetas</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Images</source>
        <translation>Imágenes</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Videos</source>
        <translation>Vídeos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Audio</source>
        <translation>Audio</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Documents</source>
        <translation>Documentos</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Archives</source>
        <translation>Archivos</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Applications</source>
        <translation>Aplicaciones</translation>
    </message>
</context>
<context>
    <name>SetAppFont</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="-15"/>
        <source>Set as vicinae font</source>
        <translation>Configurar como fuente de Vicinae</translation>
    </message>
</context>
<context>
    <name>SetRootItemAliasAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="-18"/>
        <source>Set alias</source>
        <translation>Asignar alias</translation>
    </message>
</context>
<context>
    <name>SetThemeAction</name>
    <message>
        <location filename="../src/actions/theme/theme-actions.cpp" line="+11"/>
        <source>Theme successfully updated</source>
        <translation>Tema actualizado correctamente</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Set theme</source>
        <translation>Establecer tema</translation>
    </message>
</context>
<context>
    <name>SetThemeCommand</name>
    <message>
        <location filename="../src/extensions/theme/set-theme-command.hpp" line="+9"/>
        <source>Set Theme</source>
        <translation>Establecer tema</translation>
    </message>
</context>
<context>
    <name>SetVolumeCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="+213"/>
        <source>Set Volume to %1%</source>
        <translation>Establecer volumen a %1%</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Set system volume to %1%</source>
        <translation>Establecer volumen del sistema a %1%</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to set volume</source>
        <translation>No se ha podido modificar el volumen</translation>
    </message>
</context>
<context>
    <name>SetWallpaperAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+17"/>
        <source>Set as wallpaper</source>
        <translation>Establecer como fondo de pantalla</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Wallpaper set</source>
        <translation>Fondo de pantalla establecido</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to set wallpaper</source>
        <translation>No se ha podido establecer el fondo de pantalla</translation>
    </message>
</context>
<context>
    <name>SettingsSidebar</name>
    <message>
        <location filename="../src/qml/qml/SettingsSidebar.qml" line="+99"/>
        <source>Search...</source>
        <translation>Buscar...</translation>
    </message>
</context>
<context>
    <name>SettingsSidebarModel</name>
    <message>
        <location filename="../src/qml/settings-sidebar-model.cpp" line="+90"/>
        <source>General</source>
        <translation>General</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Appearance</source>
        <translation>Apariencia</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Keybindings</source>
        <translation>Combinaciones de teclas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced</source>
        <translation>Avanzado</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>About</source>
        <translation>Acerca de</translation>
    </message>
</context>
<context>
    <name>SettingsWindow</name>
    <message>
        <location filename="../src/qml/qml/SettingsWindow.qml" line="+10"/>
        <source>General</source>
        <translation>General</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Appearance</source>
        <translation>Apariencia</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keybindings</source>
        <translation>Combinaciones de teclas</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Advanced</source>
        <translation>Avanzado</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>About</source>
        <translation>Acerca de</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Vicinae Settings</source>
        <translation>Ajustes de Vicinae</translation>
    </message>
    <message>
        <location line="+88"/>
        <source>Imported from Raycast</source>
        <translation>Importado de Raycast</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>From the Vicinae store</source>
        <translation>De la tienda de Vicinae</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Locally installed extension</source>
        <translation>Extensión instalada localmente</translation>
    </message>
</context>
<context>
    <name>ShortcutExtension</name>
    <message>
        <location filename="../src/extensions/shortcut/shortcut-extension.hpp" line="+11"/>
        <source>Manage Shortcuts</source>
        <translation>Gestionar atajos</translation>
    </message>
</context>
<context>
    <name>ShortcutField</name>
    <message>
        <location filename="../src/qml/qml/ShortcutField.qml" line="+14"/>
        <source>Record shortcut</source>
        <translation>Grabar atajo</translation>
    </message>
</context>
<context>
    <name>ShortcutFormView</name>
    <message>
        <location filename="../src/qml/qml/ShortcutFormView.qml" line="+14"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Shortcut Name</source>
        <translation>Nombre del atajo</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>The URL that will be opened by the specified app. You can make it dynamic by using placeholders such as {argument}.</source>
        <translation>La URL que será abierta por la aplicación especificada. Puedes hacerla dinámica usando marcadores de posición como {argument}.</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Open with</source>
        <translation>Abrir con</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Icon</source>
        <translation>Icono</translation>
    </message>
</context>
<context>
    <name>ShortcutFormViewHost</name>
    <message>
        <location filename="../src/qml/shortcut-form-view-host.cpp" line="+47"/>
        <source>Submit</source>
        <translation>Aceptar</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Copy of %1</source>
        <translation>Copia de %1</translation>
    </message>
    <message>
        <location line="+67"/>
        <source>Edit &quot;%1&quot;</source>
        <translation>Editar &quot;%1&quot;</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Duplicate &quot;%1&quot;</source>
        <translation>Duplicar &quot;%1&quot;</translation>
    </message>
    <message>
        <location line="+11"/>
        <location line="+131"/>
        <location line="+50"/>
        <source>Default</source>
        <translation>Predeterminado</translation>
    </message>
    <message>
        <location line="-157"/>
        <source>Selected Text</source>
        <translation>Texto seleccionado</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clipboard Text</source>
        <translation>Texto del portapapeles</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Argument</source>
        <translation>Argumento</translation>
    </message>
    <message>
        <location line="+35"/>
        <location line="+5"/>
        <location line="+5"/>
        <source>Required</source>
        <translation>Obligatorio</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Validation failed</source>
        <translation>La validación ha fallado</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Failed to update shortcut</source>
        <translation>No se ha podido actualizar el atajo</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Shortcut updated</source>
        <translation>Atajo actualizado</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to create shortcut</source>
        <translation>No se ha podido crear el atajo</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Shortcut created</source>
        <translation>Atajo creado</translation>
    </message>
</context>
<context>
    <name>ShortcutRecorderField</name>
    <message>
        <location filename="../src/qml/qml/ShortcutRecorderField.qml" line="+24"/>
        <location line="+22"/>
        <location line="+62"/>
        <source>Recording...</source>
        <translation>Grabando...</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Keybind updated</source>
        <translation>Atajo de teclado actualizado</translation>
    </message>
</context>
<context>
    <name>ShortcutRootProvider</name>
    <message>
        <location filename="../src/root-search/shortcuts/shortcut-root-provider.cpp" line="+42"/>
        <source>Shortcuts</source>
        <translation>Atajos</translation>
    </message>
</context>
<context>
    <name>ShortcutsSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/ShortcutsSettingsPage.qml" line="+55"/>
        <source>Keybindings</source>
        <translation>Atajos de teclado</translation>
    </message>
    <message>
        <location line="+78"/>
        <source>Record Shortcut</source>
        <translation>Grabar atajo</translation>
    </message>
</context>
<context>
    <name>SkipUpdateVersionAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+8"/>
        <source>Skip This Version</source>
        <translation>Saltar esta versión</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Skipped %1</source>
        <translation>%1 saltada</translation>
    </message>
</context>
<context>
    <name>SleepCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+94"/>
        <source>Put System to Sleep</source>
        <translation>Suspender el equipo</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Put system to sleep</source>
        <translation>Suspender el equipo</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>System can&apos;t sleep</source>
        <translation>El sistema no puede entrar en suspensión</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to sleep</source>
        <translation>No se ha podido suspender el equipo</translation>
    </message>
</context>
<context>
    <name>SnippetDatabase</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.cpp" line="+42"/>
        <location line="+56"/>
        <source>keyword already assigned to &quot;%1&quot;</source>
        <translation>Palabra clave ya asignada a &quot;%1&quot;</translation>
    </message>
    <message>
        <location line="-43"/>
        <source>No snippet with that ID</source>
        <translation>No hay ningún snippet con ese ID</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>No such snippet</source>
        <translation>No existe dicho snippet</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Snippet limit reached (%1)</source>
        <translation>Límite de snippets alcanzado (%1)</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Failed to save snippets on disk: %1</source>
        <translation>No se han podido guardar los snippets en el disco: %1</translation>
    </message>
</context>
<context>
    <name>SnippetExtension</name>
    <message>
        <location filename="../src/extensions/snippet/snippet-extension.cpp" line="+38"/>
        <source>Expansion</source>
        <translation>Expansión</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Enable automatic snippet expansion when triggers are typed</source>
        <translation>Activar la expansión automática de snippets cuando se escriben los desencadenantes</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Undo</source>
        <translation>Deshacer</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Press backspace immediately after expansion to undo and restore the trigger text</source>
        <translation>Presionar la tecla de retroceso inmediatamente después de la expansión para deshacerla y restaurar el texto del desencadenante</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Keyboard layout</source>
        <translation>Disposición del teclado</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>XKB layout used for trigger detection (e.g. &quot;us&quot;, &quot;fr&quot;). Leave empty for system default.</source>
        <translation>Disposición XKB utilizada para la detección de desencadenantes (por ejemplo, &quot;us&quot;, &quot;fr&quot;). Dejar vacío para usar el valor predeterminado del sistema.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Pre-paste delay (ms)</source>
        <translation>Retraso antes de pegar (ms)</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Delay between setting clipboard and injecting paste shortcut. Increase if expansions paste empty on slow compositors.</source>
        <translation>Retraso entre establecer el portapapeles y simular el atajo de pegar. Aumentar si las expansiones se pegan vacías en compositores lentos.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Key injection delay (ms)</source>
        <translation>Retraso entre pulsaciones de teclas (ms)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Delay between injected key events. Increase if expansions produce missing or garbled characters on slow compositors.</source>
        <translation>Retraso entre eventos de teclado simulados. Aumentar si las expansiones producen carácteres faltantes o corruptos en compositores lentos.</translation>
    </message>
    <message>
        <location filename="../src/extensions/snippet/snippet-extension.hpp" line="+11"/>
        <source>Snippets</source>
        <translation>Snippets</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Text expansion and snippet management</source>
        <translation>Expansión de texto y gestión de snippets</translation>
    </message>
</context>
<context>
    <name>SnippetFormView</name>
    <message>
        <location filename="../src/qml/qml/SnippetFormView.qml" line="+15"/>
        <source>Title</source>
        <translation>Título</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Euro symbol</source>
        <translation>Símbolo del euro</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Content</source>
        <translation>Contenido</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You can use {dynamic placeholders} to make the content dynamic: &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;learn more&lt;/a&gt;.</source>
        <translation>Puede usar {placeholders dinámicos} para hacer el contenido dinámico: &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;Saber más&lt;/a&gt;.</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Keyword</source>
        <translation>Palabra clave</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Typing this keyword anywhere will result in it being replaced by the content of the snippet.</source>
        <translation>Al escribir esta palabra clave en cualquier lugar, será reemplazada por el contenido del snippet.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The snippet server is not running. Keyword expansion is unavailable. &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;Learn more&lt;/a&gt;.</source>
        <translation>El servidor de snippets no se está ejecutando. La expansión de palabras clave no está disponible. &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;Saber más&lt;/a&gt;.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Applications</source>
        <translation>Aplicaciones</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Restrict expansion to specific applications. By default, it works everywhere.</source>
        <translation>Restringir la expansión a aplicaciones específicas. Por defecto, funciona en todas partes.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Expand as word</source>
        <translation>Expandir como palabra</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>If a keyword is typed, it will only be expanded after space or punctuation.</source>
        <translation>Si se escribe una palabra clave, solo se expandirá después de un espacio o signo de puntuación.</translation>
    </message>
</context>
<context>
    <name>SnippetFormViewHost</name>
    <message>
        <location filename="../src/qml/snippet-form-view-host.cpp" line="+49"/>
        <source>Submit</source>
        <translation>Enviar</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Copy of %1</source>
        <translation>Copia de %1</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Edit &quot;%1&quot;</source>
        <translation>Editar &quot;%1&quot;</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Duplicate &quot;%1&quot;</source>
        <translation>Duplicar &quot;%1&quot;</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>2 chars min.</source>
        <translation>2 caracteres como mínimo.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Content should not be empty</source>
        <translation>El contenido no debe estar vacío</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Only one {cursor} placeholder is allowed</source>
        <translation> Solo se permite un marcador de posición {cursor}.</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Validation failed</source>
        <translation>Validación fallida</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Snippet updated</source>
        <translation>Snippet actualizado</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Snippet successfully created</source>
        <translation>Snippet creado con éxito</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Cursor Position</source>
        <translation>Posición del cursor</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clipboard Text</source>
        <translation>Texto del portapapeles</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Date</source>
        <translation>Fecha</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Argument</source>
        <translation>Argumento</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Shell Command</source>
        <translation>Comando de shell</translation>
    </message>
</context>
<context>
    <name>SoftRebootCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-99"/>
        <source>Soft Reboot System</source>
        <translation>Reiniciar el equipo parcialmente</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Soft reboot the system, which usually means only userspace is rebooted.</source>
        <translation>Reiniciar el equipo parcialmente, lo que generalmente significa que solo se reinicia el espacio de usuario.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System can&apos;t soft reboot</source>
        <translation>El equipo no puede reiniciarse parcialmente</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to soft reboot</source>
        <translation>El equipo no ha podido reiniciarse parcialmente</translation>
    </message>
</context>
<context>
    <name>SponsorVicinaeCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-121"/>
        <source>Donate to Vicinae</source>
        <translation>Donar al equipo de Vicinae</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open link to Vicinae&apos;s GitHub sponsor page</source>
        <translation>Abre el enlace a la página de Github Sponsors de Vicinae</translation>
    </message>
</context>
<context>
    <name>StoreDetailView</name>
    <message>
        <location filename="../src/qml/qml/StoreDetailView.qml" line="+196"/>
        <source>Installed</source>
        <translation>Instalado</translation>
    </message>
    <message>
        <location line="+166"/>
        <source>Description</source>
        <translation>Descripción</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Commands</source>
        <translation>Comandos</translation>
    </message>
    <message>
        <location line="+74"/>
        <source>Open README</source>
        <translation>Abrir README</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Last update</source>
        <translation>Última actualización</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Contributors</source>
        <translation>Contribuidores</translation>
    </message>
    <message>
        <location line="+29"/>
        <source>Categories</source>
        <translation>Categorías</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Source Code</source>
        <translation>Código fuente</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>View Code</source>
        <translation>Ver código</translation>
    </message>
</context>
<context>
    <name>SuspendCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+36"/>
        <source>Suspend System</source>
        <translation>Suspender equipo</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Suspend the system to RAM. Unlike hibernation, this does not turn the computer off and will break on power loss.</source>
        <translation>Suspender el equipo y almacena el estado en la memoria RAM. A diferencia de la hibernación, esto no apaga el ordenador y el estado actual se perderá si hay una pérdida de energía.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System cannot suspend</source>
        <translation>El equipo no puede suspenderse</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to suspend</source>
        <translation>No se ha podido suspender el equipo</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsCommand</name>
    <message>
        <location filename="../src/extensions/wm/switch-windows-command.hpp" line="+10"/>
        <source>Switch Windows</source>
        <translation>Cambiar de ventana</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsSection</name>
    <message>
        <location filename="../src/qml/switch-windows-model.hpp" line="+28"/>
        <source>Open Windows</source>
        <translation>Abrir ventanas</translation>
    </message>
    <message>
        <location filename="../src/qml/switch-windows-model.cpp" line="+19"/>
        <source>WS %1</source>
        <translation>WS %1</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Window Actions</source>
        <translation>Acciones de ventana</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsViewHost</name>
    <message>
        <location filename="../src/qml/switch-windows-view-host.cpp" line="+12"/>
        <source>Search open window...</source>
        <translation>Buscar ventanas abiertas...</translation>
    </message>
</context>
<context>
    <name>SystemBrowseApps</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-123"/>
        <source>Browse Apps</source>
        <translation>Buscar aplicaciones</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse all applications that are installed on the system</source>
        <translation>Buscar todas las aplicaciones que están instaladas en el equipo</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Show hidden apps</source>
        <translation>Mostrar aplicaciones ocultas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sort alphabetically</source>
        <translation>Ordenar alfabéticamente</translation>
    </message>
</context>
<context>
    <name>SystemExtension</name>
    <message>
        <location line="+149"/>
        <source>System</source>
        <translation>Sistema</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>System-related commands</source>
        <translation>Comandos relacionados con el sistema</translation>
    </message>
</context>
<context>
    <name>SystemRunCommand</name>
    <message>
        <location line="-239"/>
        <source>Run Terminal Program</source>
        <translation>Ejecutar programa en terminal</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run a program in a terminal window</source>
        <translation>Ejecutar un programa en una ventana de terminal</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>command</source>
        <translation>comando</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Run in terminal</source>
        <translation>Ejecutar en terminal</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run in terminal (hold)</source>
        <translation>Ejecutar en terminal (mantener)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run directly</source>
        <translation>Ejecutar directamente</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Default Action</source>
        <translation>Acción por defecto</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to run on pressing return</source>
        <translation>La acción por defecto a ejecutar al presionar la tecla Enter</translation>
    </message>
    <message>
        <location line="+29"/>
        <source>Not a valid executable</source>
        <translation>No es un ejecutable válido</translation>
    </message>
</context>
<context>
    <name>SystemRunViewHost</name>
    <message>
        <location filename="../src/qml/system-run-view-host.cpp" line="+20"/>
        <source>Search for a program to execute...</source>
        <translation>Buscar un programa para ejecutar...</translation>
    </message>
</context>
<context>
    <name>ThemeExtension</name>
    <message>
        <location filename="../src/extensions/theme/theme-extension.hpp" line="+9"/>
        <source>Theme</source>
        <translation>Tema</translation>
    </message>
</context>
<context>
    <name>ThemeSection</name>
    <message>
        <location filename="../src/qml/theme-list-model.cpp" line="+22"/>
        <source>Default theme description</source>
        <translation>Descripción del tema predeterminado</translation>
    </message>
    <message>
        <location line="+64"/>
        <source>Open theme file</source>
        <translation>Abrir archivo de tema</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy ID</source>
        <translation>Copiar ID</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy path</source>
        <translation>Copiar ruta</translation>
    </message>
</context>
<context>
    <name>ThemeViewHost</name>
    <message>
        <location filename="../src/qml/theme-view-host.cpp" line="+22"/>
        <source>Search for a theme...</source>
        <translation>Buscar un tema...</translation>
    </message>
    <message>
        <location line="+51"/>
        <source>Current Theme</source>
        <translation>Tema actual</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Available Themes</source>
        <translation>Temas disponibles</translation>
    </message>
</context>
<context>
    <name>ToggleItemAsFavorite</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="+17"/>
        <source>Remove from favorites</source>
        <translation>Eliminar de favoritos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Add to favorites</source>
        <translation>Añadir a favoritos</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Successfuly added to favorites</source>
        <translation>Añadido a favoritos</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Successfuly removed from favorites</source>
        <translation>Eliminado de favoritos</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to add to favorites</source>
        <translation>No se ha podido añadir a favoritos</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove from favorites</source>
        <translation>No se ha podido eliminar de favoritos</translation>
    </message>
</context>
<context>
    <name>ToggleMuteCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="+163"/>
        <source>Toggle Mute</source>
        <translation>Silenciar/Desactivar silencio</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mute or unmute system audio</source>
        <translation>Activar o desactivar el sonido del equipo</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Failed to toggle mute</source>
        <translation>No se ha podido modificar el volumen</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Muted</source>
        <translation>Silenciado</translation>
    </message>
</context>
<context>
    <name>UIPlayground</name>
    <message>
        <location filename="../src/extensions/internal/internal-extension.hpp" line="-15"/>
        <source>UI Showcase</source>
        <translation>Presentación de elementos de la interfaz de usuario</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Showcase of vicinae UI elements</source>
        <translation>Presentación de elementos de la interfaz de usuario de Vicinae</translation>
    </message>
</context>
<context>
    <name>UIShowcase</name>
    <message>
        <location filename="../src/qml/qml/UIShowcase.qml" line="+26"/>
        <source>Buttons</source>
        <translation>Botones</translation>
    </message>
</context>
<context>
    <name>UninstallExtensionAction</name>
    <message>
        <location filename="../src/actions/extension/extension-actions.cpp" line="+11"/>
        <source>Are you sure?</source>
        <translation>¿Estás seguro?</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>All this extension data will be permanently lost. If you just want the extension to not appear in the root search anymore, consider disabling it instead.</source>
        <translation>Todos los datos de esta extensión serán eliminados permanentemente. Si solo quieres que la extensión no aparezca en la búsqueda principal, considera desactivarla en su lugar.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Uninstall</source>
        <translation>Desinstalar</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Extension uninstalled</source>
        <translation>Extensión desinstalada</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to uninstall extension</source>
        <translation>No se ha podido desinstalar la extensión</translation>
    </message>
    <message>
        <location filename="../src/actions/extension/extension-actions.hpp" line="+14"/>
        <source>Uninstall Extension</source>
        <translation>Desinstalar extensión</translation>
    </message>
</context>
<context>
    <name>UnpinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-23"/>
        <source>Entry unpinned</source>
        <translation>Entrada desanclada</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Unpin entry</source>
        <translation>Desanclar entrada</translation>
    </message>
</context>
<context>
    <name>UnpinEmojiAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-43"/>
        <source>Unpin emoji</source>
        <translation>Desanclar emoji</translation>
    </message>
</context>
<context>
    <name>UpdateService</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="-167"/>
        <source>Update installed</source>
        <translation>Actualización instalada</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Restarting…</source>
        <translation>Reiniciando…</translation>
    </message>
    <message>
        <location line="+88"/>
        <source>Downloading Vicinae %1…</source>
        <translation>Descargando Vicinae %1…</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading Vicinae %1… %2%</source>
        <translation>Descargando Vicinae %1… %2 %</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Installing update…</source>
        <translation>Instalando actualización…</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Update failed</source>
        <translation>La actualización ha fallado</translation>
    </message>
</context>
<context>
    <name>VicinaeExtension</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.hpp" line="+11"/>
        <source>General vicinae-related commands.</source>
        <translation>Comandos genéricos relacionados con Vicinae.</translation>
    </message>
</context>
<context>
    <name>VicinaeHotkeyGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/vicinae-hotkey-global-shortcut-backend.cpp" line="+48"/>
        <source>Unsupported trigger key</source>
        <translation>Tecla de activación no soportada</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Hotkey binding was lost</source>
        <translation>La asignación de la tecla de acceso rápido se ha perdido</translation>
    </message>
</context>
<context>
    <name>VicinaeListInstalledExtensionsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/list-installed-extensions-command.hpp" line="+11"/>
        <source>Show Installed Extensions</source>
        <translation>Mostrar extensiones instaladas</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Show all third-party extensions that have been installed. This includes local extensions as well as extensions downloaded from the stores (vicinae and raycast).</source>
        <translation>Mostrar todas las extensiones de terceros que han sido instaladas. Esto incluye extensiones locales así como extensiones descargadas de las tiendas (tanto Vicinae como Raycast).</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-store-command.hpp" line="+13"/>
        <source>Install extensions from the Vicinae store</source>
        <translation>Instalar extensiones del catálogo de Vicinae</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Always show intro</source>
        <translation>Siempre mostrar la introducción</translation>
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
# Bienvenido al catálogo de extensiones de Vicinae

El catálogo de extensiones de Vicinae ofrece extensiones construidas por la comunidad que han sido aprobadas por los contribuidores principales.

El código fuente de cada extensión está disponible en el repositorio [vicinaehq/extensions](https://github.com/vicinaehq/extensions).

Si estás buscando crear tu propia extensión, echa un vistazo a la [documentación](https://docs.vicinae.com/extensions/introduction). Si crees que tu extensión sería una buena candidata para el catálogo no dudes en enviarla.
</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Continue to store</source>
        <translation>Continuar a la tienda</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreDetailHost</name>
    <message>
        <location filename="../src/qml/vicinae-store-detail-host.cpp" line="+38"/>
        <source>Failed to load extension</source>
        <translation>No se ha podido cargar la extensión</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Could not fetch extension data from the store.</source>
        <translation>No se han podido obtener los datos de la extensión.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Extension not found</source>
        <translation>Extensión no encontrada</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The extension &quot;%1&quot; could not be found in the store.</source>
        <translation>La extensión &quot;%1&quot; no se pudo encontrar en la tienda.</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Extension Store - %1</source>
        <translation>Tienda de extensiones - %1</translation>
    </message>
    <message>
        <location line="+81"/>
        <source>Install extension</source>
        <translation>Instalar extensión</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading extension...</source>
        <translation>Descargando extensión...</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to download extension</source>
        <translation>No se ha podido descargar la extensión</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to extract extension archive</source>
        <translation>No se ha podido extraer el archivo de la extensión</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Extension installed</source>
        <translation>Extensión instalada</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Report issue</source>
        <translation>Informar de un problema</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreSection</name>
    <message>
        <location filename="../src/qml/vicinae-store-model.cpp" line="+41"/>
        <source>Show details</source>
        <translation>Mostrar detalles</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreViewHost</name>
    <message>
        <location filename="../src/qml/vicinae-store-view-host.cpp" line="+27"/>
        <source>Browse Vicinae extensions</source>
        <translation>Explorar extensiones de Vicinae</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Failed to fetch extensions</source>
        <translation>No se han podido obtener las extensiones</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Extensions</source>
        <translation>Extensiones</translation>
    </message>
</context>
<context>
    <name>VolumeDownCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-74"/>
        <source>Turn Volume Down</source>
        <translation>Bajar el volumen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Decrease system volume</source>
        <translation>Bajar el volumen del equipo</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid step value</source>
        <translation>Valor de paso no válido</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to adjust volume</source>
        <translation>No se ha podido ajustar el volumen</translation>
    </message>
</context>
<context>
    <name>VolumeUpCommand</name>
    <message>
        <location line="-61"/>
        <source>Turn Volume Up</source>
        <translation>Aumentar el volumen</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Increase system volume</source>
        <translation>Aumentar el volumen del equipo</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid step value</source>
        <translation>Valor de paso no válido</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to adjust volume</source>
        <translation>No se ha podido ajustar el volumen</translation>
    </message>
</context>
<context>
    <name>WallpaperManager</name>
    <message>
        <location filename="../src/services/wallpaper/wallpaper-manager.cpp" line="+68"/>
        <source>Setting the wallpaper is not supported in the current environment</source>
        <translation>La configuración del fondo de pantalla no es compatible con el entorno actual</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No such file: %1</source>
        <translation>No existe el archivo: %1</translation>
    </message>
</context>
<context>
    <name>WinControlPanelRootItem</name>
    <message>
        <location filename="../src/root-search/control-panel/control-panel-root-provider.cpp" line="+104"/>
        <location line="+10"/>
        <source>Control Panel</source>
        <translation>Panel de control</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Where</source>
        <translation>Dónde</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Open Applet</source>
        <translation>Abrir applet</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Path</source>
        <translation>Copiar ruta</translation>
    </message>
</context>
<context>
    <name>WinControlPanelRootProvider</name>
    <message>
        <location line="+49"/>
        <source>Control Panel</source>
        <translation>Panel de control</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Control Panel applets and system tasks.</source>
        <translation>Applets del panel de control y tareas del sistema.</translation>
    </message>
</context>
<context>
    <name>WinControlPanelTaskRootItem</name>
    <message>
        <location line="-40"/>
        <location line="+11"/>
        <source>Control Panel</source>
        <translation>Panel de control</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Task ID</source>
        <translation>ID de la tarea</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Open</source>
        <translation>Abrir</translation>
    </message>
</context>
<context>
    <name>WinSettingsPage</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="-114"/>
        <source>Display</source>
        <translation>Pantalla</translation>
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
        <translation>Sistema</translation>
    </message>
    <message>
        <location line="-17"/>
        <source>Night Light</source>
        <translation>Luz nocturna</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sound</source>
        <translation>Sonido</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Volume Mixer</source>
        <translation>Mezclador de sonido</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Notifications</source>
        <translation>Notificaciones</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Focus</source>
        <translation>No molestar</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power &amp; Battery</source>
        <translation>Energía y batería</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Storage</source>
        <translation>Almacenamiento</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Nearby Sharing</source>
        <translation>Compartir con dispositivos cercanos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Multitasking</source>
        <translation>Multitarea</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activation</source>
        <translation>Activación</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Troubleshoot</source>
        <translation>Solucionar problemas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Recovery</source>
        <translation>Recuperación</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Projecting to This PC</source>
        <translation>Proyectando a este PC</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Remote Desktop</source>
        <translation>Escritorio remoto</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clipboard</source>
        <translation>Portapapeles</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>About</source>
        <translation>Acerca de</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Optional Features</source>
        <translation>Características opcionales</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>For Developers</source>
        <translation>Para desarrolladores</translation>
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
        <translation>Bluetooth y dispositivos</translation>
    </message>
    <message>
        <location line="-8"/>
        <source>Devices</source>
        <translation>Dispositivos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Printers &amp; Scanners</source>
        <translation>Impresoras y escáneres</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mobile Devices</source>
        <translation>Dispositivos móviles</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Cameras</source>
        <translation>Cámaras</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mouse</source>
        <translation>Ratón</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Touchpad</source>
        <translation>Panel táctil</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pen &amp; Windows Ink</source>
        <translation>Pluma y Windows Ink</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>AutoPlay</source>
        <translation>Reproducción automática</translation>
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
        <translation>Red e internet</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Wi-Fi</source>
        <translation>Wi-Fi</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ethernet</source>
        <translation>Ethernet</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Mobile Hotspot</source>
        <translation>Punto de acceso móvil</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Airplane Mode</source>
        <translation>Modo avión</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Proxy</source>
        <translation>Proxy</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dial-up</source>
        <translation>Marcador</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced Network Settings</source>
        <translation>Configuración avanzada de red</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Background</source>
        <translation>Fondo de pantalla</translation>
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
        <translation>Personalización</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Colors</source>
        <translation>Colores</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Themes</source>
        <translation>Temas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lock Screen</source>
        <translation>Pantalla de bloqueo</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Touch Keyboard</source>
        <translation>Teclado táctil</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Start</source>
        <translation>Inicio</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Taskbar</source>
        <translation>Barra de tareas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Fonts</source>
        <translation>Fuentes</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dynamic Lighting</source>
        <translation>Luz dinámica</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Installed Apps</source>
        <translation>Aplicaciones instaladas</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Apps</source>
        <translation>Aplicaciones</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>Default Apps</source>
        <translation>Aplicaciones predeterminadas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Offline Maps</source>
        <translation>Mapas sin conexión</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Apps for Websites</source>
        <translation>Aplicaciones para sitios web</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Video Playback</source>
        <translation>Reproducción de video</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Startup Apps</source>
        <translation>Aplicaciones de arranque</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Your Info</source>
        <translation>Tu información</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Accounts</source>
        <translation>Cuentas</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>Email &amp; Accounts</source>
        <translation>Correo electrónico y cuentas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sign-in Options</source>
        <translation>Opciones de inicio de sesión</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Access Work or School</source>
        <translation>Acceder a la cuenta profesional o educativa</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Family &amp; Other Users</source>
        <translation>Familia y otros usuarios</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Windows Backup</source>
        <translation>Copia de seguridad de Windows</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Date &amp; Time</source>
        <translation>Fecha y hora</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Time &amp; Language</source>
        <translation>Hora e idioma</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Language &amp; Region</source>
        <translation>Idioma y región</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Typing</source>
        <translation>Escritura</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Speech</source>
        <translation>Reconocimiento de voz</translation>
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
        <translation>Juegos</translation>
    </message>
    <message>
        <location line="-1"/>
        <source>Captures</source>
        <translation>Capturas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Game Mode</source>
        <translation>Modo juego</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Text Size</source>
        <translation>Tamaño del texto</translation>
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
        <translation>Accesibilidad</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Visual Effects</source>
        <translation>Efectos visuales</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Magnifier</source>
        <translation>Lupa</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Color Filters</source>
        <translation>Filtros de color</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Contrast Themes</source>
        <translation>Temas de contraste</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Narrator</source>
        <translation>Narrador</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Audio</source>
        <translation>Accesibilidad del audio</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Captions</source>
        <translation>Subtítulos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Keyboard</source>
        <translation>Accesibilidad del teclado</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Mouse</source>
        <translation>Accesibilidad del ratón</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Eye Control</source>
        <translation>Control ocular</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Windows Security</source>
        <translation>Seguridad de Windows</translation>
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
        <translation>Privacidad y seguridad</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Find My Device</source>
        <translation>Encontrar mi dispositivo</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Privacy</source>
        <translation>Privacidad</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Location</source>
        <translation>Ubicación</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Camera Access</source>
        <translation>Acceso a la cámara</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Microphone Access</source>
        <translation>Acceso al micrófono</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activity History</source>
        <translation>Historial de actividad</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Diagnostics &amp; Feedback</source>
        <translation>Comentarios y diagnósticos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search Permissions</source>
        <translation>Buscar permisos</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Windows Update</source>
        <translation>Windows Update</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Update History</source>
        <translation>Historial de actualizaciones</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced Update Options</source>
        <translation>Opciones avanzadas de actualización</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Windows Insider Program</source>
        <translation>Programa Windows Insider</translation>
    </message>
</context>
<context>
    <name>WinSettingsPageRootItem</name>
    <message>
        <location line="+37"/>
        <source>System Settings</source>
        <translation>Configuración del equipo</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Settings</source>
        <translation>Configuración</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Category</source>
        <translation>Categoría</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Open %1 Settings</source>
        <translation>Abrir configuración de %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy URL</source>
        <translation>Copiar URL</translation>
    </message>
</context>
<context>
    <name>WinSettingsRootProvider</name>
    <message>
        <location line="+12"/>
        <source>Windows Settings</source>
        <translation>Configuración de Windows</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Pages of the Windows Settings app.</source>
        <translation>Páginas de la aplicación de Configuración.</translation>
    </message>
</context>
<context>
    <name>WindowManagementExtension</name>
    <message>
        <location filename="../src/extensions/wm/wm-extension.hpp" line="+12"/>
        <source>Window Management</source>
        <translation>Administración de ventanas</translation>
    </message>
</context>
<context>
    <name>WindowsAppDatabase</name>
    <message>
        <location filename="../src/services/app-service/windows/win-app-database.cpp" line="+993"/>
        <source>Focus window</source>
        <translation>Enfocar ventana</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Launch app</source>
        <translation>Abrir aplicación</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Default action</source>
        <translation>Acción por defecto</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Action to perform when the return key is pressed. Always default to &apos;launch&apos; if the app has no open window.</source>
        <translation>Acción a realizar cuando se presiona la tecla de retorno. Por defecto, si no hay ninguna instancia abierta de la aplicación se utilizará la acción &quot;iniciar&quot;. De lo contrario, se utilizará la acción &quot;enfocar ventana&quot;.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>File Explorer</source>
        <translation>Explorador de archivos</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Terminal</source>
        <translation>Terminal</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Command Prompt</source>
        <translation>Símbolo del sistema</translation>
    </message>
</context>
<context>
    <name>WindowsApplication</name>
    <message>
        <location filename="../src/services/app-service/windows/win-app.hpp" line="+70"/>
        <source>%1: Run as Administrator</source>
        <translation>Ejecutar como administrador: %1</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run as Administrator</source>
        <translation>Ejecutar como administrador</translation>
    </message>
</context>
<context>
    <name>WindowsGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/windows-global-shortcut-backend.cpp" line="+215"/>
        <source>unsupported or invalid trigger</source>
        <translation>Activador no soportado o inválido</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>already registered by another application</source>
        <translation>Ya registrado por otra aplicación</translation>
    </message>
</context>
<context>
    <name>X11GlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/x11-global-shortcut-backend.cpp" line="+122"/>
        <source>This shortcut is already in use by another application</source>
        <translation>Este atajo ya está en uso por otra aplicación</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Unsupported trigger key</source>
        <translation>Tecla de activación no soportada</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Trigger key is not present on this keyboard</source>
        <translation>La tecla de activación no está presente en este teclado</translation>
    </message>
</context>
<context>
    <name>X11Workspace</name>
    <message>
        <location filename="../src/services/window-manager/x11/x11-window-manager.cpp" line="+426"/>
        <source>Desktop %1</source>
        <translation>Escritorio %1</translation>
    </message>
</context>
<context>
    <name>XdgAppDatabase</name>
    <message>
        <location filename="../src/services/app-service/xdg/xdg-app-database.cpp" line="+564"/>
        <source>Focus window</source>
        <translation>Enfocar ventana</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Launch app</source>
        <translation>Abrir aplicación</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Default action</source>
        <translation>Acción por defecto</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Action to perform when the return key is pressed. Always default to &apos;launch&apos; if the app has no open window.</source>
        <translation>Acción a realizar cuando se presiona la tecla de retorno. Por defecto, si no hay ninguna instancia abierta de la aplicación se utilizará la acción &quot;iniciar&quot;. De lo contrario, se utilizará la acción &quot;enfocar ventana&quot;.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Launch Prefix</source>
        <translation>Prefijo de inicio</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Custom app launcher to use. Affects applications as well as their sub-actions.</source>
        <translation>Aplicación de inicio personalizada a utilizar. Afecta tanto a las aplicaciones como a sus acciones asociadas.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Application directories</source>
        <translation>Carpetas de aplicaciones</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Directories applications are sourced from. The list cannot be modified directly. In order to do so, you need to append additonal paths to the &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt; environment variables.</source>
        <translation>Carpetas de los que se toman las aplicaciones. La lista no puede ser modificada directamente. Para hacerlo, necesitas añadir rutas adicionales a las variables de entorno &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt;.</translation>
    </message>
</context>
<context>
    <name>XdpFileChooser</name>
    <message>
        <location filename="../src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.cpp" line="+39"/>
        <source>Open Directory</source>
        <translation>Abrir carpeta</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Open File</source>
        <translation>Abrir archivo</translation>
    </message>
</context>
<context>
    <name>browser-extension</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="-58"/>
        <source>No browser connected</source>
        <translation>No hay ningún navegador conectado</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You need to connect at least one browser to vicinae using the browser extension in order to use this command.</source>
        <translation>Es necesario conectar al menos un navegador a vicinae usando la extensión del navegador para poder usar este comando.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Open documentation</source>
        <translation>Abrir documentación</translation>
    </message>
</context>
<context>
    <name>clipboard-history-view-host</name>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.cpp" line="-207"/>
        <source>Text</source>
        <translation>Texto</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Link</source>
        <translation>Enlace</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Image</source>
        <translation>Imagen</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>File</source>
        <translation>Archivo</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Unknown</source>
        <translation>Desconocido</translation>
    </message>
</context>
<context>
    <name>emoji-categories</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-52"/>
        <source>Smileys &amp; Emotion</source>
        <translation>Emoticonos y emociones</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>People &amp; Body</source>
        <translation>Personas y partes del cuerpo</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Animals &amp; Nature</source>
        <translation>Animales y naturaleza</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Food &amp; Drink</source>
        <translation>Comida y bebida</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Travel &amp; Places</source>
        <translation>Viajes y Lugares</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activities</source>
        <translation>Actividades</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Objects</source>
        <translation>Objetos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Symbols</source>
        <translation>Símbolos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Flags</source>
        <translation>Banderas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Math</source>
        <translation>Matemáticas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Arrows</source>
        <translation>Flechas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Currency</source>
        <translation>Divisas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Punctuation</source>
        <translation>Puntuación</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Shapes</source>
        <translation>Formas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Misc Symbols</source>
        <translation>Símbolos varios</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Greek</source>
        <translation>Griego</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Number Forms</source>
        <translation>Formas numéricas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Fancy Letters</source>
        <translation>Letras decorativas</translation>
    </message>
</context>
<context>
    <name>emoji-grid-model</name>
    <message>
        <location line="+129"/>
        <source>Copy</source>
        <translation>Copiar</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy name</source>
        <translation>Copiar nombre</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy unicode codepoint</source>
        <translation>Copiar punto de código unicode</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy category</source>
        <translation>Copiar categoría</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>Skin tones</source>
        <translation>Tonos de piel</translation>
    </message>
</context>
<context>
    <name>file-list-item</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+35"/>
        <source>Copy file</source>
        <translation>Copiar archivo</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy file path</source>
        <translation>Copiar ruta del archivo</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy file name</source>
        <translation>Copiar nombre del archivo</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Copy mime type</source>
        <translation>Copiar tipo MIME</translation>
    </message>
</context>
<context>
    <name>font-categories</name>
    <message>
        <location filename="../src/font-service.cpp" line="+127"/>
        <source>Latin</source>
        <translation>Latín</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Cyrillic</source>
        <translation>Cirílico</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Greek</source>
        <translation>Griego</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Monospace</source>
        <translation>Monoespaciado</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Emoji</source>
        <translation>Emoticono</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Japanese</source>
        <translation>Japonés</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Korean</source>
        <translation>Coreano</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Simplified Chinese</source>
        <translation>Chino simplificado</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Traditional Chinese</source>
        <translation>Chino tradicional</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Arabic</source>
        <translation>Árabe</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Hebrew</source>
        <translation>Hebreo</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Thai</source>
        <translation>Tailandés</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lao</source>
        <translation>Laos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Devanagari</source>
        <translation>Devanagárico</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Bengali</source>
        <translation>Bengalí</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Gurmukhi</source>
        <translation>Gurmují</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Gujarati</source>
        <translation>Guyaratí</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tamil</source>
        <translation>Tamil</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Telugu</source>
        <translation>Telugu</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Kannada</source>
        <translation>Canarés</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Malayalam</source>
        <translation>Malabar</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Sinhala</source>
        <translation>Cingalés</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Armenian</source>
        <translation>Armenio</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Georgian</source>
        <translation>Georgiano</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Thaana</source>
        <translation>Thaana</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tibetan</source>
        <translation>Tibetano</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Myanmar</source>
        <translation>Birmano</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Khmer</source>
        <translation>Jemer</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Syriac</source>
        <translation>Siríaco</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ogham</source>
        <translation>Ogham</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Runic</source>
        <translation>Rúnico</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>N&apos;Ko</source>
        <translation>N&apos;Ko</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Symbols</source>
        <translation>Símbolos</translation>
    </message>
</context>
<context>
    <name>font-grid-model</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="+26"/>
        <source>Copy font family</source>
        <translation>Copiar familia de fuentes</translation>
    </message>
</context>
<context>
    <name>keybind-manager</name>
    <message>
        <location filename="../src/internal/keyboard/keybind-manager.cpp" line="+9"/>
        <source>Toggle action panel</source>
        <translation>Mostrar/ocultar panel de acciones</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Toggle the action panel to access and filter through the list of available actions for the currently selected item</source>
        <translation>Mostrar/ocultar el panel de acciones para acceder y filtrar la lista de acciones disponibles para el elemento seleccionado</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Open Search Filter</source>
        <translation>Buscar filtro</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the search filter selector if present</source>
        <translation>Abrir el selector de filtro de búsqueda si existe</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Open settings window</source>
        <translation>Abrir ventana de configuración</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open this settings window from the launcher window</source>
        <translation>Abrir esta ventana de configuración desde la ventana del lanzador</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Open Action</source>
        <translation>Acción de abrir genérica</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can open the selected item</source>
        <translation>Puede ser utilizado por acciones que pueden abrir el elemento seleccionado</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Copy Action</source>
        <translation>Acción de copiar genérica</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the selected item</source>
        <translation>Puede ser utilizado por acciones que pueden copiar el elemento seleccionado</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Name Action</source>
        <translation>Acción de copiar nombre</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the name of the selected item</source>
        <translation>Puede ser utilizado por acciones que pueden copiar el nombre del elemento seleccionado</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Path Action</source>
        <translation>Acción de copiar ruta</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the path of the selected item</source>
        <translation>Puede ser utilizado por acciones que pueden copiar la ruta del elemento seleccionado</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Save Action</source>
        <translation>Acción de guardar</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can save the selected item</source>
        <translation>Puede ser utilizado por acciones que pueden guardar el elemento seleccionado</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Duplicate Action</source>
        <translation>Acción de duplicar</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can duplicate the selected item</source>
        <translation>Puede ser utilizado por acciones que pueden duplicar el elemento seleccionado</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic New Action</source>
        <translation>Acción de nuevo genérica</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that create something</source>
        <translation>Puede ser utilizado por acciones que crean algo</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Move Up Action</source>
        <translation>Acción de mover hacia arriba genérica</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can move up the selected item. This does not affect list navigation controls.</source>
        <translation>Puede ser utilizado por acciones que pueden mover hacia arriba el elemento seleccionado. Esto no afecta los controles de navegación de lista.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Move Down Action</source>
        <translation>Acción de mover hacia abajo genérica</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can move down the selected item. This does not affect list navigation controls.</source>
        <translation>Puede ser utilizado por acciones que pueden mover hacia abajo el elemento seleccionado. Esto no afecta los controles de navegación de lista.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Refresh Action</source>
        <translation>Acción de actualizar genérica</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can refresh the selected item</source>
        <translation>Puede ser utilizado por acciones que pueden actualizar el elemento seleccionado</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Pin Action</source>
        <translation>Acción de anclar genérica</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can pin the selected item</source>
        <translation>Puede ser utilizado por acciones que pueden anclar el elemento seleccionado</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Remove Action</source>
        <translation>Acción de eliminar genérica</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can remove the selected item. This is normally used for small, not too impactful removals.</source>
        <translation>Puede ser utilizado por acciones que pueden eliminar el elemento seleccionado. Esto se utiliza normalmente para eliminaciones pequeñas, no demasiado impactantes.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Dangerous Remove Action</source>
        <translation>Acción de eliminado peligroso</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that perform an impactful removal, generally accompanied by a confirmation dialog.</source>
        <translation>Puede ser utilizado por acciones que realizan una eliminación con impacto, generalmente acompañada por un diálogo de confirmación.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit Action</source>
        <translation>Acción de edición</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can edit the currently selected item</source>
        <translation>Puede ser utilizado por acciones que pueden editar el elemento seleccionado</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit Secondary Action</source>
        <translation>Acción de edición secundaria</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can edit a secondary characteristic of the currently selected item</source>
        <translation>Puede ser utilizado por acciones que pueden editar un característica secundaria del elemento seleccionado</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run as Administrator</source>
        <translation>Ejecutar como administrador</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Runs the application with elevated privileges</source>
        <translation>Ejecuta la aplicación con privilegios elevados</translation>
    </message>
</context>
<context>
    <name>macos-update-installer</name>
    <message>
        <location filename="../src/services/update/macos-update-installer.mm" line="-184"/>
        <source>Update image contains more than one app</source>
        <translation>La imagen de actualización contiene más de una aplicación</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to list update image: %1</source>
        <translation>No se pudo listar la imagen de actualización: %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No app found in update image</source>
        <translation>No se ha encontrado ninguna aplicación en la imagen de actualización</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to read the update&apos;s code signature</source>
        <translation>No se ha podido leer la firma de código de la actualización</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to build the signature requirement</source>
        <translation>No se ha podido generar el requisito de firma</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Update signature verification failed (%1)</source>
        <translation>La actualización de la firma falló (%1)</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Update has no CFBundleShortVersionString</source>
        <translation>La actualización no tiene CFBundleShortVersionString</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Update version mismatch: expected %1, found %2</source>
        <translation>La versión de la actualización no coincide: se esperaba %1, se encontró %2</translation>
    </message>
</context>
<context>
    <name>shortcut-conflict</name>
    <message>
        <location filename="../src/qml/shortcut-conflict.cpp" line="+10"/>
        <source>Modifier required</source>
        <translation>Modificador necesario</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+5"/>
        <source>Already bound to &quot;%1&quot;</source>
        <translation>Ya está vinculado a &quot;%1&quot;</translation>
    </message>
</context>
<context>
    <name>system-extension</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-34"/>
        <source>Volume %1%</source>
        <translation>Volumen %1%</translation>
    </message>
</context>
<context>
    <name>utils</name>
    <message>
        <location filename="../src/utils/utils.cpp" line="+88"/>
        <source>0 bytes</source>
        <translation>0 bytes</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>bytes</source>
        <translation>bytes</translation>
    </message>
</context>
<context>
    <name>virtual-desktops</name>
    <message>
        <location filename="../src/services/window-manager/windows/virtual-desktops.cpp" line="+67"/>
        <source>Desktop %1</source>
        <translation>Escritorio %1</translation>
    </message>
</context>
<context>
    <name>TrayService</name>
    <message>
        <source>Toggle Vicinae</source>
        <translation>Mostrar/ocultar Vicinae</translation>
    </message>
    <message>
        <source>About Vicinae</source>
        <translation>Información sobre Vicinae</translation>
    </message>
    <message>
        <source>Check for Updates…</source>
        <translation>Buscar actualizaciones…</translation>
    </message>
    <message>
        <source>Update Available: %1</source>
        <translation>Actualización disponible: %1</translation>
    </message>
    <message>
        <source>Settings…</source>
        <translation>Configuración…</translation>
    </message>
    <message>
        <source>Preferences…</source>
        <translation>Preferencias…</translation>
    </message>
    <message>
        <source>Sponsor Vicinae</source>
        <translation>Patrocinar Vicinae</translation>
    </message>
    <message>
        <source>Join the Discord</source>
        <translation>Unirse al servidor de Discord</translation>
    </message>
    <message>
        <source>Follow on X</source>
        <translation>Seguir en X</translation>
    </message>
    <message>
        <source>Quit Vicinae</source>
        <translation>Salir de Vicinae</translation>
    </message>
</context>
</TS>
