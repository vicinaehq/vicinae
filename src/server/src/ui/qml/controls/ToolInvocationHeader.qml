pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

Rectangle {
    id: root
    property string title: ""
    property string subtitle: ""
    property var iconSource
    property string status: "queued"
    property var durationMs: null
    property bool expanded: false
    property var resultStatusText: null
    signal toggled

    readonly property bool running: status === "running"
    readonly property bool showDisclosure: expanded || mouse.containsMouse || activeFocus
    readonly property bool succeeded: status === "succeeded" || status === "completed"
    readonly property bool hasResultStatus: resultStatusText !== null && resultStatusText !== undefined && resultStatusText.length > 0
    property string statusText: {
        if (hasResultStatus)
            return resultStatusText;
        switch (status) {
        case "running":
            return qsTr("Running…");
        case "awaiting_approval":
            return qsTr("Waiting for approval");
        case "declined":
            return qsTr("Declined");
        case "succeeded":
        case "completed":
            return qsTr("Completed");
        case "failed":
            return qsTr("Failed");
        case "cancelled":
            return qsTr("Cancelled");
        default:
            return qsTr("Queued");
        }
    }
    readonly property string durationText: {
        if (durationMs === null || durationMs === undefined)
            return "";
        if (durationMs < 1000)
            return "%1 ms".arg(durationMs.toLocaleString(Qt.locale(), 'f', 0));
        return "%1 s".arg((durationMs / 1000).toLocaleString(Qt.locale(), 'f', 1));
    }
    readonly property string statusLabel: durationText.length > 0 ? qsTr("%1 · %2").arg(statusText).arg(durationText) : statusText

    implicitHeight: Math.max(28, titleText.implicitHeight + 10)
    radius: 6
    color: mouse.containsMouse || activeFocus ? Config.withAlpha(Theme.foreground, 0.035) : "transparent"
    border.width: activeFocus ? 1 : 0
    border.color: Theme.buttonFocusOutline
    activeFocusOnTab: true
    Accessible.role: Accessible.Button
    Accessible.name: root.subtitle.length > 0 ? qsTr("%1 — %2 — %3").arg(root.title).arg(root.subtitle).arg(root.statusLabel) : qsTr("%1 — %2").arg(root.title).arg(root.statusLabel)
    Accessible.description: root.expanded ? qsTr("Collapse tool details") : qsTr("Expand tool details")
    Accessible.onPressAction: root.toggled()
    Keys.onReturnPressed: root.toggled()
    Keys.onSpacePressed: root.toggled()

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 8
        spacing: 8
        Item {
            Layout.preferredWidth: 14
            Layout.preferredHeight: 14
            ViciImage {
                anchors.fill: parent
                visible: !root.running
                source: root.showDisclosure ? Img.icon(root.expanded ? BuiltinIcon.ChevronDown : BuiltinIcon.ChevronRight).withFillColor(Theme.textMuted) : root.iconSource || Img.icon(BuiltinIcon.WrenchScrewdriver).withFillColor(Theme.textMuted)
            }
            ViciSpinner {
                anchors.fill: parent
                visible: root.running
            }
        }
        Text {
            id: titleText
            Layout.maximumWidth: root.subtitle.length > 0 ? root.width * 0.4 : root.width * 0.7
            text: root.title.replace(/\s+/g, " ")
            textFormat: Text.PlainText
            elide: Text.ElideRight
            font.family: Theme.fontFamily
            font.pointSize: Theme.smallerFontSize
            color: root.running ? Theme.foreground : Theme.textMuted
        }
        Text {
            Layout.fillWidth: true
            visible: root.subtitle.length > 0
            text: root.subtitle.replace(/\s+/g, " ")
            textFormat: Text.PlainText
            elide: Text.ElideRight
            font.family: Theme.monoFontFamily
            font.pointSize: Theme.smallerFontSize * 0.92
            color: Theme.textMuted
        }
        Item {
            Layout.fillWidth: true
            visible: root.subtitle.length === 0
        }
        Text {
            Layout.maximumWidth: root.width * 0.4
            visible: (!root.succeeded && !root.running) || root.hasResultStatus
            text: root.statusText
            textFormat: Text.PlainText
            elide: Text.ElideRight
            font.family: Theme.fontFamily
            font.pointSize: Theme.smallerFontSize
            color: root.status === "failed" ? Theme.danger : root.status === "awaiting_approval" ? Theme.accent : Theme.textMuted
        }
    }
    ToolTip.visible: mouse.containsMouse && !root.expanded
    ToolTip.text: root.subtitle.length > 0 ? qsTr("%1\n%2").arg(root.subtitle).arg(root.statusLabel) : root.statusLabel
    ToolTip.delay: 700

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.toggled()
    }
}
