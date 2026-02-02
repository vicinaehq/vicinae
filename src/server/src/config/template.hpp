namespace config {
const std::string_view TEMPLATE = R"({
	"meta": {
      "imports": []
	},

	// Whether root search should also search files directly.
	// File search is asynchronous, so file search results may take additional time to show up.
	// Note that there is a proper "File Search" command that can be used to search files, with content preview.
	"search_files_in_root": false,

	// If the layer shell protocol is used to position the window and the keyboard interactivity is
	// set to "exclusive" (the default) then this setting will have no effect. Either switch to "on_demand" interactivity
	// or disable layer shell.
	"close_on_focus_loss": true,

	// Whether IME preedit strings should be searched in real time.
	"consider_preedit": false,
   
	// Reset the navigation state every time the window is closed
	"pop_to_root_on_close": false,

	// What favicon service to use when loading favicons is needed.
	// Available values are: 'twenty' | 'google' | 'none'
	// If this is set to 'none', favicon loading is disabled and a placeholder icon will be used
	// everywhere a favicon is expected.
	"favicon_service": "twenty",

	// EXPERIMENTAL!
	// Enable specifing editing motions in the main search bar and during navigation.
	// This will probably be changed in future updates.
	// Supports: 'default' | 'emacs'
	"keybinding": "default",

	"font": {
		"normal": {
			// The font family to use for the general vicinae UI.
			"family": "auto",
			// The point size of the font
			"size": 10.5
		}
	},

	"theme": {
		"name": "vicinae-dark"
	},

	// List of entrypoints that are tagged as "favorite".
	// They show up on the very top of the root search when no search query is active.
	// Each value is a serialized entrypoint ID.
	"favorites": [
		"@vicinae/clipboard:history",
	],

	// List of entrypoints to suggest as a fallback when no results match the
	// provided search query.
	"fallbacks": [
		"@vicinae/files:search"
	],

	"launcher_window": {
      "opacity": 0.98,
      "csd": {
         "enabled": true,
         "rounding": 10,
         "border_width": 3
      },
      "size": {
         "width": 770,
         "height": 480
      },
      "screen": "auto",
      "layer_shell": {
         "scope": "vicinae",
         "keyboard_interactivity": "exclusive",
         "layer": "top",
         "enabled": true
      }
   },

   "keybinds": {},
   "providers": {}
})";
};
