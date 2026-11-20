import QtQuick
import Vicinae

Column {
    id: root
    required property ChatSession session
    property bool compact: false
    readonly property string greeting: pickGreeting()

    function pickGreeting() {
        const hour = new Date().getHours();
        if (hour < 5 || hour >= 22)
            return qsTr("Hello");
        if (hour < 12)
            return qsTr("Good morning");
        if (hour < 18)
            return qsTr("Good afternoon");
        return qsTr("Good evening");
    }

    spacing: compact ? 10 : 12

    Text {
        width: parent.width
        text: root.greeting
        color: Theme.foreground
        font.family: Theme.fontFamily
        font.pointSize: Theme.regularFontSize + (root.compact ? 5 : 8)
        font.weight: Font.Normal
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
    }

    Text {
        width: parent.width
        visible: root.session.modelSelectorItems.length > 0
        text: root.session.needsModel ? qsTr("Choose a model to start chatting.") : qsTr("What can I help with?")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pointSize: Theme.regularFontSize
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
    }
}
