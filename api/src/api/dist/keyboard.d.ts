export type KeyEquivalent = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z" | "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" | "." | "," | ";" | "=" | "+" | "-" | "[" | "]" | "{" | "}" | "«" | "»" | "(" | ")" | "/" | "\\" | "'" | "`" | "§" | "^" | "@" | "$" | "return" | "delete" | "deleteForward" | "tab" | "arrowUp" | "arrowDown" | "arrowLeft" | "arrowRight" | "pageUp" | "pageDown" | "home" | "end" | "space" | "escape" | "enter" | "backspace";
export declare namespace Keyboard {
    type Shortcut = {
        key: KeyEquivalent;
        modifiers: KeyModifier[];
    };
}
export type KeyModifier = "cmd" | "ctrl" | "opt" | "shift";
export declare const Keyboard: {
    Shortcut: {
        Common: {
            Copy: Keyboard.Shortcut;
            CopyName: Keyboard.Shortcut;
        };
    };
};
//# sourceMappingURL=keyboard.d.ts.map