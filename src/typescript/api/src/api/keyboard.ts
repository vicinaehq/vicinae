/**
 * @category Keyboard
 */
export type KeyEquivalent =
	| "a"
	| "b"
	| "c"
	| "d"
	| "e"
	| "f"
	| "g"
	| "h"
	| "i"
	| "j"
	| "k"
	| "l"
	| "m"
	| "n"
	| "o"
	| "p"
	| "q"
	| "r"
	| "s"
	| "t"
	| "u"
	| "v"
	| "w"
	| "x"
	| "y"
	| "z"
	| "0"
	| "1"
	| "2"
	| "3"
	| "4"
	| "5"
	| "6"
	| "7"
	| "8"
	| "9"
	| "."
	| ","
	| ";"
	| "="
	| "+"
	| "-"
	| "["
	| "]"
	| "{"
	| "}"
	| "«"
	| "»"
	| "("
	| ")"
	| "/"
	| "\\"
	| "'"
	| "`"
	| "§"
	| "^"
	| "@"
	| "$"
	| "return"
	| "delete"
	| "deleteForward"
	| "tab"
	| "arrowUp"
	| "arrowDown"
	| "arrowLeft"
	| "arrowRight"
	| "pageUp"
	| "pageDown"
	| "home"
	| "end"
	| "space"
	| "escape"
	| "enter"
	| "backspace";

/**
 * @category Keyboard
 * `cmd` maps to command on macOS, control elsewhere
 * `meta` and `windows` both map to the meta key, also known as "windows" key or "super" key
 * `alt` and `opt` both map to alt on every platform, the latter being the name it is given on macOS.
 */
export type KeyModifier =
	| "cmd"
	| "windows"
	| "meta"
	| "ctrl"
	| "opt"
	| "alt"
	| "shift";

/**
 * @category Keyboard
 */
export namespace Keyboard {
	export namespace Shortcut {
		export type Common =
			| "copy"
			| "copy-deeplink"
			| "copy-name"
			| "copy-path"
			| "save"
			| "duplicate"
			| "edit"
			| "move-down"
			| "move-up"
			| "new"
			| "open"
			| "open-with"
			| "pin"
			| "refresh"
			| "remove"
			| "remove-all";

		export const Common = {
			Copy: "copy",
			CopyDeeplink: "copy",
			CopyName: "copy",
			CopyPath: "copy",
			Save: "save",
			Duplicate: "duplicate",
			Edit: "edit",
			MoveDown: "move-down",
			MoveUp: "move-up",
			New: "new",
			Open: "open",
			OpenWith: "open-with",
			Pin: "pin",
			Refresh: "refresh",
			Remove: "remove",
			RemoveAll: "remove-all",
		};
	}

	export type Shortcut = {
		key: KeyEquivalent;
		modifiers: KeyModifier[];
	};
}
