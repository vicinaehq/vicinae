import QtQuick
import Vicinae

Column {
    id: root
    required property ChatSession session
    property bool compact: false
    readonly property string greeting: pickGreeting()

    function pickGreeting() {
        const hour = new Date().getHours();
        const greetings = hour < 5 || hour >= 22 ? [qsTr("Still up? 🌙"), qsTr("Hello, night owl 🦉")] : hour < 12 ? [qsTr("Good morning ☀️"), qsTr("Morning ☕"), qsTr("Hello 👋")] : hour < 18 ? [qsTr("Good afternoon 👋"), qsTr("Afternoon!"), qsTr("Hello 👋")] : [qsTr("Good evening 👋"), qsTr("Evening!"), qsTr("Hello 👋")];
        return greetings[Math.floor(Math.random() * greetings.length)];
    }

    spacing: compact ? 12 : 16

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
        visible: root.session.needsModel && root.session.modelSelectorItems.length > 0
        text: qsTr("Choose a model to start chatting.")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pointSize: Theme.regularFontSize
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
    }
}
