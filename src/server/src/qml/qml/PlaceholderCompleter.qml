import QtQuick
import QtQuick.Controls

Item {
    id: root

    property var completions: []
    property string triggerChar: "{"
    property string text: ""
    property int cursorPosition: 0

    readonly property bool active: completionPopup.visible

    signal completionAccepted(string newText, int newCursorPos)

    function _findTriggerStart(txt, pos) {
        for (let i = pos - 1; i >= 0; i--) {
            const c = txt.charAt(i);
            if (c === '}')
                return -1;
            if (c === root.triggerChar) {
                if (i > 0 && txt.charAt(i - 1) === '\\')
                    return -1;
                return i;
            }
        }
        return -1;
    }

    function update(txt, cursorPos) {
        const triggerIdx = _findTriggerStart(txt, cursorPos);
        if (triggerIdx < 0) {
            completionPopup.close();
            return;
        }
        const afterTrigger = txt.substring(triggerIdx + 1, cursorPos);
        const query = afterTrigger.trim();
        completionPopup.filter(query);
        if (completionPopup.count > 0)
            completionPopup.open();
        else
            completionPopup.close();
    }

    function accept() {
        completionPopup.acceptHighlighted();
    }

    function moveUp() {
        completionPopup.moveUp();
    }

    function moveDown() {
        completionPopup.moveDown();
    }

    function dismiss() {
        completionPopup.close();
    }

    CompletionPopup {
        id: completionPopup
        parent: root
        x: 0
        y: root.height + 4
        width: Math.max(200, root.width)
        items: root.completions

        onItemAccepted: itemData => {
            const txt = root.text;
            const curPos = root.cursorPosition;
            const triggerIdx = root._findTriggerStart(txt, curPos);
            if (triggerIdx < 0)
                return;

            const before = txt.substring(0, triggerIdx);
            const template = itemData.template ?? (root.triggerChar + itemData.value + "}");
            const cursorOffset = itemData.cursorOffset ?? template.length;

            let endIdx = curPos;
            while (endIdx < txt.length && txt.charAt(endIdx) !== "}" && txt.charAt(endIdx) !== root.triggerChar)
                endIdx++;
            if (endIdx < txt.length && txt.charAt(endIdx) === "}")
                endIdx++;

            const after = txt.substring(endIdx);
            root.completionAccepted(before + template + after, before.length + cursorOffset);
        }
    }
}
