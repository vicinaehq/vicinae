import QtQuick
import Vicinae

ScriptOutputText {
    id: root
    required property ScriptExecutorViewHost host

    text: root.host.outputHtml

    Component.onCompleted: Qt.callLater(focusText)

    onContentHeightChanged: {
        let flick = contentItem;
        if (flick.contentY >= flick.contentHeight - root.height - 60)
            scrollToBottom();
    }
}
