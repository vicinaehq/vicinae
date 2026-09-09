pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

ScriptOutputText {
    id: root
    required property ScriptExecutorViewHost host

    text: root.host.outputHtml

    Component.onCompleted: Qt.callLater(focusText)

    onContentHeightChanged: {
        if (root.flick.contentY >= root.flick.contentHeight - root.height - 60)
            scrollToBottom();
    }
}
