<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="fr">
<context>
    <name>AboutSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AboutSettingsPage.qml" line="+57"/>
        <source>Version %1 - Commit %2
(%3)</source>
        <translation>Version %1 - Commit %2
(%3)</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Documentation</source>
        <translation>Documentation</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Report a Bug</source>
        <translation>Signaler un bug</translation>
    </message>
</context>
<context>
    <name>ActionListPanel</name>
    <message>
        <location filename="../src/qml/qml/ActionListPanel.qml" line="+113"/>
        <source>No matching actions</source>
        <translation>Aucune action correspondante</translation>
    </message>
    <message>
        <location line="+135"/>
        <source>Filter actions...</source>
        <translation>Filtrer les actions...</translation>
    </message>
</context>
<context>
    <name>AdvancedSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AdvancedSettingsPage.qml" line="+29"/>
        <source>Input &amp; Navigation</source>
        <translation>Saisie et navigation</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Pop on backspace</source>
        <translation>Retour avec Retour arrière</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pop back in navigation on backspace when no input is present.</source>
        <translation>Revenir à la vue précédente avec la touche Retour arrière lorsque le champ de saisie est vide.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Activate on single click</source>
        <translation>Activer d’un simple clic</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activate items with a single click instead of requiring a double click.</source>
        <translation>Activer les éléments d’un simple clic au lieu de nécessiter un double clic.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Wrap navigation</source>
        <translation>Navigation circulaire</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Wrap around to the opposite end when moving past the first or last item.</source>
        <translation>Revenir à l’extrémité opposée en dépassant le premier ou le dernier élément.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>IME handling</source>
        <translation>Gestion de l’IME</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Include IME Preedit strings as part of search queries.</source>
        <translation>Inclure les chaînes de pré-édition de l’IME dans les requêtes de recherche.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Keybinding Scheme</source>
        <translation>Schéma de raccourcis clavier</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Default uses the standard macOS keys (arrows, Ctrl+N/P); Vim uses Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Opt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>Par défaut utilise les touches macOS standard (flèches, Ctrl+N/P) ; Vim utilise Ctrl+J/K et Ctrl+H/L ; Emacs utilise Ctrl+N/P et Ctrl+Opt+B/F pour la navigation, ainsi que l’édition Emacs dans la barre de recherche.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Default and Vim use Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Alt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>Par défaut et Vim utilisent Ctrl+J/K et Ctrl+H/L ; Emacs utilise Ctrl+N/P et Ctrl+Alt+B/F pour la navigation, ainsi que l’édition Emacs dans la barre de recherche.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Search</source>
        <translation>Recherche</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Root file search</source>
        <translation>Fichiers dans la recherche racine</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Files are searched asynchronously, so if enabled you should expect a slight delay for file search results to show up.</source>
        <translation>Les fichiers sont recherchés de manière asynchrone : si cette option est activée, un léger délai est à prévoir avant l’affichage des résultats de fichiers.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Favicon Fetching</source>
        <translation>Récupération des favicons</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The favicon provider used to load favicons where needed. Select &apos;None&apos; to turn off favicon loading.</source>
        <translation>Le fournisseur utilisé pour charger les favicons lorsque nécessaire. Sélectionnez ’Aucun’ pour désactiver le chargement des favicons.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>System</source>
        <translation>Système</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Input server</source>
        <translation>Serveur de saisie</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Whether to spawn the input server at startup. This needs to be enabled in order to support snippets, paste to active window, and other features that require input monitoring or injection.</source>
        <translation>Indique si le serveur de saisie doit être lancé au démarrage. Il doit être activé pour prendre en charge les snippets, le collage dans la fenêtre active et d’autres fonctionnalités nécessitant la surveillance ou l’injection de saisie.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Security</source>
        <translation>Sécurité</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Encrypt sensitive data</source>
        <translation>Chiffrer les données sensibles</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Encrypt sensitive data at rest, such as clipboard history and internal databases (OAuth tokens, extension local storage, API keys). Note that some components, such as on-disk clipboard history, may not be retroactively affected when toggling this option. Turning on this option may ask you to unlock your keychain. Requires a restart in order to apply.</source>
        <translation>Chiffrer les données sensibles au repos, comme l’historique du presse-papiers et les bases de données internes (jetons OAuth, stockage local des extensions, clés API). Notez que certains composants, comme l’historique du presse-papiers sur disque, peuvent ne pas être affectés rétroactivement lors du basculement de cette option. Activer cette option peut vous demander de déverrouiller votre trousseau. Nécessite un redémarrage pour être appliqué.</translation>
    </message>
</context>
<context>
    <name>AlertWidget</name>
    <message>
        <location filename="../src/ui/alert/alert.hpp" line="+15"/>
        <source>Are you sure?</source>
        <translation>Êtes-vous sûr ?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>This action cannot be undone</source>
        <translation>Cette action est irréversible</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Confirm</source>
        <translation>Confirmer</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Cancel</source>
        <translation>Annuler</translation>
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
        <translation>Mots supplémentaires pour indexer cet élément</translation>
    </message>
</context>
<context>
    <name>AliasFormViewHost</name>
    <message>
        <location filename="../src/qml/alias-form-view-host.cpp" line="+28"/>
        <source>Set alias - %1</source>
        <translation>Définir l’alias - %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Submit</source>
        <translation>Valider</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Alias modified</source>
        <translation>Alias modifié</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to modify alias</source>
        <translation>Échec de la modification de l’alias</translation>
    </message>
</context>
<context>
    <name>AppRootItem</name>
    <message>
        <location filename="../src/root-search/apps/app-root-provider.cpp" line="+22"/>
        <location line="+29"/>
        <source>Application</source>
        <translation>Application</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Where</source>
        <translation>Emplacement</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Opens in terminal</source>
        <translation>S’ouvre dans un terminal</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Yes</source>
        <translation>Oui</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No</source>
        <translation>Non</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Open Application</source>
        <translation>Ouvrir l’application</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy App ID</source>
        <translation>Copier l’ID de l’application</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy App Location</source>
        <translation>Copier l’emplacement de l’application</translation>
    </message>
</context>
<context>
    <name>AppRootProvider</name>
    <message>
        <location line="+75"/>
        <source>Applications</source>
        <translation>Applications</translation>
    </message>
</context>
<context>
    <name>AppSelectorModel</name>
    <message>
        <location filename="../src/qml/app-selector-model.cpp" line="+20"/>
        <location line="+65"/>
        <source>%1 (Default)</source>
        <translation>%1 (par défaut)</translation>
    </message>
</context>
<context>
    <name>AppearanceSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/AppearanceSettingsPage.qml" line="+29"/>
        <location line="+7"/>
        <source>Theme</source>
        <translation>Thème</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Font</source>
        <translation>Police</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Font size</source>
        <translation>Taille de police</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The base point size used to compute font sizes. Fractional values are accepted. Recommended range is [10.0;12.0].</source>
        <translation>La taille de base en points utilisée pour calculer les tailles de police. Les valeurs fractionnaires sont acceptées. La plage recommandée est [10.0;12.0].</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>e.g. 11</source>
        <translation>ex. 11</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Icon Theme</source>
        <translation>Thème d’icônes</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The icon theme used for system icons (applications, mime types, folder icons...). Does not affect builtin Vicinae icons.</source>
        <translation>Le thème d’icônes utilisé pour les icônes système (applications, types MIME, icônes de dossiers...). N’affecte pas les icônes intégrées de Vicinae.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Window</source>
        <translation>Fenêtre</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Window material</source>
        <translation>Matériau de la fenêtre</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Background material applied to the launcher window. Lower the window opacity to see it.</source>
        <translation>Matériau d’arrière-plan appliqué à la fenêtre du lanceur. Réduisez l’opacité de la fenêtre pour le voir.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Window opacity</source>
        <translation>Opacité de la fenêtre</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>e.g. 1.0</source>
        <translation>ex. 1.0</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Compact mode</source>
        <translation>Mode compact</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Show only the search bar at root; expand when a query is entered.</source>
        <translation>N’afficher que la barre de recherche à la racine ; s’étend lorsqu’une requête est saisie.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Use layer shell</source>
        <translation>Utiliser le layer shell</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Anchor the launcher as a Wayland layer surface (wlr-layer-shell) instead of a regular window. May require reopening Vicinae to fully apply.</source>
        <translation>Ancrer le lanceur comme une surface de couche Wayland (wlr-layer-shell) plutôt qu’une fenêtre classique. Peut nécessiter de rouvrir Vicinae pour s’appliquer complètement.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Client-side decorations</source>
        <translation>Décorations côté client</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Let Vicinae draw its own rounded borders and shadow instead of relying on the windowing system.</source>
        <translation>Laisser Vicinae dessiner ses propres bordures arrondies et son ombre plutôt que de s’en remettre au système de fenêtrage.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Corner rounding</source>
        <translation>Arrondi des coins</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Radius of the launcher window corners, in pixels.</source>
        <translation>Rayon des coins de la fenêtre du lanceur, en pixels.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>e.g. 10</source>
        <translation>ex. 10</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Border width</source>
        <translation>Épaisseur de la bordure</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Thickness of the launcher window border, in pixels.</source>
        <translation>Épaisseur de la bordure de la fenêtre du lanceur, en pixels.</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>e.g. 3</source>
        <translation>ex. 3</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Shadow size</source>
        <translation>Taille de l’ombre</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Size of the drop shadow cast by the launcher window, in pixels.</source>
        <translation>Taille de l’ombre portée par la fenêtre du lanceur, en pixels.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>e.g. 12</source>
        <translation>ex. 12</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Native font rendering</source>
        <translation>Rendu de police natif</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Use the platform&apos;s native text rendering for system-consistent text. Disable for Qt distance-field rendering (usually faster). May require reopening Vicinae to fully apply.</source>
        <translation>Utiliser le rendu de texte natif de la plateforme pour un texte cohérent avec le système. Désactivez pour utiliser le rendu distance-field de Qt (généralement plus rapide). Peut nécessiter de rouvrir Vicinae pour s’appliquer complètement.</translation>
    </message>
</context>
<context>
    <name>AvailableFallbackSection</name>
    <message>
        <location filename="../src/qml/manage-fallback-model.hpp" line="+49"/>
        <source>Available</source>
        <translation>Disponibles</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-fallback-model.cpp" line="+50"/>
        <source>Enable fallback</source>
        <translation>Activer la recherche de secours</translation>
    </message>
</context>
<context>
    <name>BringToWorkspaceAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="+72"/>
        <source>Bring to current workspace</source>
        <translation>Amener sur l’espace de travail actuel</translation>
    </message>
</context>
<context>
    <name>BrowseAppsSection</name>
    <message>
        <location filename="../src/qml/browse-apps-model.hpp" line="+32"/>
        <source>Applications ({count})</source>
        <translation>Applications ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/browse-apps-model.cpp" line="+21"/>
        <source>Hidden</source>
        <translation>Masquée</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Open Application</source>
        <translation>Ouvrir l’application</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Copy App ID</source>
        <translation>Copier l’ID de l’application</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy App Location</source>
        <translation>Copier l’emplacement de l’application</translation>
    </message>
</context>
<context>
    <name>BrowseAppsViewHost</name>
    <message>
        <location filename="../src/qml/browse-apps-view-host.cpp" line="+12"/>
        <source>Search apps...</source>
        <translation>Rechercher des applications...</translation>
    </message>
</context>
<context>
    <name>BrowseFontsCommand</name>
    <message>
        <location filename="../src/extensions/font/browse-fonts-command.hpp" line="+8"/>
        <source>Search Fonts</source>
        <translation>Rechercher des polices</translation>
    </message>
</context>
<context>
    <name>BrowserExtension</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.hpp" line="+12"/>
        <source>Browser Extension</source>
        <translation>Extension navigateur</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Browser extension related commands.</source>
        <translation>Commandes liées à l’extension navigateur.</translation>
    </message>
</context>
<context>
    <name>BrowserTabActionGenerator</name>
    <message>
        <location filename="../src/actions/browser-tab-actions.hpp" line="+24"/>
        <source>Switch to tab</source>
        <translation>Basculer vers l’onglet</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Convert to shortcut</source>
        <translation>Convertir en raccourci</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Convert tab to shortcut</source>
        <translation>Convertir l’onglet en raccourci</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Close tab</source>
        <translation>Fermer l’onglet</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to close tab: %1</source>
        <translation>Échec de la fermeture de l’onglet : %1</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy URL</source>
        <translation>Copier l’URL</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy Title</source>
        <translation>Copier le titre</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy ID</source>
        <translation>Copier l’ID</translation>
    </message>
</context>
<context>
    <name>BrowserTabProvider</name>
    <message>
        <location filename="../src/root-search/browser-tabs/browser-tabs-provider.hpp" line="+70"/>
        <source>Browser Tabs</source>
        <translation>Onglets de navigateur</translation>
    </message>
</context>
<context>
    <name>BrowserTabRootItem</name>
    <message>
        <location line="-51"/>
        <source>Browser Tab</source>
        <translation>Onglet de navigateur</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Tab</source>
        <translation>Onglet</translation>
    </message>
</context>
<context>
    <name>BrowserTabsSection</name>
    <message>
        <location filename="../src/qml/browser-tabs-model.hpp" line="+17"/>
        <source>Tabs ({count})</source>
        <translation>Onglets ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/browser-tabs-model.cpp" line="+15"/>
        <source>Muted</source>
        <translation>En sourdine</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Playing</source>
        <translation>En lecture</translation>
    </message>
</context>
<context>
    <name>BrowserTabsViewHost</name>
    <message>
        <location filename="../src/qml/browser-tabs-view-host.cpp" line="+11"/>
        <source>Search, focus and close tabs</source>
        <translation>Rechercher, activer et fermer des onglets</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsSection</name>
    <message>
        <location filename="../src/qml/builtin-icons-model.hpp" line="+20"/>
        <source>Icons ({count})</source>
        <translation>Icônes ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/builtin-icons-model.cpp" line="+15"/>
        <source>Copy Icon Name</source>
        <translation>Copier le nom de l’icône</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsViewHost</name>
    <message>
        <location filename="../src/qml/builtin-icons-view-host.cpp" line="+10"/>
        <source>Search icons...</source>
        <translation>Rechercher des icônes...</translation>
    </message>
</context>
<context>
    <name>CalcHistoryListView</name>
    <message>
        <location filename="../src/qml/qml/CalcHistoryListView.qml" line="+12"/>
        <source>No results</source>
        <translation>Aucun résultat</translation>
    </message>
</context>
<context>
    <name>CalcHistorySection</name>
    <message>
        <location filename="../src/qml/calc-history-model.cpp" line="+41"/>
        <source>Copy answer</source>
        <translation>Copier la réponse</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy question</source>
        <translation>Copier la question</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy question and answer</source>
        <translation>Copier la question et la réponse</translation>
    </message>
</context>
<context>
    <name>CalcHistoryViewHost</name>
    <message>
        <location filename="../src/qml/calc-history-view-host.cpp" line="+83"/>
        <source>Search past calculations...</source>
        <translation>Rechercher dans les calculs précédents...</translation>
    </message>
</context>
<context>
    <name>CalcLiveSection</name>
    <message>
        <location filename="../src/qml/calc-history-view-host.hpp" line="+27"/>
        <source>Calculator</source>
        <translation>Calculatrice</translation>
    </message>
</context>
<context>
    <name>CalculatorExtension</name>
    <message>
        <location filename="../src/extensions/calculator/calculator-extension.hpp" line="+73"/>
        <source>Calculator</source>
        <translation>Calculatrice</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Do maths, convert units or search past calculations...</source>
        <translation>Faire des calculs, convertir des unités ou rechercher des calculs passés...</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Calculator Backend</source>
        <translation>Moteur de la calculatrice</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Which backend to use to perform calculations</source>
        <translation>Le moteur à utiliser pour effectuer les calculs</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Refresh rates on startup</source>
        <translation>Actualiser les taux au démarrage</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Whether exchange rates should be refreshed every time the vicinae server is started. If the current backend does not support it, this is ignored.</source>
        <translation>Indique si les taux de change doivent être actualisés à chaque démarrage du serveur vicinae. Si le moteur actuel ne le prend pas en charge, ce réglage est ignoré.</translation>
    </message>
</context>
<context>
    <name>CalculatorHistoryCommand</name>
    <message>
        <location line="-86"/>
        <source>Calculator history</source>
        <translation>Historique de la calculatrice</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse past calculations. You need to copy the result of a calculation for it to be saved in history.</source>
        <translation>Parcourir les calculs passés. Vous devez copier le résultat d’un calcul pour qu’il soit enregistré dans l’historique.</translation>
    </message>
</context>
<context>
    <name>CalculatorRefreshRatesCommand</name>
    <message>
        <location line="+11"/>
        <source>Refresh Exchange Rates</source>
        <translation>Actualiser les taux de change</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Refresh exchange rates used by the calculator to provide currency conversion features. Not all backends may support currency conversions or manually refreshing the rates.</source>
        <translation>Actualiser les taux de change utilisés par la calculatrice pour offrir la conversion de devises. Tous les moteurs ne prennent pas forcément en charge la conversion de devises ni l’actualisation manuelle des taux.</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>%1 can&apos;t refresh rates</source>
        <translation>%1 ne peut pas actualiser les taux</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Refreshing rates...</source>
        <translation>Actualisation des taux...</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Rates successfully refreshed</source>
        <translation>Taux actualisés avec succès</translation>
    </message>
</context>
<context>
    <name>CalculatorResultDelegate</name>
    <message>
        <location filename="../src/qml/qml/CalculatorResultDelegate.qml" line="+40"/>
        <source>Question</source>
        <translation>Question</translation>
    </message>
    <message>
        <location line="+57"/>
        <source>Answer</source>
        <translation>Réponse</translation>
    </message>
</context>
<context>
    <name>CalculatorService</name>
    <message>
        <location filename="../src/services/calculator-service/calculator-service.cpp" line="+119"/>
        <source>Pinned</source>
        <translation>Épinglés</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Today</source>
        <translation>Aujourd’hui</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>This week</source>
        <translation>Cette semaine</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>This month</source>
        <translation>Ce mois-ci</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>This year</source>
        <translation>Cette année</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>A few years ago</source>
        <translation>Il y a quelques années</translation>
    </message>
</context>
<context>
    <name>CategoryFilterAccessory</name>
    <message>
        <location filename="../src/qml/qml/CategoryFilterAccessory.qml" line="+8"/>
        <source>All</source>
        <translation>Tout</translation>
    </message>
</context>
<context>
    <name>ChangeEmojiSkinToneAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+99"/>
        <source>%1 skin tone</source>
        <translation>Couleur de peau %1</translation>
    </message>
</context>
<context>
    <name>ClearClipboardHistoryCommand</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.cpp" line="+35"/>
        <source>Clear Clipboard History</source>
        <translation>Effacer l’historique du presse-papiers</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clear the clipboard history</source>
        <translation>Effacer l’historique du presse-papiers</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Are you sure?</source>
        <translation>Êtes-vous sûr ?</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Your clipboard history will be gone forever :(</source>
        <translation>Votre historique du presse-papiers sera perdu pour toujours :(</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to clear clipboard history</source>
        <translation>Échec de l’effacement de l’historique du presse-papiers</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard history cleared</source>
        <translation>Historique du presse-papiers effacé</translation>
    </message>
</context>
<context>
    <name>ClipboardClearCommand</name>
    <message>
        <location line="-39"/>
        <source>Clear Current Clipboard Data</source>
        <translation>Effacer les données actuelles du presse-papiers</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clear the current content of the clipboard</source>
        <translation>Effacer le contenu actuel du presse-papiers</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Failed to clear clipboard</source>
        <translation>Échec de l’effacement du presse-papiers</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard cleared</source>
        <translation>Presse-papiers effacé</translation>
    </message>
</context>
<context>
    <name>ClipboardExtension</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.hpp" line="+13"/>
        <source>Clipboard</source>
        <translation>Presse-papiers</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>System clipboard integration</source>
        <translation>Intégration du presse-papiers système</translation>
    </message>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-extension.cpp" line="+55"/>
        <source>Erase on startup</source>
        <translation>Effacer au démarrage</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Erase clipboard history every time the vicinae server is started</source>
        <translation>Effacer l’historique du presse-papiers à chaque démarrage du serveur Vicinae</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Clipboard monitoring</source>
        <translation>Surveillance du presse-papiers</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Whether clipboard activity is recorded in the history. Every clipboard action performed while this is turned off will not be recorded.</source>
        <translation>Détermine si l’activité du presse-papiers est enregistrée dans l’historique. Aucune action du presse-papiers effectuée lorsque cette option est désactivée ne sera enregistrée.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Ignore Passwords</source>
        <translation>Ignorer les mots de passe</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Ignore selections that can be identified as a password. This relies on the application providing an explicit hint that the selection is a password. While most password managers and private browser windows do, some might not implement this properly.</source>
        <translation>Ignorer les sélections pouvant être identifiées comme des mots de passe. Cela repose sur le fait que l’application indique explicitement que la sélection est un mot de passe. La plupart des gestionnaires de mots de passe et des fenêtres de navigation privée le font, mais certains peuvent ne pas l’implémenter correctement.</translation>
    </message>
</context>
<context>
    <name>ClipboardFilterAccessory</name>
    <message>
        <location filename="../src/qml/qml/ClipboardFilterAccessory.qml" line="+8"/>
        <source>All</source>
        <translation>Tout</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Text</source>
        <translation>Texte</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Images</source>
        <translation>Images</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Links</source>
        <translation>Liens</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Files</source>
        <translation>Fichiers</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryCommand</name>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-history-command.cpp" line="+16"/>
        <source>Paste</source>
        <translation>Coller</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy</source>
        <translation>Copier</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Default Action</source>
        <translation>Action par défaut</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to perform on pressing return. Paste is only available if your environment supports it.</source>
        <translation>L’action par défaut à effectuer en appuyant sur Entrée. Coller n’est disponible que si votre environnement le prend en charge.</translation>
    </message>
    <message>
        <location filename="../src/extensions/clipboard/clipboard-history-command.hpp" line="+11"/>
        <source>Clipboard History</source>
        <translation>Historique du presse-papiers</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse your clipboard&apos;s history, pin, edit and remove entries.</source>
        <translation>Parcourir l’historique de votre presse-papiers, épingler, modifier et supprimer des entrées.</translation>
    </message>
</context>
<context>
    <name>ClipboardHistorySection</name>
    <message>
        <location filename="../src/qml/clipboard-history-model.cpp" line="+58"/>
        <source>Open Settings</source>
        <translation>Ouvrir les paramètres</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryView</name>
    <message>
        <location filename="../src/qml/qml/ClipboardHistoryView.qml" line="+187"/>
        <source>Type</source>
        <translation>Type</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Size</source>
        <translation>Taille</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copied at</source>
        <translation>Copié le</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>Preview not available for this content type</source>
        <translation>Aperçu non disponible pour ce type de contenu</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryViewHost</name>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.hpp" line="+84"/>
        <source>Loading...</source>
        <translation>Chargement...</translation>
    </message>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.cpp" line="+101"/>
        <source>Browse clipboard history...</source>
        <translation>Parcourir l’historique du presse-papiers...</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard monitoring unavailable</source>
        <translation>Surveillance du presse-papiers indisponible</translation>
    </message>
    <message>
        <location line="+67"/>
        <source>Pause clipboard</source>
        <translation>Suspendre le presse-papiers</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Resume clipboard</source>
        <translation>Reprendre le presse-papiers</translation>
    </message>
    <message numerus="yes">
        <location line="+8"/>
        <source>%n Items</source>
        <translation>
            <numerusform>%n élément</numerusform>
            <numerusform>%n éléments</numerusform>
        </translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Decryption failed</source>
        <translation>Échec du déchiffrement</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Vicinae could not decrypt the data for this selection. It was most likely encrypted with a different key and cannot be recovered. You can remove this entry from the history.</source>
        <translation>Vicinae n’a pas pu déchiffrer les données de cette sélection. Elles ont très probablement été chiffrées avec une autre clé et ne peuvent pas être récupérées. Vous pouvez supprimer cette entrée de l’historique.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Data unavailable</source>
        <translation>Données indisponibles</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The data for this selection could not be found on disk.</source>
        <translation>Les données de cette sélection sont introuvables sur le disque.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Data is encrypted</source>
        <translation>Les données sont chiffrées</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Data for this selection was previously encrypted but the clipboard is not currently configured to use encryption. You should be able to fix this by enabling it in the settings.</source>
        <translation>Les données de cette sélection ont été chiffrées, mais le presse-papiers n’est pas actuellement configuré pour utiliser le chiffrement. Vous devriez pouvoir corriger cela en l’activant dans les paramètres.</translation>
    </message>
</context>
<context>
    <name>ClipboardService</name>
    <message>
        <location filename="../src/services/clipboard/clipboard-service.cpp" line="+316"/>
        <source>Image (%1x%2)</source>
        <translation>Image (%1x%2)</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Image</source>
        <translation>Image</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Unknown</source>
        <translation>Inconnu</translation>
    </message>
</context>
<context>
    <name>CloseWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="-37"/>
        <source>Close window</source>
        <translation>Fermer la fenêtre</translation>
    </message>
</context>
<context>
    <name>CommandLineSection</name>
    <message>
        <location filename="../src/qml/system-run-model.hpp" line="+19"/>
        <source>Execute query</source>
        <translation>Exécuter la requête</translation>
    </message>
    <message>
        <location filename="../src/qml/system-run-model.cpp" line="+44"/>
        <source>Open in %1 (hold)</source>
        <translation>Ouvrir dans %1 (maintenir)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open in %1</source>
        <translation>Ouvrir dans %1</translation>
    </message>
</context>
<context>
    <name>CommandListView</name>
    <message>
        <location filename="../src/qml/qml/CommandListView.qml" line="+12"/>
        <source>No results</source>
        <translation>Aucun résultat</translation>
    </message>
</context>
<context>
    <name>CommandRootItem</name>
    <message>
        <location filename="../src/root-search/extensions/extension-root-provider.cpp" line="+28"/>
        <location line="+48"/>
        <source>Command</source>
        <translation>Commande</translation>
    </message>
    <message>
        <location line="-43"/>
        <location line="+27"/>
        <source>Open command</source>
        <translation>Ouvrir la commande</translation>
    </message>
    <message>
        <location line="-13"/>
        <source>Copy extension path</source>
        <translation>Copier le chemin de l’extension</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Internal Command</source>
        <translation>Commande interne</translation>
    </message>
</context>
<context>
    <name>CompletionPopup</name>
    <message>
        <location filename="../src/qml/qml/CompletionPopup.qml" line="+11"/>
        <source>Filter...</source>
        <translation>Filtrer...</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorAnswerAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+29"/>
        <source>Answer copied to clipboard</source>
        <translation>Réponse copiée dans le presse-papiers</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy answer</source>
        <translation>Échec de la copie de la réponse</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy Result</source>
        <translation>Copier le résultat</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorQuestionAndAnswerAction</name>
    <message>
        <location line="+18"/>
        <source>Answer copied to clipboard</source>
        <translation>Réponse copiée dans le presse-papiers</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy answer</source>
        <translation>Échec de la copie de la réponse</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy Question And Answer</source>
        <translation>Copier la question et la réponse</translation>
    </message>
</context>
<context>
    <name>CopyClipboardSelection</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+29"/>
        <source>Selection copied to clipboard</source>
        <translation>Sélection copiée dans le presse-papiers</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to copy to clipboard</source>
        <translation>Échec de la copie dans le presse-papiers</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy to clipboard</source>
        <translation>Copier dans le presse-papiers</translation>
    </message>
</context>
<context>
    <name>CopyItemDeeplink</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+75"/>
        <source>Deeplink copied in clipboard</source>
        <translation>Deeplink copié dans le presse-papiers</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Deeplink</source>
        <translation>Copier le deeplink</translation>
    </message>
</context>
<context>
    <name>CopyShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="+230"/>
        <source>Copied to clipboard</source>
        <translation>Copié dans le presse-papiers</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy shortcut</source>
        <translation>Copier le raccourci</translation>
    </message>
</context>
<context>
    <name>CopyToClipboardAction</name>
    <message>
        <location filename="../src/clipboard-actions.hpp" line="+23"/>
        <source>Copied to clipboard</source>
        <translation>Copié dans le presse-papiers</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy to clipboard</source>
        <translation>Copier dans le presse-papiers</translation>
    </message>
</context>
<context>
    <name>CreateExtensionCommand</name>
    <message>
        <location filename="../src/extensions/developer/developer-extension.hpp" line="+10"/>
        <source>Create Extension</source>
        <translation>Créer une extension</translation>
    </message>
</context>
<context>
    <name>CreateExtensionFormView</name>
    <message>
        <location filename="../src/qml/qml/CreateExtensionFormView.qml" line="+15"/>
        <source>Author</source>
        <translation>Auteur</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>If you plan on submitting your extension to the &lt;a href=&quot;vicinae://launch/core/store&quot;&gt;Vicinae store&lt;/a&gt;, this must exactly match your GitHub handle. Otherwise, you can set it to anything.</source>
        <translation>Si vous prévoyez de soumettre votre extension à la &lt;a href=&quot;vicinae://launch/core/store&quot;&gt;boutique Vicinae&lt;/a&gt;, ceci doit correspondre exactement à votre identifiant GitHub. Sinon, vous pouvez y mettre ce que vous voulez.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Username</source>
        <translation>Nom d’utilisateur</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Extension Title</source>
        <translation>Titre de l’extension</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>My Extension</source>
        <translation>Mon extension</translation>
    </message>
    <message>
        <location line="+8"/>
        <location line="+42"/>
        <source>Description</source>
        <translation>Description</translation>
    </message>
    <message>
        <location line="-36"/>
        <source>An extension that does super cool things</source>
        <translation>Une extension qui fait des choses super cool</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Location</source>
        <translation>Emplacement</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Command Title</source>
        <translation>Titre de la commande</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>My Wonderful Command</source>
        <translation>Ma merveilleuse commande</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>My command does this, and that...</source>
        <translation>Ma commande fait ceci, et cela...</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Template</source>
        <translation>Modèle</translation>
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
# Extension créée avec succès

Votre nouvelle extension %1 a été créée avec succès dans `%2`.

Pour que les commandes de cette extension soient détectées par Vicinae, vous devez exécuter votre extension en mode développement au moins une fois :

```bash
cd %2
npm install
npm run dev
```

Vous pouvez en apprendre davantage sur le développement d’extensions dans la [documentation Vicinae](https://docs.vicinae.com/).
</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Open in %1</source>
        <translation>Ouvrir dans %1</translation>
    </message>
</context>
<context>
    <name>CreateExtensionViewHost</name>
    <message>
        <location filename="../src/qml/create-extension-view-host.cpp" line="+44"/>
        <source>Create extension</source>
        <translation>Créer une extension</translation>
    </message>
    <message>
        <location line="+18"/>
        <location line="+4"/>
        <location line="+19"/>
        <location line="+5"/>
        <source>Min. 3 chars</source>
        <translation>Min. 3 caractères</translation>
    </message>
    <message>
        <location line="-20"/>
        <source>Min. 16 chars</source>
        <translation>Min. 16 caractères</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Must exist</source>
        <translation>Doit exister</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Form has errors</source>
        <translation>Le formulaire contient des erreurs</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Failed to create extension</source>
        <translation>Échec de la création de l’extension</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Extension created!</source>
        <translation>Extension créée !</translation>
    </message>
</context>
<context>
    <name>CreateShortcutCommand</name>
    <message>
        <location filename="../src/extensions/shortcut/create-shortcut-command.hpp" line="+10"/>
        <source>Create Shortcut</source>
        <translation>Créer un raccourci</translation>
    </message>
</context>
<context>
    <name>CreateShortcutFromActiveBrowserTabCommand</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="+44"/>
        <source>Create Shortcut from Active Tab</source>
        <translation>Créer un raccourci depuis l’onglet actif</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Create a vicinae shortcut from the currently active browser tab. May yield unexpected results if many browsers are connected at once.</source>
        <translation>Créer un raccourci Vicinae depuis l’onglet de navigateur actuellement actif. Peut donner des résultats inattendus si plusieurs navigateurs sont connectés à la fois.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>No active tab!</source>
        <translation>Aucun onglet actif !</translation>
    </message>
</context>
<context>
    <name>CreateSnippetCommand</name>
    <message>
        <location filename="../src/extensions/snippet/create-snippet-command.hpp" line="+10"/>
        <source>Create Snippet</source>
        <translation>Créer un snippet</translation>
    </message>
</context>
<context>
    <name>DMenuSection</name>
    <message>
        <location filename="../src/qml/dmenu-model.cpp" line="+93"/>
        <source>Select entry</source>
        <translation>Sélectionner l’entrée</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pass search text</source>
        <translation>Transmettre le texte de recherche</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Select and copy entry</source>
        <translation>Sélectionner et copier l’entrée</translation>
    </message>
</context>
<context>
    <name>DMenuView</name>
    <message>
        <location filename="../src/qml/qml/DMenuView.qml" line="+79"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Path</source>
        <translation>Chemin</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>Type</translation>
    </message>
</context>
<context>
    <name>DMenuViewHost</name>
    <message>
        <location filename="../src/qml/dmenu-view-host.cpp" line="+34"/>
        <source>Search entries...</source>
        <translation>Rechercher des entrées...</translation>
    </message>
    <message>
        <location line="+73"/>
        <source>Pass search text</source>
        <translation>Transmettre le texte de recherche</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Pass and copy search text</source>
        <translation>Transmettre et copier le texte de recherche</translation>
    </message>
</context>
<context>
    <name>DetailListView</name>
    <message>
        <location filename="../src/qml/qml/DetailListView.qml" line="+32"/>
        <source>No results</source>
        <translation>Aucun résultat</translation>
    </message>
</context>
<context>
    <name>DeveloperExtension</name>
    <message>
        <location filename="../src/extensions/developer/developer-extension.hpp" line="+10"/>
        <source>Developer</source>
        <translation>Développeur</translation>
    </message>
</context>
<context>
    <name>DisableApplication</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+8"/>
        <source>Disable item</source>
        <translation>Désactiver l’élément</translation>
    </message>
</context>
<context>
    <name>DisableItemAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="+88"/>
        <source>Are you sure?</source>
        <translation>Êtes-vous sûr ?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You will need to go in the settings to manually re-enable it.</source>
        <translation>Vous devrez aller dans les paramètres pour le réactiver manuellement.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Disable</source>
        <translation>Désactiver</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Item disabled</source>
        <translation>Élément désactivé</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to disable</source>
        <translation>Échec de la désactivation</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Disable item</source>
        <translation>Désactiver l’élément</translation>
    </message>
</context>
<context>
    <name>DismissNewsAction</name>
    <message>
        <location filename="../src/services/news/news-service.cpp" line="+34"/>
        <source>Dismiss</source>
        <translation>Ignorer</translation>
    </message>
</context>
<context>
    <name>DuplicateShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-50"/>
        <source>Duplicate link</source>
        <translation>Dupliquer le lien</translation>
    </message>
</context>
<context>
    <name>EditClipboardKeywordsAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+49"/>
        <source>Additional keywords that will be used to index this selection.</source>
        <translation>Mots-clés supplémentaires qui seront utilisés pour indexer cette sélection.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit keywords</source>
        <translation>Modifier les mots-clés</translation>
    </message>
</context>
<context>
    <name>EditEmojiKeywordsAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+35"/>
        <source>Additional keywords that will be used to index this glyph</source>
        <translation>Mots-clés supplémentaires qui seront utilisés pour indexer ce glyphe</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Edit keyword</source>
        <translation>Modifier le mot-clé</translation>
    </message>
</context>
<context>
    <name>EditKeywordsFormView</name>
    <message>
        <location filename="../src/qml/qml/EditKeywordsFormView.qml" line="+19"/>
        <source>Keywords</source>
        <translation>Mots-clés</translation>
    </message>
</context>
<context>
    <name>EditKeywordsViewHost</name>
    <message>
        <location filename="../src/qml/edit-keywords-view-host.cpp" line="+26"/>
        <source>Submit</source>
        <translation>Valider</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Keywords edited</source>
        <translation>Mots-clés modifiés</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to edit keywords</source>
        <translation>Échec de la modification des mots-clés</translation>
    </message>
</context>
<context>
    <name>EditShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-41"/>
        <source>Edit shortcut</source>
        <translation>Modifier le raccourci</translation>
    </message>
</context>
<context>
    <name>EmojiCategoryFilterAccessory</name>
    <message>
        <location filename="../src/qml/qml/EmojiCategoryFilterAccessory.qml" line="+8"/>
        <source>All</source>
        <translation>Tout</translation>
    </message>
</context>
<context>
    <name>EmojiGridModel</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.hpp" line="+58"/>
        <source>Search for emojis and symbols...</source>
        <translation>Rechercher des émojis et des symboles...</translation>
    </message>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+241"/>
        <source>Pinned</source>
        <translation>Épinglés</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Recently used</source>
        <translation>Utilisés récemment</translation>
    </message>
</context>
<context>
    <name>EmojiGridViewHost</name>
    <message>
        <location filename="../src/qml/emoji-grid-view-host.hpp" line="+40"/>
        <source>All</source>
        <translation>Tous</translation>
    </message>
</context>
<context>
    <name>EmptyView</name>
    <message>
        <location filename="../src/qml/qml/EmptyView.qml" line="+7"/>
        <source>No results</source>
        <translation>Aucun résultat</translation>
    </message>
</context>
<context>
    <name>EnabledFallbackSection</name>
    <message>
        <location filename="../src/qml/manage-fallback-model.hpp" line="-19"/>
        <source>Enabled</source>
        <translation>Activées</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-fallback-model.cpp" line="-22"/>
        <source>Disable fallback</source>
        <translation>Désactiver la recherche de secours</translation>
    </message>
</context>
<context>
    <name>Expansion</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.hpp" line="+31"/>
        <source>Keyword cannot be empty</source>
        <translation>Le mot-clé ne peut pas être vide</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Keyword exceeds maximum length of %1</source>
        <translation>Le mot-clé dépasse la longueur maximale de %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keyword must only contain printable ASCII characters (no spaces)</source>
        <translation>Le mot-clé ne doit contenir que des caractères ASCII imprimables (sans espaces)</translation>
    </message>
</context>
<context>
    <name>ExtensionBoilerplateGenerator</name>
    <message>
        <location filename="../src/services/extension-boilerplate-generator/extension-boilerplate-generator.cpp" line="+24"/>
        <source>Simple List</source>
        <translation>Liste simple</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>List with Detail</source>
        <translation>Liste avec détail</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Controlled List</source>
        <translation>Liste contrôlée</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Simple Detail</source>
        <translation>Détail simple</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>No View</source>
        <translation>Sans vue</translation>
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
        <translation># L’extension a planté 💥 !

Cette extension a levé une exception non interceptée et a donc planté.

Vous trouverez la stacktrace complète ci-dessous. Vous pouvez aussi la copier directement depuis le menu d’actions.

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
        <translation>Un ou plusieurs champs contiennent des erreurs</translation>
    </message>
</context>
<context>
    <name>ExtensionGridModel</name>
    <message>
        <location filename="../src/qml/extension-grid-model.cpp" line="+186"/>
        <source>Search...</source>
        <translation>Rechercher...</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>No results</source>
        <translation>Aucun résultat</translation>
    </message>
</context>
<context>
    <name>ExtensionGridView</name>
    <message>
        <location filename="../src/qml/qml/ExtensionGridView.qml" line="+9"/>
        <source>No results</source>
        <translation>Aucun résultat</translation>
    </message>
</context>
<context>
    <name>ExtensionListModel</name>
    <message>
        <location filename="../src/qml/extension-list-model.cpp" line="+189"/>
        <source>Search...</source>
        <translation>Rechercher...</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No results</source>
        <translation>Aucun résultat</translation>
    </message>
</context>
<context>
    <name>ExtensionSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/ExtensionSettingsPage.qml" line="+94"/>
        <source>Description</source>
        <translation>Description</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Preferences</source>
        <translation>Préférences</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Commands</source>
        <translation>Commandes</translation>
    </message>
    <message>
        <location line="+105"/>
        <source>Shortcut</source>
        <translation>Raccourci</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Add Alias</source>
        <translation>Ajouter un alias</translation>
    </message>
</context>
<context>
    <name>ExtensionView</name>
    <message>
        <location filename="../src/qml/qml/ExtensionView.qml" line="+99"/>
        <source>No results</source>
        <translation>Aucun résultat</translation>
    </message>
</context>
<context>
    <name>FileExtension</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="+65"/>
        <source>System files</source>
        <translation>Fichiers système</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Integrate with system files</source>
        <translation>Intégration avec les fichiers système</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Enabled</source>
        <translation>Activé</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Whether to run the file indexer in the background. When turned off, the indexer process is stopped entirely and file search becomes unavailable until it is turned back on.</source>
        <translation>Indique si l’indexeur de fichiers doit s’exécuter en arrière-plan. Lorsqu’il est désactivé, le processus d’indexation est entièrement arrêté et la recherche de fichiers devient indisponible jusqu’à sa réactivation.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Search paths</source>
        <translation>Chemins de recherche</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Directories that Vicinae will search</source>
        <translation>Répertoires dans lesquels Vicinae effectuera ses recherches</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Excluded search paths</source>
        <translation>Chemins de recherche exclus</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Directories to exclude from file indexing</source>
        <translation>Répertoires à exclure de l’indexation des fichiers</translation>
    </message>
</context>
<context>
    <name>FilePreview</name>
    <message>
        <location filename="../src/qml/qml/FilePreview.qml" line="+42"/>
        <source>Preview not available for this file type</source>
        <translation>Aperçu non disponible pour ce type de fichier</translation>
    </message>
</context>
<context>
    <name>FocusWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="-17"/>
        <source>Focus window</source>
        <translation>Activer la fenêtre</translation>
    </message>
</context>
<context>
    <name>FontBrowserViewHost</name>
    <message>
        <location filename="../src/qml/font-browser-view-host.hpp" line="+38"/>
        <source>All</source>
        <translation>Toutes</translation>
    </message>
</context>
<context>
    <name>FontExtension</name>
    <message>
        <location filename="../src/extensions/font/font-extension.hpp" line="+9"/>
        <source>Font</source>
        <translation>Police</translation>
    </message>
</context>
<context>
    <name>FontGridModel</name>
    <message>
        <location filename="../src/qml/font-grid-model.hpp" line="+47"/>
        <source>Search fonts...</source>
        <translation>Rechercher des polices...</translation>
    </message>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="+133"/>
        <source>All Fonts (%1)</source>
        <translation>Toutes les polices (%1)</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Results (%1)</source>
        <translation>Résultats (%1)</translation>
    </message>
</context>
<context>
    <name>Footer</name>
    <message>
        <location filename="../src/qml/qml/Footer.qml" line="+60"/>
        <source>Actions</source>
        <translation>Actions</translation>
    </message>
</context>
<context>
    <name>ForceQuitAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+95"/>
        <source>Force Quit Application</source>
        <translation>Forcer la fermeture de l’application</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to force quit %1</source>
        <translation>Échec de la fermeture forcée de %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Force quit %1</source>
        <translation>Forcer la fermeture de %1</translation>
    </message>
</context>
<context>
    <name>ForgetTelemetryCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+210"/>
        <source>Forget Past Vicinae Telemetry</source>
        <translation>Oublier la télémétrie passée de Vicinae</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Asks the vicinae server to anonymize telemetry data that was sent with your vicinae instance ID attached. The ID is only linked to your vicinae install, which has no direct relationship with your system.</source>
        <translation>Demande au serveur vicinae d’anonymiser les données de télémétrie envoyées avec l’ID de votre instance vicinae. Cet ID est uniquement lié à votre installation vicinae, qui n’a aucun lien direct avec votre système.</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Processing...</source>
        <translation>Traitement...</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Past telemetry was successfully detached from your vicinae user ID.</source>
        <translation>La télémétrie passée a bien été dissociée de votre ID utilisateur vicinae.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to forget past telemetry data</source>
        <translation>Impossible d’oublier les données de télémétrie passées</translation>
    </message>
</context>
<context>
    <name>FormAppSelector</name>
    <message>
        <location filename="../src/qml/qml/FormAppSelector.qml" line="+50"/>
        <source>All applications</source>
        <translation>Toutes les applications</translation>
    </message>
    <message>
        <location line="+70"/>
        <source>+ Restrict to app…</source>
        <translation>+ Restreindre à une app…</translation>
    </message>
</context>
<context>
    <name>FormFilePicker</name>
    <message>
        <location filename="../src/qml/qml/FormFilePicker.qml" line="+84"/>
        <source>Select files</source>
        <translation>Sélectionner des fichiers</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Select a file</source>
        <translation>Sélectionner un fichier</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Select a directory</source>
        <translation>Sélectionner un dossier</translation>
    </message>
    <message>
        <location line="+46"/>
        <source>No directory selected</source>
        <translation>Aucun dossier sélectionné</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No file selected</source>
        <translation>Aucun fichier sélectionné</translation>
    </message>
    <message>
        <location line="+115"/>
        <source>+ Add folder…</source>
        <translation>+ Ajouter un dossier…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>+ Add file…</source>
        <translation>+ Ajouter un fichier…</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsModel</name>
    <message>
        <location filename="../src/qml/general-settings-model.cpp" line="+176"/>
        <location line="+11"/>
        <source>None</source>
        <translation>Aucun</translation>
    </message>
    <message>
        <location line="-10"/>
        <location line="+10"/>
        <source>Blurred</source>
        <translation>Flouté</translation>
    </message>
    <message>
        <location line="-8"/>
        <location line="+8"/>
        <source>Liquid Glass</source>
        <translation>Liquid Glass</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Window material</source>
        <translation>Matériau de la fenêtre</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Themes</source>
        <translation>Thèmes</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Fonts</source>
        <translation>Polices</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Icon Themes</source>
        <translation>Thèmes d’icônes</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Favicon Services</source>
        <translation>Services de favicons</translation>
    </message>
    <message>
        <location line="+13"/>
        <location line="+10"/>
        <source>Default</source>
        <translation>Par défaut</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Keybinding Schemes</source>
        <translation>Schémas de raccourcis clavier</translation>
    </message>
    <message>
        <location line="+48"/>
        <location line="+10"/>
        <source>System default</source>
        <translation>Langue du système</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>Languages</source>
        <translation>Langues</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/GeneralSettingsPage.qml" line="+29"/>
        <source>Behavior</source>
        <translation>Comportement</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Launcher hotkey</source>
        <translation>Raccourci du lanceur</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Global shortcut to toggle the Vicinae launcher.</source>
        <translation>Raccourci global pour afficher ou masquer le lanceur Vicinae.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Close on focus loss</source>
        <translation>Fermer à la perte du focus</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Close on Escape</source>
        <translation>Fermer avec Échap</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pressing Escape closes the launcher instead of navigating one view back.</source>
        <translation>Appuyer sur Échap ferme le lanceur au lieu de revenir à la vue précédente.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Pop to root on close</source>
        <translation>Revenir à la racine à la fermeture</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reset the navigation state when the launcher window is closed.</source>
        <translation>Réinitialiser l’état de navigation lorsque la fenêtre du lanceur est fermée.</translation>
    </message>
    <message>
        <location line="+10"/>
        <location line="+7"/>
        <source>Language</source>
        <translation>Langue</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Requires restarting Vicinae to take effect.</source>
        <translation>Nécessite un redémarrage de Vicinae pour prendre effet.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Privacy</source>
        <translation>Confidentialité</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Basic usage statistics</source>
        <translation>Statistiques d’utilisation de base</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Send basic system and vicinae installation information on startup to help improve Vicinae.</source>
        <translation>Envoyer au démarrage des informations de base sur le système et l’installation de Vicinae afin d’aider à améliorer Vicinae.</translation>
    </message>
</context>
<context>
    <name>GenericGridView</name>
    <message>
        <location filename="../src/qml/qml/GenericGridView.qml" line="+33"/>
        <source>No results</source>
        <translation>Aucun résultat</translation>
    </message>
</context>
<context>
    <name>GenericListView</name>
    <message>
        <location filename="../src/qml/qml/GenericListView.qml" line="+25"/>
        <source>No results</source>
        <translation>Aucun résultat</translation>
    </message>
</context>
<context>
    <name>GlobalShortcutService</name>
    <message>
        <location filename="../src/services/global-shortcuts/global-shortcut-service.cpp" line="+43"/>
        <source>Toggle Vicinae</source>
        <translation>Afficher/Masquer Vicinae</translation>
    </message>
    <message>
        <location line="+84"/>
        <source>the launcher hotkey</source>
        <translation>le raccourci clavier du lanceur</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>another command</source>
        <translation>une autre commande</translation>
    </message>
</context>
<context>
    <name>Gnome::Workspace</name>
    <message>
        <location filename="../src/services/window-manager/gnome/gnome-workspace.cpp" line="+18"/>
        <source>Workspace %1</source>
        <translation>Espace de travail %1</translation>
    </message>
</context>
<context>
    <name>HibernateCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+127"/>
        <source>Hibernate System</source>
        <translation>Mettre le système en veille prolongée</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Suspend the system to disk. This turns off the system completely and saves its state on disk, to be restored on next boot.</source>
        <translation>Suspendre le système sur disque. Cela éteint complètement le système et enregistre son état sur disque, afin de le restaurer au prochain démarrage.</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>System can&apos;t hibernate</source>
        <translation>Le système ne peut pas passer en veille prolongée</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to hibernate</source>
        <translation>Échec de la mise en veille prolongée</translation>
    </message>
</context>
<context>
    <name>IconBrowserCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+25"/>
        <source>Search Builtin Icons</source>
        <translation>Rechercher les icônes intégrées</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search Vicinae builtin set of icons</source>
        <translation>Rechercher dans le jeu d’icônes intégré de Vicinae</translation>
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
        <translation>Inspecter le stockage local</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Browse data stored in Vicinae&apos;s local storage. This includes data stored for builtin extensions as well as third-party extensions making use of the LocalStorage API.</source>
        <translation>Parcourir les données du stockage local de Vicinae. Cela inclut les données des extensions intégrées ainsi que celles des extensions tierces utilisant l’API LocalStorage.</translation>
    </message>
</context>
<context>
    <name>InstallUpdateAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+188"/>
        <source>Install Update</source>
        <translation>Installer la mise à jour</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>An update is already in progress</source>
        <translation>Une mise à jour est déjà en cours</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsSection</name>
    <message>
        <location filename="../src/qml/installed-extensions-model.hpp" line="+18"/>
        <source>Installed Extensions ({count})</source>
        <translation>Extensions installées ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/installed-extensions-model.cpp" line="+36"/>
        <source>Local</source>
        <translation>Locale</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Copy</source>
        <translation>Copier</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy Name</source>
        <translation>Copier le nom</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy ID</source>
        <translation>Copier l’ID</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy Path</source>
        <translation>Copier le chemin</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy Author</source>
        <translation>Copier l’auteur</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsViewHost</name>
    <message>
        <location filename="../src/qml/installed-extensions-view-host.cpp" line="+12"/>
        <source>Search extensions...</source>
        <translation>Rechercher des extensions...</translation>
    </message>
</context>
<context>
    <name>InternalExtension</name>
    <message>
        <location filename="../src/extensions/internal/internal-extension.hpp" line="+32"/>
        <location line="+1"/>
        <source>Internal Commands</source>
        <translation>Commandes internes</translation>
    </message>
</context>
<context>
    <name>KeyboardBridge</name>
    <message>
        <location filename="../src/qml/keyboard-bridge.hpp" line="+51"/>
        <source>Modifier required</source>
        <translation>Modificateur requis</translation>
    </message>
</context>
<context>
    <name>LauncherWindow</name>
    <message>
        <location filename="../src/qml/qml/LauncherWindow.qml" line="+29"/>
        <source>Vicinae Launcher</source>
        <translation>Lanceur Vicinae</translation>
    </message>
    <message>
        <location filename="../src/qml/launcher-window.cpp" line="+571"/>
        <source>Open Settings</source>
        <translation>Ouvrir les paramètres</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keyboard Shortcuts</source>
        <translation>Raccourcis clavier</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Documentation</source>
        <translation>Documentation</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+5"/>
        <source>Opened in browser</source>
        <translation>Ouvert dans le navigateur</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Report a Bug</source>
        <translation>Signaler un bug</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>About Vicinae</source>
        <translation>À propos de Vicinae</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemSection</name>
    <message>
        <location filename="../src/qml/local-storage-model.hpp" line="+19"/>
        <source>Items ({count})</source>
        <translation>Éléments ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/local-storage-model.cpp" line="+31"/>
        <source>Show value</source>
        <translation>Afficher la valeur</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemViewHost</name>
    <message>
        <location filename="../src/qml/local-storage-view-host.cpp" line="+23"/>
        <source>Search items...</source>
        <translation>Rechercher des éléments...</translation>
    </message>
</context>
<context>
    <name>LocalStorageNamespaceSection</name>
    <message>
        <location filename="../src/qml/local-storage-model.hpp" line="-11"/>
        <source>Namespaces ({count})</source>
        <translation>Espaces de noms ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/local-storage-model.cpp" line="-16"/>
        <source>Browse namespace</source>
        <translation>Parcourir l’espace de noms</translation>
    </message>
</context>
<context>
    <name>LocalStorageViewHost</name>
    <message>
        <location filename="../src/qml/local-storage-view-host.cpp" line="-15"/>
        <source>Search namespaces...</source>
        <translation>Rechercher des espaces de noms...</translation>
    </message>
</context>
<context>
    <name>LockCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-48"/>
        <source>Lock Session</source>
        <translation>Verrouiller la session</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lock the current user session</source>
        <translation>Verrouiller la session utilisateur en cours</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System can&apos;t lock</source>
        <translation>Le système ne peut pas se verrouiller</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to lock</source>
        <translation>Échec du verrouillage</translation>
    </message>
</context>
<context>
    <name>LogOutCommand</name>
    <message>
        <location line="+176"/>
        <source>Log Out</source>
        <translation>Fermer la session</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Terminate the current user session. If you simply want to lock your session you should use &apos;Lock Session&apos; instead.</source>
        <translation>Mettre fin à la session utilisateur en cours. Si vous voulez simplement verrouiller votre session, utilisez plutôt ’Verrouiller la session’.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>System can&apos;t logout</source>
        <translation>Le système ne peut pas fermer la session</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to log out</source>
        <translation>Échec de la fermeture de session</translation>
    </message>
</context>
<context>
    <name>MacOSGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/macos-global-shortcut-backend.cpp" line="+238"/>
        <source>unsupported or invalid trigger</source>
        <translation>déclencheur non pris en charge ou non valide</translation>
    </message>
</context>
<context>
    <name>MacSettingsRootItem</name>
    <message>
        <location filename="../src/root-search/macos-settings/macos-settings-root-provider.mm" line="+132"/>
        <location line="+9"/>
        <source>System Settings</source>
        <translation>Réglages Système</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Bundle ID</source>
        <translation>Identifiant de bundle</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Legacy ID</source>
        <translation>Ancien identifiant</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Where</source>
        <translation>Emplacement</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Open %1 Settings</source>
        <translation>Ouvrir les réglages %1</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy URL</source>
        <translation>Copier l’URL</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy Bundle ID</source>
        <translation>Copier l’identifiant de bundle</translation>
    </message>
</context>
<context>
    <name>MacSettingsRootProvider</name>
    <message>
        <location line="+13"/>
        <source>System Settings</source>
        <translation>Réglages Système</translation>
    </message>
</context>
<context>
    <name>MacosUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/macos-update-installer.mm" line="+208"/>
        <source>This installation cannot update itself</source>
        <translation>Cette installation ne peut pas se mettre à jour elle-même</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Mounting update image…</source>
        <translation>Montage de l’image de mise à jour…</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Failed to mount the update image</source>
        <translation>Échec du montage de l’image de mise à jour</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Could not find the update image mount point</source>
        <translation>Impossible de trouver le point de montage de l’image de mise à jour</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Verifying update…</source>
        <translation>Vérification de la mise à jour…</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Installing update…</source>
        <translation>Installation de la mise à jour…</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to stage update: %1</source>
        <translation>Échec de la préparation de la mise à jour : %1</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Failed to move the current app aside: %1</source>
        <translation>Échec de la mise à l’écart de l’application actuelle : %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to install the new app: %1</source>
        <translation>Échec de l’installation de la nouvelle application : %1</translation>
    </message>
</context>
<context>
    <name>ManageFallbackActions</name>
    <message>
        <location filename="../src/actions/fallback-actions.hpp" line="+15"/>
        <source>Manage Fallback Actions</source>
        <translation>Gérer les actions de secours</translation>
    </message>
</context>
<context>
    <name>ManageFallbackCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/configure-fallback-command.hpp" line="+11"/>
        <source>Configure Fallback Commands</source>
        <translation>Configurer les commandes de secours</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Configure what commands are to be presented as fallback options when nothing matches the search in the root search.</source>
        <translation>Configurer les commandes à proposer comme options de secours lorsqu’aucun résultat ne correspond dans la recherche racine.</translation>
    </message>
</context>
<context>
    <name>ManageFallbackViewHost</name>
    <message>
        <location filename="../src/qml/manage-fallback-view-host.cpp" line="+12"/>
        <source>Search commands...</source>
        <translation>Rechercher des commandes...</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsCommand</name>
    <message>
        <location filename="../src/extensions/shortcut/manage-shortcuts-command.hpp" line="+9"/>
        <source>Manage Shortcuts</source>
        <translation>Gérer les raccourcis</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsSection</name>
    <message>
        <location filename="../src/qml/manage-shortcuts-model.hpp" line="+17"/>
        <source>Shortcuts ({count})</source>
        <translation>Raccourcis ({count})</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsViewHost</name>
    <message>
        <location filename="../src/qml/manage-shortcuts-view-host.cpp" line="+28"/>
        <source>Search shortcuts...</source>
        <translation>Rechercher des raccourcis...</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Application</source>
        <translation>Application</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Opened</source>
        <translation>Ouvertures</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Last Opened</source>
        <translation>Dernière ouverture</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Never</source>
        <translation>Jamais</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Created at</source>
        <translation>Créé le</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsCommand</name>
    <message>
        <location filename="../src/extensions/snippet/manage-snippets-command.hpp" line="+10"/>
        <source>Manage Snippets</source>
        <translation>Gérer les snippets</translation>
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
        <translation>Copier dans le presse-papiers</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copied to clipboard</source>
        <translation>Copié dans le presse-papiers</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy to clipboard</source>
        <translation>Échec de la copie dans le presse-papiers</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Edit snippet</source>
        <translation>Modifier le snippet</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Duplicate snippet</source>
        <translation>Dupliquer le snippet</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Remove snippet</source>
        <translation>Supprimer le snippet</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove snippet</source>
        <translation>Échec de la suppression du snippet</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsViewHost</name>
    <message>
        <location filename="../src/qml/manage-snippets-view-host.hpp" line="+54"/>
        <source>No snippets</source>
        <translation>Aucun snippet</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Create a snippet to get started</source>
        <translation>Créez un snippet pour commencer</translation>
    </message>
    <message>
        <location filename="../src/qml/manage-snippets-view-host.cpp" line="+28"/>
        <source>Search for snippets...</source>
        <translation>Rechercher des snippets...</translation>
    </message>
    <message>
        <location line="+25"/>
        <source>Text</source>
        <translation>Texte</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>File</source>
        <translation>Fichier</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>Type</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Created at</source>
        <translation>Créé le</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Updated at</source>
        <translation>Mis à jour le</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Keyword</source>
        <translation>Mot-clé</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Apps</source>
        <translation>Applications</translation>
    </message>
    <message>
        <location line="+68"/>
        <source>Create snippet</source>
        <translation>Créer un snippet</translation>
    </message>
</context>
<context>
    <name>MarkItemAsFavorite</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="-60"/>
        <source>Mark as favorite</source>
        <translation>Marquer comme favori</translation>
    </message>
</context>
<context>
    <name>MarkdownShowcase</name>
    <message>
        <location filename="../src/extensions/internal/markdown-showcase-command.hpp" line="+172"/>
        <source>Markdown Showcase</source>
        <translation>Démonstration Markdown</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Preview all supported markdown features</source>
        <translation>Prévisualiser toutes les fonctionnalités markdown prises en charge</translation>
    </message>
</context>
<context>
    <name>MarkdownView</name>
    <message>
        <location filename="../src/qml/qml/markdown/MarkdownView.qml" line="+265"/>
        <source>Copy</source>
        <translation>Copier</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Select All</source>
        <translation>Tout sélectionner</translation>
    </message>
</context>
<context>
    <name>MdCallout</name>
    <message>
        <location filename="../src/qml/qml/markdown/MdCallout.qml" line="+35"/>
        <source>Caution</source>
        <translation>Attention</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Warning</source>
        <translation>Avertissement</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Important</source>
        <translation>Important</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tip</source>
        <translation>Astuce</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Note</source>
        <translation>Note</translation>
    </message>
</context>
<context>
    <name>MdCodeBlock</name>
    <message>
        <location filename="../src/qml/qml/markdown/MdCodeBlock.qml" line="+58"/>
        <source>Copied!</source>
        <translation>Copié !</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Copy</source>
        <translation>Copier</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceView</name>
    <message>
        <location filename="../src/qml/qml/MissingPreferenceView.qml" line="+28"/>
        <source>Welcome to %1</source>
        <translation>Bienvenue dans %1</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Before you can use this command, you need to fill in the required preference fields below.</source>
        <translation>Avant de pouvoir utiliser cette commande, vous devez remplir les champs de préférences requis ci-dessous.</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceViewHost</name>
    <message>
        <location filename="../src/qml/missing-preference-view-host.cpp" line="+202"/>
        <source>Save preferences</source>
        <translation>Enregistrer les préférences</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Please fill in all required fields</source>
        <translation>Veuillez remplir tous les champs obligatoires</translation>
    </message>
</context>
<context>
    <name>NavigationController</name>
    <message>
        <location filename="../src/navigation-controller.cpp" line="+654"/>
        <source>Extension manager is not running</source>
        <translation>Le gestionnaire d’extensions n’est pas en cours d’exécution</translation>
    </message>
</context>
<context>
    <name>NewsService</name>
    <message>
        <location filename="../src/services/news/news-service.cpp" line="+68"/>
        <source>Telemetry</source>
        <translation>Télémétrie</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>We now collect basic usage statistics on startup</source>
        <translation>Nous collectons désormais des statistiques d’utilisation basiques au démarrage</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Learn more</source>
        <translation>En savoir plus</translation>
    </message>
</context>
<context>
    <name>NullUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/null-update-installer.hpp" line="+14"/>
        <source>Self update is not supported on this platform</source>
        <translation>La mise à jour automatique n’est pas prise en charge sur cette plateforme</translation>
    </message>
</context>
<context>
    <name>OAuthOverlayView</name>
    <message>
        <location filename="../src/qml/qml/OAuthOverlayView.qml" line="+91"/>
        <source>Continue with %1</source>
        <translation>Continuer avec %1</translation>
    </message>
    <message>
        <location line="+33"/>
        <source>You&apos;re in!</source>
        <translation>Vous êtes connecté !</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Successfully connected to %1.
Back to command in an instant...</source>
        <translation>Connexion à %1 réussie.
Retour à la commande dans un instant...</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-31"/>
        <source>Manage OAuth Token Sets</source>
        <translation>Gérer les jeux de jetons OAuth</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Manage OAuth token sets that have been saved by extensions providing OAuth integrations.</source>
        <translation>Gérer les jeux de jetons OAuth enregistrés par les extensions fournissant des intégrations OAuth.</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreSection</name>
    <message>
        <location filename="../src/qml/oauth-token-store-model.hpp" line="+17"/>
        <source>OAuth Token Sets ({count})</source>
        <translation>Jeux de jetons OAuth ({count})</translation>
    </message>
    <message>
        <location filename="../src/qml/oauth-token-store-model.cpp" line="+20"/>
        <source>Expired</source>
        <translation>Expiré</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Remove token set</source>
        <translation>Supprimer le jeu de jetons</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Are you sure?</source>
        <translation>Êtes-vous sûr ?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You will need to go through the OAuth login flow again the next time you want to use this service</source>
        <translation>Vous devrez repasser par le processus de connexion OAuth la prochaine fois que vous voudrez utiliser ce service</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to remove token set</source>
        <translation>Échec de la suppression du jeu de jetons</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Token set removed</source>
        <translation>Jeu de jetons supprimé</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy</source>
        <translation>Copier</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy Access Token</source>
        <translation>Copier le jeton d’accès</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Refresh Token</source>
        <translation>Copier le jeton de rafraîchissement</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy ID Token</source>
        <translation>Copier le jeton d’identité</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy Scopes</source>
        <translation>Copier les scopes</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Expiration Date</source>
        <translation>Copier la date d’expiration</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreViewHost</name>
    <message>
        <location filename="../src/qml/oauth-token-store-view-host.cpp" line="+12"/>
        <source>Search token sets...</source>
        <translation>Rechercher des jeux de jetons...</translation>
    </message>
</context>
<context>
    <name>OnboardingWindow</name>
    <message>
        <location filename="../src/qml/qml/OnboardingWindow.qml" line="+39"/>
        <source>Grant Access</source>
        <translation>Autoriser l’accès</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Granted</source>
        <translation>Accordé</translation>
    </message>
    <message>
        <location line="+16"/>
        <location line="+39"/>
        <source>Welcome to Vicinae</source>
        <translation>Bienvenue dans Vicinae</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Let&apos;s set it up. It only takes a minute.</source>
        <translation>Passons à la configuration. Cela ne prend qu’une minute.</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Permissions</source>
        <translation>Autorisations</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Vicinae needs additional permissions in order to make the best of your Mac.</source>
        <translation>Vicinae a besoin d’autorisations supplémentaires pour tirer le meilleur parti de votre Mac.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Accessibility</source>
        <translation>Accessibilité</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Used to paste, expand snippets, and move windows.</source>
        <translation>Utilisé pour coller, développer les snippets et déplacer les fenêtres.</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Full Disk Access</source>
        <translation>Accès complet au disque</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lets file search cover your entire disk.</source>
        <translation>Permet à la recherche de fichiers de couvrir tout votre disque.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Notifications</source>
        <translation>Notifications</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lets extensions send desktop notifications.</source>
        <translation>Permet aux extensions d’envoyer des notifications de bureau.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Accessibility is required: global shortcuts, paste, and snippet expansion cannot work without it.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Full disk access needs to be explicitly enabled if you want file search to cover all your files.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Make it your own</source>
        <translation>Personnalisez-le</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>You will be able to change these settings later.</source>
        <translation>Vous pourrez modifier ces paramètres plus tard.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Theme</source>
        <translation>Thème</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Shared across the entire app.</source>
        <translation>Partagé dans toute l’application.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Global hotkey</source>
        <translation>Raccourci global</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Opens the launcher from anywhere.</source>
        <translation>Ouvre le lanceur depuis n’importe où.</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Launch at login</source>
        <translation>Lancer à l’ouverture de session</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Starts Vicinae in the background at login.</source>
        <translation>Démarre Vicinae en arrière-plan à l’ouverture de session.</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Setup complete</source>
        <translation>Configuration terminée</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Vicinae is running. Open the launcher with:</source>
        <translation>Vicinae est en cours d’exécution. Ouvrez le lanceur avec :</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Vicinae is open source software.</source>
        <translation>Vicinae est un logiciel open source.</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Sponsor</source>
        <translation>Sponsoriser</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Back</source>
        <translation>Retour</translation>
    </message>
    <message>
        <location line="+44"/>
        <source>Finish</source>
        <translation>Terminer</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Continue</source>
        <translation>Continuer</translation>
    </message>
</context>
<context>
    <name>OpenAboutCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/open-about-command.hpp" line="+13"/>
        <source>About</source>
        <translation>À propos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the &quot;About&quot; tab of the vicinae settings.</source>
        <translation>Ouvrir l’onglet « À propos » des paramètres vicinae.</translation>
    </message>
</context>
<context>
    <name>OpenAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-56"/>
        <source>Failed to start app</source>
        <translation>Échec du démarrage de l’application</translation>
    </message>
</context>
<context>
    <name>OpenAppLocationAction</name>
    <message>
        <location line="-36"/>
        <source>Open Location</source>
        <translation>Ouvrir l’emplacement</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to open app location</source>
        <translation>Échec de l’ouverture de l’emplacement de l’application</translation>
    </message>
</context>
<context>
    <name>OpenBuiltinCommandAction</name>
    <message>
        <location filename="../src/command-actions.hpp" line="+17"/>
        <source>Open command</source>
        <translation>Ouvrir la commande</translation>
    </message>
</context>
<context>
    <name>OpenCalculatorHistoryAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+9"/>
        <source>Open Calculator History</source>
        <translation>Ouvrir l’historique de la calculatrice</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-32"/>
        <source>Open shortcut</source>
        <translation>Ouvrir le raccourci</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutWithAction</name>
    <message>
        <location line="+85"/>
        <source>Open with...</source>
        <translation>Ouvrir avec...</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelItemAction</name>
    <message>
        <location filename="../src/root-search/control-panel/control-panel-root-provider.cpp" line="+45"/>
        <source>Failed to open settings</source>
        <translation>Échec de l’ouverture des paramètres</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelTaskAction</name>
    <message>
        <location line="+26"/>
        <source>Failed to open settings</source>
        <translation>Échec de l’ouverture des paramètres</translation>
    </message>
</context>
<context>
    <name>OpenDefaultVicinaeConfig</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-157"/>
        <source>Open Default Config File</source>
        <translation>Ouvrir le fichier de configuration par défaut</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the default vicinae configuration file</source>
        <translation>Ouvrir le fichier de configuration par défaut de vicinae</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Failed to open temporary file</source>
        <translation>Échec de l’ouverture du fichier temporaire</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Failed to open default config file</source>
        <translation>Échec de l’ouverture du fichier de configuration par défaut</translation>
    </message>
</context>
<context>
    <name>OpenDiscordCommand</name>
    <message>
        <location line="-66"/>
        <source>Join the Discord Server</source>
        <translation>Rejoindre le serveur Discord</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open link to join the official Vicinae discord server.</source>
        <translation>Ouvrir le lien pour rejoindre le serveur Discord officiel de Vicinae.</translation>
    </message>
</context>
<context>
    <name>OpenDocumentationCommand</name>
    <message>
        <location line="-16"/>
        <source>Open Online Documentation</source>
        <translation>Ouvrir la documentation en ligne</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Navigate to the official vicinae documentation website.</source>
        <translation>Accéder au site officiel de la documentation de Vicinae.</translation>
    </message>
</context>
<context>
    <name>OpenFileAction</name>
    <message>
        <location filename="../src/actions/files/file-actions.hpp" line="+18"/>
        <source>Open with %1</source>
        <translation>Ouvrir avec %1</translation>
    </message>
</context>
<context>
    <name>OpenInBrowserAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="+105"/>
        <source>Open in browser</source>
        <translation>Ouvrir dans le navigateur</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+95"/>
        <source>Failed to open in browser</source>
        <translation>Échec de l’ouverture dans le navigateur</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Opened in browser</source>
        <translation>Ouvert dans le navigateur</translation>
    </message>
</context>
<context>
    <name>OpenInTerminalAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="-62"/>
        <source>Open in %1</source>
        <translation>Ouvrir dans %1</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-85"/>
        <source>Failed to start app</source>
        <translation>Échec du démarrage de l’application</translation>
    </message>
</context>
<context>
    <name>OpenItemPreferencesAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="-28"/>
        <source>Open Preferences</source>
        <translation>Ouvrir les préférences</translation>
    </message>
</context>
<context>
    <name>OpenKeybindSettingsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+156"/>
        <source>Open Vicinae Keybind Settings</source>
        <translation>Ouvrir les paramètres de raccourcis clavier de Vicinae</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the vicinae keybind settings window</source>
        <translation>Ouvrir la fenêtre des paramètres de raccourcis clavier de vicinae</translation>
    </message>
</context>
<context>
    <name>OpenRawProgramAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.hpp" line="+25"/>
        <source>Execute program</source>
        <translation>Exécuter le programme</translation>
    </message>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+35"/>
        <source>Failed to start app</source>
        <translation>Échec du démarrage de l’application</translation>
    </message>
</context>
<context>
    <name>OpenSettingsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-40"/>
        <source>Open Vicinae Settings</source>
        <translation>Ouvrir les paramètres de Vicinae</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open the vicinae settings window, which is an independent floating window.</source>
        <translation>Ouvrir la fenêtre des paramètres de vicinae, qui est une fenêtre flottante indépendante.</translation>
    </message>
</context>
<context>
    <name>OpenSettingsPaneAction</name>
    <message>
        <location filename="../src/root-search/macos-settings/macos-settings-root-provider.mm" line="-135"/>
        <source>Failed to open System Settings</source>
        <translation>Échec de l’ouverture des Réglages Système</translation>
    </message>
</context>
<context>
    <name>OpenShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="-128"/>
        <source>No app with id %1</source>
        <translation>Aucune application avec l’ID %1</translation>
    </message>
    <message>
        <location line="+9"/>
        <location line="+7"/>
        <source>Open shortcut</source>
        <translation>Ouvrir le raccourci</translation>
    </message>
</context>
<context>
    <name>OpenShortcutFromSearchText</name>
    <message>
        <location line="+43"/>
        <source>Open shortcut</source>
        <translation>Ouvrir le raccourci</translation>
    </message>
</context>
<context>
    <name>OpenVicinaeConfig</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-81"/>
        <source>Open Config File</source>
        <translation>Ouvrir le fichier de configuration</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the main vicinae configuration file</source>
        <translation>Ouvrir le fichier de configuration principal de Vicinae</translation>
    </message>
</context>
<context>
    <name>OpenWindowsSettingAction</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="+147"/>
        <source>Failed to open settings</source>
        <translation>Échec de l’ouverture des paramètres</translation>
    </message>
</context>
<context>
    <name>OpenWithAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="+54"/>
        <source>Open with...</source>
        <translation>Ouvrir avec...</translation>
    </message>
</context>
<context>
    <name>PasteToFocusedWindowAction</name>
    <message>
        <location filename="../src/clipboard-actions.hpp" line="+11"/>
        <source>Paste to active window</source>
        <translation>Coller dans la fenêtre active</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy to focused window</source>
        <translation>Copier dans la fenêtre active</translation>
    </message>
</context>
<context>
    <name>PinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+28"/>
        <source>Entry pinned</source>
        <translation>Entrée épinglée</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pin entry</source>
        <translation>Épingler l’entrée</translation>
    </message>
</context>
<context>
    <name>PinClipboardAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="-26"/>
        <source>Selection pinned</source>
        <translation>Sélection épinglée</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Selection unpinned</source>
        <translation>Sélection désépinglée</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to change pin status</source>
        <translation>Échec du changement d’état d’épinglage</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Pin</source>
        <translation>Épingler</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unpin</source>
        <translation>Désépingler</translation>
    </message>
</context>
<context>
    <name>PinEmojiAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-322"/>
        <source>Pin emoji</source>
        <translation>Épingler l’emoji</translation>
    </message>
</context>
<context>
    <name>PinWindowAction</name>
    <message>
        <location filename="../src/actions/wm/window-actions.hpp" line="+37"/>
        <source>Unpin from all workspaces</source>
        <translation>Désépingler de tous les espaces de travail</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Pin to all workspaces</source>
        <translation>Épingler sur tous les espaces de travail</translation>
    </message>
</context>
<context>
    <name>PowerManagementCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-283"/>
        <source>Ask for confirmation</source>
        <translation>Demander une confirmation</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Custom program</source>
        <translation>Programme personnalisé</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Custom POSIX shell command to run instead of the default implementation</source>
        <translation>Commande shell POSIX personnalisée à exécuter à la place de l’implémentation par défaut</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Failed to execute custom program %1</source>
        <translation>Échec de l’exécution du programme personnalisé %1</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Are you sure</source>
        <translation>Êtes-vous sûr</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>High-impact operation, please confirm</source>
        <translation>Opération à fort impact, veuillez confirmer</translation>
    </message>
</context>
<context>
    <name>PowerManagementExtension</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.hpp" line="+8"/>
        <source>Power Management</source>
        <translation>Gestion de l’alimentation</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power off, suspend, sleep, hibernate your computer.</source>
        <translation>Éteindre, suspendre, mettre votre ordinateur en veille ou en veille prolongée.</translation>
    </message>
</context>
<context>
    <name>PowerOffCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+137"/>
        <source>Power Off System</source>
        <translation>Éteindre le système</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power off the system</source>
        <translation>Éteindre le système</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>System cannot power off</source>
        <translation>Le système ne peut pas s’éteindre</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to power off</source>
        <translation>Échec de l’extinction</translation>
    </message>
</context>
<context>
    <name>PreviewFontAction</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="-105"/>
        <source>Preview font</source>
        <translation>Prévisualiser la police</translation>
    </message>
</context>
<context>
    <name>ProgramsSection</name>
    <message>
        <location filename="../src/qml/system-run-model.hpp" line="+21"/>
        <source>Programs (%1)</source>
        <translation>Programmes (%1)</translation>
    </message>
    <message>
        <location filename="../src/qml/system-run-model.cpp" line="+52"/>
        <source>Open in %1 (hold)</source>
        <translation>Ouvrir dans %1 (maintenir)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open in %1</source>
        <translation>Ouvrir dans %1</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Copy exec path</source>
        <translation>Copier le chemin de l’exécutable</translation>
    </message>
</context>
<context>
    <name>ProviderSearchSection</name>
    <message>
        <location filename="../src/qml/provider-search-model.hpp" line="+11"/>
        <source>Results ({count})</source>
        <translation>Résultats ({count})</translation>
    </message>
</context>
<context>
    <name>ProviderSearchViewHost</name>
    <message>
        <location filename="../src/qml/provider-search-view-host.cpp" line="+15"/>
        <source>Search %1</source>
        <translation>Rechercher dans %1</translation>
    </message>
</context>
<context>
    <name>PruneMemoryCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+56"/>
        <source>Prune Vicinae Memory Usage</source>
        <translation>Réduire l’utilisation mémoire de Vicinae</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Try pruning vicinae&apos;s memory usage by clearing pixmap cache and calling malloc_trim(). Mostly provided for internal testing.</source>
        <translation>Tente de réduire l’utilisation mémoire de vicinae en vidant le cache de pixmaps et en appelant malloc_trim(). Principalement fourni pour des tests internes.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Pruned 🥊</source>
        <translation>Mémoire nettoyée 🥊</translation>
    </message>
</context>
<context>
    <name>PutCalculatorAnswerInSearchBar</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-20"/>
        <source>Put answer in search bar</source>
        <translation>Insérer la réponse dans la barre de recherche</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/utils/utils.cpp" line="+50"/>
        <source>in the future</source>
        <translation>dans le futur</translation>
    </message>
    <message numerus="yes">
        <location line="+8"/>
        <source>%n year(s) ago</source>
        <translation>
            <numerusform>il y a %n an</numerusform>
            <numerusform>il y a %n ans</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+3"/>
        <source>%n month(s) ago</source>
        <translation>
            <numerusform>il y a %n mois</numerusform>
            <numerusform>il y a %n mois</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n day(s) ago</source>
        <translation>
            <numerusform>il y a %n jour</numerusform>
            <numerusform>il y a %n jours</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n hour(s) ago</source>
        <translation>
            <numerusform>il y a %n heure</numerusform>
            <numerusform>il y a %n heures</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n minute(s) ago</source>
        <translation>
            <numerusform>il y a %n minute</numerusform>
            <numerusform>il y a %n minutes</numerusform>
        </translation>
    </message>
    <message>
        <location line="+2"/>
        <source>just now</source>
        <translation>à l’instant</translation>
    </message>
</context>
<context>
    <name>QuitAppAction</name>
    <message>
        <location filename="../src/actions/app/app-actions.cpp" line="-43"/>
        <source>Quit Application</source>
        <translation>Quitter l’application</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to quit %1</source>
        <translation>Échec de la fermeture de %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Quit %1</source>
        <translation>Quitter %1</translation>
    </message>
</context>
<context>
    <name>RaycastCompatExtension</name>
    <message>
        <location filename="../src/extensions/raycast/raycast-compat-extension.hpp" line="+15"/>
        <source>Raycast compatibility features</source>
        <translation>Fonctionnalités de compatibilité Raycast</translation>
    </message>
</context>
<context>
    <name>RaycastStoreCommand</name>
    <message>
        <location filename="../src/extensions/raycast/raycast-store-command.hpp" line="+13"/>
        <source>Install compatible extensions from the Raycast store</source>
        <translation>Installer des extensions compatibles depuis la boutique Raycast</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Always show intro</source>
        <translation>Toujours afficher l’introduction</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>
# Welcome to the Raycast Extension Store

Vicinae provides direct integration with the official [Raycast store](https://www.raycast.com/store), allowing you to search and install Raycast extensions directly from Vicinae.
</source>
        <translation>
# Bienvenue dans la boutique d’extensions Raycast

Vicinae s’intègre directement avec la [boutique Raycast](https://www.raycast.com/store) officielle, ce qui vous permet de rechercher et d’installer des extensions Raycast directement depuis Vicinae.
</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>
Each extension has a colored compatibility indicator showing how well it works on Linux.

Vicinae also has its own [extension store](vicinae://launch/core/store), which does not suffer from these limitations.
</source>
        <translation>
Chaque extension possède un indicateur de compatibilité coloré montrant dans quelle mesure elle fonctionne sous Linux.

Vicinae dispose également de sa propre [boutique d’extensions](vicinae://launch/core/store), qui ne souffre pas de ces limitations.
</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>
Vicinae also has its own [extension store](vicinae://launch/core/store).
</source>
        <translation>
Vicinae dispose également de sa propre [boutique d’extensions](vicinae://launch/core/store).
</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Continue to store</source>
        <translation>Continuer vers la boutique</translation>
    </message>
</context>
<context>
    <name>RaycastStoreDetailHost</name>
    <message>
        <location filename="../src/qml/raycast-store-detail-host.cpp" line="+43"/>
        <source>Failed to load extension</source>
        <translation>Échec du chargement de l’extension</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The extension &quot;%1&quot; could not be loaded. It may not exist or the store may be unreachable.</source>
        <translation>L’extension &quot;%1&quot; n’a pas pu être chargée. Elle n’existe peut-être pas ou la boutique est peut-être inaccessible.</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Extension Store - %1</source>
        <translation>Boutique d’extensions - %1</translation>
    </message>
    <message>
        <location line="+31"/>
        <source>This extension should be fully compatible.</source>
        <translation>Cette extension devrait être entièrement compatible.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>This extension works but has a few quirks.</source>
        <translation>Cette extension fonctionne mais présente quelques défauts.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>This extension is not compatible.</source>
        <translation>Cette extension n’est pas compatible.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No compatibility data is available for this extension.</source>
        <translation>Aucune donnée de compatibilité n’est disponible pour cette extension.</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>No compatibility data is available — this extension may or may not work.</source>
        <translation>Aucune donnée de compatibilité n’est disponible — cette extension peut fonctionner ou non.</translation>
    </message>
    <message>
        <location line="+83"/>
        <source>Install extension</source>
        <translation>Installer l’extension</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading extension...</source>
        <translation>Téléchargement de l’extension...</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to download extension</source>
        <translation>Échec du téléchargement de l’extension</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to extract extension archive</source>
        <translation>Échec de l’extraction de l’archive de l’extension</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Extension installed</source>
        <translation>Extension installée</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Report issue</source>
        <translation>Signaler un problème</translation>
    </message>
</context>
<context>
    <name>RaycastStoreSection</name>
    <message>
        <location filename="../src/qml/raycast-store-model.cpp" line="+45"/>
        <source>Show details</source>
        <translation>Afficher les détails</translation>
    </message>
</context>
<context>
    <name>RaycastStoreViewHost</name>
    <message>
        <location filename="../src/qml/raycast-store-view-host.cpp" line="+37"/>
        <source>Browse Raycast extensions</source>
        <translation>Parcourir les extensions Raycast</translation>
    </message>
    <message>
        <location line="+32"/>
        <source>Failed to fetch extensions</source>
        <translation>Échec de la récupération des extensions</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Extensions</source>
        <translation>Extensions</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to search extensions</source>
        <translation>Échec de la recherche d’extensions</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Results</source>
        <translation>Résultats</translation>
    </message>
</context>
<context>
    <name>RebootCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-72"/>
        <source>Reboot System</source>
        <translation>Redémarrer le système</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reboot the system</source>
        <translation>Redémarrer le système</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>System can&apos;t reboot</source>
        <translation>Le système ne peut pas redémarrer</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to reboot</source>
        <translation>Échec du redémarrage</translation>
    </message>
</context>
<context>
    <name>RebuildFileIndexCommand</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="-72"/>
        <source>Rebuild File Index</source>
        <translation>Reconstruire l’index des fichiers</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Fully rebuild the file index. Running this manually can be useful if the file search feels particularly out of date.</source>
        <translation>Reconstruire entièrement l’index des fichiers. L’exécuter manuellement peut être utile si la recherche de fichiers semble particulièrement obsolète.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Are you sure?</source>
        <translation>Êtes-vous sûr ?</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Rebuilding the entire index can be time consuming and CPU intensive, depending on the number of files present in your home directory.</source>
        <translation>Reconstruire l’index complet peut prendre du temps et solliciter fortement le processeur, selon le nombre de fichiers présents dans votre répertoire personnel.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Reset</source>
        <translation>Réinitialiser</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Index rebuild started...</source>
        <translation>Reconstruction de l’index démarrée...</translation>
    </message>
</context>
<context>
    <name>RefreshAppsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/refresh-apps-command.hpp" line="+12"/>
        <source>Refresh Apps</source>
        <translation>Actualiser les applications</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Force a refresh of the application database. The database should normally automatically update itself on changes, but this can help working around some edge cases.</source>
        <translation>Forcer une actualisation de la base de données des applications. La base de données devrait normalement se mettre à jour automatiquement lors des changements, mais ceci peut aider à contourner certains cas particuliers.</translation>
    </message>
    <message>
        <location filename="../src/extensions/vicinae/refresh-apps-command.cpp" line="+15"/>
        <source>Apps successfully refreshed</source>
        <translation>Applications actualisées avec succès</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to refresh apps</source>
        <translation>Échec de l’actualisation des applications</translation>
    </message>
</context>
<context>
    <name>ReloadScriptDirectoriesCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="+29"/>
        <source>Reload Script Directories</source>
        <translation>Recharger les répertoires de scripts</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reload script directories</source>
        <translation>Recharger les répertoires de scripts</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>New scan triggered, index will update shortly</source>
        <translation>Nouvelle analyse déclenchée, l’index sera mis à jour sous peu</translation>
    </message>
</context>
<context>
    <name>RemoveAllCalculatorHistoryRecordsAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="+97"/>
        <source>Delete all entries</source>
        <translation>Supprimer toutes les entrées</translation>
    </message>
</context>
<context>
    <name>RemoveAllSelectionsAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="+27"/>
        <source>Are you sure?</source>
        <translation>Êtes-vous sûr ?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>All your clipboard history will be lost forever</source>
        <translation>Tout l’historique de votre presse-papiers sera définitivement perdu</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Delete all</source>
        <translation>Tout supprimer</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>All selections were removed</source>
        <translation>Toutes les sélections ont été supprimées</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove all selections</source>
        <translation>Échec de la suppression de toutes les sélections</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Remove all</source>
        <translation>Tout supprimer</translation>
    </message>
</context>
<context>
    <name>RemoveCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-43"/>
        <source>Entry removed</source>
        <translation>Entrée supprimée</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove entry</source>
        <translation>Échec de la suppression de l’entrée</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Delete entry</source>
        <translation>Supprimer l’entrée</translation>
    </message>
</context>
<context>
    <name>RemoveSelectionAction</name>
    <message>
        <location filename="../src/extensions/clipboard/history/clipboard-history-actions.hpp" line="-70"/>
        <source>Entry removed</source>
        <translation>Entrée supprimée</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove entry</source>
        <translation>Échec de la suppression de l’entrée</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Remove entry</source>
        <translation>Supprimer l’entrée</translation>
    </message>
</context>
<context>
    <name>RemoveShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut/shortcut-actions.hpp" line="+33"/>
        <source>Removed link</source>
        <translation>Lien supprimé</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove link</source>
        <translation>Échec de la suppression du lien</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Remove link</source>
        <translation>Supprimer le lien</translation>
    </message>
</context>
<context>
    <name>ReportVicinaeBugCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/report-bug-command.hpp" line="+10"/>
        <source>Report a Vicinae Bug</source>
        <translation>Signaler un bug Vicinae</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Navigate to Vicinae issue creation page with all relevant informations pre-filled.</source>
        <translation>Accéder à la page de création de tickets Vicinae avec toutes les informations pertinentes préremplies.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Title</source>
        <translation>Titre</translation>
    </message>
</context>
<context>
    <name>ResetEmojiRankingAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="+21"/>
        <source>Reset ranking</source>
        <translation>Réinitialiser le classement</translation>
    </message>
</context>
<context>
    <name>ResetEmojiSkinToneAction</name>
    <message>
        <location line="+32"/>
        <source>Reset to preference</source>
        <translation>Rétablir la préférence</translation>
    </message>
</context>
<context>
    <name>ResetItemRanking</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="-25"/>
        <source>Ranking was successfully reset</source>
        <translation>Le classement a été réinitialisé avec succès</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Unable to reset ranking</source>
        <translation>Impossible de réinitialiser le classement</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Are you sure?</source>
        <translation>Êtes-vous sûr ?</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You will have to rebuild search history for this item in order for it to reappear on top of the root search results.</source>
        <translation>Vous devrez reconstruire l’historique de recherche de cet élément pour qu’il réapparaisse en haut des résultats de la recherche principale.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Reset</source>
        <translation>Réinitialiser</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Reset ranking</source>
        <translation>Réinitialiser le classement</translation>
    </message>
</context>
<context>
    <name>RevealFileInFolderAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+26"/>
        <source>Show in file browser</source>
        <translation>Afficher dans le gestionnaire de fichiers</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to open folder</source>
        <translation>Échec de l’ouverture du dossier</translation>
    </message>
</context>
<context>
    <name>RootCalculatorSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+79"/>
        <source>Calculator</source>
        <translation>Calculatrice</translation>
    </message>
</context>
<context>
    <name>RootFallbackSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="+407"/>
        <source>Use &quot;%1&quot; with...</source>
        <translation>Utiliser &quot;%1&quot; avec...</translation>
    </message>
</context>
<context>
    <name>RootFavoritesSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+67"/>
        <source>Favorites</source>
        <translation>Favoris</translation>
    </message>
</context>
<context>
    <name>RootFilesSection</name>
    <message>
        <location line="+50"/>
        <source>Files</source>
        <translation>Fichiers</translation>
    </message>
</context>
<context>
    <name>RootLinkSection</name>
    <message>
        <location line="-137"/>
        <source>Link</source>
        <translation>Lien</translation>
    </message>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="-295"/>
        <source>Open in %1</source>
        <translation>Ouvrir dans %1</translation>
    </message>
</context>
<context>
    <name>RootNewsSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="+64"/>
        <source>What&apos;s New</source>
        <translation>Nouveautés</translation>
    </message>
</context>
<context>
    <name>RootResultsSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="+199"/>
        <source>Suggestions</source>
        <translation>Suggestions</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Results (%1)</source>
        <translation>Résultats (%1)</translation>
    </message>
</context>
<context>
    <name>RootSearchActionGenerator</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="+55"/>
        <source>Copy ID</source>
        <translation>Copier l’ID</translation>
    </message>
</context>
<context>
    <name>RootShortcutItem</name>
    <message>
        <location filename="../src/root-search/shortcuts/shortcut-root-provider.cpp" line="+67"/>
        <location line="+11"/>
        <source>Shortcut</source>
        <translation>Raccourci</translation>
    </message>
</context>
<context>
    <name>RootUpdateSection</name>
    <message>
        <location filename="../src/qml/root-search-sources.hpp" line="-21"/>
        <location filename="../src/qml/root-search-sources.cpp" line="-125"/>
        <source>Update</source>
        <translation>Mise à jour</translation>
    </message>
    <message>
        <location filename="../src/qml/root-search-sources.cpp" line="-14"/>
        <source>Vicinae %1 is available</source>
        <translation>Vicinae %1 est disponible</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>You are running %1</source>
        <translation>Vous utilisez %1</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>View Release Notes</source>
        <translation>Voir les notes de version</translation>
    </message>
</context>
<context>
    <name>RootViewHost</name>
    <message>
        <location filename="../src/qml/root-view-host.hpp" line="+15"/>
        <source>Search for anything...</source>
        <translation>Rechercher...</translation>
    </message>
</context>
<context>
    <name>ScriptExecutorViewHost</name>
    <message>
        <location filename="../src/qml/script-executor-view-host.cpp" line="+76"/>
        <source>Script execution failed: %1</source>
        <translation>Échec de l’exécution du script : %1</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Running... (%1s ago)</source>
        <translation>En cours... (il y a %1s)</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Done in %1s (exit=%2)</source>
        <translation>Terminé en %1s (exit=%2)</translation>
    </message>
    <message>
        <location line="+12"/>
        <location line="+9"/>
        <source>Script process killed</source>
        <translation>Processus du script tué</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Running...</source>
        <translation>En cours...</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Kill process</source>
        <translation>Tuer le processus</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Run script again</source>
        <translation>Relancer le script</translation>
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
        <translation>Mode</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Path</source>
        <translation>Chemin</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Author</source>
        <translation>Auteur</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Open script directory</source>
        <translation>Ouvrir le répertoire du script</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy path to script</source>
        <translation>Copier le chemin du script</translation>
    </message>
</context>
<context>
    <name>ScriptRootProvider</name>
    <message>
        <location line="+47"/>
        <source>Script Commands</source>
        <translation>Commandes de script</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Custom directories</source>
        <translation>Répertoires personnalisés</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Additional list of directories to source scripts from. These directories always take precedence over the default system ones</source>
        <translation>Liste supplémentaire de répertoires d’où charger des scripts. Ces répertoires ont toujours priorité sur les répertoires système par défaut</translation>
    </message>
</context>
<context>
    <name>SearchBrowserTabsCommand</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="+15"/>
        <source>Search Browser Tabs</source>
        <translation>Rechercher des onglets de navigateur</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search tabs from all connected browsers</source>
        <translation>Rechercher les onglets de tous les navigateurs connectés</translation>
    </message>
</context>
<context>
    <name>SearchEmojiCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/search-emoji-command.hpp" line="+15"/>
        <source>Search Emojis &amp; Symbols</source>
        <translation>Rechercher des émojis et symboles</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search for any emoji or symbol</source>
        <translation>Rechercher n’importe quel émoji ou symbole</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Paste</source>
        <translation>Coller</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy</source>
        <translation>Copier</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Default Action</source>
        <translation>Action par défaut</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to perform on pressing return. Paste is only available if your environment supports it.</source>
        <translation>L’action par défaut à effectuer en appuyant sur Entrée. Coller n’est disponible que si votre environnement le prend en charge.</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Skin tone</source>
        <translation>Couleur de peau</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Skin tone to use for relevant emojis.</source>
        <translation>Couleur de peau à utiliser pour les émojis concernés.</translation>
    </message>
</context>
<context>
    <name>SearchEmojiGridSource</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.hpp" line="-22"/>
        <source>Results (%1)</source>
        <translation>Résultats (%1)</translation>
    </message>
</context>
<context>
    <name>SearchFilesCommand</name>
    <message>
        <location filename="../src/extensions/file/file-extension.hpp" line="-38"/>
        <source>Search Files</source>
        <translation>Rechercher des fichiers</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search files on your system</source>
        <translation>Rechercher des fichiers sur votre système</translation>
    </message>
</context>
<context>
    <name>SearchFilesView</name>
    <message>
        <location filename="../src/qml/qml/SearchFilesView.qml" line="+37"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Path</source>
        <translation>Chemin</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>Type</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Last modified</source>
        <translation>Dernière modification</translation>
    </message>
</context>
<context>
    <name>SearchFilesViewHost</name>
    <message>
        <location filename="../src/qml/search-files-view-host.cpp" line="+59"/>
        <source>Search for files...</source>
        <translation>Rechercher des fichiers...</translation>
    </message>
    <message>
        <location line="+30"/>
        <location line="+4"/>
        <source>Direct file path</source>
        <translation>Chemin de fichier direct</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Recently Accessed</source>
        <translation>Consultés récemment</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Results</source>
        <translation>Résultats</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>All</source>
        <translation>Tous</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Other</source>
        <translation>Autres</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Directories</source>
        <translation>Dossiers</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Images</source>
        <translation>Images</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Videos</source>
        <translation>Vidéos</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Audio</source>
        <translation>Audio</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Documents</source>
        <translation>Documents</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Archives</source>
        <translation>Archives</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Applications</source>
        <translation>Applications</translation>
    </message>
</context>
<context>
    <name>SetAppFont</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="-15"/>
        <source>Set as vicinae font</source>
        <translation>Définir comme police de Vicinae</translation>
    </message>
</context>
<context>
    <name>SetRootItemAliasAction</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.hpp" line="-18"/>
        <source>Set alias</source>
        <translation>Définir un alias</translation>
    </message>
</context>
<context>
    <name>SetThemeAction</name>
    <message>
        <location filename="../src/actions/theme/theme-actions.cpp" line="+11"/>
        <source>Theme successfully updated</source>
        <translation>Thème mis à jour avec succès</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Set theme</source>
        <translation>Appliquer le thème</translation>
    </message>
</context>
<context>
    <name>SetThemeCommand</name>
    <message>
        <location filename="../src/extensions/theme/set-theme-command.hpp" line="+9"/>
        <source>Set Theme</source>
        <translation>Définir le thème</translation>
    </message>
</context>
<context>
    <name>SetVolumeCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="+213"/>
        <source>Set Volume to %1%</source>
        <translation>Régler le volume à %1%</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Set system volume to %1%</source>
        <translation>Régler le volume du système à %1%</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to set volume</source>
        <translation>Échec du réglage du volume</translation>
    </message>
</context>
<context>
    <name>SetWallpaperAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+17"/>
        <source>Set as wallpaper</source>
        <translation>Définir comme fond d’écran</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Wallpaper set</source>
        <translation>Fond d’écran défini</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to set wallpaper</source>
        <translation>Échec de la définition du fond d’écran</translation>
    </message>
</context>
<context>
    <name>SettingsSidebar</name>
    <message>
        <location filename="../src/qml/qml/SettingsSidebar.qml" line="+99"/>
        <source>Search...</source>
        <translation>Rechercher...</translation>
    </message>
</context>
<context>
    <name>SettingsSidebarModel</name>
    <message>
        <location filename="../src/qml/settings-sidebar-model.cpp" line="+90"/>
        <source>General</source>
        <translation>Général</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Appearance</source>
        <translation>Apparence</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Keybindings</source>
        <translation>Raccourcis clavier</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced</source>
        <translation>Avancé</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>About</source>
        <translation>À propos</translation>
    </message>
</context>
<context>
    <name>SettingsWindow</name>
    <message>
        <location filename="../src/qml/qml/SettingsWindow.qml" line="+10"/>
        <source>General</source>
        <translation>Général</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Appearance</source>
        <translation>Apparence</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keybindings</source>
        <translation>Raccourcis clavier</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Advanced</source>
        <translation>Avancé</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>About</source>
        <translation>À propos</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Vicinae Settings</source>
        <translation>Paramètres de Vicinae</translation>
    </message>
    <message>
        <location line="+88"/>
        <source>Imported from Raycast</source>
        <translation>Importée depuis Raycast</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>From the Vicinae store</source>
        <translation>Depuis la boutique Vicinae</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Locally installed extension</source>
        <translation>Extension installée localement</translation>
    </message>
</context>
<context>
    <name>ShortcutExtension</name>
    <message>
        <location filename="../src/extensions/shortcut/shortcut-extension.hpp" line="+11"/>
        <source>Manage Shortcuts</source>
        <translation>Gérer les raccourcis</translation>
    </message>
</context>
<context>
    <name>ShortcutField</name>
    <message>
        <location filename="../src/qml/qml/ShortcutField.qml" line="+14"/>
        <source>Record shortcut</source>
        <translation>Enregistrer le raccourci</translation>
    </message>
</context>
<context>
    <name>ShortcutFormView</name>
    <message>
        <location filename="../src/qml/qml/ShortcutFormView.qml" line="+14"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Shortcut Name</source>
        <translation>Nom du raccourci</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>The URL that will be opened by the specified app. You can make it dynamic by using placeholders such as {argument}.</source>
        <translation>L’URL qui sera ouverte par l’application spécifiée. Vous pouvez la rendre dynamique en utilisant des espaces réservés tels que {argument}.</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Open with</source>
        <translation>Ouvrir avec</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Icon</source>
        <translation>Icône</translation>
    </message>
</context>
<context>
    <name>ShortcutFormViewHost</name>
    <message>
        <location filename="../src/qml/shortcut-form-view-host.cpp" line="+47"/>
        <source>Submit</source>
        <translation>Valider</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Copy of %1</source>
        <translation>Copie de %1</translation>
    </message>
    <message>
        <location line="+67"/>
        <source>Edit &quot;%1&quot;</source>
        <translation>Modifier &quot;%1&quot;</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Duplicate &quot;%1&quot;</source>
        <translation>Dupliquer &quot;%1&quot;</translation>
    </message>
    <message>
        <location line="+11"/>
        <location line="+131"/>
        <location line="+50"/>
        <source>Default</source>
        <translation>Par défaut</translation>
    </message>
    <message>
        <location line="-157"/>
        <source>Selected Text</source>
        <translation>Texte sélectionné</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clipboard Text</source>
        <translation>Texte du presse-papiers</translation>
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
        <translation>Requis</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Validation failed</source>
        <translation>Échec de la validation</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Failed to update shortcut</source>
        <translation>Échec de la mise à jour du raccourci</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Shortcut updated</source>
        <translation>Raccourci mis à jour</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to create shortcut</source>
        <translation>Échec de la création du raccourci</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Shortcut created</source>
        <translation>Raccourci créé</translation>
    </message>
</context>
<context>
    <name>ShortcutRecorderField</name>
    <message>
        <location filename="../src/qml/qml/ShortcutRecorderField.qml" line="+24"/>
        <location line="+22"/>
        <location line="+62"/>
        <source>Recording...</source>
        <translation>Enregistrement...</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Keybind updated</source>
        <translation>Raccourci clavier mis à jour</translation>
    </message>
</context>
<context>
    <name>ShortcutRootProvider</name>
    <message>
        <location filename="../src/root-search/shortcuts/shortcut-root-provider.cpp" line="+42"/>
        <source>Shortcuts</source>
        <translation>Raccourcis</translation>
    </message>
</context>
<context>
    <name>ShortcutsSettingsPage</name>
    <message>
        <location filename="../src/qml/qml/ShortcutsSettingsPage.qml" line="+55"/>
        <source>Keybindings</source>
        <translation>Raccourcis clavier</translation>
    </message>
    <message>
        <location line="+78"/>
        <source>Record Shortcut</source>
        <translation>Enregistrer un raccourci</translation>
    </message>
</context>
<context>
    <name>SkipUpdateVersionAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+8"/>
        <source>Skip This Version</source>
        <translation>Ignorer cette version</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Skipped %1</source>
        <translation>%1 ignorée</translation>
    </message>
</context>
<context>
    <name>SleepCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+94"/>
        <source>Put System to Sleep</source>
        <translation>Mettre le système en veille</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Put system to sleep</source>
        <translation>Mettre le système en veille</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>System can&apos;t sleep</source>
        <translation>Le système ne peut pas se mettre en veille</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to sleep</source>
        <translation>Échec de la mise en veille</translation>
    </message>
</context>
<context>
    <name>SnippetDatabase</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.cpp" line="+42"/>
        <location line="+56"/>
        <source>keyword already assigned to &quot;%1&quot;</source>
        <translation>mot-clé déjà assigné à &quot;%1&quot;</translation>
    </message>
    <message>
        <location line="-43"/>
        <source>No snippet with that ID</source>
        <translation>Aucun snippet avec cet ID</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>No such snippet</source>
        <translation>Snippet introuvable</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Snippet limit reached (%1)</source>
        <translation>Limite de snippets atteinte (%1)</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Failed to save snippets on disk: %1</source>
        <translation>Échec de l’enregistrement des snippets sur le disque : %1</translation>
    </message>
</context>
<context>
    <name>SnippetExtension</name>
    <message>
        <location filename="../src/extensions/snippet/snippet-extension.cpp" line="+38"/>
        <source>Expansion</source>
        <translation>Expansion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Enable automatic snippet expansion when triggers are typed</source>
        <translation>Activer l’expansion automatique des snippets lors de la saisie des déclencheurs</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Undo</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Press backspace immediately after expansion to undo and restore the trigger text</source>
        <translation>Appuyez sur retour arrière juste après l’expansion pour annuler et restaurer le texte du déclencheur</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Keyboard layout</source>
        <translation>Disposition du clavier</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>XKB layout used for trigger detection (e.g. &quot;us&quot;, &quot;fr&quot;). Leave empty for system default.</source>
        <translation>Disposition XKB utilisée pour la détection des déclencheurs (par ex. &quot;us&quot;, &quot;fr&quot;). Laissez vide pour utiliser la valeur par défaut du système.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Pre-paste delay (ms)</source>
        <translation>Délai avant collage (ms)</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Delay between setting clipboard and injecting paste shortcut. Increase if expansions paste empty on slow compositors.</source>
        <translation>Délai entre l’écriture dans le presse-papiers et l’injection du raccourci de collage. Augmentez-le si les expansions collent un contenu vide sur les compositeurs lents.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Key injection delay (ms)</source>
        <translation>Délai d’injection des touches (ms)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Delay between injected key events. Increase if expansions produce missing or garbled characters on slow compositors.</source>
        <translation>Délai entre les événements de touches injectés. Augmentez-le si les expansions produisent des caractères manquants ou altérés sur les compositeurs lents.</translation>
    </message>
    <message>
        <location filename="../src/extensions/snippet/snippet-extension.hpp" line="+11"/>
        <source>Snippets</source>
        <translation>Snippets</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Text expansion and snippet management</source>
        <translation>Expansion de texte et gestion des snippets</translation>
    </message>
</context>
<context>
    <name>SnippetFormView</name>
    <message>
        <location filename="../src/qml/qml/SnippetFormView.qml" line="+15"/>
        <source>Title</source>
        <translation>Titre</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Euro symbol</source>
        <translation>Symbole euro</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Content</source>
        <translation>Contenu</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You can use {dynamic placeholders} to make the content dynamic: &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;learn more&lt;/a&gt;.</source>
        <translation>Vous pouvez utiliser des {espaces réservés dynamiques} pour rendre le contenu dynamique : &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;en savoir plus&lt;/a&gt;.</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Keyword</source>
        <translation>Mot-clé</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Typing this keyword anywhere will result in it being replaced by the content of the snippet.</source>
        <translation>Taper ce mot-clé n’importe où le remplacera par le contenu du snippet.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The snippet server is not running. Keyword expansion is unavailable. &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;Learn more&lt;/a&gt;.</source>
        <translation>Le serveur de snippets n’est pas en cours d’exécution. L’expansion par mot-clé est indisponible. &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;En savoir plus&lt;/a&gt;.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Applications</source>
        <translation>Applications</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Restrict expansion to specific applications. By default, it works everywhere.</source>
        <translation>Restreindre l’expansion à certaines applications. Par défaut, elle fonctionne partout.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Expand as word</source>
        <translation>Développer en tant que mot</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>If a keyword is typed, it will only be expanded after space or punctuation.</source>
        <translation>Si un mot-clé est tapé, il ne sera développé qu’après un espace ou une ponctuation.</translation>
    </message>
</context>
<context>
    <name>SnippetFormViewHost</name>
    <message>
        <location filename="../src/qml/snippet-form-view-host.cpp" line="+49"/>
        <source>Submit</source>
        <translation>Valider</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Copy of %1</source>
        <translation>Copie de %1</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Edit &quot;%1&quot;</source>
        <translation>Modifier &quot;%1&quot;</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Duplicate &quot;%1&quot;</source>
        <translation>Dupliquer &quot;%1&quot;</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>2 chars min.</source>
        <translation>2 caractères min.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Content should not be empty</source>
        <translation>Le contenu ne doit pas être vide</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Only one {cursor} placeholder is allowed</source>
        <translation>Un seul espace réservé {cursor} est autorisé</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Validation failed</source>
        <translation>Échec de la validation</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Snippet updated</source>
        <translation>Snippet mis à jour</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Snippet successfully created</source>
        <translation>Snippet créé avec succès</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Cursor Position</source>
        <translation>Position du curseur</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clipboard Text</source>
        <translation>Texte du presse-papiers</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Date</source>
        <translation>Date</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Argument</source>
        <translation>Argument</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Shell Command</source>
        <translation>Commande shell</translation>
    </message>
</context>
<context>
    <name>SoftRebootCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="-99"/>
        <source>Soft Reboot System</source>
        <translation>Redémarrer le système à chaud</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Soft reboot the system, which usually means only userspace is rebooted.</source>
        <translation>Redémarrer le système à chaud, ce qui signifie généralement que seul l’espace utilisateur est redémarré.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System can&apos;t soft reboot</source>
        <translation>Le système ne peut pas redémarrer à chaud</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to soft reboot</source>
        <translation>Échec du redémarrage à chaud</translation>
    </message>
</context>
<context>
    <name>SponsorVicinaeCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.cpp" line="-121"/>
        <source>Donate to Vicinae</source>
        <translation>Faire un don à Vicinae</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open link to Vicinae&apos;s GitHub sponsor page</source>
        <translation>Ouvrir le lien vers la page de sponsor GitHub de Vicinae</translation>
    </message>
</context>
<context>
    <name>StoreDetailView</name>
    <message>
        <location filename="../src/qml/qml/StoreDetailView.qml" line="+196"/>
        <source>Installed</source>
        <translation>Installée</translation>
    </message>
    <message>
        <location line="+166"/>
        <source>Description</source>
        <translation>Description</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Commands</source>
        <translation>Commandes</translation>
    </message>
    <message>
        <location line="+74"/>
        <source>Open README</source>
        <translation>Ouvrir le README</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Last update</source>
        <translation>Dernière mise à jour</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Contributors</source>
        <translation>Contributeurs</translation>
    </message>
    <message>
        <location line="+29"/>
        <source>Categories</source>
        <translation>Catégories</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Source Code</source>
        <translation>Code source</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>View Code</source>
        <translation>Voir le code</translation>
    </message>
</context>
<context>
    <name>SuspendCommand</name>
    <message>
        <location filename="../src/extensions/power-management/power-management-extension.cpp" line="+36"/>
        <source>Suspend System</source>
        <translation>Mettre en veille le système</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Suspend the system to RAM. Unlike hibernation, this does not turn the computer off and will break on power loss.</source>
        <translation>Mettre le système en veille en RAM. Contrairement à l’hibernation, cela n’éteint pas l’ordinateur et ne résistera pas à une coupure de courant.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System cannot suspend</source>
        <translation>Le système ne peut pas se mettre en veille</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to suspend</source>
        <translation>Échec de la mise en veille</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsCommand</name>
    <message>
        <location filename="../src/extensions/wm/switch-windows-command.hpp" line="+10"/>
        <source>Switch Windows</source>
        <translation>Changer de fenêtre</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsSection</name>
    <message>
        <location filename="../src/qml/switch-windows-model.hpp" line="+28"/>
        <source>Open Windows</source>
        <translation>Fenêtres ouvertes</translation>
    </message>
    <message>
        <location filename="../src/qml/switch-windows-model.cpp" line="+19"/>
        <source>WS %1</source>
        <translation>WS %1</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Window Actions</source>
        <translation>Actions de fenêtre</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsViewHost</name>
    <message>
        <location filename="../src/qml/switch-windows-view-host.cpp" line="+12"/>
        <source>Search open window...</source>
        <translation>Rechercher une fenêtre ouverte...</translation>
    </message>
</context>
<context>
    <name>SystemBrowseApps</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-123"/>
        <source>Browse Apps</source>
        <translation>Parcourir les applications</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse all applications that are installed on the system</source>
        <translation>Parcourir toutes les applications installées sur le système</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Show hidden apps</source>
        <translation>Afficher les applications masquées</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sort alphabetically</source>
        <translation>Trier par ordre alphabétique</translation>
    </message>
</context>
<context>
    <name>SystemExtension</name>
    <message>
        <location line="+149"/>
        <source>System</source>
        <translation>Système</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>System-related commands</source>
        <translation>Commandes liées au système</translation>
    </message>
</context>
<context>
    <name>SystemRunCommand</name>
    <message>
        <location line="-239"/>
        <source>Run Terminal Program</source>
        <translation>Exécuter un programme dans un terminal</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run a program in a terminal window</source>
        <translation>Exécuter un programme dans une fenêtre de terminal</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>command</source>
        <translation>commande</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Run in terminal</source>
        <translation>Exécuter dans un terminal</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run in terminal (hold)</source>
        <translation>Exécuter dans un terminal (garder ouvert)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run directly</source>
        <translation>Exécuter directement</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Default Action</source>
        <translation>Action par défaut</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The default action to run on pressing return</source>
        <translation>L’action par défaut à exécuter en appuyant sur Entrée</translation>
    </message>
    <message>
        <location line="+29"/>
        <source>Not a valid executable</source>
        <translation>Exécutable non valide</translation>
    </message>
</context>
<context>
    <name>SystemRunViewHost</name>
    <message>
        <location filename="../src/qml/system-run-view-host.cpp" line="+20"/>
        <source>Search for a program to execute...</source>
        <translation>Rechercher un programme à exécuter...</translation>
    </message>
</context>
<context>
    <name>ThemeExtension</name>
    <message>
        <location filename="../src/extensions/theme/theme-extension.hpp" line="+9"/>
        <source>Theme</source>
        <translation>Thème</translation>
    </message>
</context>
<context>
    <name>ThemeSection</name>
    <message>
        <location filename="../src/qml/theme-list-model.cpp" line="+22"/>
        <source>Default theme description</source>
        <translation>Description du thème par défaut</translation>
    </message>
    <message>
        <location line="+64"/>
        <source>Open theme file</source>
        <translation>Ouvrir le fichier du thème</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy ID</source>
        <translation>Copier l’ID</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy path</source>
        <translation>Copier le chemin</translation>
    </message>
</context>
<context>
    <name>ThemeViewHost</name>
    <message>
        <location filename="../src/qml/theme-view-host.cpp" line="+22"/>
        <source>Search for a theme...</source>
        <translation>Rechercher un thème...</translation>
    </message>
    <message>
        <location line="+51"/>
        <source>Current Theme</source>
        <translation>Thème actuel</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Available Themes</source>
        <translation>Thèmes disponibles</translation>
    </message>
</context>
<context>
    <name>ToggleItemAsFavorite</name>
    <message>
        <location filename="../src/actions/root-search/root-search-actions.cpp" line="+17"/>
        <source>Remove from favorites</source>
        <translation>Retirer des favoris</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Add to favorites</source>
        <translation>Ajouter aux favoris</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Successfuly added to favorites</source>
        <translation>Ajouté aux favoris avec succès</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Successfuly removed from favorites</source>
        <translation>Retiré des favoris avec succès</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to add to favorites</source>
        <translation>Échec de l’ajout aux favoris</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove from favorites</source>
        <translation>Échec du retrait des favoris</translation>
    </message>
</context>
<context>
    <name>ToggleMuteCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="+163"/>
        <source>Toggle Mute</source>
        <translation>Activer/désactiver la sourdine</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mute or unmute system audio</source>
        <translation>Couper ou rétablir le son du système</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Failed to toggle mute</source>
        <translation>Échec du basculement de la sourdine</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Muted</source>
        <translation>En sourdine</translation>
    </message>
</context>
<context>
    <name>UIPlayground</name>
    <message>
        <location filename="../src/extensions/internal/internal-extension.hpp" line="-15"/>
        <source>UI Showcase</source>
        <translation>Vitrine de l’interface</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Showcase of vicinae UI elements</source>
        <translation>Vitrine des éléments d’interface de vicinae</translation>
    </message>
</context>
<context>
    <name>UIShowcase</name>
    <message>
        <location filename="../src/qml/qml/UIShowcase.qml" line="+26"/>
        <source>Buttons</source>
        <translation>Boutons</translation>
    </message>
</context>
<context>
    <name>UninstallExtensionAction</name>
    <message>
        <location filename="../src/actions/extension/extension-actions.cpp" line="+11"/>
        <source>Are you sure?</source>
        <translation>Êtes-vous sûr ?</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>All this extension data will be permanently lost. If you just want the extension to not appear in the root search anymore, consider disabling it instead.</source>
        <translation>Toutes les données de cette extension seront définitivement perdues. Si vous souhaitez simplement que l’extension n’apparaisse plus dans la recherche principale, envisagez plutôt de la désactiver.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Uninstall</source>
        <translation>Désinstaller</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Extension uninstalled</source>
        <translation>Extension désinstallée</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to uninstall extension</source>
        <translation>Échec de la désinstallation de l’extension</translation>
    </message>
    <message>
        <location filename="../src/actions/extension/extension-actions.hpp" line="+14"/>
        <source>Uninstall Extension</source>
        <translation>Désinstaller l’extension</translation>
    </message>
</context>
<context>
    <name>UnpinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator/calculator-actions.hpp" line="-23"/>
        <source>Entry unpinned</source>
        <translation>Entrée désépinglée</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Unpin entry</source>
        <translation>Désépingler l’entrée</translation>
    </message>
</context>
<context>
    <name>UnpinEmojiAction</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-43"/>
        <source>Unpin emoji</source>
        <translation>Désépingler l’émoji</translation>
    </message>
</context>
<context>
    <name>UpdateService</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="-167"/>
        <source>Update installed</source>
        <translation>Mise à jour installée</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Restarting…</source>
        <translation>Redémarrage…</translation>
    </message>
    <message>
        <location line="+88"/>
        <source>Downloading Vicinae %1…</source>
        <translation>Téléchargement de Vicinae %1…</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading Vicinae %1… %2%</source>
        <translation>Téléchargement de Vicinae %1… %2%</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Installing update…</source>
        <translation>Installation de la mise à jour…</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Update failed</source>
        <translation>Échec de la mise à jour</translation>
    </message>
</context>
<context>
    <name>VicinaeExtension</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-extension.hpp" line="+11"/>
        <source>General vicinae-related commands.</source>
        <translation>Commandes générales liées à vicinae.</translation>
    </message>
</context>
<context>
    <name>VicinaeHotkeyGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/vicinae-hotkey-global-shortcut-backend.cpp" line="+48"/>
        <source>Unsupported trigger key</source>
        <translation>Touche de déclenchement non prise en charge</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Hotkey binding was lost</source>
        <translation>L’association du raccourci clavier a été perdue</translation>
    </message>
</context>
<context>
    <name>VicinaeListInstalledExtensionsCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/list-installed-extensions-command.hpp" line="+11"/>
        <source>Show Installed Extensions</source>
        <translation>Afficher les extensions installées</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Show all third-party extensions that have been installed. This includes local extensions as well as extensions downloaded from the stores (vicinae and raycast).</source>
        <translation>Afficher toutes les extensions tierces installées. Cela inclut les extensions locales ainsi que les extensions téléchargées depuis les boutiques (vicinae et raycast).</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreCommand</name>
    <message>
        <location filename="../src/extensions/vicinae/vicinae-store-command.hpp" line="+13"/>
        <source>Install extensions from the Vicinae store</source>
        <translation>Installer des extensions depuis la boutique Vicinae</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Always show intro</source>
        <translation>Toujours afficher l’introduction</translation>
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
# Bienvenue dans la boutique d’extensions vicinae

La boutique d’extensions vicinae propose des extensions créées par la communauté et approuvées par nos contributeurs principaux.

Le code source de chaque extension listée ici est disponible dans le dépôt [vicinaehq/extensions](https://github.com/vicinaehq/extensions).

Si vous souhaitez créer votre propre extension, consultez la [documentation](https://docs.vicinae.com/extensions/introduction). Si vous pensez que votre extension aurait sa place dans la boutique, n’hésitez pas à la soumettre !
</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Continue to store</source>
        <translation>Continuer vers la boutique</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreDetailHost</name>
    <message>
        <location filename="../src/qml/vicinae-store-detail-host.cpp" line="+38"/>
        <source>Failed to load extension</source>
        <translation>Échec du chargement de l’extension</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Could not fetch extension data from the store.</source>
        <translation>Impossible de récupérer les données de l’extension depuis la boutique.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Extension not found</source>
        <translation>Extension introuvable</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The extension &quot;%1&quot; could not be found in the store.</source>
        <translation>L’extension &quot;%1&quot; est introuvable dans la boutique.</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Extension Store - %1</source>
        <translation>Boutique d’extensions - %1</translation>
    </message>
    <message>
        <location line="+81"/>
        <source>Install extension</source>
        <translation>Installer l’extension</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading extension...</source>
        <translation>Téléchargement de l’extension...</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to download extension</source>
        <translation>Échec du téléchargement de l’extension</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to extract extension archive</source>
        <translation>Échec de l’extraction de l’archive de l’extension</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Extension installed</source>
        <translation>Extension installée</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Report issue</source>
        <translation>Signaler un problème</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreSection</name>
    <message>
        <location filename="../src/qml/vicinae-store-model.cpp" line="+41"/>
        <source>Show details</source>
        <translation>Afficher les détails</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreViewHost</name>
    <message>
        <location filename="../src/qml/vicinae-store-view-host.cpp" line="+27"/>
        <source>Browse Vicinae extensions</source>
        <translation>Parcourir les extensions Vicinae</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Failed to fetch extensions</source>
        <translation>Échec de la récupération des extensions</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Extensions</source>
        <translation>Extensions</translation>
    </message>
</context>
<context>
    <name>VolumeDownCommand</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-74"/>
        <source>Turn Volume Down</source>
        <translation>Baisser le volume</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Decrease system volume</source>
        <translation>Diminuer le volume du système</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid step value</source>
        <translation>Valeur de pas non valide</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to adjust volume</source>
        <translation>Échec de l’ajustement du volume</translation>
    </message>
</context>
<context>
    <name>VolumeUpCommand</name>
    <message>
        <location line="-61"/>
        <source>Turn Volume Up</source>
        <translation>Augmenter le volume</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Increase system volume</source>
        <translation>Augmenter le volume du système</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid step value</source>
        <translation>Valeur de pas non valide</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to adjust volume</source>
        <translation>Échec de l’ajustement du volume</translation>
    </message>
</context>
<context>
    <name>WallpaperManager</name>
    <message>
        <location filename="../src/services/wallpaper/wallpaper-manager.cpp" line="+68"/>
        <source>Setting the wallpaper is not supported in the current environment</source>
        <translation>La définition du fond d’écran n’est pas prise en charge dans l’environnement actuel</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No such file: %1</source>
        <translation>Fichier introuvable : %1</translation>
    </message>
</context>
<context>
    <name>WinControlPanelRootItem</name>
    <message>
        <location filename="../src/root-search/control-panel/control-panel-root-provider.cpp" line="+104"/>
        <location line="+10"/>
        <source>Control Panel</source>
        <translation>Panneau de configuration</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Where</source>
        <translation>Emplacement</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Open Applet</source>
        <translation>Ouvrir l’applet</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Path</source>
        <translation>Copier le chemin</translation>
    </message>
</context>
<context>
    <name>WinControlPanelRootProvider</name>
    <message>
        <location line="+49"/>
        <source>Control Panel</source>
        <translation>Panneau de configuration</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Control Panel applets and system tasks.</source>
        <translation>Applets du Panneau de configuration et tâches système.</translation>
    </message>
</context>
<context>
    <name>WinControlPanelTaskRootItem</name>
    <message>
        <location line="-40"/>
        <location line="+11"/>
        <source>Control Panel</source>
        <translation>Panneau de configuration</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Task ID</source>
        <translation>Identifiant de tâche</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Open</source>
        <translation>Ouvrir</translation>
    </message>
</context>
<context>
    <name>WinSettingsPage</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="-114"/>
        <source>Display</source>
        <translation>Affichage</translation>
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
        <translation>Système</translation>
    </message>
    <message>
        <location line="-17"/>
        <source>Night Light</source>
        <translation>Éclairage nocturne</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sound</source>
        <translation>Son</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Volume Mixer</source>
        <translation>Mélangeur de volume</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Notifications</source>
        <translation>Notifications</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Focus</source>
        <translation>Concentration</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power &amp; Battery</source>
        <translation>Alimentation et batterie</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Storage</source>
        <translation>Stockage</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Nearby Sharing</source>
        <translation>Partage de proximité</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Multitasking</source>
        <translation>Multitâche</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activation</source>
        <translation>Activation</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Troubleshoot</source>
        <translation>Résolution des problèmes</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Recovery</source>
        <translation>Récupération</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Projecting to This PC</source>
        <translation>Projection sur ce PC</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Remote Desktop</source>
        <translation>Bureau à distance</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clipboard</source>
        <translation>Presse-papiers</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>About</source>
        <translation>Informations système</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Optional Features</source>
        <translation>Fonctionnalités facultatives</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>For Developers</source>
        <translation>Espace développeurs</translation>
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
        <translation>Bluetooth et appareils</translation>
    </message>
    <message>
        <location line="-8"/>
        <source>Devices</source>
        <translation>Appareils</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Printers &amp; Scanners</source>
        <translation>Imprimantes et scanners</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mobile Devices</source>
        <translation>Appareils mobiles</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Cameras</source>
        <translation>Caméras</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mouse</source>
        <translation>Souris</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Touchpad</source>
        <translation>Pavé tactile</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pen &amp; Windows Ink</source>
        <translation>Stylet et Windows Ink</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>AutoPlay</source>
        <translation>Exécution automatique</translation>
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
        <translation>Réseau et Internet</translation>
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
        <translation>Point d’accès sans fil mobile</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Airplane Mode</source>
        <translation>Mode avion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Proxy</source>
        <translation>Proxy</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dial-up</source>
        <translation>Accès réseau à distance</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced Network Settings</source>
        <translation>Paramètres réseau avancés</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Background</source>
        <translation>Arrière-plan</translation>
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
        <translation>Personnalisation</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Colors</source>
        <translation>Couleurs</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Themes</source>
        <translation>Thèmes</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lock Screen</source>
        <translation>Écran de verrouillage</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Touch Keyboard</source>
        <translation>Clavier tactile</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Start</source>
        <translation>Démarrer</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Taskbar</source>
        <translation>Barre des tâches</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Fonts</source>
        <translation>Polices</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dynamic Lighting</source>
        <translation>Éclairage dynamique</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Installed Apps</source>
        <translation>Applications installées</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Apps</source>
        <translation>Applications</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>Default Apps</source>
        <translation>Applications par défaut</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Offline Maps</source>
        <translation>Cartes hors connexion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Apps for Websites</source>
        <translation>Applications pour les sites web</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Video Playback</source>
        <translation>Lecture vidéo</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Startup Apps</source>
        <translation>Applications de démarrage</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Your Info</source>
        <translation>Vos informations</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Accounts</source>
        <translation>Comptes</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>Email &amp; Accounts</source>
        <translation>E-mail et comptes</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sign-in Options</source>
        <translation>Options de connexion</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Access Work or School</source>
        <translation>Accès professionnel ou scolaire</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Family &amp; Other Users</source>
        <translation>Famille et autres utilisateurs</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Windows Backup</source>
        <translation>Sauvegarde Windows</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Date &amp; Time</source>
        <translation>Date et heure</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Time &amp; Language</source>
        <translation>Heure et langue</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Language &amp; Region</source>
        <translation>Langue et région</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Typing</source>
        <translation>Saisie</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Speech</source>
        <translation>Voix</translation>
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
        <translation>Jeux</translation>
    </message>
    <message>
        <location line="-1"/>
        <source>Captures</source>
        <translation>Captures</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Game Mode</source>
        <translation>Mode Jeu</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Text Size</source>
        <translation>Taille du texte</translation>
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
        <translation>Accessibilité</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Visual Effects</source>
        <translation>Effets visuels</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Magnifier</source>
        <translation>Loupe</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Color Filters</source>
        <translation>Filtres de couleur</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Contrast Themes</source>
        <translation>Thèmes de contraste</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Narrator</source>
        <translation>Narrateur</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Audio</source>
        <translation>Audio (accessibilité)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Captions</source>
        <translation>Sous-titres</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Keyboard</source>
        <translation>Clavier (accessibilité)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Mouse</source>
        <translation>Souris (accessibilité)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Eye Control</source>
        <translation>Contrôle visuel</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Windows Security</source>
        <translation>Sécurité Windows</translation>
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
        <translation>Confidentialité et sécurité</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Find My Device</source>
        <translation>Localiser mon appareil</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Privacy</source>
        <translation>Confidentialité</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Location</source>
        <translation>Localisation</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Camera Access</source>
        <translation>Accès à la caméra</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Microphone Access</source>
        <translation>Accès au microphone</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activity History</source>
        <translation>Historique des activités</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Diagnostics &amp; Feedback</source>
        <translation>Diagnostics et commentaires</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search Permissions</source>
        <translation>Autorisations de recherche</translation>
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
        <translation>Historique des mises à jour</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced Update Options</source>
        <translation>Options avancées de mise à jour</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Windows Insider Program</source>
        <translation>Programme Windows Insider</translation>
    </message>
</context>
<context>
    <name>WinSettingsPageRootItem</name>
    <message>
        <location line="+37"/>
        <source>System Settings</source>
        <translation>Paramètres système</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Settings</source>
        <translation>Paramètres</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Category</source>
        <translation>Catégorie</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Open %1 Settings</source>
        <translation>Ouvrir les paramètres %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy URL</source>
        <translation>Copier l’URL</translation>
    </message>
</context>
<context>
    <name>WinSettingsRootProvider</name>
    <message>
        <location line="+12"/>
        <source>Windows Settings</source>
        <translation>Paramètres Windows</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Pages of the Windows Settings app.</source>
        <translation>Pages de l’application Paramètres de Windows.</translation>
    </message>
</context>
<context>
    <name>WindowManagementExtension</name>
    <message>
        <location filename="../src/extensions/wm/wm-extension.hpp" line="+12"/>
        <source>Window Management</source>
        <translation>Gestion des fenêtres</translation>
    </message>
</context>
<context>
    <name>WindowsAppDatabase</name>
    <message>
        <location filename="../src/services/app-service/windows/win-app-database.cpp" line="+993"/>
        <source>Focus window</source>
        <translation>Activer la fenêtre</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Launch app</source>
        <translation>Lancer l’application</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Default action</source>
        <translation>Action par défaut</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Action to perform when the return key is pressed. Always default to &apos;launch&apos; if the app has no open window.</source>
        <translation>Action à effectuer lorsque la touche Entrée est pressée. Utilise toujours ’launch’ par défaut si l’application n’a aucune fenêtre ouverte.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>File Explorer</source>
        <translation>Explorateur de fichiers</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Terminal</source>
        <translation>Terminal</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Command Prompt</source>
        <translation>Invite de commandes</translation>
    </message>
</context>
<context>
    <name>WindowsApplication</name>
    <message>
        <location filename="../src/services/app-service/windows/win-app.hpp" line="+70"/>
        <source>%1: Run as Administrator</source>
        <translation>%1 : Exécuter en tant qu’administrateur</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run as Administrator</source>
        <translation>Exécuter en tant qu’administrateur</translation>
    </message>
</context>
<context>
    <name>WindowsGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/windows-global-shortcut-backend.cpp" line="+215"/>
        <source>unsupported or invalid trigger</source>
        <translation>déclencheur non pris en charge ou non valide</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>already registered by another application</source>
        <translation>déjà enregistré par une autre application</translation>
    </message>
</context>
<context>
    <name>X11GlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/x11-global-shortcut-backend.cpp" line="+122"/>
        <source>This shortcut is already in use by another application</source>
        <translation>Ce raccourci est déjà utilisé par une autre application</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Unsupported trigger key</source>
        <translation>Touche de déclenchement non prise en charge</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Trigger key is not present on this keyboard</source>
        <translation>La touche de déclenchement n’est pas présente sur ce clavier</translation>
    </message>
</context>
<context>
    <name>X11Workspace</name>
    <message>
        <location filename="../src/services/window-manager/x11/x11-window-manager.cpp" line="+426"/>
        <source>Desktop %1</source>
        <translation>Bureau %1</translation>
    </message>
</context>
<context>
    <name>XdgAppDatabase</name>
    <message>
        <location filename="../src/services/app-service/xdg/xdg-app-database.cpp" line="+564"/>
        <source>Focus window</source>
        <translation>Activer la fenêtre</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Launch app</source>
        <translation>Lancer l’application</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Default action</source>
        <translation>Action par défaut</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Action to perform when the return key is pressed. Always default to &apos;launch&apos; if the app has no open window.</source>
        <translation>Action à effectuer lorsque la touche Entrée est pressée. Utilise toujours ’launch’ par défaut si l’application n’a aucune fenêtre ouverte.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Launch Prefix</source>
        <translation>Préfixe de lancement</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Custom app launcher to use. Affects applications as well as their sub-actions.</source>
        <translation>Lanceur d’applications personnalisé à utiliser. Affecte les applications ainsi que leurs sous-actions.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Application directories</source>
        <translation>Répertoires d’applications</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Directories applications are sourced from. The list cannot be modified directly. In order to do so, you need to append additonal paths to the &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt; environment variables.</source>
        <translation>Répertoires d’où sont chargées les applications. La liste ne peut pas être modifiée directement. Pour ce faire, vous devez ajouter des chemins supplémentaires à la variable d’environnement &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt;.</translation>
    </message>
</context>
<context>
    <name>XdpFileChooser</name>
    <message>
        <location filename="../src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.cpp" line="+39"/>
        <source>Open Directory</source>
        <translation>Ouvrir un dossier</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Open File</source>
        <translation>Ouvrir un fichier</translation>
    </message>
</context>
<context>
    <name>browser-extension</name>
    <message>
        <location filename="../src/extensions/browser/browser-extension.cpp" line="-58"/>
        <source>No browser connected</source>
        <translation>Aucun navigateur connecté</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You need to connect at least one browser to vicinae using the browser extension in order to use this command.</source>
        <translation>Vous devez connecter au moins un navigateur à vicinae via l’extension de navigateur pour utiliser cette commande.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Open documentation</source>
        <translation>Ouvrir la documentation</translation>
    </message>
</context>
<context>
    <name>clipboard-history-view-host</name>
    <message>
        <location filename="../src/qml/clipboard-history-view-host.cpp" line="-207"/>
        <source>Text</source>
        <translation>Texte</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Link</source>
        <translation>Lien</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Image</source>
        <translation>Image</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>File</source>
        <translation>Fichier</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Unknown</source>
        <translation>Inconnu</translation>
    </message>
</context>
<context>
    <name>emoji-categories</name>
    <message>
        <location filename="../src/qml/emoji-grid-model.cpp" line="-52"/>
        <source>Smileys &amp; Emotion</source>
        <translation>Smileys et émotions</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>People &amp; Body</source>
        <translation>Personnes et corps</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Animals &amp; Nature</source>
        <translation>Animaux et nature</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Food &amp; Drink</source>
        <translation>Nourriture et boissons</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Travel &amp; Places</source>
        <translation>Voyages et lieux</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activities</source>
        <translation>Activités</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Objects</source>
        <translation>Objets</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Symbols</source>
        <translation>Symboles</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Flags</source>
        <translation>Drapeaux</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Math</source>
        <translation>Mathématiques</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Arrows</source>
        <translation>Flèches</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Currency</source>
        <translation>Devises</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Punctuation</source>
        <translation>Ponctuation</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Shapes</source>
        <translation>Formes</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Misc Symbols</source>
        <translation>Symboles divers</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Greek</source>
        <translation>Grec</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Number Forms</source>
        <translation>Formes numériques</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Fancy Letters</source>
        <translation>Lettres stylisées</translation>
    </message>
</context>
<context>
    <name>emoji-grid-model</name>
    <message>
        <location line="+129"/>
        <source>Copy</source>
        <translation>Copier</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy name</source>
        <translation>Copier le nom</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy unicode codepoint</source>
        <translation>Copier le point de code Unicode</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy category</source>
        <translation>Copier la catégorie</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>Skin tones</source>
        <translation>Teintes de peau</translation>
    </message>
</context>
<context>
    <name>file-list-item</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+35"/>
        <source>Copy file</source>
        <translation>Copier le fichier</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy file path</source>
        <translation>Copier le chemin du fichier</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy file name</source>
        <translation>Copier le nom du fichier</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Copy mime type</source>
        <translation>Copier le type MIME</translation>
    </message>
</context>
<context>
    <name>font-categories</name>
    <message>
        <location filename="../src/font-service.cpp" line="+127"/>
        <source>Latin</source>
        <translation>Latin</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Cyrillic</source>
        <translation>Cyrillique</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Greek</source>
        <translation>Grec</translation>
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
        <translation>Japonais</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Korean</source>
        <translation>Coréen</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Simplified Chinese</source>
        <translation>Chinois simplifié</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Traditional Chinese</source>
        <translation>Chinois traditionnel</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Arabic</source>
        <translation>Arabe</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Hebrew</source>
        <translation>Hébreu</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Thai</source>
        <translation>Thaï</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lao</source>
        <translation>Lao</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Devanagari</source>
        <translation>Dévanagari</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Bengali</source>
        <translation>Bengali</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Gurmukhi</source>
        <translation>Gourmoukhî</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Gujarati</source>
        <translation>Goudjarati</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tamil</source>
        <translation>Tamoul</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Telugu</source>
        <translation>Télougou</translation>
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
        <translation>Cingalais</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Armenian</source>
        <translation>Arménien</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Georgian</source>
        <translation>Géorgien</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Thaana</source>
        <translation>Thâna</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tibetan</source>
        <translation>Tibétain</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Myanmar</source>
        <translation>Birman</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Khmer</source>
        <translation>Khmer</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Syriac</source>
        <translation>Syriaque</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ogham</source>
        <translation>Ogham</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Runic</source>
        <translation>Runique</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>N&apos;Ko</source>
        <translation>N&apos;Ko</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Symbols</source>
        <translation>Symboles</translation>
    </message>
</context>
<context>
    <name>font-grid-model</name>
    <message>
        <location filename="../src/qml/font-grid-model.cpp" line="+26"/>
        <source>Copy font family</source>
        <translation>Copier la famille de police</translation>
    </message>
</context>
<context>
    <name>keybind-manager</name>
    <message>
        <location filename="../src/internal/keyboard/keybind-manager.cpp" line="+9"/>
        <source>Toggle action panel</source>
        <translation>Afficher/Masquer le panneau d’actions</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Toggle the action panel to access and filter through the list of available actions for the currently selected item</source>
        <translation>Afficher ou masquer le panneau d’actions pour accéder à la liste des actions disponibles pour l’élément actuellement sélectionné et la filtrer</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Open Search Filter</source>
        <translation>Ouvrir le filtre de recherche</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the search filter selector if present</source>
        <translation>Ouvrir le sélecteur de filtre de recherche s’il est présent</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Open settings window</source>
        <translation>Ouvrir la fenêtre des paramètres</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open this settings window from the launcher window</source>
        <translation>Ouvrir cette fenêtre de paramètres depuis la fenêtre du lanceur</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Open Action</source>
        <translation>Action d’ouverture générique</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can open the selected item</source>
        <translation>Peut être utilisé par les actions capables d’ouvrir l’élément sélectionné</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Copy Action</source>
        <translation>Action de copie générique</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the selected item</source>
        <translation>Peut être utilisé par les actions capables de copier l’élément sélectionné</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Name Action</source>
        <translation>Action de copie du nom</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the name of the selected item</source>
        <translation>Peut être utilisé par les actions capables de copier le nom de l’élément sélectionné</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Path Action</source>
        <translation>Action de copie du chemin</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the path of the selected item</source>
        <translation>Peut être utilisé par les actions capables de copier le chemin de l’élément sélectionné</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Save Action</source>
        <translation>Action d’enregistrement</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can save the selected item</source>
        <translation>Peut être utilisé par les actions capables d’enregistrer l’élément sélectionné</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Duplicate Action</source>
        <translation>Action de duplication</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can duplicate the selected item</source>
        <translation>Peut être utilisé par les actions capables de dupliquer l’élément sélectionné</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic New Action</source>
        <translation>Action de création générique</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that create something</source>
        <translation>Peut être utilisé par les actions qui créent quelque chose</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Move Up Action</source>
        <translation>Action générique de déplacement vers le haut</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can move up the selected item. This does not affect list navigation controls.</source>
        <translation>Peut être utilisé par les actions capables de déplacer l’élément sélectionné vers le haut. Cela n’affecte pas les contrôles de navigation dans les listes.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Move Down Action</source>
        <translation>Action générique de déplacement vers le bas</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can move down the selected item. This does not affect list navigation controls.</source>
        <translation>Peut être utilisé par les actions capables de déplacer l’élément sélectionné vers le bas. Cela n’affecte pas les contrôles de navigation dans les listes.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Refresh Action</source>
        <translation>Action d’actualisation générique</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can refresh the selected item</source>
        <translation>Peut être utilisé par les actions capables d’actualiser l’élément sélectionné</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Pin Action</source>
        <translation>Action d’épinglage générique</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can pin the selected item</source>
        <translation>Peut être utilisé par les actions capables d’épingler l’élément sélectionné</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Remove Action</source>
        <translation>Action de suppression</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can remove the selected item. This is normally used for small, not too impactful removals.</source>
        <translation>Peut être utilisé par les actions capables de supprimer l’élément sélectionné. Ceci est normalement utilisé pour des suppressions mineures, sans grande conséquence.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Dangerous Remove Action</source>
        <translation>Action de suppression dangereuse</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that perform an impactful removal, generally accompanied by a confirmation dialog.</source>
        <translation>Peut être utilisé par les actions effectuant une suppression importante, généralement accompagnée d’un dialogue de confirmation.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit Action</source>
        <translation>Action de modification</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can edit the currently selected item</source>
        <translation>Peut être utilisé par les actions capables de modifier l’élément actuellement sélectionné</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit Secondary Action</source>
        <translation>Action de modification secondaire</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can edit a secondary characteristic of the currently selected item</source>
        <translation>Peut être utilisé par les actions capables de modifier une caractéristique secondaire de l’élément actuellement sélectionné</translation>
    </message>
</context>
<context>
    <name>macos-update-installer</name>
    <message>
        <location filename="../src/services/update/macos-update-installer.mm" line="-184"/>
        <source>Update image contains more than one app</source>
        <translation>L’image de mise à jour contient plusieurs applications</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to list update image: %1</source>
        <translation>Échec de la lecture du contenu de l’image de mise à jour : %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No app found in update image</source>
        <translation>Aucune application trouvée dans l’image de mise à jour</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to read the update&apos;s code signature</source>
        <translation>Échec de la lecture de la signature de code de la mise à jour</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to build the signature requirement</source>
        <translation>Échec de la construction de l’exigence de signature</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Update signature verification failed (%1)</source>
        <translation>Échec de la vérification de la signature de la mise à jour (%1)</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Update has no CFBundleShortVersionString</source>
        <translation>La mise à jour n’a pas de CFBundleShortVersionString</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Update version mismatch: expected %1, found %2</source>
        <translation>La version de la mise à jour ne correspond pas : %1 attendue, %2 trouvée</translation>
    </message>
</context>
<context>
    <name>shortcut-conflict</name>
    <message>
        <location filename="../src/qml/shortcut-conflict.cpp" line="+10"/>
        <source>Modifier required</source>
        <translation>Modificateur requis</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+5"/>
        <source>Already bound to &quot;%1&quot;</source>
        <translation>Déjà associé à &quot;%1&quot;</translation>
    </message>
</context>
<context>
    <name>system-extension</name>
    <message>
        <location filename="../src/extensions/system/system-extension.hpp" line="-34"/>
        <source>Volume %1%</source>
        <translation>Volume %1%</translation>
    </message>
</context>
<context>
    <name>utils</name>
    <message>
        <location filename="../src/utils/utils.cpp" line="+88"/>
        <source>0 bytes</source>
        <translation>0 octet</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>bytes</source>
        <translation>octets</translation>
    </message>
</context>
<context>
    <name>virtual-desktops</name>
    <message>
        <location filename="../src/services/window-manager/windows/virtual-desktops.cpp" line="+67"/>
        <source>Desktop %1</source>
        <translation>Bureau %1</translation>
    </message>
</context>
</TS>
